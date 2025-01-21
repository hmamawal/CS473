#ifndef BASIC_SHAPE_HPP
#define BASIC_SHAPE_HPP

#include "vertex_attribute.hpp"

class BasicShape {
    protected:
        VAOStruct vao;
        unsigned int VBO;
        int number_of_vertices;
        GLuint primitive;

    public:
        BasicShape();

        // Accepts a VAO Structure, vertex data, number of bytes, number of vertices, and optionally a type of primitive to draw and 
        //  initializes the basic shape object for use.
        void Initialize(VAOStruct vao, float *vertex_data, int number_of_bytes, int number_of_vertices, GLuint = GL_TRIANGLES);

        // Draws the basic shape to the screen.
        void Draw(unsigned int shader_program);

        // Deallocates the shape from memory.
        void DeallocateShape();


};

#endif