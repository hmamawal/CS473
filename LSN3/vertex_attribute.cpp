#include "vertex_attribute.hpp"

AttributePointer BuildAttribute(int values_per_vertex, int type_data, bool normalize, int stride_bytes, int offset_bytes)
{
    AttributePointer attribute;
    attribute.values_per_vertex = values_per_vertex;
    attribute.type_data = type_data;
    attribute.normalize = normalize;
    attribute.stride_bytes = stride_bytes;
    attribute.offset_bytes = offset_bytes;
    return attribute;
}

void BindVAO(VAOStruct vao, unsigned int VBO, unsigned int buffer_type){
    glBindVertexArray(vao.ID);
    glBindBuffer(buffer_type, VBO);
    for (int i = 0; i < vao.attributes.size(); i++) {
        AttributePointer a = vao.attributes[i];
        glVertexAttribPointer(i, a.values_per_vertex, a.type_data, a.normalize, a.stride_bytes, (void*)a.offset_bytes);
        glEnableVertexAttribArray(i);
    }
}