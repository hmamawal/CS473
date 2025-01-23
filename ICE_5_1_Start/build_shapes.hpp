#ifndef BUILD_SHAPES_HPP
#define BUILD_SHAPES_HPP

#include "basic_shape.hpp"

//Creates and returns a BasicShape triangle given a VAO, a float size, and a location (centered on that location)
BasicShape GetTriangle (VAOStruct vao,float size=1.0, glm::vec3 location = glm::vec3(0.0));

BasicShape GetHull(VAOStruct vao, float width=1.0f, float height=1.0f, glm::vec3 location=glm::vec3(0.0f));

//Creates and returns a BasicShape circle given a VAO, a float radius, the number of points to render, and a location (centered)
BasicShape GetCircle (VAOStruct vao, float radius=1.0, int points= 40, glm::vec3 location = glm::vec3(0.0));


#endif //BUILD_SHAPES_HPP