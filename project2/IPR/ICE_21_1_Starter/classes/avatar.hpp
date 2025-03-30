#ifndef AVATAR_HPP
#define AVATAR_HPP

#include "basic_shape.hpp"

class Avatar {
    protected:
        BasicShape body;
        float initial_angle_y;
        float current_angle_y;
        float speed = 10.0;
        glm::vec3 location;
        glm::vec3 scale;

    public:
        Avatar(BasicShape body_shape,float orient_y=0.0,
                          glm::vec3 init_location=glm::vec3(0.0));
        void ProcessInput(GLFWwindow *window,float time_passed);
        void SetScale(glm::vec3 a_scale);
        void Draw(Shader *shader_program);

        ~Avatar();


};


#endif //AVATAR_HPP