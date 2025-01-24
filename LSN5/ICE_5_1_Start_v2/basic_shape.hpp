#ifndef BASIC_SHAPE_HPP
#define BASIC_SHAPE_HPP

#include "vertex_attribute.hpp"
#include "Shader.hpp"

//A shape class that capable of drawing a single associated VBO using a pre-existing VAO.  
class BasicShape {
    protected:
        VAOStruct vao;
        unsigned int vbo = 0;
        unsigned int ebo = 0; // EBO identifier
        bool vao_init = false;
        bool vbo_init = false;
        bool ebo_init = false; // EBO initialization flag
        int number_vbo_vertices = 0;
        int number_ebo_indices = 0; // Number of indices in the EBO
        bool vbo_delete = false;
        bool ebo_delete = false; // EBO deletion flag
        GLuint primitive;
        GLuint ebo_primitive; // Primitive to use when drawing the EBO
    public:
        //Takes no inputs and returns a BasicShape object with default values for all data members.
        BasicShape();

        //Initializes the BasicShape data members with provided VAOStruct, vertex data, and primitive.
        //  Also generates a new VBO and assigns the buffer ID to a vbo data member.  Returns nothing.
        void Initialize(VAOStruct vao, float *vertex_data, int data_byte_size, int number_vertices,int primitive=GL_TRIANGLES);

        //Initializes the BasicShape data members with provided EBO data and primitive.
        //  Also generates a new EBO and assigns the buffer ID to an ebo data member.  Returns nothing.
        void InitializeEBO(unsigned int *indices, int data_byte_size, int number_indices, int ebo_primitive = GL_LINE_LOOP);

        //Draws the object using the provided shader program.
        //  Assuming that the object's vao and vbo have been initialized.
        void Draw (Shader shader);

        //Draws the object using the currently used shader program.
        void Draw ();

        //Draws the EBO using the specified line width.
        void DrawEBO(float lineWidth = 1.0f);

        //If the shape was initialized, the destructor deletes the associated VBO.
        ~BasicShape();
};

#endif //BASIC_SHAPE_HPP