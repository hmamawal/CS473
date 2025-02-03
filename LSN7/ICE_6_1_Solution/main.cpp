#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Global Variables
glm::vec4 clear_color(0.0,0.0,0.0,1.0);
glm::vec4 outline_color = glm::vec4(0.0,0.0,0.0,1.0);
float angle_z = 0.0;

bool thrusters_fired = false;
float speed = 0.00005;
glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
glm::vec3 location = {0.0f, 0.0f, 0.0f};

bool clear_key_pressed[] {false,false,false};

VAOStruct position_vao; // Declare position_vao as a global variable

struct Projectile {
    glm::vec3 position;
    glm::vec3 velocity;
    BasicShape shape;
};

std::vector<Projectile> projectiles;

int main () {
    GLFWwindow *window = InitializeEnvironment("ICE 6.1",600,600);
    if (window == NULL) {
        return -1;
    }
    
    //create shader program
    Shader shader("./Shaders/vertex.glsl","./Shaders/fragment.glsl");
    
    //get a texture from memory
    unsigned int space_texture = GetTexture("./images/space.jpg");
    unsigned int hull_texture = GetTexture("./images/hull_texture.png");
    unsigned int planet_texture = GetTexture("./images/mars.jpg");

    float rect_vertices[] {
         0.9f,  0.9f, 0.0f, // top right
        0.9f, 0.4f, 0.0f, // bottom right
        0.4f, 0.4f, 0.0f, // bottom left
        0.4f,  0.9f, 0.0f   // top left 
    };

    unsigned int indices[] {
        0,1,3,1,2,3
    };

    unsigned int rect_VBO;
    glGenBuffers(1,&rect_VBO);

    unsigned int EBO; 
    glGenBuffers(1,&EBO);

    //1. define your Vertex Array Object, used to hold attribute pointers
    glGenVertexArrays(1,&(position_vao.id));
    AttributePointer position_attr = BuildAttribute(3,GL_FLOAT,false,3*sizeof(float),0);
    position_vao.attributes.push_back(position_attr);

    VAOStruct texture_vao;
    glGenVertexArrays(1,&(texture_vao.id));
    AttributePointer position_attr2 = BuildAttribute(3,GL_FLOAT,false,5*sizeof(float),0);
    AttributePointer texture_attr = BuildAttribute(2,GL_FLOAT,false,5*sizeof(float),3*sizeof(float));
    texture_vao.attributes.push_back(position_attr2);
    texture_vao.attributes.push_back(texture_attr);

    BasicShape triangle = GetTriangle(position_vao,0.25,glm::vec3(0.0,0.15,0.0));
    BasicShape circle = GetCircle(texture_vao,0.2,40,glm::vec3(0.5,0.5,0.0),true);
    BasicShape hull = GetTexturedHull(texture_vao,glm::vec3(-0.13,-0.11,0.0),0.26,0.4);
    BasicShape view_screen = GetTexturedHull(texture_vao,glm::vec3(-0.10,0.22,0.0),0.2,0.03);
    BasicShape left_canon = GetRectangle(position_vao,glm::vec3(-0.2,-0.1,0.0),0.1,0.2);
    BasicShape right_canon = GetRectangle(position_vao,glm::vec3(0.1,-0.1,0.0),0.1,0.2);
    BasicShape background = GetTextureRectangle(texture_vao,glm::vec3(-1.0,-1.0,0.0),2.0,2.0,1.0);

    shader.use();

    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        //input
        processInput(window);
        
        // set the model matrix to the identity matrix
        shader.setMat4("model",identity);

        //render commands here
        //set the clear color to wipe the window
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);

        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT);

        //Draw the background
        shader.setBool("is_textured",true);
        glBindTexture(GL_TEXTURE_2D,space_texture);
        background.Draw();
        shader.setBool("is_textured",false);
        glBindTexture(GL_TEXTURE_2D,0);

        //Draw the planet
        glBindTexture(GL_TEXTURE_2D,planet_texture);
        shader.setBool("is_textured",true);
        circle.Draw();
        shader.setBool("is_textured",false);
        shader.setVec4("set_color",outline_color);
        circle.DrawEBO();

        //Draw the ship

        // change the model matrix to move the ship
        glm::mat4 orient = glm::mat4(1.0f);
        orient = glm::rotate(orient, glm::radians(-90.0f), glm::vec3(0.0,0.0,1.0));
        orient = glm::scale(orient,glm::vec3(0.5,0.5,1.0));
        orient = glm::translate(orient,glm::vec3(0.0,-0.165,0.0));

        model = identity;
        model = glm::translate(model, location);
        model = glm::rotate(model, glm::radians(angle_z), glm::vec3(0.0,0.0,1.0));

        model = model * orient;
        shader.setMat4("model",model);

        shader.setVec4("set_color",glm::vec4(0.8,0.5,0.5,1.0));
        left_canon.Draw();
        right_canon.Draw();
        shader.setVec4("set_color",outline_color);
        left_canon.DrawEBO();
        right_canon.DrawEBO();
        shader.setVec4("set_color",glm::vec4(0.5,0.5,0.8,1.0));
        triangle.Draw();
        shader.setVec4("set_color",outline_color);
        triangle.DrawEBO();
        glBindTexture(GL_TEXTURE_2D,hull_texture);
        shader.setBool("is_textured",true);
        //shader.setVec4("set_color",glm::vec4(0.5,0.5,0.5,1.0));
        hull.Draw();
        shader.setBool("is_textured",false);
        shader.setVec4("set_color",outline_color);
        hull.DrawEBO();

        shader.setBool("is_textured",false);
        shader.setVec4("set_color",glm::vec4(0.4,0.4,0.7,1.0));
        view_screen.Draw();
        shader.setVec4("set_color",outline_color);
        view_screen.DrawEBO();

        // Move and draw projectiles
        for (auto& projectile : projectiles) {
            projectile.position += projectile.velocity;
            glm::mat4 projectile_model = glm::translate(identity, projectile.position);
            shader.setMat4("model", projectile_model);
            projectile.shape.Draw();
        }

        //check and call events and swap the buffers

        //Applications are usually drawn in a double buffer...one for drawing one for displaying
        //you draw to one and then you swap the buffers...avoids flickering.
        glfwSwapBuffers(window);

        //checks if any events are triggered (keyboard or mouse input)
        glfwPollEvents();
    }

    //terminate your window when you leave the render loop
    glfwTerminate();
    //must have a return value 0 indicates successful run...in this case yes.
    return 0;

}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // use left and right arrows to rotate left and right
    if (glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS) {
        angle_z += 0.01;
    }
    if (glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS) {
        angle_z -= 0.01;
    }

    //when the up arrow is pressed (once) fire thrusters in the direction of the current heading
    if ((glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS) && (!thrusters_fired)) {
        thrusters_fired = true;
        velocity.x += speed * cos(glm::radians(angle_z));
        velocity.y += speed * sin(glm::radians(angle_z));
    }
    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_RELEASE) {
        thrusters_fired = false;
    }

    // when the space button is pressed, create a new projectile
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Projectile new_projectile;
        new_projectile.position = location;
        new_projectile.velocity = glm::vec3(0.01f * cos(glm::radians(angle_z)), 0.01f * sin(glm::radians(angle_z)), 0.0f);
        new_projectile.shape = GetRectangle(position_vao, new_projectile.position, 0.02f, 0.02f);
        projectiles.push_back(new_projectile);
    }

    // each iteration, add velocity to the location
    glm::vec3 new_location = location + velocity;

    // Bound the movement to the edges of the screen
    if (new_location.x < -1.0f || new_location.x > 1.0f) {
        velocity.x = 0.0f;
    }
    if (new_location.y < -1.0f || new_location.y > 1.0f) {
        velocity.y = 0.0f;
    }

    location += velocity;
    
    //Color Key Processing
    int keys[] {GLFW_KEY_R,GLFW_KEY_G,GLFW_KEY_B};
    for (int i = 0; i < 3; i++) {
        if ((glfwGetKey(window,keys[i])==GLFW_PRESS) && (!clear_key_pressed[i])){
            clear_key_pressed[i] = !clear_key_pressed[i];
            clear_color[i] += 0.1;
            if (clear_color[i] > 1.1) {
                clear_color[i] = 0.0;
            }
            PrintColor(clear_color);
        }
        if (glfwGetKey(window,keys[i])==GLFW_RELEASE) {
            clear_key_pressed[i] = false;
        }
    }
}