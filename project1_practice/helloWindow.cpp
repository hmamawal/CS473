#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "utilities/stb_image.h"
#include "utilities/environment.hpp" // For framebuffer_size_callback

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include "classes/basic_shape.hpp"
#include "classes/vertex_attribute.hpp"
#include "classes/Shader.hpp"
#include "utilities/build_shapes.hpp"
#include "utilities/constants.hpp"
#include "classes/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "classes/Player.hpp"
#include "classes/Tile.hpp"
#include "classes/Duck.hpp"
#include "classes/Fox.hpp"
#include "Game/ResourceManager.hpp"
#include "Game/Collision.hpp"
#include "Game/level_loader.hpp"

//
// Function declarations
//
void processInput(GLFWwindow *window);

// Render hearts (unchanged from before)
void renderHearts(unsigned int shaderProgram, unsigned int VAO, unsigned int fullTexture, unsigned int emptyTexture, int currentHearts, int maxHearts) {
    float heartSize = 20.0f;
    for (int i = 0; i < maxHearts; ++i) {
        float x = SCR_WIDTH - (i + 1) * (heartSize + 10.0f);
        float y = SCR_HEIGHT - heartSize - 10.0f;
        unsigned int textureToUse = (i < currentHearts) ? fullTexture : emptyTexture;
        float vertices[] = {
            x, y,                      0.0f, 0.0f,
            x + heartSize, y,          1.0f, 0.0f,
            x + heartSize, y + heartSize, 1.0f, 1.0f,
            x, y,                      0.0f, 0.0f,
            x + heartSize, y + heartSize, 1.0f, 1.0f,
            x, y + heartSize,          0.0f, 1.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, textureToUse);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

// Render a wall tile (unchanged)
void renderTile(const Tile& tile, unsigned int shaderProgram, unsigned int VAO, unsigned int texture) {
    if (tile.isWall) {
        float vertices[] = {
            tile.x, tile.y,                     0.0f, 0.0f,
            tile.x + TILE_SIZE, tile.y,         1.0f, 0.0f,
            tile.x + TILE_SIZE, tile.y + TILE_SIZE, 1.0f, 1.0f,
            tile.x, tile.y,                     0.0f, 0.0f,
            tile.x + TILE_SIZE, tile.y + TILE_SIZE, 1.0f, 1.0f,
            tile.x, tile.y + TILE_SIZE,         0.0f, 1.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}


// Render the fox as a composite shape: four legs, a horizontal body, and a circular head.
void renderFox(const Fox& fox, unsigned int shaderProgram, unsigned int VAO) {
    // Fox dimensions:
    float legWidth = 3.0f;
    float legHeight = 3.0f;
    float bodyWidth = 22.0f;  // wider horizontal rectangle
    float bodyHeight = 8.0f;
    float headRadius = 4.0f;

    // Lambda to draw a rectangle (two triangles)
    auto drawRectangle = [&](float x, float y, float width, float height) {
        float vertices[] = {
            x,         y,           0.0f, 0.0f,
            x + width, y,           1.0f, 0.0f,
            x + width, y + height,  1.0f, 1.0f,
            x,         y,           0.0f, 0.0f,
            x + width, y + height,  1.0f, 1.0f,
            x,         y + height,  0.0f, 1.0f
        };
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };

    // --- Draw 4 Legs ---
    // We choose leg positions relative to fox.x (bottom left of the fox composite shape).
    drawRectangle(fox.x + 1,        fox.y, legWidth, legHeight);
    drawRectangle(fox.x + 7,        fox.y, legWidth, legHeight);
    drawRectangle(fox.x + 13,       fox.y, legWidth, legHeight);
    drawRectangle(fox.x + 19,       fox.y, legWidth, legHeight);

    // --- Draw Body ---
    // Body sits above the legs.
    float bodyX = fox.x;
    float bodyY = fox.y + legHeight;
    drawRectangle(bodyX, bodyY, bodyWidth, bodyHeight);

    // --- Draw Head as a Circle ---
    // Place the head on the left side of the body.
    int segments = 20;
    std::vector<float> circleVertices;
    circleVertices.reserve((segments + 2) * 4); // each vertex: 4 floats
    // Head center: to the left of the body, vertically centered on the body.
    float centerX = bodyX - headRadius;
    float centerY = bodyY + bodyHeight / 2.0f;
    circleVertices.push_back(centerX);
    circleVertices.push_back(centerY);
    circleVertices.push_back(0.5f); // texture coordinate (arbitrary)
    circleVertices.push_back(0.5f);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        float vx = centerX + headRadius * cos(angle);
        float vy = centerY + headRadius * sin(angle);
        float tx = (cos(angle) + 1.0f) * 0.5f;
        float ty = (sin(angle) + 1.0f) * 0.5f;
        circleVertices.push_back(vx);
        circleVertices.push_back(vy);
        circleVertices.push_back(tx);
        circleVertices.push_back(ty);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, circleVertices.size() * sizeof(float), circleVertices.data());
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
}

// Render the duck as a composite shape: a circular body, a circular head, and two triangular wings.
void renderDuck(const Duck& duck, unsigned int shaderProgram, unsigned int VAO) {
    // Duck dimensions:
    float bodyRadius = 5.0f; // Body circle (diameter = 10)
    float headRadius = 3.0f; // Head circle
    int segments = 20;       // For circle approximations

    // --- Draw Body as a Circle ---
    std::vector<float> bodyVertices;
    bodyVertices.reserve((segments + 2) * 4);
    // Assume duck.x, duck.y is the bottom left of the duck's composite shape.
    float bodyCenterX = duck.x + bodyRadius;
    float bodyCenterY = duck.y + bodyRadius;
    bodyVertices.push_back(bodyCenterX);
    bodyVertices.push_back(bodyCenterY);
    bodyVertices.push_back(0.5f);
    bodyVertices.push_back(0.5f);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        float vx = bodyCenterX + bodyRadius * cos(angle);
        float vy = bodyCenterY + bodyRadius * sin(angle);
        float tx = (cos(angle) + 1.0f) * 0.5f;
        float ty = (sin(angle) + 1.0f) * 0.5f;
        bodyVertices.push_back(vx);
        bodyVertices.push_back(vy);
        bodyVertices.push_back(tx);
        bodyVertices.push_back(ty);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bodyVertices.size() * sizeof(float), bodyVertices.data());
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

    // --- Draw Head as a Circle ---
    std::vector<float> headVertices;
    headVertices.reserve((segments + 2) * 4);
    // Place the head to the right side of the body.
    float headCenterX = duck.x + 2 * bodyRadius + headRadius; // e.g., duck.x + 10 + 3 = duck.x + 13
    float headCenterY = duck.y + bodyRadius;
    headVertices.push_back(headCenterX);
    headVertices.push_back(headCenterY);
    headVertices.push_back(0.5f);
    headVertices.push_back(0.5f);
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        float vx = headCenterX + headRadius * cos(angle);
        float vy = headCenterY + headRadius * sin(angle);
        float tx = (cos(angle) + 1.0f) * 0.5f;
        float ty = (sin(angle) + 1.0f) * 0.5f;
        headVertices.push_back(vx);
        headVertices.push_back(vy);
        headVertices.push_back(tx);
        headVertices.push_back(ty);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, headVertices.size() * sizeof(float), headVertices.data());
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

    // --- Draw Wings as Triangles ---
    // Define a top wing
    float topWingVertices[] = {
        bodyCenterX,                bodyCenterY,                0.0f, 0.0f,
        bodyCenterX - 3.0f,         bodyCenterY + bodyRadius,   1.0f, 0.0f,
        bodyCenterX + 2.0f,         bodyCenterY + bodyRadius/2, 0.5f, 1.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(topWingVertices), topWingVertices);
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Define a bottom wing
    float bottomWingVertices[] = {
        bodyCenterX,                bodyCenterY,                0.0f, 0.0f,
        bodyCenterX - 3.0f,         bodyCenterY - bodyRadius,   1.0f, 0.0f,
        bodyCenterX + 2.0f,         bodyCenterY - bodyRadius/2, 0.5f, 1.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bottomWingVertices), bottomWingVertices);
    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


//
// MAIN
//
int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Duck Hunter", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize the player and heart count.
    Player player = { 0.0f, 0.0f };
    int playerHearts = MAX_HEARTS;
    std::vector<Duck> ducks;
    std::vector<Fox> foxes;
    std::vector<Tile> tiles = loadMap("level.txt", player, ducks, foxes);

    // Build and compile our shader program.
    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            // Flip the y-coordinate to match our 2D coordinate system
            vec2 flippedPos = vec2(aPos.x, 600.0 - aPos.y);
            gl_Position = vec4(flippedPos / vec2(800.0, 600.0) * 2.0 - 1.0, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D texture1;
        void main() {
            FragColor = texture(texture1, TexCoord);
        }
    )";

    ResourceManager resourceManager;
    unsigned int shaderProgram  = resourceManager.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
    unsigned int tileTexture    = ResourceManager::loadTexture("images/minecraft_tree.png");
    unsigned int heartTexture   = ResourceManager::loadTexture("images/heart_full.png");
    unsigned int heartEmptyTex  = ResourceManager::loadTexture("images/heart_empty.png");
    

    // Set up vertex data (and buffer(s)) and configure vertex attributes.
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // Allocate a larger dynamic buffer for our composite shapes.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1000, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- Game Loop ---
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Track if player moved this frame
        bool movedThisFrame = false;
        // For the player's orientation (in the main loop):
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            // Move up
            if (!checkCollision(player.getX(), player.getY() - PLAYER_SPEED, PLAYER_WIDTH, PLAYER_HEIGHT, tiles))
                player.setY(player.getY() - PLAYER_SPEED);
            player.setAngle(90.0f); // Facing 'north'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            // Move down
            if (!checkCollision(player.getX(), player.getY() + PLAYER_SPEED, PLAYER_WIDTH, PLAYER_HEIGHT, tiles))
                player.setY(player.getY() + PLAYER_SPEED);
            player.setAngle(270.0f); // Facing 'south'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            // Move left
            if (!checkCollision(player.getX() - PLAYER_SPEED, player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT, tiles))
                player.setX(player.getX() - PLAYER_SPEED);
            player.setAngle(180.0f); // Facing 'west'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            // Move right
            if (!checkCollision(player.getX() + PLAYER_SPEED, player.getY(), PLAYER_WIDTH, PLAYER_HEIGHT, tiles))
                player.setX(player.getX() + PLAYER_SPEED);
            player.setAngle(0.0f); // Facing 'east'
            movedThisFrame = true;
        }

        // Update leg swing angle if the player is moving
        if (movedThisFrame) {
            legSwingAngle += LEG_SWING_SPEED * legSwingDirection;
            if (legSwingAngle > 30.0f || legSwingAngle < -30.0f) {
                legSwingDirection *= -1.0f;
            }
        } else {
            legSwingAngle = 0.0f; // Reset leg swing angle if not moving
        }
        
        // Move ducks and foxes.
        for (Duck& duck : ducks) {
            duck.move(tiles);
        }
        for (Fox& fox : foxes) {
            fox.move(tiles);
        }

        // Check collision with foxes: use the player's composite bounding box.
        for (auto it = foxes.begin(); it != foxes.end(); ) {
            if (player.getX() < it->x + ENTITY_SIZE &&
                player.getX() + PLAYER_WIDTH > it->x &&
                player.getY() < it->y + ENTITY_SIZE &&
                player.getY() + PLAYER_HEIGHT > it->y) {
                playerHearts--;
                it = foxes.erase(it);
                if (playerHearts <= 0) {
                    std::cout << "Game Over! You lost all your hearts." << std::endl;
                    glfwSetWindowShouldClose(window, true);
                    break;
                }
            } else {
                ++it;
            }
        }

        // Check collision with ducks: gain a heart (up to MAX_HEARTS) and remove the duck.
        for (auto it = ducks.begin(); it != ducks.end(); ) {
            if (player.getX() < it->x + ENTITY_SIZE &&
                player.getX() + PLAYER_WIDTH > it->x &&
                player.getY() < it->y + ENTITY_SIZE &&
                player.getY() + PLAYER_HEIGHT > it->y) {
                if (playerHearts < MAX_HEARTS)
                    playerHearts++;
                it = ducks.erase(it);
            } else {
                ++it;
            }
        }

        // Render scene.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render each tile.
        for (const Tile& tile : tiles) {
            renderTile(tile, shaderProgram, VAO, tileTexture);
        }
        // Render composite player.
        player.render(legSwingAngle, shaderProgram, VAO);

        // Render ducks and foxes.
        for (const Duck& duck : ducks) {
            renderDuck(duck, shaderProgram, VAO);
        }
        for (const Fox& fox : foxes) {
            renderFox(fox, shaderProgram, VAO);
        }
        // Render hearts.
        renderHearts(shaderProgram, VAO, heartTexture, heartEmptyTex, playerHearts, MAX_HEARTS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

//
// Utility functions
//

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

