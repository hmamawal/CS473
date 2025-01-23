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

BasicShape::~BasicShape() {

    if (this->vbo_delete) {
        glDeleteBuffers(1, &(this->vbo));
    }
}