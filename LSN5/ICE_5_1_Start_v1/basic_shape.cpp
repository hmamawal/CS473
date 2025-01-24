#include "basic_shape.hpp"

BasicShape::BasicShape() {

}

void BasicShape::Initialize(VAOStruct vao, float *vertex_data, int data_byte_size, int number_vertices, int primitive) {
    this->vao = vao;
    glGenBuffers(1, &(this->vbo));
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, data_byte_size, vertex_data, GL_STATIC_DRAW);
    this->vao_init = true;
    this->vbo_init = true;
    this->vbo_delete = true;
    this->number_vbo_vertices = number_vertices;
    this->primitive = primitive;
}

void BasicShape::InitializeEBO(unsigned int *indices, int number_indices, int primitive) {
    BindVAO(this->vao, this->vbo);
    glGenBuffers(1, &(this->ebo_id));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_indices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    this->ebo_number_indices = number_indices;
    this->ebo_initialized = true;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void BasicShape::Draw(Shader shader) {
    glUseProgram(shader.ID);
    this->Draw();
}

void BasicShape::DrawEBO() {
    if (this->ebo_initialized) {
        BindVAO(this->vao, this->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo_id);
        glDrawElements(this->ebo_primitive, this->ebo_number_indices, GL_UNSIGNED_INT, 0);
    } else {
        std::cout << "EBO not initialized" << std::endl;
    }
}

void BasicShape::Draw() {
    BindVAO(this->vao, this->vbo);
    glDrawArrays(this->primitive, 0, this->number_vbo_vertices);
}

BasicShape::~BasicShape() {
    if (this->vbo_delete) {
        glDeleteBuffers(1, &(this->vbo));
    }
}