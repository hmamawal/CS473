#include "avatar.hpp"
#include <glm/gtc/matrix_transform.hpp>

Avatar::Avatar(BasicShape body_shape,float orient_y,
                    glm::vec3 location)
{
    this->body = body_shape;
    this->initial_angle_y = orient_y;
    this->location = location;
    this->scale = glm::vec3(1.0);
    this->current_angle_y = 0.0;
}
void Avatar::ProcessInput(GLFWwindow *window,float time_passed)
{
    if (glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS) {
        float change_x = this->speed 
                         * time_passed 
                         * cos(glm::radians(this->current_angle_y));
        float change_z = this->speed 
                         * time_passed 
                         * (-sin(glm::radians(this->current_angle_y)));
        this->location += glm::vec3(change_x,0.0,change_z);

        if (glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS) {
            this->current_angle_y += 0.1;
        }

        if (glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) {
            this->current_angle_y -= 0.1;
        }
    }
}
void Avatar::SetScale(glm::vec3 a_scale) {
    this->scale = a_scale;
}
void Avatar::Draw(Shader *shader_program) {
    
    //draw the ship using its materials
    shader_program->setBool("is_imported",true);
    glm::mat4 avatar_transform(1.0);
    //1. complete initial rotation about y
    //2. scale 
    //3. rotate to current y
    //4. translate to the location

    avatar_transform = glm::translate(avatar_transform,this->location);
    avatar_transform = glm::rotate(avatar_transform,glm::radians(this->current_angle_y),
                        glm::vec3(0.0,1.0,0.0));
    avatar_transform = glm::scale(avatar_transform,this->scale);
    avatar_transform = glm::rotate(avatar_transform,glm::radians(this->initial_angle_y),
                        glm::vec3(0.0,1.0,0.0));
    shader_program->setMat4("transform",avatar_transform);
    shader_program->setMat4("model",glm::mat4(1.0));
    this->body.Draw();
    

}

Avatar::~Avatar() {
    this->body.DeallocateShape();
}