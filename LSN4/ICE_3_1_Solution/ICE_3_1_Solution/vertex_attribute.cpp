#include "vertex_attribute.hpp"

AttributePointer BuildAttribute (int values_per_vertex, int data_type, bool normalized, int stride_bytes, int offset_bytes) {
    AttributePointer attribute;
    attribute.values_per_vertex = values_per_vertex;
    attribute.data_type = data_type;
    attribute.normalized = normalized;
    attribute.stride_bytes = stride_bytes;
    attribute.offset_bytes = offset_bytes;
    return attribute;
}


void BindVAO (VAOStruct vao, unsigned int VBO, unsigned int buffer_type) {
    glBindVertexArray(vao.ID);
    glBindBuffer(buffer_type,VBO);
    for (int i = 0; i < vao.attributes.size(); i++) {
        glVertexAttribPointer(i,
                              vao.attributes[i].values_per_vertex,
                              vao.attributes[i].data_type,
                                vao.attributes[i].normalized,
                                vao.attributes[i].stride_bytes,
                                (void*) (intptr_t)vao.attributes[i].offset_bytes);
        glEnableVertexAttribArray(i);
    }
}