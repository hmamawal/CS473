#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Global Variables
glm::vec4 clear_color(0.0,0.0,0.0,1.0);
glm::vec4 outline_color = glm::vec4(0.0,0.0,0.0,1.0);

bool clear_key_pressed[] {false,false,false};

float angle_z = 0.0;
float angle_x = 0.0;
float last_frame = 0.0;
float delta_time = 0.0;

// camera global variable
Camera camera(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 1.0, 0.0), -90.0, 0.0);

int main () {
    GLFWwindow *window = InitializeEnvironment("ICE 9.2",800,800);
    if (window == NULL) {
        return -1;
    }
    
    //create shader program
    Shader shader("./Shaders/vertex.glsl","./Shaders/fragment.glsl");
    
    //get a texture from memory
    unsigned int tile_texture = GetTexture("./images/tile.jpg");
    unsigned int wood_texture = GetTexture("./images/wood.jpg");


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


    BasicShape bottom = GetTextureRectangle(texture_vao,glm::vec3(-0.25,-0.25,0.0),0.5,0.5);
    BasicShape front_side = GetTexturedTriangle(texture_vao,glm::vec3(-0.25, -0.25, 0.0),
                                                           glm::vec3(0.25, -0.25, 0.0),
                                                           glm::vec3(0.0,0.0,0.5));
    
    BasicShape right_side = GetTexturedTriangle(texture_vao,glm::vec3(0.25, -0.25, 0.0),
                                                           glm::vec3(0.25, 0.25, 0.0),
                                                           glm::vec3(0.0,0.0,0.5));
    
    BasicShape back_side = GetTexturedTriangle(texture_vao,glm::vec3(0.25, 0.25, 0.0),
                                                           glm::vec3(-0.25, 0.25, 0.0),
                                                           glm::vec3(0.0,0.0,0.5));
    
    BasicShape left_side = GetTexturedTriangle(texture_vao,glm::vec3(-0.25, 0.25, 0.0),
                                                           glm::vec3(-0.25, -0.25, 0.0),
                                                           glm::vec3(0.0,0.0,0.5));
    shader.use();

    std::vector<glm::vec3> pyramid_locations {
        glm::vec3(0.0,0.0,0.0),
        glm::vec3(1.0,0.0,0.0),
        glm::vec3(0.0,-1.0,0.0),
        glm::vec3(-1.0,0.0,0.0),
        glm::vec3(0.0,1.0,0.0),

    };


    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 local = identity;
    glm::mat4 model = identity;
    glm::mat4 view = identity;
    glm::mat4 projection = identity;

    view = glm::translate(view,glm::vec3(0.0,0.0,-4.0));
    projection = glm::perspective(glm::radians(45.0f),1.0f,0.1f,100.0f);
    

    glEnable(GL_DEPTH_TEST);
    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {

        // calculate the delta time
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;


        //input
        processInput(window);
        
        
        shader.setMat4("local",identity);
        shader.setMat4("model",identity);
        view = camera.GetViewMatrix();
        shader.setMat4("view",view);
        shader.setMat4("projection",projection);


        //render commands here
        //set the clear color to wipe the window
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);


        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < pyramid_locations.size(); i++) {
            //set the model matrix to the identity matrix
            local = glm::rotate(identity,glm::radians(angle_x),glm::vec3(1.0,0.0,0.0));
            local = glm::rotate(local,glm::radians(angle_z),glm::vec3(0.0,0.0,1.0));
            shader.setMat4("local",local);
            model = glm::rotate(identity,glm::radians(-45.0f),glm::vec3(1.0,0.0,0.0));
            model = glm::translate(model,pyramid_locations[i]);
            shader.setMat4("model",model);

            //render the shapes here
            glBindTexture(GL_TEXTURE_2D,tile_texture);
            shader.setBool("is_textured",true);
            bottom.Draw();
            glBindTexture(GL_TEXTURE_2D,wood_texture);
            back_side.Draw();
            right_side.Draw();
            left_side.Draw();
            front_side.Draw();

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

    // camera controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, delta_time);
    // go up with q and down with e
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, delta_time);


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

    //rotation key processing
    if (glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS) {
        angle_z += 0.01;
    }
    if (glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS) {
        angle_z -= 0.01;
    }
    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS) {
        angle_x -= 0.01;
    }
    if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS) {
        angle_x += 0.01;
    }
}



