#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Instantiate a Camera object at position (0, 3, 12)
Camera camera(glm::vec3(0.0f, 3.0f, 12.0f));

// Camera movement variables
float lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        firstMouse = true;
        return;
    }
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;  // reversed y-coordinates

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float grassTopVertices[] ={

   -10.0f, 0.0f, -10.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
    10.0f, 0.0f, -10.0f,    0.0f, 1.0f, 0.0f,    10.0f, 0.0f,
    10.0f, 0.0f,  10.0f,    0.0f, 1.0f, 0.0f,    10.0f, 10.0f,

    10.0f, 0.0f,  10.0f,    0.0f, 1.0f, 0.0f,    10.0f, 10.0f,
   -10.0f, 0.0f,  10.0f,    0.0f, 1.0f, 0.0f,    0.0f, 10.0f,
   -10.0f, 0.0f, -10.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
};


float groundVertices[] = {

    // Bottom Face (y = -1.0f) 
   -10.0f, -1.0f, -10.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
    10.0f, -1.0f, -10.0f,   0.0f, -1.0f, 0.0f,   10.0f, 0.0f,
    10.0f, -1.0f,  10.0f,   0.0f, -1.0f, 0.0f,   10.0f, 10.0f,

    10.0f, -1.0f,  10.0f,   0.0f, -1.0f, 0.0f,   10.0f, 10.0f,
   -10.0f, -1.0f,  10.0f,   0.0f, -1.0f, 0.0f,   0.0f, 10.0f,
   -10.0f, -1.0f, -10.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,

    //Front Face (z = 10.0f)
   -10.0f, -1.0f, 10.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
    10.0f, -1.0f, 10.0f,    0.0f, 0.0f, 1.0f,    10.0f, 0.0f,
    10.0f,  0.0f, 10.0f,    0.0f, 0.0f, 1.0f,    10.0f, 1.0f,

    10.0f,  0.0f, 10.0f,    0.0f, 0.0f, 1.0f,    10.0f, 1.0f,
   -10.0f,  0.0f, 10.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
   -10.0f, -1.0f, 10.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

    // Back Face (z = -10.0f) 
   -10.0f, -1.0f, -10.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
    10.0f, -1.0f, -10.0f,   0.0f, 0.0f, -1.0f,   10.0f, 0.0f,
    10.0f,  0.0f, -10.0f,   0.0f, 0.0f, -1.0f,   10.0f, 1.0f,

    10.0f,  0.0f, -10.0f,   0.0f, 0.0f, -1.0f,   10.0f, 1.0f,
   -10.0f,  0.0f, -10.0f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
   -10.0f, -1.0f, -10.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,

    //Left Face (x = -10.0f) 
   -10.0f, -1.0f, -10.0f,  -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
   -10.0f, -1.0f,  10.0f,  -1.0f, 0.0f, 0.0f,    10.0f, 0.0f,
   -10.0f,  0.0f,  10.0f,  -1.0f, 0.0f, 0.0f,    10.0f, 1.0f,

   -10.0f,  0.0f,  10.0f,  -1.0f, 0.0f, 0.0f,    10.0f, 1.0f,
   -10.0f,  0.0f, -10.0f,  -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
   -10.0f, -1.0f, -10.0f,  -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

    //Right Face (x = 10.0f)
    10.0f, -1.0f, -10.0f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
    10.0f, -1.0f,  10.0f,   1.0f, 0.0f, 0.0f,    10.0f, 0.0f,
    10.0f,  0.0f,  10.0f,   1.0f, 0.0f, 0.0f,    10.0f, 1.0f,

    10.0f,  0.0f,  10.0f,   1.0f, 0.0f, 0.0f,    10.0f, 1.0f,
    10.0f,  0.0f, -10.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
    10.0f, -1.0f, -10.0f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f
};

float pathVertices[] = {
    -0.7f, 0.01f, -10.0f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
     0.7f, 0.01f, -10.0f,   0.0f, 1.0f, 0.0f,    6.0f, 0.0f,
     0.7f, 0.01f,  10.0f,   0.0f, 1.0f, 0.0f,    6.0f, 40.0f,
     0.7f, 0.01f,  10.0f,   0.0f, 1.0f, 0.0f,    6.0f, 40.0f,
    -0.7f, 0.01f,  10.0f,   0.0f, 1.0f, 0.0f,    0.0f, 40.0f,
    -0.7f, 0.01f, -10.0f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f
};

float trunkVertices[] = {
    // positions            // tex coords
    // Front face
    -0.15f, 0.0f, 0.15f,    0.0f, 0.0f,
     0.15f, 0.0f, 0.15f,    6.0f, 0.0f,
     0.15f, 2.0f, 0.15f,    6.0f, 8.0f,
     0.15f, 2.0f, 0.15f,    6.0f, 8.0f,
    -0.15f, 2.0f, 0.15f,    0.0f, 8.0f,
    -0.15f, 0.0f, 0.15f,    0.0f, 0.0f,

    // Back face
    -0.15f, 0.0f, -0.15f,   0.0f, 0.0f,
    -0.15f, 2.0f, -0.15f,   0.0f, 8.0f,
     0.15f, 2.0f, -0.15f,   6.0f, 8.0f,
     0.15f, 2.0f, -0.15f,   6.0f, 8.0f,
     0.15f, 0.0f, -0.15f,   6.0f, 0.0f,
    -0.15f, 0.0f, -0.15f,   0.0f, 0.0f,

    // Left face
    -0.15f, 0.0f, -0.15f,   0.0f, 0.0f,
    -0.15f, 0.0f,  0.15f,   6.0f, 0.0f,
    -0.15f, 2.0f,  0.15f,   6.0f, 8.0f,
    -0.15f, 2.0f,  0.15f,   6.0f, 8.0f,
    -0.15f, 2.0f, -0.15f,   0.0f, 8.0f,
    -0.15f, 0.0f, -0.15f,   0.0f, 0.0f,

    // Right face
     0.15f, 0.0f, -0.15f,   0.0f, 0.0f,
     0.15f, 2.0f, -0.15f,   0.0f, 8.0f,
     0.15f, 2.0f,  0.15f,   6.0f, 8.0f,
     0.15f, 2.0f,  0.15f,   6.0f, 8.0f,
     0.15f, 0.0f,  0.15f,   6.0f, 0.0f,
     0.15f, 0.0f, -0.15f,   0.0f, 0.0f,

    // Top face
    -0.15f, 2.0f, -0.15f,   0.0f, 0.0f,
    -0.15f, 2.0f,  0.15f,   0.0f, 8.0f,
     0.15f, 2.0f,  0.15f,   6.0f, 8.0f,
     0.15f, 2.0f,  0.15f,   6.0f, 8.0f,
     0.15f, 2.0f, -0.15f,   6.0f, 0.0f,
    -0.15f, 2.0f, -0.15f,   0.0f, 0.0f,

    // Bottom face
    -0.15f, 0.0f, -0.15f,   0.0f, 0.0f,
     0.15f, 0.0f, -0.15f,   6.0f, 0.0f,
     0.15f, 0.0f,  0.15f,   6.0f, 8.0f,
     0.15f, 0.0f,  0.15f,   6.0f, 8.0f,
    -0.15f, 0.0f,  0.15f,   0.0f, 8.0f,
    -0.15f, 0.0f, -0.15f,   0.0f, 0.0f
};

float leavesVertices[] = {
    // positions            // tex coords
    // Front face
    -0.8f, 1.5f, 0.8f,     0.0f, 0.0f,
     0.8f, 1.5f, 0.8f,     1.0f, 0.0f,
     0.8f, 3.5f, 0.8f,     1.0f, 1.0f,
     0.8f, 3.5f, 0.8f,     1.0f, 1.0f,
    -0.8f, 3.5f, 0.8f,     0.0f, 1.0f,
    -0.8f, 1.5f, 0.8f,     0.0f, 0.0f,

    // Back face
    -0.8f, 1.5f, -0.8f,    0.0f, 0.0f,
    -0.8f, 3.5f, -0.8f,    0.0f, 1.0f,
     0.8f, 3.5f, -0.8f,    1.0f, 1.0f,
     0.8f, 3.5f, -0.8f,    1.0f, 1.0f,
     0.8f, 1.5f, -0.8f,    1.0f, 0.0f,
    -0.8f, 1.5f, -0.8f,    0.0f, 0.0f,

    // Left face
    -0.8f, 1.5f, -0.8f,    0.0f, 0.0f,
    -0.8f, 1.5f,  0.8f,    1.0f, 0.0f,
    -0.8f, 3.5f,  0.8f,    1.0f, 1.0f,
    -0.8f, 3.5f,  0.8f,    1.0f, 1.0f,
    -0.8f, 3.5f, -0.8f,    0.0f, 1.0f,
    -0.8f, 1.5f, -0.8f,    0.0f, 0.0f,

    // Right face
     0.8f, 1.5f, -0.8f,    0.0f, 0.0f,
     0.8f, 3.5f, -0.8f,    0.0f, 1.0f,
     0.8f, 3.5f,  0.8f,    1.0f, 1.0f,
     0.8f, 3.5f,  0.8f,    1.0f, 1.0f,
     0.8f, 1.5f,  0.8f,    1.0f, 0.0f,
     0.8f, 1.5f, -0.8f,    0.0f, 0.0f,

    // Top face
    -0.8f, 3.5f, -0.8f,    0.0f, 0.0f,
    -0.8f, 3.5f,  0.8f,    0.0f, 1.0f,
     0.8f, 3.5f,  0.8f,    1.0f, 1.0f,
     0.8f, 3.5f,  0.8f,    1.0f, 1.0f,
     0.8f, 3.5f, -0.8f,    1.0f, 0.0f,
    -0.8f, 3.5f, -0.8f,    0.0f, 0.0f,

    // Bottom face
    -0.8f, 1.5f, -0.8f,    0.0f, 0.0f,
     0.8f, 1.5f, -0.8f,    1.0f, 0.0f,
     0.8f, 1.5f,  0.8f,    1.0f, 1.0f,
     0.8f, 1.5f,  0.8f,    1.0f, 1.0f,
    -0.8f, 1.5f,  0.8f,    0.0f, 1.0f,
    -0.8f, 1.5f, -0.8f,    0.0f, 0.0f
};


unsigned int groundVAO, groundVBO, pathVAO, pathVBO, trunkVAO, trunkVBO, leavesVAO, leavesVBO, grassVAO, grassVBO;

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Project", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    

    // ground VAO
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);
    // position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    //grass VAO
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassTopVertices), grassTopVertices, GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    // Path VAO
    glGenVertexArrays(1, &pathVAO);
    glGenBuffers(1, &pathVBO);
    glBindVertexArray(pathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pathVertices), pathVertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    // tree VAO
    glGenVertexArrays(1, &trunkVAO);
    glGenBuffers(1, &trunkVBO);
    glBindVertexArray(trunkVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trunkVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(trunkVertices), trunkVertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    // Leaves VAO
    glGenVertexArrays(1, &leavesVAO);
    glGenBuffers(1, &leavesVBO);
    glBindVertexArray(leavesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, leavesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leavesVertices), leavesVertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    // Shader sources
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 2) in vec2 aTexCoord;
        out vec2 TexCoord;
        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D texture1;
        void main() {
            FragColor = texture(texture1, TexCoord);
        }
    )";

    // Compile shaders and link program
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // Load grass texture
    int texWidth, texHeight, nrChannels;
    unsigned char* data = stbi_load("resources/grass.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load grass texture." << std::endl;
        return -1;
    }

    GLuint grassTexture;
    glGenTextures(1, &grassTexture);
    glBindTexture(GL_TEXTURE_2D, grassTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 1) ? GL_RED : (nrChannels == 3) ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    // Load dirt texture
    int dirtWidth, dirtHeight, dirtChannels;
    unsigned char* dirtData = stbi_load("resources/dirt.jpg", &dirtWidth, &dirtHeight, &dirtChannels, 0);
    if (!dirtData) {
        std::cerr << "Failed to load dirt texture.\n";
        return -1;
    }

    GLuint dirtTexture;
    glGenTextures(1, &dirtTexture);
    glBindTexture(GL_TEXTURE_2D, dirtTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum dirtFormat = (dirtChannels == 1) ? GL_RED : (dirtChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, dirtFormat, dirtWidth, dirtHeight, 0, dirtFormat, GL_UNSIGNED_BYTE, dirtData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(dirtData);


    //Load path teaxture
    int stoneWidth, stoneHeight, stoneChannels;
    unsigned char* stoneData = stbi_load("resources/stone.jpg", &stoneWidth, &stoneHeight, &stoneChannels, 0);
    if (!stoneData) {
        std::cerr << "Failed to load stone texture." << std::endl;
        return -1;
    }

    GLuint stoneTexture;
    glGenTextures(1, &stoneTexture);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum stoneFormat = (stoneChannels == 1) ? GL_RED : (stoneChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, stoneFormat, stoneWidth, stoneHeight, 0, stoneFormat, GL_UNSIGNED_BYTE, stoneData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(stoneData);


    //Load Bark texture
    int barkWidth, barkHeight, barkChannels;
    unsigned char* barkData = stbi_load("resources/bark.jpg", &barkWidth, &barkHeight, &barkChannels, 0);
    if (!barkData) { std::cerr << "Failed to load bark texture." << std::endl; return -1; }
    GLuint barkTexture;
    glGenTextures(1, &barkTexture);
    glBindTexture(GL_TEXTURE_2D, barkTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum barkFormat = (barkChannels == 1) ? GL_RED : (barkChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, barkFormat, barkWidth, barkHeight, 0, barkFormat, GL_UNSIGNED_BYTE, barkData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(barkData);


    //Load leave textures
    int leavesWidth, leavesHeight, leavesChannels;
    unsigned char* leavesData = stbi_load("resources/leaves.jpg", &leavesWidth, &leavesHeight, &leavesChannels, 0);
    if (!leavesData) {
        std::cerr << "Failed to load leaves texture. Using grass texture instead." << std::endl;
        // If you don't have a leaves texture, we'll use the grass texture
        leavesData = stbi_load("resources/grass.jpg", &leavesWidth, &leavesHeight, &leavesChannels, 0);
    }

    GLuint leavesTexture;
    glGenTextures(1, &leavesTexture);
    glBindTexture(GL_TEXTURE_2D, leavesTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum leavesFormat = (leavesChannels == 1) ? GL_RED : (leavesChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, leavesFormat, leavesWidth, leavesHeight, 0, leavesFormat, GL_UNSIGNED_BYTE, leavesData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(leavesData);


    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.5f, 0.75f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Set projection and view matrices once
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);


        //Draw grass
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glBindVertexArray(grassVAO);
        // Set model matrix for ground (identity matrix - no transformation)
        glm::mat4 groundModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &groundModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //draw dirt
        glBindTexture(GL_TEXTURE_2D, dirtTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glBindVertexArray(groundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
        glBindVertexArray(0);

        // Draw stone path
        glBindTexture(GL_TEXTURE_2D, stoneTexture);
        glBindVertexArray(pathVAO);
        // Set model matrix for path (identity matrix - no transformation)
        glm::mat4 pathModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &pathModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // Draw tree trunks
        glBindTexture(GL_TEXTURE_2D, barkTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glBindVertexArray(trunkVAO);

        // Tree positions (4 corners)
        glm::vec3 trunkPositions[4] = {
            glm::vec3(-6.0f, 0.0f, -6.0f), // bottom-left
            glm::vec3( 6.0f, 0.0f, -6.0f), // bottom-right
            glm::vec3(-6.0f, 0.0f,  6.0f), // top-left
            glm::vec3( 6.0f, 0.0f,  6.0f)  // top-right
        };

        // Draw each tree trunk
        for (int i = 0; i < 4; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), trunkPositions[i]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Draw tree leaves
        glBindTexture(GL_TEXTURE_2D, leavesTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glBindVertexArray(leavesVAO);

        // Draw leaves for each tree (same positions as trunks)
        for (int i = 0; i < 4; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), trunkPositions[i]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
