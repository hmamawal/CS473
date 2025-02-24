#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "utilities/stb_image.h"
#include "utilities/environment.hpp" // Include the header for framebuffer_size_callback

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "classes/basic_shape.hpp"
#include "classes/vertex_attribute.hpp"
#include "classes/Shader.hpp"
#include "utilities/build_shapes.hpp"
#include "classes/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int TILE_SIZE = 20;
const float PLAYER_SIZE = 20.0f;
const float PLAYER_SPEED = 100.0f;

struct Tile {
    float x, y;
    bool isWall;
};

struct Player {
    float x, y;
};

void processInput(GLFWwindow *window);
unsigned int compileShader(unsigned int type, const std::string& source);
unsigned int createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
unsigned int loadTexture(const char* path);
bool checkCollision(float x, float y, const std::vector<Tile>& tiles);

std::vector<Tile> loadMap(const std::string& filename, Player& player) {
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
                player.x = tile.x;
                player.y = tile.y;
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

void renderTile(const Tile& tile, unsigned int shaderProgram, unsigned int VAO, unsigned int texture) {
    if (tile.isWall) {
        float vertices[] = {
            // positions        // texture coords
            tile.x, tile.y,            0.0f, 0.0f,
            tile.x + TILE_SIZE, tile.y,            1.0f, 0.0f,
            tile.x + TILE_SIZE, tile.y + TILE_SIZE, 1.0f, 1.0f,

            tile.x, tile.y,            0.0f, 0.0f,
            tile.x + TILE_SIZE, tile.y + TILE_SIZE, 1.0f, 1.0f,
            tile.x, tile.y + TILE_SIZE, 0.0f, 1.0f
        };

        glBindBuffer(GL_ARRAY_BUFFER, VAO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void renderPlayer(const Player& player, unsigned int shaderProgram, unsigned int VAO) {
    float vertices[] = {
        // positions        // texture coords
        player.x, player.y,            0.0f, 0.0f,
        player.x + PLAYER_SIZE, player.y,            1.0f, 0.0f,
        player.x + PLAYER_SIZE, player.y + PLAYER_SIZE, 1.0f, 1.0f,

        player.x, player.y,            0.0f, 0.0f,
        player.x + PLAYER_SIZE, player.y + PLAYER_SIZE, 1.0f, 1.0f,
        player.x, player.y + PLAYER_SIZE, 0.0f, 1.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glUseProgram(shaderProgram);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

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

    // Initialize the player
    Player player = { 0.0f, 0.0f };

    // Load the map
    std::vector<Tile> tiles = loadMap("level.txt", player);

    // Build and compile our shader program
    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        void main() {
            // Flip the y-coordinate
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

    // Load texture
    unsigned int texture = loadTexture("images/minecraft_tree.png");

    // Set up vertex data (and buffer(s)) and configure vertex attributes
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // Update player position
        float playerSpeed = PLAYER_SPEED * 0.01f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !checkCollision(player.x, player.y - playerSpeed, tiles))
            player.y -= playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !checkCollision(player.x, player.y + playerSpeed, tiles))
            player.y += playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !checkCollision(player.x - playerSpeed, player.y, tiles))
            player.x -= playerSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !checkCollision(player.x + playerSpeed, player.y, tiles))
            player.x += playerSpeed;

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the tiles
        for (const Tile& tile : tiles) {
            renderTile(tile, shaderProgram, VAO, texture);
        }

        // Render the player
        renderPlayer(player, shaderProgram, VAO);

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

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
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
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

bool checkCollision(float x, float y, const std::vector<Tile>& tiles) {
    for (const Tile& tile : tiles) {
        if (tile.isWall) {
            if (x < tile.x + TILE_SIZE &&
                x + PLAYER_SIZE > tile.x &&
                y < tile.y + TILE_SIZE &&
                y + PLAYER_SIZE > tile.y) {
                return true;
            }
        }
    }
    return false;
}