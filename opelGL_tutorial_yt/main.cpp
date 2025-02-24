#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

int main(int argc, char **argv)
{
    glfwInit();
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE); // make window non-resizable
    glfwOpenWindow(800*16/9, 800, 8, 8, 8, 8, 0, 0, GLFW_WINDOW);
    glfwSetWindowTitle("OpenGL Game");

    bool running = true;

    while(running)
    {
        glClear(GL_COLOR_BUFFER_BIT); // clear the color buffer
        glwSwapBuffers(); // swap the front and back buffers

        running = glfwGetWindowParam(GLFW_OPENED); // check if the window was told to be closed
    }

    glfwTerminate(); // clean up GLFW
}