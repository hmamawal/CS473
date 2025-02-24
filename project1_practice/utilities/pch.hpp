#ifndef PCH_HPP
#define PCH_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "environment.hpp" // For framebuffer_size_callback

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

// Classes
#include "../classes/basic_shape.hpp"
#include "../classes/vertex_attribute.hpp"
#include "../classes/Shader.hpp"
#include "../classes/camera.hpp"
#include "../classes/Player.hpp"
#include "../classes/Tile.hpp"
#include "../classes/Duck.hpp"
#include "../classes/Fox.hpp"
#include "../classes/Heart.hpp"

// Utilities
#include "build_shapes.hpp"
#include "constants.hpp"

// Game
#include "../Game/ResourceManager.hpp"
#include "../Game/Collision.hpp"
#include "../Game/level_loader.hpp"

#endif // PCH_HPP