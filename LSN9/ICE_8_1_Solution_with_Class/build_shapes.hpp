#ifndef BUILD_SHAPES_HPP
#define BUILD_SHAPES_HPP

#include "basic_shape.hpp"


//Creates and returns a BasicShape triangle given a VAO, a float size, and a location (centered on that location)
BasicShape GetTriangle (VAOStruct vao,float size=1.0, glm::vec3 location = glm::vec3(0.0));
//Creates and returns a BasicShape circle given a VAO, a float radius, the number of points to render, and a location (centered)
BasicShape GetCircle (VAOStruct vao, float radius=1.0, int points= 40, glm::vec3 location = glm::vec3(0.0), bool is_textured = false);

//Creates and returns a BasicShape hull given a VAO and a location
BasicShape GetHull (VAOStruct vao, glm::vec3 lower_left, float width, float height);

//Creates and returns a BasicShape hull given a VAO and a location
BasicShape GetTexturedHull (VAOStruct vao, glm::vec3 lower_left, float width, float height);

//Creates and returns a BasicShape rectangle given a VAO, location, width, and height
BasicShape GetRectangle(VAOStruct vao, glm::vec3 lower_left, float width, float height);

//Creates and returns a BasicShape rectangle given a VAO, location, width, and height
BasicShape GetTextureRectangle(VAOStruct vao, glm::vec3 lower_left, float width, float height, float texture_size = 1.0);

//Creates and returns a new texture given a string file path as input.
unsigned int GetTexture(std::string file_path);

#endif //BUILD_SHAPES_HPP