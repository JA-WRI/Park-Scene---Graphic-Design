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

   -7.0f, 0.0f, -7.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
    7.0f, 0.0f, -7.0f,    0.0f, 1.0f, 0.0f,    10.0f, 0.0f,
    7.0f, 0.0f,  7.0f,    0.0f, 1.0f, 0.0f,    10.0f, 10.0f,

    7.0f, 0.0f,  7.0f,    0.0f, 1.0f, 0.0f,    10.0f, 10.0f,
   -7.0f, 0.0f,  7.0f,    0.0f, 1.0f, 0.0f,    0.0f, 10.0f,
   -7.0f, 0.0f, -7.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
};


float groundVertices[] = {

    // Bottom Face (y = -1.0f) 
   -7.0f, -1.0f, -7.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
    7.0f, -1.0f, -7.0f,   0.0f, -1.0f, 0.0f,   10.0f, 0.0f,
    7.0f, -1.0f,  7.0f,   0.0f, -1.0f, 0.0f,   10.0f, 10.0f,

    7.0f, -1.0f,  7.0f,   0.0f, -1.0f, 0.0f,   10.0f, 10.0f,
   -7.0f, -1.0f,  7.0f,   0.0f, -1.0f, 0.0f,   0.0f, 10.0f,
   -7.0f, -1.0f, -7.0f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,

    //Front Face (z = 10.0f)
   -7.0f, -1.0f, 7.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
    7.0f, -1.0f, 7.0f,    0.0f, 0.0f, 1.0f,    10.0f, 0.0f,
    7.0f,  0.0f, 7.0f,    0.0f, 0.0f, 1.0f,    10.0f, 1.0f,

    7.0f,  0.0f, 7.0f,    0.0f, 0.0f, 1.0f,    10.0f, 1.0f,
   -7.0f,  0.0f, 7.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
   -7.0f, -1.0f, 7.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

    // Back Face (z = -10.0f) 
   -7.0f, -1.0f, -7.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
    7.0f, -1.0f, -7.0f,   0.0f, 0.0f, -1.0f,   10.0f, 0.0f,
    7.0f,  0.0f, -7.0f,   0.0f, 0.0f, -1.0f,   10.0f, 1.0f,

    7.0f,  0.0f, -7.0f,   0.0f, 0.0f, -1.0f,   10.0f, 1.0f,
   -7.0f,  0.0f, -7.0f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
   -7.0f, -1.0f, -7.0f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,

    //Left Face (x = -10.0f) 
   -7.0f, -1.0f, -7.0f,  -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
   -7.0f, -1.0f,  7.0f,  -1.0f, 0.0f, 0.0f,    10.0f, 0.0f,
   -7.0f,  0.0f,  7.0f,  -1.0f, 0.0f, 0.0f,    10.0f, 1.0f,

   -7.0f,  0.0f,  7.0f,  -1.0f, 0.0f, 0.0f,    10.0f, 1.0f,
   -7.0f,  0.0f, -7.0f,  -1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
   -7.0f, -1.0f, -7.0f,  -1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

    //Right Face (x = 10.0f)
    7.0f, -1.0f, -7.0f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
    7.0f, -1.0f,  7.0f,   1.0f, 0.0f, 0.0f,    10.0f, 0.0f,
    7.0f,  0.0f,  7.0f,   1.0f, 0.0f, 0.0f,    10.0f, 1.0f,

    7.0f,  0.0f,  7.0f,   1.0f, 0.0f, 0.0f,    10.0f, 1.0f,
    7.0f,  0.0f, -7.0f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
    7.0f, -1.0f, -7.0f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f
};

float pathVertices[] = {
    -0.7f, 0.01f, -7.0f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
     0.7f, 0.01f, -7.0f,   0.0f, 1.0f, 0.0f,    6.0f, 0.0f,
     0.7f, 0.01f,  7.0f,   0.0f, 1.0f, 0.0f,    6.0f, 40.0f,
     0.7f, 0.01f,  7.0f,   0.0f, 1.0f, 0.0f,    6.0f, 40.0f,
    -0.7f, 0.01f,  7.0f,   0.0f, 1.0f, 0.0f,    0.0f, 40.0f,
    -0.7f, 0.01f, -7.0f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f
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


float sandVertices[] = {
    // Original rectangle (middle)
    -6.5f, 0.05f, -5.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom-left
    -2.0f, 0.05f, -5.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom-right
    -2.0f, 0.05f,  0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top-right

    -2.0f, 0.05f,  0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top-right
    -6.5f, 0.05f,  0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // top-left
    -6.5f, 0.05f, -5.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom-left

    // New rectangle (above original, from 0.0f to +5.0f)
    -6.5f, 0.05f,  0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom-left
    -2.0f, 0.05f,  0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom-right
    -2.0f, 0.05f,  5.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top-right

    -2.0f, 0.05f,  5.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top-right
    -6.5f, 0.05f,  5.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // top-left
    -6.5f, 0.05f,  0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f  // bottom-left
};

float cubeVertices[] = {
    // positions          // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

float benchVertices[] = {
    // positions          // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

//imitialize all the elemtens VBO and VAO
unsigned int groundVAO, groundVBO, pathVAO, pathVBO, trunkVAO, trunkVBO, leavesVAO, leavesVBO, grassVAO, grassVBO;
unsigned int sandVAO, sandVBO,towerVAO, towerVBO,bladeVAO, bladeVBO, benchVAO, benchVBO;

float bladeRotation = 0.0f;



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


    // path VAO
    glGenVertexArrays(1, &pathVAO);
    glGenBuffers(1, &pathVBO);
    glBindVertexArray(pathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pathVertices), pathVertices, GL_STATIC_DRAW);
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


    // leaves VAO
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

    // sand VAO
    glGenVertexArrays(1, &sandVAO);
    glGenBuffers(1, &sandVBO);
    glBindVertexArray(sandVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sandVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sandVertices), sandVertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    //bench VAO
    unsigned int benchVAO, benchVBO;
    glGenVertexArrays(1, &benchVAO);
    glGenBuffers(1, &benchVBO);

    glBindVertexArray(benchVAO);
    glBindBuffer(GL_ARRAY_BUFFER, benchVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(benchVertices), benchVertices, GL_STATIC_DRAW);
    // position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);


    // windmill Tower
    glGenVertexArrays(1, &towerVAO);
    glGenBuffers(1, &towerVBO);
    glBindVertexArray(towerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, towerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0); // Unbind


    //windmill Blades
    glGenVertexArrays(1, &bladeVAO);
    glGenBuffers(1, &bladeVBO);
    glBindVertexArray(bladeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, bladeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
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


    // load grass texture
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

    // load wood texture
    int woodWidth, woodHeight, woodChannels;
    unsigned char* wooddata = stbi_load("resources/wood.jpg", &woodWidth, &woodHeight, &woodChannels, 0);
    if (!wooddata) {
        std::cerr << "Failed to load wood texture." << std::endl;
        return -1;
    }

    GLuint woodTexture;
    glGenTextures(1, &woodTexture);
    glBindTexture(GL_TEXTURE_2D, woodTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum woodformat = (woodChannels == 1) ? GL_RED : (woodChannels == 3) ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, woodformat, woodWidth, woodHeight, 0, woodformat, GL_UNSIGNED_BYTE, wooddata);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(wooddata);

    // load steel texture
    int SteelWidth, SteelHeight, SteelChannels;
    unsigned char* steelData = stbi_load("resources/steel.jpg", &SteelWidth, &SteelHeight, &SteelChannels, 0);
    if (!steelData) {
        std::cerr << "Failed to load steel texture." << std::endl;
        return -1;
    }

    GLuint steelTexture;
    glGenTextures(1, &steelTexture);
    glBindTexture(GL_TEXTURE_2D, steelTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum steelFormat = (SteelChannels == 1) ? GL_RED : (SteelChannels == 3) ? GL_RGB : GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, steelFormat, SteelWidth, SteelHeight, 0, steelFormat, GL_UNSIGNED_BYTE, steelData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(steelData);

    
    // load dirt texture
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


    // load path teaxture
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


    // load Bark texture
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


    // load leave texture
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


    // load sand texture
    int sandWidth, sandHeight, sandChannels;
    unsigned char* sandData = stbi_load("resources/sand2.jpg", &sandWidth, &sandHeight, &sandChannels, 0);
    if (!sandData) {
        std::cerr << "Failed to load sand texture. Using dirt texture instead." << std::endl;
        // Fallback to dirt texture if sand texture doesn't exist
        sandData = stbi_load("resources/dirt.jpg", &sandWidth, &sandHeight, &sandChannels, 0);
    }

    GLuint sandTexture;
    glGenTextures(1, &sandTexture);
    glBindTexture(GL_TEXTURE_2D, sandTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum sandFormat = (sandChannels == 1) ? GL_RED : (sandChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, sandFormat, sandWidth, sandHeight, 0, sandFormat, GL_UNSIGNED_BYTE, sandData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(sandData);

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


        // draw grass
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

        // draw stone path
        glBindTexture(GL_TEXTURE_2D, stoneTexture);
        glBindVertexArray(pathVAO);
        // Set model matrix for path (identity matrix - no transformation)
        glm::mat4 pathModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &pathModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // draw tree trunks
        glBindTexture(GL_TEXTURE_2D, barkTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glBindVertexArray(trunkVAO);

        // tree positions (4 corners)
        glm::vec3 trunkPositions[4] = {
            glm::vec3(-6.0f, 0.0f, -6.0f), // bottom-left
            glm::vec3( 6.0f, 0.0f, -6.0f), // bottom-right
            glm::vec3(-6.0f, 0.0f,  6.0f), // top-left
            glm::vec3( 6.0f, 0.0f,  6.0f)  // top-right
        };

        // draw each tree trunk
        for (int i = 0; i < 4; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), trunkPositions[i]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // draw tree leaves
        glBindTexture(GL_TEXTURE_2D, leavesTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glBindVertexArray(leavesVAO);

        // draw leaves for each tree (same positions as trunks)
        for (int i = 0; i < 4; ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), trunkPositions[i]);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // draw sand patch
        glBindTexture(GL_TEXTURE_2D, sandTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glBindVertexArray(sandVAO);
        glm::mat4 sandModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &sandModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        // update blade rotation
        bladeRotation += deltaTime * 50.0f;  // 50 degrees/second
        if (bladeRotation > 360.0f)
            bladeRotation -= 360.0f;
        // Bind steel texture
        glBindTexture(GL_TEXTURE_2D, steelTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);


        // Draw widnmill base
        glBindVertexArray(towerVAO);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, -4.0f));
        model = glm::scale(model, glm::vec3(1.0f, 5.0f, 0.0f)); // Tall tower
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw Blades (Hierarchical)
        glBindVertexArray(bladeVAO);
        glm::mat4 bladeBase = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 4.5f, -4.0f)); // Top of tower
        bladeBase = glm::rotate(bladeBase, glm::radians(bladeRotation), glm::vec3(0.0f, 0.0f, 1.0f));

        for (int i = 0; i < 4; ++i) {
            glm::mat4 bladeModel = glm::rotate(bladeBase, glm::radians(90.0f * i), glm::vec3(0.0f, 0.0f, 1.0f));
            bladeModel = glm::translate(bladeModel, glm::vec3(1.0f, 0.0f, 0.0f));
            bladeModel = glm::scale(bladeModel, glm::vec3(2.0f, 0.2f, 0.1f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &bladeModel[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
         
        // Draw Bench
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        // Bench position and rotation
        glm::vec3 benchPos = glm::vec3(0.0f, 0.0f, 0.0f);
        float benchRotation = 90.0f;

        // Bench Base Transform
        glBindVertexArray(benchVAO);
        glm::mat4 benchBase = glm::mat4(1.0f);
        benchBase = glm::translate(benchBase, benchPos + glm::vec3(1.0f, 0.6f,-2.0f));
        benchBase = glm::rotate(benchBase, glm::radians(benchRotation), glm::vec3(0.0f, 1.0f, 0.0f));
        benchBase = glm::rotate(benchBase, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Flip upright

        // Draw Bench Seat
        glm::mat4 benchModel = glm::scale(benchBase, glm::vec3(2.0f, 0.2f, 0.6f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &benchModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw Bench Legs
        glm::vec3 legScale = glm::vec3(0.1f, 0.6f, 0.1f);
        float sx = 2.0f * 0.5f - 0.1f;
        float sz = 0.6f * 0.5f - 0.1f;
        glm::vec3 legOffsets[4] = {
            { -sx, 0.0f, -sz },
            {  sx, 0.0f, -sz },
            { -sx, 0.0f,  sz },
            {  sx, 0.0f,  sz }
        };

        for (int i = 0; i < 4; ++i) {
            glm::mat4 legModel = glm::translate(benchBase, legOffsets[i] + glm::vec3(0.0f, 0.3f, 0.0f));
            legModel = glm::scale(legModel, legScale);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &legModel[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Delete all Vertex Array Objects
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteVertexArrays(1, &grassVAO);
    glDeleteVertexArrays(1, &pathVAO);
    glDeleteVertexArrays(1, &trunkVAO);
    glDeleteVertexArrays(1, &leavesVAO);
    glDeleteVertexArrays(1, &sandVAO);
    glDeleteVertexArrays(1, &towerVAO);
    glDeleteVertexArrays(1, &bladeVAO);
    glDeleteVertexArrays(1, &benchVAO);
    
    // Delete all Vertex Buffer Objects
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &grassVBO);
    glDeleteBuffers(1, &pathVBO);
    glDeleteBuffers(1, &trunkVBO);
    glDeleteBuffers(1, &leavesVBO);
    glDeleteBuffers(1, &sandVBO);
    glDeleteBuffers(1, &towerVBO);
    glDeleteBuffers(1, &bladeVBO);
    glDeleteBuffers(1, &benchVBO);
    
    // Delete all Textures
    glDeleteTextures(1, &grassTexture);
    glDeleteTextures(1, &woodTexture);
    glDeleteTextures(1, &steelTexture);
    glDeleteTextures(1, &dirtTexture);
    glDeleteTextures(1, &stoneTexture);
    glDeleteTextures(1, &barkTexture);
    glDeleteTextures(1, &leavesTexture);
    glDeleteTextures(1, &sandTexture);
    
    // Delete Shader Program
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}