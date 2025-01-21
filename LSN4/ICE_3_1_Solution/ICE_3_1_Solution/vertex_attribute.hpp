#ifndef VERTEX_ATTRIBUTE_HPP
#define VERTEX_ATTRIBUTE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

struct AttributePointer {
    int values_per_vertex;
    int data_type;
    bool normalized;
    int stride_bytes;
    int offset_bytes;
};

struct VAOStruct {
    unsigned int ID;
    std::vector<AttributePointer> attributes;
};

// Returns an AttributePointer given the required input values defined in the AttributePointer struct
AttributePointer BuildAttribute (int values_per_vertex, int data_type, bool normalized, int stride_bytes, int offset_bytes);

// Accepts a VAO Structure, a VBO identifier, and an optional buffer type and returns nothing.
void BindVAO (VAOStruct vao, unsigned int VBO, unsigned int buffer_type = GL_ARRAY_BUFFER);



#endif //VERTEX_ATTRIBUTE_HPP