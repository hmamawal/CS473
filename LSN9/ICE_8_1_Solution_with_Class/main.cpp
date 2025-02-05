#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include "space_ship.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Global Variables
glm::vec4 clear_color(0.0,0.0,0.0,1.0);
glm::vec4 outline_color = glm::vec4(0.0,0.0,0.0,1.0);

bool clear_key_pressed[] {false,false,false};

int main () {
    GLFWwindow *window = InitializeEnvironment("ICE 8.1 with Class",800,800);
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
    VAOStruct position_vao;
    glGenVertexArrays(1,&(position_vao.id));
    AttributePointer position_attr = BuildAttribute(3,GL_FLOAT,false,3*sizeof(float),0);
    position_vao.attributes.push_back(position_attr);

    VAOStruct texture_vao;
    glGenVertexArrays(1,&(texture_vao.id));
    AttributePointer position_attr2 = BuildAttribute(3,GL_FLOAT,false,5*sizeof(float),0);
    AttributePointer texture_attr = BuildAttribute(2,GL_FLOAT,false,5*sizeof(float),3*sizeof(float));
    texture_vao.attributes.push_back(position_attr2);
    texture_vao.attributes.push_back(texture_attr);

    //declare your spaceship
    SpaceShip ship;
    ship.Initialize(position_vao,texture_vao,&shader,hull_texture);
    BasicShape circle = GetCircle(texture_vao,0.2,40,glm::vec3(0.5,0.5,0.0),true);
    BasicShape background = GetTextureRectangle(texture_vao,glm::vec3(-1.0,-1.0,0.0),2.0,2.0,1.0);

    shader.use();


    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 local = identity;
    glm::mat4 model = identity;


    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        //input
        processInput(window);
        ship.ProcessInput(window);
        ship.Update();
        
        //set the model matrix to the identity matrix
        shader.setMat4("local",identity);
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
        ship.Draw();

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



