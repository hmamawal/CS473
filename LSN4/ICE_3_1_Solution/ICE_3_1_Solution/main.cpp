
#include "environment_setup.hpp"
#include "vertex_attribute.hpp"
#include "basic_shape.hpp"
#include "shader.hpp"


//function declarations

// accepts a GLFW window, checks for input (keys and/or mouse) and returns nothing.
void processInput(GLFWwindow *window);

// accepts a glm::vec4 color and prints the color to the console.
void PrintColor (glm::vec4 color);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

glm::vec4 clear_color (0.0,0.0,0.0,1.0);
bool key_flags[3] = {false, false, false};
glm::vec4 offset (0.0, 0.0, 0.0, 1.0);

int main()
{

    GLFWwindow *window = InitializeEnvironment(SCR_WIDTH, SCR_HEIGHT, "Hello Triangle");
    Shader shader_program("./shaders/vertex.glsl", "./shaders/fragment.glsl");
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float left_eye_vertices[] = {
        // positions        //colors
        -0.7f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // left  
        -0.3f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // right 
         -0.55f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // top   
    }; 

    float right_eye_vertices[] = {
        // positions        //colors
         0.3f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // left  
         0.7f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,// right 
         0.55f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // top   
    }; 

    float smile_vertices[] = {
        // positions        //colors
        0.0f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom 
        0.8f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f, // right 
        -0.8f, -0.3f, 0.0f, 0.0f, 0.0f, 1.0f // left  
    }; 

    BasicShape left_eye, right_eye, smile;
    VAOStruct vao;
    glGenVertexArrays(1, &(vao.ID));
    vao.attributes.push_back(BuildAttribute(3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0)); // position attribute
    vao.attributes.push_back(BuildAttribute(3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 3 * sizeof(float))); // color attribute
    left_eye.Initialize(vao, left_eye_vertices, sizeof(left_eye_vertices), 3);
    right_eye.Initialize(vao, right_eye_vertices, sizeof(right_eye_vertices), 3);
    smile.Initialize(vao, smile_vertices, sizeof(smile_vertices), 3);

    shader_program.use();
    shader_program.setVec4("set_color", glm::vec4(1.0,0.0,0.0,1.0));


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        shader_program.setVec4("offset", offset);

        // render
        // ------
        glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT);

        shader_program.setBool("use_color", true);
        //shader_program.setVec4("set_color", glm::vec4(1.0,0.0,0.0,1.0));
        left_eye.Draw(shader_program.ID);
        shader_program.setBool("use_color", false);
        //shader_program.setVec4("set_color", glm::vec4(1.0,1.0,0.0,1.0));
        right_eye.Draw(shader_program.ID);
        smile.Draw(shader_program.ID);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    left_eye.DeallocateShape();
    right_eye.DeallocateShape();
    smile.DeallocateShape();
    glDeleteProgram(shader_program.ID);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) & (!key_flags[0])) {
        clear_color.r += 0.1;
        key_flags[0] = true;
        if (clear_color.r >= 1.1) {
            clear_color.r = 0.0;
        }
        PrintColor(clear_color);


    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
        key_flags[0] = false;
    }

    // do for glfw key up
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        offset.y += 0.001;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        offset.y -= 0.001;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        offset.x -= 0.001;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        offset.x += 0.001;
    }
}

void PrintColor (glm::vec4 color) {
    std::cout << "Color: (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
}