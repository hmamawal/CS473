#ifndef BUILD_SHAPES_HPP
#define BUILD_SHAPES_HPP

#include "basic_shape.hpp"

//given a filepath to a texture file, create and return a texture (unsigned int) for use in the 
// main program
unsigned int GetTexture(std::string filepath);

//Creates and returns a BasicShape triangle given a VAO, a float size, and a location (centered on that location)
BasicShape GetTriangle (VAOStruct vao,float size=1.0, glm::vec3 location = glm::vec3(0.0));
//Creates and returns a BasicShape circle given a VAO, a float radius, the number of points to render, and a location (centered)
BasicShape GetCircle (VAOStruct vao, float radius=1.0, int points= 40, glm::vec3 location = glm::vec3(0.0));

//Creates and returns a BasicShape hull given a VAO and a location
BasicShape GetHull (VAOStruct vao, glm::vec3 lower_left, float width, float height);

//Creates and returns a BasicShape rectangle given a VAO, location, width, and height
BasicShape GetRectangle(VAOStruct vao, glm::vec3 lower_left, float width, float height);

// Creates and returns a BasicShape textured rectangle given a VAO, location, width, and height
BasicShape GetTexturedRectangle(VAOStruct vao, glm::vec3 lower_left, float width, float height);

#endif //BUILD_SHAPES_HPP