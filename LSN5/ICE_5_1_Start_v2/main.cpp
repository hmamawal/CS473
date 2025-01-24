#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"

//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Global Variables
glm::vec4 clear_color(0.0,0.0,0.0,1.0);
bool clear_key_pressed[] {false,false,false};

glm::vec4 offset_vec(0.0,0.0,0.0,0.0);

int main () {
    GLFWwindow *window = InitializeEnvironment("LearnOpenGL",600,600);
    if (window == NULL) {
        return -1;
    }
    
    Shader shader("./Shaders/vertex.glsl","./Shaders/fragment.glsl");
    
    // Define your Vertex Array Object, used to hold attribute pointers
    VAOStruct position_vao;
    glGenVertexArrays(1,&(position_vao.id));
    AttributePointer position_attr = BuildAttribute(3,GL_FLOAT,false,3*sizeof(float),0);
    position_vao.attributes.push_back(position_attr);

    // Create shapes
    BasicShape ship_hull = GetShipHull(position_vao, glm::vec3(0.0, -0.3, 0.0));
    BasicShape windshield = GetWindshield(position_vao, glm::vec3(0.0, 0.0, 0.0));
    BasicShape cannon1 = GetCannon(position_vao, glm::vec3(-0.2, 0.1, 0.0));
    BasicShape cannon2 = GetCannon(position_vao, glm::vec3(0.2, 0.1, 0.0));

    shader.use();
    // The render loop -- the function in the condition checks if the 
    // window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render commands here
        // set the clear color to wipe the window
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);

        // clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw ship hull
        shader.setVec4("set_color", glm::vec4(0.6, 0.3, 0.0, 1.0)); // Brown color
        ship_hull.Draw();

        // Draw windshield
        shader.setVec4("set_color", glm::vec4(0.0, 0.5, 1.0, 1.0)); // Blue color
        windshield.Draw();

        // Draw cannons
        shader.setVec4("set_color", glm::vec4(0.3, 0.3, 0.3, 1.0)); // Gray color
        cannon1.Draw();
        cannon2.Draw();

        // Draw outlines for ship hull, windshield, and cannons
        //shader.setVec4("set_color", glm::vec4(1.0, 1.0, 1.0, 1.0)); // White color for outlines
        //ship_hull.DrawEBO(1.0f);
        //windshield.DrawEBO(1.0f);
        //cannon1.DrawEBO(1.0f);
        //cannon2.DrawEBO(1.0f);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // terminate your window when you leave the render loop
    glfwTerminate();
    // must have a return value 0 indicates successful run...in this case yes.
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_RELEASE){
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); 
    }
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
    // Arrow Key processing
    int dir[] {GLFW_KEY_UP,GLFW_KEY_DOWN,GLFW_KEY_RIGHT,GLFW_KEY_LEFT};
    int vals[] {1,-1,1,-1};
    int offset_vals[] {1,1,0,0};
    float move_amount = 0.001;
    for (int i = 0; i < 4; i++) {
        if (glfwGetKey(window,dir[i])==GLFW_PRESS) {
            offset_vec[offset_vals[i]]+=vals[i]*move_amount;
        }
    }
}
