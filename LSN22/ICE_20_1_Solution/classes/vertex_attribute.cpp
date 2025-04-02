#include "vertex_attribute.hpp"

// BuildAttribute:
// Creates an AttributePointer struct with specified parameters.
// Parameters:
//  - number_vals_per: Number of values per vertex attribute
//  - data_type: Data type (e.g., GL_FLOAT)
//  - is_normalized: Flag indicating if the data should be normalized
//  - stride: Byte stride between consecutive vertex attributes
//  - offset: Byte offset of the first attribute in the buffer
AttributePointer BuildAttribute(int number_vals_per, int data_type, bool is_normalized, int stride, int offset)
{
    AttributePointer attribute;
    attribute.number_per_vertex = number_vals_per;
    attribute.type_data = data_type;
    attribute.normalize_data = is_normalized;
    attribute.stride_bytes = stride;
    attribute.offset_bytes = offset;
    return attribute;
}

// BindVAO:
// Binds a Vertex Array Object (VAO) and its associated Vertex Buffer Object (VBO),
// then sets up the vertex attribute pointers based on the VAOStruct's attributes.
// Parameters:
//  - vao: Struct containing the VAO id and a list of attribute descriptors
//  - vbo: Identifier for the linked Vertex Buffer Object
//  - buffer_type: The type of buffer being bound (e.g., GL_ARRAY_BUFFER)
void BindVAO (VAOStruct vao, unsigned int vbo, int buffer_type) 
{
    glBindVertexArray(vao.id);
    glBindBuffer(buffer_type, vbo);
    for(int i = 0; i < vao.attributes.size(); i++) {
        glVertexAttribPointer(i, vao.attributes[i].number_per_vertex,
                                vao.attributes[i].type_data,
                                vao.attributes[i].normalize_data,
                                vao.attributes[i].stride_bytes,
                                (void*)(intptr_t)vao.attributes[i].offset_bytes);
        glEnableVertexAttribArray(i);
    }
}