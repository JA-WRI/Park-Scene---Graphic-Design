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

float bladeRotation = 0.0f;

// Interactive elements state variables
bool windmillRunning = true;
float windmillSpeed = 50.0f;
bool dayTime = true;
float timeOfDay = 0.5f; // 0.0 = night, 1.0 = day
bool flashlightOn = true;
float treeScale = 1.0f;
bool treeGrowing = false;
float benchRotationY = 0.0f;
bool lightShow = false;
float lightShowTimer = 0.0f;

// Key press states to prevent multiple triggers
bool spacePressed = false;
bool tPressed = false;
bool fPressed = false;
bool gPressed = false;
bool bPressed = false;
bool lPressed = false;


// Add these after your existing global variables (around line 20)
struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    glm::vec3 direction; // For movement
    float speed;
};

Light lights[4];

float globalLightIntensity = 1.0f;  // Multiply all light intensities by this
float ambientStrength = 0.4f;       // Base ambient lighting strength

// Shadow mapping constants
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
unsigned int depthMapFBO[4];  // One framebuffer per light
unsigned int depthMap[4];     // One depth texture per light


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
    // Camera movement
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
    
    // Interactive controls
    // SPACE - Toggle windmill on/off
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
        windmillRunning = !windmillRunning;
        spacePressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        spacePressed = false;
    }
    
    // T - Toggle day/night
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !tPressed) {
        dayTime = !dayTime;
        tPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
        tPressed = false;
    }
    
    // F - Toggle flashlight
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fPressed) {
        flashlightOn = !flashlightOn;
        fPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
        fPressed = false;
    }
    
    // G - Toggle tree growing/shrinking
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gPressed) {
        treeGrowing = !treeGrowing;
        gPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
        gPressed = false;
    }
    
    // B - Rotate bench
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bPressed) {
        benchRotationY += 45.0f;
        if (benchRotationY >= 360.0f) benchRotationY = 0.0f;
        bPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
        bPressed = false;
    }
    
    // L - Toggle light show
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lPressed) {
        lightShow = !lightShow;
        lPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
        lPressed = false;
    }
    
    // Arrow keys for windmill speed control
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        windmillSpeed = std::min(windmillSpeed + 20.0f * deltaTime, 200.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        windmillSpeed = std::max(windmillSpeed - 20.0f * deltaTime, 0.0f);
    }
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
    // positions            // normals           // tex coords
    // Front face
    -0.15f, 0.0f, 0.15f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.15f, 0.0f, 0.15f,    0.0f, 0.0f, 1.0f,   6.0f, 0.0f,
     0.15f, 2.0f, 0.15f,    0.0f, 0.0f, 1.0f,   6.0f, 8.0f,
     0.15f, 2.0f, 0.15f,    0.0f, 0.0f, 1.0f,   6.0f, 8.0f,
    -0.15f, 2.0f, 0.15f,    0.0f, 0.0f, 1.0f,   0.0f, 8.0f,
    -0.15f, 0.0f, 0.15f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

    // Back face
    -0.15f, 0.0f, -0.15f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
    -0.15f, 2.0f, -0.15f,   0.0f, 0.0f, -1.0f,  0.0f, 8.0f,
     0.15f, 2.0f, -0.15f,   0.0f, 0.0f, -1.0f,  6.0f, 8.0f,
     0.15f, 2.0f, -0.15f,   0.0f, 0.0f, -1.0f,  6.0f, 8.0f,
     0.15f, 0.0f, -0.15f,   0.0f, 0.0f, -1.0f,  6.0f, 0.0f,
    -0.15f, 0.0f, -0.15f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

    // Left face
    -0.15f, 0.0f, -0.15f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    -0.15f, 0.0f,  0.15f,   -1.0f, 0.0f, 0.0f,  6.0f, 0.0f,
    -0.15f, 2.0f,  0.15f,   -1.0f, 0.0f, 0.0f,  6.0f, 8.0f,
    -0.15f, 2.0f,  0.15f,   -1.0f, 0.0f, 0.0f,  6.0f, 8.0f,
    -0.15f, 2.0f, -0.15f,   -1.0f, 0.0f, 0.0f,  0.0f, 8.0f,
    -0.15f, 0.0f, -0.15f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,

    // Right face
     0.15f, 0.0f, -0.15f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.15f, 2.0f, -0.15f,   1.0f, 0.0f, 0.0f,   0.0f, 8.0f,
     0.15f, 2.0f,  0.15f,   1.0f, 0.0f, 0.0f,   6.0f, 8.0f,
     0.15f, 2.0f,  0.15f,   1.0f, 0.0f, 0.0f,   6.0f, 8.0f,
     0.15f, 0.0f,  0.15f,   1.0f, 0.0f, 0.0f,   6.0f, 0.0f,
     0.15f, 0.0f, -0.15f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,

    // Top face
    -0.15f, 2.0f, -0.15f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
    -0.15f, 2.0f,  0.15f,   0.0f, 1.0f, 0.0f,   0.0f, 8.0f,
     0.15f, 2.0f,  0.15f,   0.0f, 1.0f, 0.0f,   6.0f, 8.0f,
     0.15f, 2.0f,  0.15f,   0.0f, 1.0f, 0.0f,   6.0f, 8.0f,
     0.15f, 2.0f, -0.15f,   0.0f, 1.0f, 0.0f,   6.0f, 0.0f,
    -0.15f, 2.0f, -0.15f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,

    // Bottom face
    -0.15f, 0.0f, -0.15f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
     0.15f, 0.0f, -0.15f,   0.0f, -1.0f, 0.0f,  6.0f, 0.0f,
     0.15f, 0.0f,  0.15f,   0.0f, -1.0f, 0.0f,  6.0f, 8.0f,
     0.15f, 0.0f,  0.15f,   0.0f, -1.0f, 0.0f,  6.0f, 8.0f,
    -0.15f, 0.0f,  0.15f,   0.0f, -1.0f, 0.0f,  0.0f, 8.0f,
    -0.15f, 0.0f, -0.15f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f
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


float leavesVertices[] = {
    // positions            // normals           // tex coords
    // Front face
    -0.8f, 1.5f, 0.8f,     0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
     0.8f, 1.5f, 0.8f,     0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
     0.8f, 3.5f, 0.8f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
     0.8f, 3.5f, 0.8f,     0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
    -0.8f, 3.5f, 0.8f,     0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
    -0.8f, 1.5f, 0.8f,     0.0f, 0.0f, 1.0f,    0.0f, 0.0f,

    // Back face
    -0.8f, 1.5f, -0.8f,    0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
    -0.8f, 3.5f, -0.8f,    0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
     0.8f, 3.5f, -0.8f,    0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
     0.8f, 3.5f, -0.8f,    0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
     0.8f, 1.5f, -0.8f,    0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
    -0.8f, 1.5f, -0.8f,    0.0f, 0.0f, -1.0f,   0.0f, 0.0f,

    // Left face
    -0.8f, 1.5f, -0.8f,    -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.8f, 1.5f,  0.8f,    -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.8f, 3.5f,  0.8f,    -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.8f, 3.5f,  0.8f,    -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.8f, 3.5f, -0.8f,    -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.8f, 1.5f, -0.8f,    -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,

    // Right face
     0.8f, 1.5f, -0.8f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
     0.8f, 3.5f, -0.8f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
     0.8f, 3.5f,  0.8f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
     0.8f, 3.5f,  0.8f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,
     0.8f, 1.5f,  0.8f,    1.0f, 0.0f, 0.0f,    1.0f, 0.0f,
     0.8f, 1.5f, -0.8f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f,

    // Top face
    -0.8f, 3.5f, -0.8f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
    -0.8f, 3.5f,  0.8f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
     0.8f, 3.5f,  0.8f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
     0.8f, 3.5f,  0.8f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
     0.8f, 3.5f, -0.8f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
    -0.8f, 3.5f, -0.8f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,

    // Bottom face
    -0.8f, 1.5f, -0.8f,    0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
     0.8f, 1.5f, -0.8f,    0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     0.8f, 1.5f,  0.8f,    0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
     0.8f, 1.5f,  0.8f,    0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
    -0.8f, 1.5f,  0.8f,    0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
    -0.8f, 1.5f, -0.8f,    0.0f, -1.0f, 0.0f,   0.0f, 0.0f
};

float cubeVertices[] = {
    // positions          // normals           // texture coords
    // Back face (z = -0.5)
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    // Front face (z = 0.5)
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    // Left face (x = -0.5)
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    // Right face (x = 0.5)
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    // Bottom face (y = -0.5)
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    // Top face (y = 0.5)
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

float benchVertices[] = {
    // positions          // normals           // texture coords
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

//imitialize all the elemtens VBO and VAO
unsigned int groundVAO, groundVBO, pathVAO, pathVBO, trunkVAO, trunkVBO, leavesVAO, leavesVBO, grassVAO, grassVBO;
unsigned int sandVAO, sandVBO,towerVAO, towerVBO,bladeVAO, bladeVBO, benchVAO, benchVBO;


void renderScene(GLuint shaderProgram, bool shadowPass, GLuint grassTexture, GLuint dirtTexture, 
                 GLuint stoneTexture, GLuint barkTexture, GLuint leavesTexture, GLuint sandTexture, 
                 GLuint steelTexture, GLuint woodTexture) {
    // Helper function to render all objects
    auto renderObject = [&](unsigned int VAO, unsigned int texture, glm::mat4 model, int vertexCount) {
        if (!shadowPass) {
            glActiveTexture(GL_TEXTURE0);  // Use texture unit 0 for main textures
            glBindTexture(GL_TEXTURE_2D, texture);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        }
        glBindVertexArray(VAO);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    };

    // Render grass
    renderObject(grassVAO, grassTexture, glm::mat4(1.0f), 6);
    
    // Render ground
    renderObject(groundVAO, dirtTexture, glm::mat4(1.0f), 36);
    
    // Render path
    renderObject(pathVAO, stoneTexture, glm::mat4(1.0f), 6);
    
    // Render trees
    glm::vec3 trunkPositions[4] = {
        glm::vec3(-6.0f, 0.0f, -6.0f), glm::vec3(6.0f, 0.0f, -6.0f),
        glm::vec3(-6.0f, 0.0f, 6.0f), glm::vec3(6.0f, 0.0f, 6.0f)
    };
    
    for (int i = 0; i < 4; ++i) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), trunkPositions[i]);
        model = glm::scale(model, glm::vec3(treeScale));
        renderObject(trunkVAO, barkTexture, model, 36);
        renderObject(leavesVAO, leavesTexture, model, 36);
    }
    
    // Render sand
    renderObject(sandVAO, sandTexture, glm::mat4(1.0f), 12);
    
    // Render windmill tower
    glm::mat4 towerModel = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, -4.0f));
    towerModel = glm::scale(towerModel, glm::vec3(0.8f, 5.0f, 0.8f));
    renderObject(towerVAO, steelTexture, towerModel, 36);
    
    // Render windmill blades
    glm::mat4 bladeBase = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 4.5f, -3.6f));
    bladeBase = glm::rotate(bladeBase, glm::radians(bladeRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    
    for (int i = 0; i < 4; ++i) {
        glm::mat4 bladeModel = glm::rotate(bladeBase, glm::radians(90.0f * i), glm::vec3(0.0f, 0.0f, 1.0f));
        bladeModel = glm::translate(bladeModel, glm::vec3(1.0f, 0.0f, 0.0f));
        bladeModel = glm::scale(bladeModel, glm::vec3(2.0f, 0.2f, 0.15f));
        renderObject(bladeVAO, steelTexture, bladeModel, 36);
    }
    
    // Render bench
    glm::mat4 benchBase = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.6f, -2.0f));
    benchBase = glm::rotate(benchBase, glm::radians(90.0f + benchRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    benchBase = glm::rotate(benchBase, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    
    glm::mat4 benchModel = glm::scale(benchBase, glm::vec3(2.0f, 0.2f, 0.6f));
    renderObject(benchVAO, woodTexture, benchModel, 36);
    
    // Bench legs
    glm::vec3 legOffsets[4] = {{-0.9f, 0.3f, -0.25f}, {0.9f, 0.3f, -0.25f}, {-0.9f, 0.3f, 0.25f}, {0.9f, 0.3f, 0.25f}};
    for (int i = 0; i < 4; ++i) {
        glm::mat4 legModel = glm::translate(benchBase, legOffsets[i]);
        legModel = glm::scale(legModel, glm::vec3(0.1f, 0.6f, 0.1f));
        renderObject(benchVAO, woodTexture, legModel, 36);
    }
}

void checkShaderCompilation(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
    }
}

void checkProgramLinking(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << std::endl;
    }
}


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

    // Print controls to console
    std::cout << "\n=== INTERACTIVE PARK SCENE CONTROLS ===" << std::endl;
    std::cout << "WASD + QE: Move camera" << std::endl;
    std::cout << "Mouse + Left Click: Look around" << std::endl;
    std::cout << "SPACE: Toggle windmill on/off" << std::endl;
    std::cout << "UP/DOWN arrows: Control windmill speed" << std::endl;
    std::cout << "T: Toggle day/night cycle" << std::endl;
    std::cout << "F: Toggle flashlight" << std::endl;
    std::cout << "G: Toggle tree growing/shrinking" << std::endl;
    std::cout << "B: Rotate bench (45° increments)" << std::endl;
    std::cout << "L: Toggle colorful light show" << std::endl;
    std::cout << "ESC: Exit" << std::endl;
    std::cout << "====================================\n" << std::endl;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);


    // leaves VAO
    glGenVertexArrays(1, &leavesVAO);
    glGenBuffers(1, &leavesVBO);
    glBindVertexArray(leavesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, leavesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leavesVertices), leavesVertices, GL_STATIC_DRAW);
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
    glGenVertexArrays(1, &benchVAO);
    glGenBuffers(1, &benchVBO);

    glBindVertexArray(benchVAO);
    glBindBuffer(GL_ARRAY_BUFFER, benchVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(benchVertices), benchVertices, GL_STATIC_DRAW);
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


    // windmill Tower VAO setup
    glGenVertexArrays(1, &towerVAO);
    glGenBuffers(1, &towerVBO);
    glBindVertexArray(towerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, towerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind


    // windmill Blades VAO setup
    glGenVertexArrays(1, &bladeVAO);
    glGenBuffers(1, &bladeVBO);
    glBindVertexArray(bladeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, bladeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Shader sources (with lighting calculations)
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoord;
        out vec4 FragPosLightSpace[4];  // Shadow coordinates for each light
        
        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
        uniform mat4 lightSpaceMatrix[4];  // Light space matrices for each light
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            
            for(int i = 0; i < 4; i++) {
                FragPosLightSpace[i] = lightSpaceMatrix[i] * vec4(FragPos, 1.0);
            }
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoord;
        in vec4 FragPosLightSpace[4];
        
        uniform sampler2D texture1;
        uniform sampler2D shadowMap0;
        uniform sampler2D shadowMap1;
        uniform sampler2D shadowMap2;
        uniform sampler2D shadowMap3;
        uniform vec3 viewPos;
        uniform vec3 viewDir;
        
        // Light properties
        uniform vec3 lightPositions[4];
        uniform vec3 lightColors[4];
        uniform float lightIntensities[4];
        
        // Material properties
        uniform float shininess;
        uniform vec3 ambient;
        
        float ShadowCalculation(vec4 fragPosLightSpace, int lightIndex) {
            vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
            projCoords = projCoords * 0.5 + 0.5;
            
            // If outside shadow map bounds, no shadow
            if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
                projCoords.y < 0.0 || projCoords.y > 1.0)
                return 0.0;
            
            float closestDepth;
            
            // Select the correct shadow map
            if (lightIndex == 0) {
                closestDepth = texture(shadowMap0, projCoords.xy).r;
            } else if (lightIndex == 1) {
                closestDepth = texture(shadowMap1, projCoords.xy).r;
            } else if (lightIndex == 2) {
                closestDepth = texture(shadowMap2, projCoords.xy).r;
            } else {
                closestDepth = texture(shadowMap3, projCoords.xy).r;
            }
            
            float currentDepth = projCoords.z;
            
            // Adaptive bias based on surface angle
            vec3 normal = normalize(Normal);
            vec3 lightDir = normalize(lightPositions[lightIndex] - FragPos);
            float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
            
            // PCF (Percentage Closer Filtering) for softer shadows
            float shadow = 0.0;
            vec2 texelSize = 1.0 / textureSize(shadowMap0, 0);
            for(int x = -1; x <= 1; ++x) {
                for(int y = -1; y <= 1; ++y) {
                    float pcfDepth;
                    vec2 offset = vec2(x, y) * texelSize;
                    
                    if (lightIndex == 0) {
                        pcfDepth = texture(shadowMap0, projCoords.xy + offset).r;
                    } else if (lightIndex == 1) {
                        pcfDepth = texture(shadowMap1, projCoords.xy + offset).r;
                    } else if (lightIndex == 2) {
                        pcfDepth = texture(shadowMap2, projCoords.xy + offset).r;
                    } else {
                        pcfDepth = texture(shadowMap3, projCoords.xy + offset).r;
                    }
                    
                    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;
            
            return shadow;
        }
        
        vec3 calculatePhongLighting(vec3 lightPos, vec3 lightColor, float intensity, int lightIndex) {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            
            // Special handling for flashlight (light index 3)
            float spotEffect = 1.0;
            if (lightIndex == 3) {
                vec3 spotDir = normalize(viewDir);
                float spotCosine = dot(-lightDir, spotDir);
                float spotCutoff = cos(radians(30.0));
                float spotOuterCutoff = cos(radians(40.0));
                
                if (spotCosine < spotOuterCutoff) {
                    spotEffect = 0.0;
                } else if (spotCosine < spotCutoff) {
                    spotEffect = (spotCosine - spotOuterCutoff) / (spotCutoff - spotOuterCutoff);
                }
            }
            
            // Ambient (always present, no shadows)
            vec3 ambientLight = ambient * lightColor * 0.15;
            
            // Diffuse
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor * 0.8;
            
            // Specular
            vec3 viewDirNorm = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDirNorm, reflectDir), 0.0), shininess);
            vec3 specular = spec * lightColor * 0.2;
            
            // Attenuation
            float distance = length(lightPos - FragPos);
            float attenuation = intensity / (1.0 + 0.09 * distance + 0.032 * distance * distance);
            
            // Shadow calculation
            float shadow = ShadowCalculation(FragPosLightSpace[lightIndex], lightIndex);
            
            // Apply shadow only to diffuse and specular, not ambient
            return ambientLight * attenuation + (1.0 - shadow) * (diffuse + specular) * attenuation * spotEffect;
        }
        
        void main() {
            vec4 textureColor = texture(texture1, TexCoord);
            
            // Base ambient lighting (scene ambient)
            vec3 baseAmbient = vec3(0.1, 0.1, 0.15) * textureColor.rgb;
            
            vec3 lighting = vec3(0.0);
            
            for(int i = 0; i < 4; i++) {
                lighting += calculatePhongLighting(lightPositions[i], lightColors[i], lightIntensities[i], i);
            }
            
            vec3 result = baseAmbient + lighting * textureColor.rgb;
            result = min(result, vec3(1.0)); // Clamp to prevent overexposure
            
            FragColor = vec4(result, textureColor.a);
        }
    )";
    
    // Shadow mapping shaders
    const char* shadowVertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        
        uniform mat4 lightSpaceMatrix;
        uniform mat4 model;
        
        void main() {
            gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
        }
    )";

    const char* shadowFragmentShaderSource = R"(
        #version 330 core
        
        void main() {
            // gl_FragDepth is written automatically
        }
    )";

    // Compile shaders and link program
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinking(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Compile shadow shaders
    GLuint shadowVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shadowVertexShader, 1, &shadowVertexShaderSource, NULL);
    glCompileShader(shadowVertexShader);
    checkShaderCompilation(shadowVertexShader, "SHADOW_VERTEX");

    GLuint shadowFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shadowFragmentShader, 1, &shadowFragmentShaderSource, NULL);
    glCompileShader(shadowFragmentShader);
    checkShaderCompilation(shadowFragmentShader, "SHADOW_FRAGMENT");

    GLuint shadowShaderProgram = glCreateProgram();
    glAttachShader(shadowShaderProgram, shadowVertexShader);
    glAttachShader(shadowShaderProgram, shadowFragmentShader);
    glLinkProgram(shadowShaderProgram);
    checkProgramLinking(shadowShaderProgram);

    glDeleteShader(shadowVertexShader);
    glDeleteShader(shadowFragmentShader);


    // Initialize lights with much lower intensities
    lights[0] = {
        glm::vec3(3.0f, 5.0f, 3.0f),     // Higher position for better shadows
        glm::vec3(1.0f, 0.8f, 0.6f),     // warm white color
        4.0f,                             // intensity
        glm::vec3(0.5f, 0.0f, 0.3f),     // movement direction
        1.0f                              // slower speed
    };

    lights[1] = {
        glm::vec3(-4.0f, 4.0f, -2.0f),   // position
        glm::vec3(0.6f, 0.8f, 1.0f),     // cool blue color
        3.5f,                             // intensity
        glm::vec3(-0.3f, 0.1f, 0.5f),    // movement direction
        0.8f                              // speed
    };

    lights[2] = {
        glm::vec3(1.0f, 6.0f, -4.0f),    // position
        glm::vec3(0.8f, 1.0f, 0.7f),     // green-ish color
        3.0f,                             // intensity
        glm::vec3(0.4f, 0.0f, -0.4f),    // movement direction
        1.2f                              // speed
    };

    // Camera flashlight (4th light)
    lights[3] = {
        camera.Position,                      // Will be updated each frame
        glm::vec3(1.0f, 1.0f, 0.9f),         // Bright white/yellow light
        10.0f,                                // Higher intensity for flashlight effect
        glm::vec3(0.0f, 0.0f, 0.0f),         // No automatic movement
        0.0f                                  // No speed (camera controlled)
    };


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

    // Setup shadow mapping
    for(int i = 0; i < 4; i++) {
        glGenFramebuffers(1, &depthMapFBO[i]);
        
        glGenTextures(1, &depthMap[i]);
        glBindTexture(GL_TEXTURE_2D, depthMap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        
        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Shadow framebuffer not complete!" << std::endl;
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update interactive elements (keep your existing update code)
        if (windmillRunning) {
            bladeRotation += deltaTime * windmillSpeed;
        } else {
            windmillSpeed = std::max(windmillSpeed - 30.0f * deltaTime, 0.0f);
            bladeRotation += deltaTime * windmillSpeed;
        }
        if (bladeRotation > 360.0f)
            bladeRotation -= 360.0f;

        float targetTime = dayTime ? 1.0f : 0.0f;
        timeOfDay += (targetTime - timeOfDay) * 2.0f * deltaTime;
        timeOfDay = std::max(0.0f, std::min(1.0f, timeOfDay));

        if (treeGrowing) {
            treeScale += deltaTime * 0.5f;
            if (treeScale > 1.5f) treeScale = 1.5f;
        } else {
            treeScale -= deltaTime * 0.5f;
            if (treeScale < 0.5f) treeScale = 0.5f;
        }

        if (lightShow) {
            lightShowTimer += deltaTime;
            for (int i = 0; i < 3; i++) {
                lights[i].color.r = 0.5f + 0.5f * sin(lightShowTimer * 3.0f + i * 2.0f);
                lights[i].color.g = 0.5f + 0.5f * sin(lightShowTimer * 2.0f + i * 1.5f);
                lights[i].color.b = 0.5f + 0.5f * sin(lightShowTimer * 4.0f + i * 1.0f);
                lights[i].intensity = 2.0f + 2.0f * sin(lightShowTimer * 2.5f + i);
            }
        } else {
            lights[0].color = glm::vec3(1.0f, 0.8f, 0.6f);
            lights[0].intensity = 3.0f;
            lights[1].color = glm::vec3(0.6f, 0.8f, 1.0f);
            lights[1].intensity = 2.5f;
            lights[2].color = glm::vec3(0.8f, 1.0f, 0.7f);
            lights[2].intensity = 2.0f;
        }

        // Update light positions
        for (int i = 0; i < 3; i++) {
            lights[i].position += lights[i].direction * lights[i].speed * deltaTime;
            
            if (lights[i].position.x > 6.0f || lights[i].position.x < -6.0f) {
                lights[i].direction.x *= -1.0f;
            }
            if (lights[i].position.y > 6.0f || lights[i].position.y < 1.0f) {
                lights[i].direction.y *= -1.0f;
            }
            if (lights[i].position.z > 6.0f || lights[i].position.z < -6.0f) {
                lights[i].direction.z *= -1.0f;
            }
        }
        lights[3].position = camera.Position;
        lights[3].intensity = flashlightOn ? 10.0f : 0.0f;

        processInput(window);

        // 1. SHADOW MAPPING PASS
        glm::mat4 lightSpaceMatrix[4];
        for(int i = 0; i < 4; i++) {
            // Better light projection matrix
            glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 30.0f);
            
            // Improve light view matrix - look at scene center with slight offset
            glm::vec3 lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 lightUp = glm::vec3(0.0f, 1.0f, 0.0f);
            
            // For moving lights, make sure they're always looking at the scene
            if (glm::length(lights[i].position - lightTarget) < 1.0f) {
                lightTarget = lights[i].position + glm::vec3(1.0f, -1.0f, 1.0f);
            }
            
            glm::mat4 lightView = glm::lookAt(lights[i].position, lightTarget, lightUp);
            lightSpaceMatrix[i] = lightProjection * lightView;
            
            glUseProgram(shadowShaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[i][0][0]);
            
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
            glClear(GL_DEPTH_BUFFER_BIT);
            
            // Enable front face culling for shadow pass to reduce peter panning
            glCullFace(GL_FRONT);
            
            renderScene(shadowShaderProgram, true, grassTexture, dirtTexture, stoneTexture, 
                    barkTexture, leavesTexture, sandTexture, steelTexture, woodTexture);
            
            // Restore back face culling
            glCullFace(GL_BACK);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // 2. NORMAL RENDERING PASS
        glViewport(0, 0, 800, 600);
        float bgR = 0.05f + 0.45f * timeOfDay;
        float bgG = 0.08f + 0.52f * timeOfDay;
        float bgB = 0.15f + 0.65f * timeOfDay;
        glClearColor(bgR, bgG, bgB, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.f / 600.f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &camera.Position[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewDir"), 1, &camera.Front[0]);

        // Set light properties and bind shadow maps to texture units 5-8
        for (int i = 0; i < 4; i++) {
            std::string lightPos = "lightPositions[" + std::to_string(i) + "]";
            std::string lightCol = "lightColors[" + std::to_string(i) + "]";
            std::string lightInt = "lightIntensities[" + std::to_string(i) + "]";
            std::string lightSpace = "lightSpaceMatrix[" + std::to_string(i) + "]";
            
            glUniform3fv(glGetUniformLocation(shaderProgram, lightPos.c_str()), 1, &lights[i].position[0]);
            glUniform3fv(glGetUniformLocation(shaderProgram, lightCol.c_str()), 1, &lights[i].color[0]);
            glUniform1f(glGetUniformLocation(shaderProgram, lightInt.c_str()), lights[i].intensity);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, lightSpace.c_str()), 1, GL_FALSE, &lightSpaceMatrix[i][0][0]);
        }
        
        // Bind individual shadow maps
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, depthMap[0]);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap0"), 5);
        
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, depthMap[1]);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap1"), 6);
        
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, depthMap[2]);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap2"), 7);
        
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, depthMap[3]);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap3"), 8);

        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), 32.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "ambient"), 0.02f, 0.02f, 0.03f);

        renderScene(shaderProgram, false, grassTexture, dirtTexture, stoneTexture, 
                   barkTexture, leavesTexture, sandTexture, steelTexture, woodTexture);

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