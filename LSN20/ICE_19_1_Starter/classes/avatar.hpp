#ifndef AVATAR_HPP
#define AVATAR_HPP
#include "basic_shape.hpp"

// this is for the space ship
class Avatar {
    protected:
        // protected data members here
        BasicShape body;
        // add more here as needed (e.g., turret)
        float initial_y;
        float current_y;
        float speed; //per second
        glm::vec3 position;
        glm::vec3 scale;
        int shader_state;

    public:
        // public data members here

        // constructor for avatar class that takes a BasicShape object,
        // a float for the initial y and a position
        Avatar(BasicShape shape, int shader_state, float y = 0.0f, glm::vec3 pos = glm::vec3(0.0,0.0,0.0));

        ~Avatar();

        // ProcesInput manages ship movement and all other
        void ProcessInput(GLFWwindow *window, float time_passed);

        void Scale (glm::vec3 scale);

        void Draw (Shader *shader_program=NULL); 
        
};
#endif // AVATAR_HPP