#ifndef SPACE_SHIP_HPP
#define SPACE_SHIP_HPP

#include "build_shapes.hpp"
#include <glm/gtc/matrix_transform.hpp>

//This structure is used to hold information needed to draw a single shape.
struct ShapeStruct {
    BasicShape shape;
    //A bool value that, if true, causes the Draw member function to draw the EBO
    // (an outline in this case).
    bool draw_ebo = false;
    glm::vec4 ebo_color = glm::vec4(0.0,0.0,0.0,1.0);
    glm::vec4 color = glm::vec4(0.0,0.0,0.0,1.0);
    //The texture to use when drawing the shape.
    unsigned int texture = 0;

    //a pointer to the shader program to be used when drawing the shape.
    Shader *shader;
};

class SpaceShip {
    protected:
        glm::vec3 location;
        glm::vec3 velocity;
        float angle_z;
        bool thrusters_fired;
        float speed;
        std::vector<ShapeStruct> shapes;
    
    public:
        //Default constructor
        SpaceShip();
        //Draws the ship (uses the shader if provided)
        void Draw(Shader *shader);
        //Draws the ship (but assumes the shader is already being used)
        void Draw();
        //Updates the state of the ship every iteration (e.g., updates location
        // each iteration).
        void Update();
        //handles input for the ship.
        void ProcessInput(GLFWwindow *window);

        //initializes the ship.  There are MANY ways of doing this, this function 
        // takes two different VAOs as input, a shader program, and two textures.
        void Initialize(VAOStruct vao_position, VAOStruct vao_texture, Shader *shader,
                                    unsigned int texture1=0, unsigned int texture2=0);
        //deallocates all the shapes used for the ship.
        void Deallocate();


};



#endif //SPACE_SHIP_HPP
