#ifndef VERTEX_ATTRIBUTE_HPP
#define VERTEX_ATTRIBUTE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>


struct AttributePointer {
    int values_per_vertex;
    int type_data;
    bool normalize;
    int stride_bytes;
    int offset_bytes;
};

struct VAOStruct {
    unsigned int ID;
    std::vector<AttributePointer> attributes;
};

AttributePointer BuildAttribute(int values_per_vertex, int type_data, bool normalize, int stride_bytes, int offset_bytes);

void BindVAO(VAOStruct vao, unsigned int VBO, unsigned int buffer_type = GL_ARRAY_BUFFER);

#endif //VERTEX_ATTRIBUTE_HPP