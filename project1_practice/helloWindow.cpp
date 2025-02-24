#include "utilities/pch.hpp"

//
// Function declarations
//
void processInput(GLFWwindow *window);

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

    // Initialize hearts
    std::vector<Heart> hearts;
    for (int i = 0; i < MAX_HEARTS; ++i) {
        hearts.emplace_back(SCR_WIDTH - (i + 1) * 30.0f, SCR_HEIGHT - 30.0f, true);
    }

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
            if (!checkCollision(player.getX(), player.getY() - PLAYER_SPEED, 6.0f, 12.0f, tiles))
                player.setY(player.getY() - PLAYER_SPEED);
            player.setAngle(90.0f); // Facing 'north'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            // Move down
            if (!checkCollision(player.getX(), player.getY() + PLAYER_SPEED, 6.0f, 12.0f, tiles))
                player.setY(player.getY() + PLAYER_SPEED);
            player.setAngle(270.0f); // Facing 'south'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            // Move left
            if (!checkCollision(player.getX() - PLAYER_SPEED, player.getY(), 6.0f, 12.0f, tiles))
                player.setX(player.getX() - PLAYER_SPEED);
            player.setAngle(180.0f); // Facing 'west'
            movedThisFrame = true;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            // Move right
            if (!checkCollision(player.getX() + PLAYER_SPEED, player.getY(), 6.0f, 12.0f, tiles))
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
            if (player.getX() < it->getX() + ENTITY_SIZE &&
                player.getX() + 6.0f > it->getX() &&
                player.getY() < it->getY() + ENTITY_SIZE &&
                player.getY() + 12.0f > it->getY()) {
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
            if (player.getX() < it->getX() + ENTITY_SIZE &&
                player.getX() + 6.0f > it->getX() &&
                player.getY() < it->getY() + ENTITY_SIZE &&
                player.getY() + 12.0f > it->getY()) {
                if (playerHearts < MAX_HEARTS)
                    playerHearts++;
                it = ducks.erase(it);
            } else {
                ++it;
            }
        }

        // Update hearts based on playerHearts
        for (int i = 0; i < MAX_HEARTS; ++i) {
            hearts[i].setFull(i < playerHearts);
        }

        // Render scene.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render each tile.
        for (const Tile& tile : tiles) {
            tile.render(shaderProgram, VAO, tileTexture);
        }
        // Render composite player.
        player.render(legSwingAngle, shaderProgram, VAO);

        // Render ducks and foxes.
        for (const Duck& duck : ducks) {
            duck.render(shaderProgram, VAO);
        }
        for (const Fox& fox : foxes) {
            fox.render(shaderProgram, VAO);
        }
        // Render hearts.
        for (const Heart& heart : hearts) {
            heart.render(shaderProgram, VAO, heartTexture, heartEmptyTex);
        }

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