#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

// Declare a global glm::vec4 variable to store the clear color
glm::vec4 clearColor(0.0, 0.0, 0.0, 1.0);

// Declare a boolean array to track key states
bool keyPressed[3] = { false, false, false };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void PrintColor(const glm::vec4& color);
void handleColorChange(GLFWwindow *window, int key, float& colorComponent, int index);

// settings
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

int main()
{
    double rgba[]{0.0,0.7,0.5,1.0};
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    handleColorChange(window, GLFW_KEY_R, clearColor.r, 0);
    handleColorChange(window, GLFW_KEY_G, clearColor.g, 1);
    handleColorChange(window, GLFW_KEY_B, clearColor.b, 2);
}

// Handle color change for a specific key and color component
void handleColorChange(GLFWwindow *window, int key, float& colorComponent, int index)
{
    if(glfwGetKey(window, key) == GLFW_PRESS && !keyPressed[index]) {
        colorComponent = std::min(colorComponent + 0.1f, 1.0f); // Increase color component, cap at 1.0
        if (colorComponent > 1.0f) colorComponent = 0.0f; // Reset if above 1.0
        PrintColor(clearColor);
        keyPressed[index] = true;
    }
    if(glfwGetKey(window, key) == GLFW_RELEASE) {
        keyPressed[index] = false;
    }
}

// Print the current clear color to the console
void PrintColor(const glm::vec4& color)
{
    std::cout << "Clear Color - R: " << color.r << ", G: " << color.g << ", B: " << color.b << ", A: " << color.a << std::endl;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}