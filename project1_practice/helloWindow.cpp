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
#include "classes/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "classes/Player.hpp"
#include "classes/Tile.hpp"
#include "classes/Duck.hpp"
#include "classes/Fox.hpp"

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int TILE_SIZE = 20;

// At global scope or near the top of main.cpp:
static float legSwingAngle = 0.0f;      // Current rotation offset for the legs
static float legSwingDirection = 1.0f;  // 1 means increasing angle, -1 means decreasing
static bool  isWalking = false;         // Track if the player is currently moving
const float LEG_SWING_SPEED = 2.0f; // Adjust this value to change the speed

// For ducks and foxes, we still use a simple block of size 10:
const float ENTITY_SIZE = 10.0f;

// --- New Composite Player Constants ---
// We now assume the player's visual is 10 units wide and 25 units tall
// (with 5 for legs, 10 for body, and roughly 10 for the head's diameter).
const float PLAYER_WIDTH = 10.0f;
const float PLAYER_HEIGHT = 25.0f;
const float PLAYER_SPEED = 0.50f;

// Maximum hearts the player can have.
const int MAX_HEARTS = 3;

//
// Function declarations
//
void processInput(GLFWwindow *window);
unsigned int compileShader(unsigned int type, const std::string& source);
unsigned int createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
unsigned int loadTexture(const char* path);
// Updated collision function takes width and height (for the composite player)
bool checkCollision(float x, float y, float width, float height, const std::vector<Tile>& tiles);

// Load the map from a text file. (For simplicity, only walls and entities are processed.)
std::vector<Tile> loadMap(const std::string& filename, Player& player, std::vector<Duck>& ducks, std::vector<Fox>& foxes) {
    std::vector<Tile> tiles;
    std::ifstream file(filename);
    std::string line;
    int y = 0;
    while (std::getline(file, line)) {
        for (int x = 0; x < line.size(); ++x) {
            Tile tile;
            tile.x = x * TILE_SIZE;
            tile.y = y * TILE_SIZE;
            if (line[x] == '#') {
                tile.isWall = true;
                tiles.push_back(tile);
            } else if (line[x] == 'P') {
                // Set player's starting position.
                player.x = x * TILE_SIZE;
                player.y = y * TILE_SIZE;
                tile.isWall = false;
                tiles.push_back(tile);
            } else if (line[x] == 'D') {
                Duck duck;
                duck.x = x * TILE_SIZE;
                duck.y = y * TILE_SIZE;
                ducks.push_back(duck);
                tile.isWall = false;
                tiles.push_back(tile);
            } else if (line[x] == 'F') {
                Fox fox;
                fox.x = x * TILE_SIZE;
                fox.y = y * TILE_SIZE;
                foxes.push_back(fox);
                tile.isWall = false;
                tiles.push_back(tile);
            } else {
                tile.isWall = false;
                tiles.push_back(tile);
            }
        }
        ++y;
    }
    return tiles;
}

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

void rotatePoint(float& px, float& py, float cx, float cy, float angleDeg)
{
    float rad = glm::radians(angleDeg);
    float tx = px - cx;
    float ty = py - cy;

    float cosA = cos(rad);
    float sinA = sin(rad);

    float rx = tx * cosA - ty * sinA;
    float ry = tx * sinA + ty * cosA;

    px = rx + cx;
    py = ry + cy;
}

// ------------------------------
// NEW: Render the player as a composite shape
// ------------------------------
void renderPlayer(const Player& player, float swingAngle, unsigned int shaderProgram, unsigned int VAO)
{
    // Dimensions
    float legHeight = 5.0f, legWidth = 3.0f;
    float bodyHeight = 10.0f, bodyWidth = 10.0f;
    float headRadius = 5.0f;

    // Overall pivot for the player's rotation
    float pivotX = player.x + bodyWidth * 0.5f;
    float pivotY = player.y + (legHeight + bodyHeight + headRadius) * 0.5f;

    // We'll define a helper that draws a rectangle in local coordinates,
    // but rotates it around a given pivot for "leg swing," and THEN around
    // the player's overall pivot for "direction facing."
    auto drawRectangleWithLegSwing = [&](float x, float y,
                                         float w, float h,
                                         float legPivotX, float legPivotY,
                                         float localAngle) // e.g. +swingAngle or -swingAngle
    {
        float vertices[] = {
            x,      y,      0.0f, 0.0f,
            x + w,  y,      1.0f, 0.0f,
            x + w,  y + h,  1.0f, 1.0f,
            x,      y,      0.0f, 0.0f,
            x + w,  y + h,  1.0f, 1.0f,
            x,      y + h,  0.0f, 1.0f
        };

        // Step 1) Rotate around legPivotX, legPivotY for "leg swing"
        for (int i = 0; i < 6; i++) {
            float& vx = vertices[i*4 + 0];
            float& vy = vertices[i*4 + 1];
            rotatePoint(vx, vy, legPivotX, legPivotY, localAngle);
        }

        // Step 2) Rotate around the player's overall pivot
        for (int i = 0; i < 6; i++) {
            float& vx = vertices[i*4 + 0];
            float& vy = vertices[i*4 + 1];
            rotatePoint(vx, vy, pivotX, pivotY, player.angle);
        }

        // Now send to GPU
        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    };


    //
    // --- DRAW LEGS ---
    //
    // The top of each leg is the pivot for the leg swing
    float leftLegTopX  = player.x + 2.0f;
    float leftLegTopY  = player.y + legHeight;    // top = bottom + legHeight
    float rightLegTopX = player.x + 7.0f;
    float rightLegTopY = player.y + legHeight;

    // Left leg swings with +swingAngle, right leg with -swingAngle (or vice versa).
    drawRectangleWithLegSwing(
        player.x + 2.0f, // rect lower-left X
        player.y,        // rect lower-left Y
        legWidth,
        legHeight,
        leftLegTopX,     // pivot X
        leftLegTopY,     // pivot Y
        swingAngle       // local rotation for left leg
    );

    drawRectangleWithLegSwing(
        player.x + 7.0f,
        player.y,
        legWidth,
        legHeight,
        rightLegTopX,
        rightLegTopY,
        -swingAngle      // local rotation for right leg
    );


    //
    // --- DRAW BODY ---
    //
    // For the body, we do not have a local pivot (we don’t want it “swinging”),
    // so localAngle = 0. But we still must rotate around the player's pivot at the end.
    drawRectangleWithLegSwing(
        player.x,
        player.y + legHeight, // body just above legs
        bodyWidth,
        bodyHeight,
        player.x,             // an unused pivot
        player.y + legHeight,
        0.0f                  // no local swing
    );


    //
    // --- DRAW HEAD (CIRCLE) ---
    //
    // We’ll do what you had before, but add the final rotation around the player's pivot.
    {
        int segments = 20;
        std::vector<float> circleVertices;
        circleVertices.reserve((segments + 2) * 4);

        float centerX = player.x + bodyWidth/2.0f;
        float centerY = player.y + legHeight + bodyHeight + headRadius;

        // Center vertex
        circleVertices.push_back(centerX);
        circleVertices.push_back(centerY);
        circleVertices.push_back(0.5f);
        circleVertices.push_back(0.5f);

        for (int i = 0; i <= segments; i++) {
            float angle = 2.0f * 3.14159f * i / segments;
            float vx = centerX + headRadius * cos(angle);
            float vy = centerY + headRadius * sin(angle);
            float tx = (cos(angle) + 1.0f) * 0.5f;
            float ty = (sin(angle) + 1.0f) * 0.5f;

            // Rotate around the player's pivot (no local pivot for the head)
            rotatePoint(vx, vy, pivotX, pivotY, player.angle);

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
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Load textures.
    unsigned int tileTexture = loadTexture("images/minecraft_tree.png");
    unsigned int heartTexture = loadTexture("images/heart_full.png");
    unsigned int heartEmptyTexture = loadTexture("images/heart_empty.png");

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
            if (!checkCollision(player.x, player.y - PLAYER_SPEED, PLAYER_WIDTH, PLAYER_HEIGHT, tiles))
                player.y -= PLAYER_SPEED;
            player.angle = 90.0f; // Facing 'north'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            // Move down
            if (!checkCollision(player.x, player.y + PLAYER_SPEED, PLAYER_WIDTH, PLAYER_HEIGHT, tiles))
                player.y += PLAYER_SPEED;
            player.angle = 270.0f; // Facing 'south'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            // Move left
            if (!checkCollision(player.x - PLAYER_SPEED, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, tiles))
                player.x -= PLAYER_SPEED;
            player.angle = 180.0f; // Facing 'west'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            // Move right
            if (!checkCollision(player.x + PLAYER_SPEED, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, tiles))
                player.x += PLAYER_SPEED;
            player.angle = 0.0f; // Facing 'east'
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
            if (player.x < it->x + ENTITY_SIZE &&
                player.x + PLAYER_WIDTH > it->x &&
                player.y < it->y + ENTITY_SIZE &&
                player.y + PLAYER_HEIGHT > it->y) {
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
            if (player.x < it->x + ENTITY_SIZE &&
                player.x + PLAYER_WIDTH > it->x &&
                player.y < it->y + ENTITY_SIZE &&
                player.y + PLAYER_HEIGHT > it->y) {
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
        renderPlayer(player, legSwingAngle, shaderProgram, VAO);

        // Render ducks and foxes.
        for (const Duck& duck : ducks) {
            renderDuck(duck, shaderProgram, VAO);
        }
        for (const Fox& fox : foxes) {
            renderFox(fox, shaderProgram, VAO);
        }
        // Render hearts.
        renderHearts(shaderProgram, VAO, heartTexture, heartEmptyTexture, playerHearts, MAX_HEARTS);

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

unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

unsigned int createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

// Updated collision function uses a passed-in width and height.
bool checkCollision(float x, float y, float width, float height, const std::vector<Tile>& tiles) {
    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            if (x < tile.x + TILE_SIZE &&
                x + width > tile.x &&
                y < tile.y + TILE_SIZE &&
                y + height > tile.y) {
                return true;
            }
        }
    }
    return false;
}
