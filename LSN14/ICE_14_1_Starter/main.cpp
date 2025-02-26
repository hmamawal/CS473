#include "utilities/environment.hpp"
#include "classes/basic_shape.hpp"
#include "classes/vertex_attribute.hpp"
#include "classes/Shader.hpp"
#include "utilities/build_shapes.hpp"
#include "classes/camera.hpp"
#include "classes/Font.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Processes mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//Global Variables
glm::vec4 clear_color(0.0,0.0,0.0,1.0);
glm::vec4 outline_color = glm::vec4(0.0,0.0,0.0,1.0);

bool clear_key_pressed[] {false,false,false};
bool light_key_pressed = false;
int light_key = GLFW_KEY_1;
bool light_on = true;

float angle_z = 0.0;
float angle_x = 0.0;

Camera camera(glm::vec3(0.0,-4.5,3.0),glm::vec3(0.0,1.0,0.0),-90.0,0.0f);
float delta_time = 0.0;
float last_frame = 0.0;

//mouse movement variables
float last_x = 400;
float last_y = 400;
bool first_mouse = true;
float pitch = 0.0;
float yaw = -90.0;
glm::vec3 camera_front = glm::vec3(0.0,0.0,-1.0);

Font arial_font("./fonts/ArialBlackLarge.bmp","./fonts/ArialBlack.csv",0.1,0.1);

int main () {
    GLFWwindow *window = InitializeEnvironment("ICE 14.1",800,800);
    if (window == NULL) {
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback); 
    
    //create shader program
    Shader shader("./Shaders/vertex.glsl","./Shaders/fragment.glsl");
    Shader font_shader("./Shaders/vertex.glsl","./Shaders/font_fragment.glsl");

    //get a texture from memory
    unsigned int tile_texture = GetTexture("./images/tile.jpg");
    unsigned int wood_texture = GetTexture("./images/wood.jpg");
    unsigned int floor_texture = GetTexture("./images/hull_texture.png");


    //1. define your Vertex Array Object, used to hold attribute pointers
    VAOStruct position_vao;
    glGenVertexArrays(1,&(position_vao.id));
    AttributePointer position_attr = BuildAttribute(3,GL_FLOAT,false,3*sizeof(float),0);
    position_vao.attributes.push_back(position_attr);

    VAOStruct texture_vao;
    glGenVertexArrays(1,&(texture_vao.id));
    AttributePointer position_attr2 = BuildAttribute(3,GL_FLOAT,false,8*sizeof(float),0);
    AttributePointer normal_attr = BuildAttribute(3,GL_FLOAT,false,8*sizeof(float),3*sizeof(float));
    AttributePointer texture_attr = BuildAttribute(2,GL_FLOAT,false,8*sizeof(float),6*sizeof(float));
    texture_vao.attributes.push_back(position_attr2);
    texture_vao.attributes.push_back(normal_attr);
    texture_vao.attributes.push_back(texture_attr);

    //initialize the font
    arial_font.initialize(texture_vao);
 
    BasicShape floor = GetTexturedRectangle(texture_vao, glm::vec3(-10.0,-10.0,0.0),20.0,20.0,20.0,false);

    BasicShape bottom = GetTexturedRectangle(texture_vao,glm::vec3(-0.25,-0.25,0.0),0.5,0.5,1.0,true);
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


    glm::mat4 identity = glm::mat4(1.0f);
    glm::mat4 local = identity;
    glm::mat4 model = identity;
    glm::mat4 view = identity;
    glm::mat4 projection = identity;

    view = glm::translate(view,glm::vec3(0.0,0.0,-4.0));
    projection = glm::perspective(glm::radians(45.0f),1.0f,0.1f,100.0f);
    
    glm::vec3 white_light (1.0,1.0,1.0);
    shader.setVec4("directional_light.ambient",glm::vec4(0.1f*white_light,1.0));
    shader.setVec4("directional_light.diffuse",glm::vec4(white_light,1.0));
    shader.setVec4("directional_light.specular",glm::vec4(white_light,1.0));
    shader.setVec4("directional_light.direction",glm::vec4(-1.0,-1.0,0.0,1.0));
    shader.setBool("directional_light.on",true);

    //font shader settings
    font_shader.use();
    font_shader.setMat4("local",identity);
    font_shader.setMat4("model",identity);
    font_shader.setMat4("view",identity);
    font_shader.setMat4("projection",glm::ortho(-1.0,1.0,-1.0,1.0,-1.0,1.0));
    font_shader.setVec4("transparentColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    font_shader.setFloat("alpha", 0.0);//0.3); - for transparency
    font_shader.setInt("texture1", 0);
    
    shader.use();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {

        //calculate the delta time
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        


        //input
        processInput(window);
        shader.setBool("directional_light.on",light_on);
        
        view = camera.GetViewMatrix();
        shader.setMat4("local",identity);
        shader.setMat4("model",identity);
        shader.setMat4("view",view);
        shader.setMat4("projection",projection);
        shader.setVec4("view_position",glm::vec4(camera.Position,1.0));

        shader.setVec4("spot_light.direction",glm::vec4(camera.Front,1.0));
        shader.setVec4("spot_light.position",glm::vec4(camera.Position,1.0));

        
        //render commands here
        //set the clear color to wipe the window
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);


        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Draw the floor
        local = glm::translate(identity,glm::vec3(0.0,-5.0,0.0));
        local = glm::rotate(local, glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
        shader.setMat4("local",local);
        shader.setMat4("model",identity);
        shader.setBool("is_textured",true);
        glBindTexture(GL_TEXTURE_2D,floor_texture);
        floor.Draw();

        //Draw the pyramid
        local = glm::translate(identity, glm::vec3(0.0,-5.0,0.0));
        local = glm::rotate(local,glm::radians(-90.0f),glm::vec3(1.0,0.0,0.0));
        local = glm::rotate(local,glm::radians(angle_z),glm::vec3(0.0,0.0,1.0));
        shader.setMat4("local",local);
        model = identity;
        shader.setMat4("model",model);

        //Draw the pyramid
        glBindTexture(GL_TEXTURE_2D,tile_texture);
        shader.setBool("is_textured",true);
        bottom.Draw();
        glBindTexture(GL_TEXTURE_2D,wood_texture);
        back_side.Draw();
        right_side.Draw();
        left_side.Draw();
        front_side.Draw();

        // heads up display

        //Draw the text
        font_shader.use();
        // TODO: get location of the camera with camera.Position, then 
        // display camera x, y, z to display location

        // display the distance from the camera to the origin
        std::string display_string = "Distance: ";
        display_string += std::to_string(glm::distance(camera.Position, 
                                            glm::vec3(0.0, 0.0, 0.0))); // the vec3(0.0, 0.0, 0.0) is the origin
        display_string = display_string.substr(0, display_string.find(".")+2); // limit to 6 characters
        arial_font.DrawText(display_string,glm::vec2(0.0,0.0),font_shader);

        //arial_font.DrawText("Hello, World!",glm::vec2(0.0,0.0),font_shader);
        //arial_font.DrawCharacter('*',glm::vec2(-0.05,0.0),font_shader);
        shader.use();

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (first_mouse)
    {
        last_x = xpos;
        last_y = ypos;
        first_mouse = false;
    } 
  
    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; 
    last_x = xpos;
    last_y = ypos;

    float sensitivity = 0.2f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;
    camera.ProcessMouseMovement(xoffset,yoffset);
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

    //rotation key processing
    if (glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS) {
        angle_z += 90.0*delta_time;
    }
    if (glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS) {
        angle_z -= 90.0*delta_time;
    }
    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS) {
        angle_x -= 90.0*delta_time;
    }
    if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS) {
        angle_x += 90.0*delta_time;
    }

    //camera key processing
    if (glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD,delta_time);
    }
    if (glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD,delta_time);
    }
    if (glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT,delta_time);
    }
    if (glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT,delta_time);
    }

    if ((glfwGetKey(window,light_key)==GLFW_PRESS) && (!(light_key_pressed))) {
        light_key_pressed = true;
        light_on = !light_on;
    }
    if (glfwGetKey(window,light_key)==GLFW_RELEASE){
        light_key_pressed = false;
    }


}



