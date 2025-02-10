#ifndef BASIC_SHAPE_HPP
#define BASIC_SHAPE_HPP

#include "vertex_attribute.hpp"
#include "Shader.hpp"

//A shape class that capable of drawing a single associated VBO using a pre-existing VAO.  
class BasicShape {
    protected:
        VAOStruct vao;
        unsigned int vbo = 0;
        int number_vbo_vertices = 0;
        unsigned int ebo_id = 0;
        int ebo_number_indices = 0;
        bool ebo_initialized = false;
        int ebo_primitive;
        GLuint primitive;
    public:
        //Takes no inputs and returns a BasicShape object with default values for all data members.
        BasicShape();

        //Initializes the BasicShape data members with provided VAOStruct, vertex data, and primitive.
        //  Also generates a new VBO and assigns the buffer ID to a vbo data member.  Returns nothing.
        void Initialize(VAOStruct vao, float *vertex_data, int data_byte_size, int number_vertices,int primitive=GL_TRIANGLES);

        //Initializes the EBO information for a Basic shape, generating the element buffer object.  
        //Returns nothing
        void InitializeEBO(unsigned int* ebo_data, int number_bytes, int number_indices,int primitive = GL_LINE_LOOP);

        //Draws the object using the provided shader program.
        //  Assuming that the object's vao and vbo have been initialized.
        void Draw (Shader shader);

        //Draws the object using the currently used shader program.
        void Draw ();

        //Draws the object EBO using the currently used shader program.
        void DrawEBO();

        //If the shape was initialized, the destructor deletes the associated VBO.
        void DeallocateShape();
};

#endif //BASIC_SHAPE_HPP