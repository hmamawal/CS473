#ifndef BUILD_SHAPES_HPP
#define BUILD_SHAPES_HPP

#include "basic_shape.hpp"

//Creates and returns a BasicShape triangle given a VAO, a float size, and a location (centered on that location)
BasicShape GetTriangle (VAOStruct vao,float size=1.0, glm::vec3 location = glm::vec3(0.0));
//Creates and returns a BasicShape circle given a VAO, a float radius, the number of points to render, and a location (centered)
BasicShape GetCircle (VAOStruct vao, float radius=1.0, int points= 40, glm::vec3 location = glm::vec3(0.0));

//Creates and returns a BasicShape ship hull given a VAO and a location (centered on that location)
BasicShape GetShipHull(VAOStruct vao, glm::vec3 location = glm::vec3(0.0));

//Creates and returns a BasicShape windshield given a VAO and a location (centered on that location)
BasicShape GetWindshield(VAOStruct vao, glm::vec3 location = glm::vec3(0.0));

//Creates and returns a BasicShape cannon given a VAO and a location (centered on that location)
BasicShape GetCannon(VAOStruct vao, glm::vec3 location = glm::vec3(0.0));

#endif //BUILD_SHAPES_HPP