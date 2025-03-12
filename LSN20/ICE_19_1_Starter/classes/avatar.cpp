#include "avatar.hpp"
#include <glm/gtc/matrix_transform.hpp> // library that keeps track of all the scale and rotation and translation matrices


Avatar::Avatar(BasicShape shape, int shader_state, float y,  glm::vec3 pos) {
    this->body = shape;
    this->initial_y = y;
    this->current_y = 0.0;
    this->position = pos;
    this->speed = 10.0; //could pass this in or set it
    this->scale = glm::vec3(1.0,1.0,1.0);
    this->shader_state = shader_state;
}

// todo: add a second boolean input to determine if you ".use()" your shader or not
void Avatar::Draw (Shader *shader_program)
{
    if (shader_program != NULL) {
        shader_program->use();
    }
    glm::mat4 local(1.0);
    local = glm::scale(local,this->scale);
    local = glm::translate(local, this->position);
    local = glm::rotate(local, glm::radians(this->current_y),glm::vec3(0.0,1.0,0.0));
    local = glm::rotate(local, glm::radians(this->initial_y), glm::vec3(0.0,1.0,0.0));
    shader_program->setMat4("local",local);
    shader_program->setInt("shader_state",this->shader_state);
    this->body.Draw();
}

void Avatar::ProcessInput(GLFWwindow *window, float time_passed)
{
    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS) {
        //move forward
        // turn if left or right is pressed
        float change_x = this->speed * time_passed * cos(glm::radians(this->current_y));
        float change_z = this->speed * time_passed * (-sin(glm::radians(this->current_y)));
        this->position += glm::vec3(change_x,0.0,change_z);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            this->current_y += 90.0 * time_passed;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            this->current_y -= 90.0 * time_passed;
        }
    }
}

Avatar::~Avatar()
{
    this->body.DeallocateShape();
}