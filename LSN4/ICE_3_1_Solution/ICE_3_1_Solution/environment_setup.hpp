#ifndef ENVIRONMENT_SETUP_HPP
#define ENVIRONMENT_SETUP_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

GLFWwindow* InitializeEnvironment (int screen_width, int screen_height, std::string window_name);



#endif //ENVIRONMENT_SETUP_HPP