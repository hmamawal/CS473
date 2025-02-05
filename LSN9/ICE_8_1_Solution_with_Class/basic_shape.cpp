#include "basic_shape.hpp"

BasicShape::BasicShape () {

}

void BasicShape::Initialize (VAOStruct vao, float *vertex_data, int data_byte_size, int number_vertices, int primitive)
{
    this->vao = vao;
    glGenBuffers(1,&(this->vbo));
    glBindBuffer(GL_ARRAY_BUFFER,this->vbo);
    glBufferData(GL_ARRAY_BUFFER,data_byte_size,vertex_data,GL_STATIC_DRAW);
    this->number_vbo_vertices = number_vertices;
    this->primitive = primitive;
}

void BasicShape::InitializeEBO(unsigned int* ebo_data, int number_bytes, int number_indices,int primitive) 
{
    glGenBuffers(1, &(this->ebo_id));
    BindVAO(this->vao,this->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,number_bytes,ebo_data,GL_STATIC_DRAW);
    this->ebo_initialized = true;
    this->ebo_number_indices = number_indices;
    this->ebo_primitive = primitive;
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

void BasicShape::DrawEBO() {
    BindVAO(this->vao,this->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,this->ebo_id);
    glDrawElements(this->ebo_primitive,this->ebo_number_indices,GL_UNSIGNED_INT,0);
}

void BasicShape::DeallocateShape() {

    if (this->vbo > 0) {
        glDeleteBuffers(1, &(this->vbo));
    }
    if (this->ebo_initialized) {
        glDeleteBuffers(1, &(this->ebo_id));
    }
}