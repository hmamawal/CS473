#include "basic_shape.hpp"

BasicShape::BasicShape() {
    vao.ID = 0;
    VBO = 0;
    number_of_vertices = 0;
    primitive = GL_TRIANGLES;
}

void BasicShape::Initialize(VAOStruct vao, float *vertex_data, int number_of_bytes, int number_of_vertices, GLuint primitive) {
    this->vao = vao;
    this->number_of_vertices = number_of_vertices;
    this->primitive = primitive;

    glGenBuffers(1, &(this->VBO));
    BindVAO(this->vao, (this->VBO));
    glBufferData(GL_ARRAY_BUFFER, number_of_bytes, vertex_data, GL_STATIC_DRAW);
}

void BasicShape::Draw(unsigned int shader_program) {
    glUseProgram(shader_program);
    BindVAO(this->vao, this->VBO);
    glDrawArrays(this->primitive, 0 ,this->number_of_vertices);
    glBindVertexArray(0);

}

void BasicShape::DeallocateShape() {
    glDeleteVertexArrays(1, &(this->vao.ID));
    glDeleteBuffers(1, &(this->VBO));
}