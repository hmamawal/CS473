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
    AttributePointer position_attr_2 = BuildAttribute(3,GL_FLOAT,false,5*sizeof(float),0);
    AttributePointer texture_attr = BuildAttribute(2,GL_FLOAT,false,5*sizeof(float),3*sizeof(float));

    texture_vao.attributes.push_back(position_attr_2);
    texture_vao.attributes.push_back(texture_attr);

    unsigned int space_texture = GetTexture("./Images/space.jpg");
    //unsigned int planet_texture = GetTexture("./Images/mars.jpg");

    BasicShape triangle = GetTriangle(position_vao,0.25,glm::vec3(0.0,0.15,0.0));
    BasicShape circle = GetCircle(position_vao,0.2,40,glm::vec3(0.5,0.5,0.0));
    BasicShape hull = GetHull(position_vao,glm::vec3(-0.13,-0.1,0.0),0.26,0.4);
    BasicShape left_canon = GetRectangle(position_vao,glm::vec3(-0.2,-0.1,0.0),0.1,0.2);
    BasicShape right_canon = GetRectangle(position_vao,glm::vec3(0.1,-0.1,0.0),0.1,0.2);

    BasicShape background = GetTexturedRectangle(texture_vao,glm::vec3(-1.0,-1.0,0.0),2.0,2.0);
    //Set up the EBO
    glBindVertexArray(position_vao.id);
    glBindBuffer(GL_ARRAY_BUFFER,rect_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(rect_vertices),rect_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


    shader.use();
    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        //input
        processInput(window);
        shader.setVec4("offset_vec",glm::vec4(0.0));
        //render commands here
        //set the clear color to wipe the window
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);


        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT);

        // draw the background
        glBindTexture(GL_TEXTURE_2D,space_texture);
        shader.setBool("is_textured",true);
        background.Draw();
        shader.setBool("is_textured",false);

        glm::vec4 outline_color = glm::vec4(1.0,1.0,1.0,1.0);
        //Draw the planet
        shader.setVec4("set_color",glm::vec4(0.3,0.7,0.3,1.0));
        circle.Draw();
        shader.setVec4("set_color",outline_color);
        circle.DrawEBO();

        shader.setVec4("offset_vec",offset_vec);
        //Draw the ship
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
        shader.setVec4("set_color",glm::vec4(0.5,0.5,0.5,1.0));
        hull.Draw();
        shader.setVec4("set_color",outline_color);
        hull.DrawEBO();
        
        
        //Draw EBO
        // shader.setVec4("set_color",glm::vec4(1.0,0.0,0.0,1.0));
        // BindVAO(position_vao,rect_VBO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        // glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

        // shader.setVec4("set_color",glm::vec4(1.0,1.0,1.0,1.0));
        // glLineWidth(3.0);
        // glDrawElements(GL_LINE_LOOP,6,GL_UNSIGNED_INT,0);
        // glLineWidth(1.0);
        

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
    if(glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_RELEASE){
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); 
    }
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
    //Arrow Key processing
    int dir[] {GLFW_KEY_UP,GLFW_KEY_DOWN,GLFW_KEY_RIGHT,GLFW_KEY_LEFT};
    int vals[] {1,-1,1,-1};
    int offset_vals[] {1,1,0,0};
    float move_amount = 0.01;
    for (int i = 0; i < 4; i++) {
        if (glfwGetKey(window,dir[i])==GLFW_PRESS) {
            offset_vec[offset_vals[i]]+=vals[i]*move_amount;
        }
    }
}



