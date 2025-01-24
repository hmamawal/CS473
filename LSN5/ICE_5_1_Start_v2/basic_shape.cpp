#include "basic_shape.hpp"

BasicShape::BasicShape () {

}

void BasicShape::Initialize (VAOStruct vao, float *vertex_data, int data_byte_size, int number_vertices, int primitive)
{
    this->vao = vao;
    glGenBuffers(1,&(this->vbo));
    glBindBuffer(GL_ARRAY_BUFFER,this->vbo);
    glBufferData(GL_ARRAY_BUFFER,data_byte_size,vertex_data,GL_STATIC_DRAW);
    this->vao_init = true;
    this->vbo_init = true;
    this->vbo_delete = true;
    this->number_vbo_vertices = number_vertices;
    this->primitive = primitive;
}

void BasicShape::InitializeEBO(unsigned int *indices, int data_byte_size, int number_indices, int ebo_primitive)
{
    glGenBuffers(1, &this->ebo);
    glBindVertexArray(this->vao.id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_byte_size, indices, GL_STATIC_DRAW);

    this->ebo_init = true;
    this->ebo_delete = true;
    this->number_ebo_indices = number_indices;
    this->ebo_primitive = ebo_primitive;
}

void BasicShape::Draw (Shader shader) 
{
    glUseProgram(shader.ID);
    this->Draw();
    
}

void BasicShape::Draw ()
{
    BindVAO(this->vao,this->vbo);
    glDrawArrays(this->primitive,0,this->number_vbo_vertices);
}

void BasicShape::DrawEBO(float lineWidth)
{
    if (this->ebo_init) {
        BindVAO(this->vao, this->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
        glLineWidth(lineWidth);
        glDrawElements(this->ebo_primitive, this->number_ebo_indices, GL_UNSIGNED_INT, 0);
        glLineWidth(1.0f);
    }
}

BasicShape::~BasicShape() {

    if (this->vbo_delete) {
        glDeleteBuffers(1, &(this->vbo));
    }
}