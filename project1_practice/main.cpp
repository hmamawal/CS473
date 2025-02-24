#include "utilities/environment.hpp"
#include "classes/basic_shape.hpp"
#include "classes/vertex_attribute.hpp"
#include "classes/Shader.hpp"
#include "utilities/build_shapes.hpp"
#include "classes/camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Processes user input (keyboard currently).
// Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

// Processes mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Global Variables
glm::vec4 clear_color(0.0,0.0,0.0,1.0);
glm::vec4 outline_color = glm::vec4(0.0,0.0,0.0,1.0);

bool clear_key_pressed[] {false,false,false};

float angle_z = 0.0;
float angle_x = 0.0;

Camera camera(glm::vec3(0.0,0.0,3.0),glm::vec3(0.0,1.0,0.0),-90.0,0.0f);
float delta_time = 0.0;
float last_frame = 0.0;

// Mouse movement variables
float last_x = 400;
float last_y = 400;
bool first_mouse = true;
float pitch = 0.0;
float yaw = -90.0;
glm::vec3 camera_front = glm::vec3(0.0,0.0,-1.0);

int main () {
    GLFWwindow *window = InitializeEnvironment("ICE 11.1",800,800);
    if (window == NULL) {
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback); 
    
    // Create shader program
    Shader shader("./Shaders/vertex.glsl","./Shaders/fragment.glsl");
    
    // Get textures from memory
    unsigned int tile_texture = GetTexture("./images/tile.jpg");
    unsigned int wood_texture = GetTexture("./images/wood.jpg");

    // Define Vertex Array Objects
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

    // Create shapes
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

    // Set point light properties
    shader.setVec4("point_light.position", glm::vec4(0.0, 10.0, 0.0, 1.0));
    shader.setVec4("point_light.ambient", glm::vec4(0.1, 0.1, 0.1, 1.0));
    shader.setVec4("point_light.diffuse", glm::vec4(1.0, 1.0, 1.0, 1.0));
    shader.setVec4("point_light.specular", glm::vec4(1.0, 1.0, 1.0, 1.0));

    // Set directional light properties
    shader.setVec4("directional_light.direction", glm::vec4(0.0, -1.0, -1.0, 0.0));
    shader.setVec4("directional_light.ambient", glm::vec4(0.0, 1.0, 0.0, 1.0));
    shader.setVec4("directional_light.diffuse", glm::vec4(0.0, 1.0, 0.0, 1.0));
    shader.setVec4("directional_light.specular", glm::vec4(0.0, 1.0, 0.0, 1.0));

    // The render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate the delta time
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // Input
        processInput(window);
        
        view = camera.GetViewMatrix();
        shader.setMat4("local",identity);
        shader.setMat4("model",identity);
        shader.setMat4("view",view);
        shader.setMat4("projection",projection);
        shader.setFloat("ambient_strength",0.1);
        shader.setVec4("view_position",glm::vec4(camera.Position,1.0));

        // Render commands
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int j = 0; j < 10; j++) {
            for (int i = 0; i < pyramid_locations.size(); i++) {
                local = glm::rotate(identity,glm::radians(angle_x),glm::vec3(1.0,0.0,0.0));
                local = glm::rotate(local,glm::radians(angle_z),glm::vec3(0.0,0.0,1.0));
                shader.setMat4("local",local);
                model = glm::translate(identity,pyramid_locations[i]+glm::vec3(0.0,0.0,-2.0*j));
                shader.setMat4("model",model);

                // Render the shapes
                glBindTexture(GL_TEXTURE_2D,tile_texture);
                shader.setBool("is_textured",true);
                bottom.Draw();
                glBindTexture(GL_TEXTURE_2D,wood_texture);
                back_side.Draw();
                right_side.Draw();
                left_side.Draw();
                front_side.Draw();
            }
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate the window
    glfwTerminate();
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

    // Color Key Processing
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

    // Rotation key processing
    if (glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS) {
        angle_z += 2.0;
    }
    if (glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS) {
        angle_z -= 2.0;
    }
    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS) {
        angle_x -= 2.0;
    }
    if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS) {
        angle_x += 2.0;
    }

    // Camera key processing
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
}



