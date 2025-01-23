#ifndef BASIC_SHAPE_HPP
#define BASIC_SHAPE_HPP

#include "vertex_attribute.hpp"
#include "Shader.hpp"

//A shape class that capable of drawing a single associated VBO using a pre-existing VAO.  
class BasicShape {
    protected:
        VAOStruct vao;
        unsigned int vbo = 0;
        bool vao_init = false;
        bool vbo_init = false;
        int number_vbo_vertices = 0;
        bool vbo_delete = false;
        GLuint primitive;
    public:
        //Takes no inputs and returns a BasicShape object with default values for all data members.
        BasicShape();

        //Initializes the BasicShape data members with provided VAOStruct, vertex data, and primitive.
        //  Also generates a new VBO and assigns the buffer ID to a vbo data member.  Returns nothing.
        void Initialize(VAOStruct vao, float *vertex_data, int data_byte_size, int number_vertices,int primitive=GL_TRIANGLES);

        //Draws the object using the provided shader program.
        //  Assuming that the object's vao and vbo have been initialized.
        void Draw (Shader shader);

        //Draws the object using the currently used shader program.
        void Draw ();

        //If the shape was initialized, the destructor deletes the associated VBO.
        ~BasicShape();
};

#endif //BASIC_SHAPE_HPP