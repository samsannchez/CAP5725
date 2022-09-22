#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI glm::pi <GLfloat> ()
#define stacks 30
#define slices 30
#define radius 2

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void getsphereCoords();

int numberOfPoints = 0;
int numberOfIndexes = 0;
float vertices[(stacks + 1) * slices * 3];
int indices[stacks * slices * 10];

int dx = 0, dy = 0, dz = 0;

const char* vertexShaderSource =
"#version 330 core\n"

"layout (location = 0) in vec3 position;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"out vec3 fragNormal;\n"

"void main()\n"
"{\n"
"    gl_Position = projection * view * vec4(position, 1.0f);\n"
"    fragNormal = vec3(1.0f,1.0f,1.0f);\n"
"}\n\0";

const char* fragmentShaderSource =
"#version 330 core\n"

"in vec3 fragNormal;\n"

"out vec4 Fragcolor;\n"

"void main()\n"
"{\n"
"    Fragcolor = vec4(fragNormal,1.0f);\n"
"}\n\0";




// Window dimensions
const int WIDTH = 800, HEIGHT = 600;

// The MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //uncomment if you are using macos
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Samantha Sanchez Juarez", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glewExperimental = GL_TRUE;
    
// Initialize GLEW to setup the OpenGL Function pointers
if (glewInit()!= GLEW_OK){
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    /////////////////SHADERS/////////////////////////////////
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    // Link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    /////////////////SHADERS/////////////////////////////////
    
    getsphereCoords();

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);


    while (!glfwWindowShouldClose(window)){

        //Call function to exit when esc key is pressed
        processInput(window);

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // Create transformations
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 model;
        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f),  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 1.0f, 100.0f);
        model = glm::rotate(model, (float)glfwGetTime() * 50.0f, glm::vec3(1.0f, 0.3f, 0.5f));
        // Get their uniform location
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        unsigned int projLoc = glGetUniformLocation(shaderProgram,"projection" );
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Pass the matrices to the shader
                                                    
        glDrawElements(GL_TRIANGLES, numberOfIndexes, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        // Now with the uniform matrix being replaced with new transformations, draw it again.
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
                                                    
        // Check if any events have been triggered
        glfwPollEvents();

    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}
                                                    
//Function that will exit out of the window when esc key is pressed
void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
        }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        dy--;
        }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        dy++;
        }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        dx++;
        }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        dx--;
        }

}
                                    
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
glViewport(0, 0, width, height);
}

                                                    
void getsphereCoords(){
    // Calc The Vertices
    for (int i = 0; i <= slices; i++)
    {
        float phi = i * (glm::pi<float>() / slices) ;


        // Loop Through Slices
        for (int j = 0; j < stacks; j++)
        {
            float theta = j * (glm::pi <float>() * 2 / stacks);

            // Calc The Vertex Positions
            float x = cosf(theta) * sinf(phi);
            float y = cosf(phi);
            float z = sinf(theta) * sinf(phi);

            // Push Back Vertex Data
            vertices[numberOfPoints++] = x * radius;
            vertices[numberOfPoints++] = y * radius;
            vertices[numberOfPoints++] = z * radius;
        }
    }
    // Calc The Index Positions
    for (int i = 0; i < numberOfPoints / 3 - stacks; i++)
    {
        if ((i + 1) % stacks == 0)
        {
            indices[numberOfIndexes++] = i;
            indices[numberOfIndexes++] = i - stacks + 1;
            indices[numberOfIndexes++] = i + stacks;

            indices[numberOfIndexes++] = i - stacks + 1;
            indices[numberOfIndexes++] = i + stacks;
            if (i + 1 == numberOfPoints / 3)
                indices[numberOfIndexes++] = numberOfPoints - stacks;
            else
                indices[numberOfIndexes++] = i + 1;
        }
        else
        {
            indices[numberOfIndexes++] = i;
            indices[numberOfIndexes++] = i + 1;
            indices[numberOfIndexes++] = i + stacks;

            indices[numberOfIndexes++] = i + 1;
            indices[numberOfIndexes++] = i + stacks;
            indices[numberOfIndexes++] = i + stacks + 1;
        }
    }
}
