#include "space_ship.hpp"

SpaceShip::SpaceShip() {
    this->angle_z = 0.0;
    this->thrusters_fired = false;
    this->speed = 0.01;
    this->velocity = glm::vec3(0.0,0.0,0.0);
    this->location = glm::vec3(0.0,0.0,0.0);
}

void SpaceShip::Initialize(VAOStruct vao_position, VAOStruct vao_texture, Shader *shader,
                                unsigned int texture1, unsigned int texture2) {
    // BasicShape left_canon = GetRectangle(position_vao,glm::vec3(-0.2,-0.1,0.0),0.1,0.2);
    // BasicShape right_canon = GetRectangle(position_vao,glm::vec3(0.1,-0.1,0.0),0.1,0.2);
    ShapeStruct left_canon;
    left_canon.shape = GetRectangle(vao_position,glm::vec3(-0.2,-0.1,0.0),0.1,0.2);
    left_canon.color = glm::vec4(0.8,0.5,0.5,1.0);
    left_canon.draw_ebo = true;
    left_canon.texture = 0;
    left_canon.ebo_color = glm::vec4(0.0,0.0,0.0,1.0);
    left_canon.shader = shader;
    this->shapes.push_back(left_canon);

    ShapeStruct right_canon;
    right_canon.shape = GetRectangle(vao_position,glm::vec3(0.1,-0.1,0.0),0.1,0.2);
    right_canon.color = glm::vec4(0.8,0.5,0.5,1.0);
    right_canon.draw_ebo = true;
    right_canon.texture = 0;
    right_canon.ebo_color = glm::vec4(0.0,0.0,0.0,1.0);
    right_canon.shader = shader;
    this->shapes.push_back(right_canon);
    
    ShapeStruct wing;
    //BasicShape triangle = GetTriangle(position_vao,0.25,glm::vec3(0.0,0.15,0.0));
    wing.shape = GetTriangle(vao_position,0.25,glm::vec3(0.0,0.15,0.0));
    wing.color = glm::vec4(0.4,0.4,0.7,1.0);
    wing.texture = 0;
    wing.draw_ebo = true;
    wing.ebo_color = glm::vec4(0.0,0.0,0.0,1.0);
    wing.shader = shader;
    this->shapes.push_back(wing);
    
    ShapeStruct hull;
    hull.shape = GetTexturedHull(vao_texture,glm::vec3(-0.13,-0.11,0.0),0.26,0.4);
    hull.texture = texture1;
    hull.draw_ebo = true;
    hull.ebo_color = glm::vec4(0.0,0.0,0.0,1.0);
    hull.color = glm::vec4(1.0,1.0,1.0,1.0);
    hull.shader = shader;
    this->shapes.push_back(hull);
    ShapeStruct view_screen;
    view_screen.shape = GetTexturedHull(vao_texture,glm::vec3(-0.10,0.22,0.0),0.2,0.03);
    view_screen.texture = 0;
    view_screen.draw_ebo = true;
    view_screen.ebo_color = glm::vec4(0.0,0.0,0.0,1.0);
    view_screen.color = glm::vec4(0.4,0.4,0.7,1.0);
    view_screen.shader = shader;
    this->shapes.push_back(view_screen);
    
    
}

void SpaceShip::Draw() {

    //change the local matrix to move the ship.
    glm::mat4 identity(1.0f);
    glm::mat4 orient = glm::mat4(1.0f);
    orient = glm::rotate(orient,glm::radians(-90.0f),glm::vec3(0.0,0.0,1.0));
    orient = glm::scale(orient,glm::vec3(0.5,0.5,1.0));
    orient = glm::translate(orient,glm::vec3(0.0,-0.165,0.0));
    
    glm::mat4 local = identity;
    local = glm::translate(local,location);
    local = glm::rotate(local,glm::radians(angle_z),glm::vec3(0.0,0.0,1.0));

    local = local * orient;


    for (int i = 0; i < this->shapes.size(); i++) {
        this->shapes[i].shader->setMat4("local",local);
        if (this->shapes[i].texture > 0) {
            glBindTexture(GL_TEXTURE_2D,this->shapes[i].texture);
            this->shapes[i].shader->setBool("is_textured",true);
        } else {
            this->shapes[i].shader->setBool("is_textured",false);
            this->shapes[i].shader->setVec4("set_color",this->shapes[i].color);
        }
        this->shapes[i].shape.Draw();
        if (this->shapes[i].draw_ebo) {
            this->shapes[i].shader->setVec4("set_color",this->shapes[i].ebo_color);
            this->shapes[i].shader->setBool("is_textured",false);
            this->shapes[i].shape.DrawEBO();
        }
    }
}

void SpaceShip::Draw(Shader *shader) {
    glUseProgram(shader->ID);
    this->Draw();
}

void SpaceShip::Update () {
    //each iteration, add velocity to the location
    if ((this->location.x + this->velocity.x > 1.0) || (this->location.x + this->velocity.x < -1.0)) {
        this->velocity.x = 0.0;
    }
    if ((this->location.y + this->velocity.y > 1.0) || (this->location.y + this->velocity.y < -1.0)) {
        this->velocity.y = 0.0;
    }

    this->location += this->velocity;
}

void SpaceShip::ProcessInput(GLFWwindow *window) {

     //Use left and right arrows to rotate left and right
    if (glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS) {
        this->angle_z += 2.0;
    }
    if (glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS) {
        this->angle_z -= 2.0;
    }

    //When the up arrow is pressed (once) fire thrusters in the direction of the 
    // current heading
    if ((glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS) && (!(this->thrusters_fired))) {
        this->thrusters_fired = true;
        this->velocity.x += speed * cos(glm::radians(this->angle_z));
        this->velocity.y += speed * sin(glm::radians(this->angle_z));
    }

    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_RELEASE) {
        this->thrusters_fired = false;
    }
}

void SpaceShip::Deallocate() {
    for (int i = 0; i < this->shapes.size(); i++) {
        this->shapes[i].shape.DeallocateShape();
    }
}