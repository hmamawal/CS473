#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream> 
#include <vector>
#include <string>

// load the map layout from a text file 
std::vector<std::string> LoadMapLayout(std::string file_name) {
    std::vector<std::string> map_layout;
    std::ifstream file(file_name);
    std::string line;
    while (std::getline(file,line)) {
        map_layout.push_back(line);
    }
    return map_layout;
}



//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Global Variables
glm::vec4 clear_color(1.0, 1.0, 1.0, 1.0);
glm::vec4 outline_color = glm::vec4(0.0,0.0,0.0,1.0);

bool clear_key_pressed[] {false,false,false};

float angle_z = 0.0;
float angle_x = 0.0;
float last_frame = 0.0;
float delta_time = 0.0;

// camera global variable
//Camera camera(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 1.0, 0.0), -90.0, 0.0);
Camera camera(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), -90.0, 0.0);


int main () {
    GLFWwindow *window = InitializeEnvironment("ICE 9.2",800,800);
    if (window == NULL) {
        return -1;
    }
    
    //create shader program
    Shader shader("./Shaders/vertex.glsl","./Shaders/fragment.glsl");
    
    //get a texture from memory
    unsigned int wood_texture = GetTexture("./images/wood.jpg");


    //1. define your Vertex Array Object, used to hold attribute pointers
    VAOStruct texture_vao;
    glGenVertexArrays(1,&(texture_vao.id));
    AttributePointer position_attr = BuildAttribute(3,GL_FLOAT,false,5*sizeof(float),0);
    AttributePointer texture_attr = BuildAttribute(2,GL_FLOAT,false,5*sizeof(float),3*sizeof(float));
    texture_vao.attributes.push_back(position_attr);
    texture_vao.attributes.push_back(texture_attr);

    // define cell size for the map grid
    float cell_size = 0.5f;

    // load the layout from level.txt; note that the walls/trees are marked with '#' in the text file
    std::vector<std::string> map_layout = LoadMapLayout("level.txt");
    std::cout << "Loaded " << map_layout.size() << " rows." << std::endl;
    if (!map_layout.empty())
        std::cout << "First row: " << map_layout[0] << std::endl;


    // for a 2d view, we'll use an orthographic projection
    // Compute map dimensions based on level layout and cell size
    float mapWidth  = static_cast<float>(map_layout[0].size()) * cell_size;
    float mapHeight = static_cast<float>(map_layout.size()) * cell_size;

    // Use an orthographic projection covering the entire level
    glm::mat4 projection = glm::ortho(0.0f, mapWidth, 0.0f, mapHeight, -1.0f, 1.0f);
    //glm::mat4 projection = glm::ortho(0.0f, 800.0f/100.0f, 0.0f, 800.0f/100.0f, -1.0f, 1.0f);
    glm::mat4 identity = glm::mat4(1.0f);

    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {

        // calculate the delta time
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;


        //input
        processInput(window);
        
        shader.use();
        shader.setMat4("projection",projection);
        glm::mat4 view = camera.GetViewMatrix();
        //shader.setMat4("view", glm::mat4(1.0f)); // for debugging
        shader.setMat4("local",identity);
        shader.setMat4("view",view);


        //render commands here
        //set the clear color to wipe the window
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);


        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // iterate over the level layout and render a "tree" (wall) rectangle where a '#' is found
        for (size_t row = 0; row < map_layout.size(); row++) {
            for (size_t col = 0; col < map_layout[row].size(); col++) {
                if (map_layout[row][col] == '#') {
                    // Adjust the y position so that the first row of the file is at the bottom.
                    glm::vec3 pos(col * cell_size, map_layout.size() * cell_size - row * cell_size, 0.0f);
                    // Create a rectangle at position pos
                    BasicShape tree = GetTextureRectangle(texture_vao, pos, cell_size, cell_size);
                    //shader.setBool("is_textured", true);
                    shader.setBool("is_textured", false);
                    glBindTexture(GL_TEXTURE_2D, wood_texture);
                    tree.Draw();
                }
            }
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



