// DEBUG
#define DEBUG true

// Image Loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// C++ Inclusions
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

// Include GLAD first, because it includes multiple definitions required for next header files
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM Inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.hpp"
#include "shader.hpp"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;

// Camera variables
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 40.0f);
glm::vec3 camTrg = glm::vec3(0.0f, 0.0f, 39.0f);
glm::vec2 camRot = glm::vec2(0.0f, 0.0f);
glm::vec3 camDir;
glm::vec3 camDirRt;
glm::vec3 camDirUp;

float camSpd = 200.0;
glm::vec2 lastMousePos = glm::vec2(400.0f, 400.0f);

float camFov = 90.0f;

float mouseSensitivity = 0.01f;
bool mouseMoved = false;

// Delta time variable

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);
void updateDt();

float dt;
float lastFrame;

// Main function
int main()
{
    // STB Image Loader set up
    stbi_set_flip_vertically_on_load(true);

    // GLFW Initialization
    glfwInit();

    // GLFW Configuration
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required for macos
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetCursorPos(window, 400.0, 400.0);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Define viewport for OpenGL
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Callbacks
    glfwSetWindowSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Define GL states
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    // Shaders
    Shader objectShader = Shader("src/assets/shaders/objectShader/objectShader.vs", "src/assets/shaders/objectShader/objectShader.fs");
    objectShader.Use();
    objectShader.Set("material.diff", 0);
    objectShader.Set("material.spec", 1);
    objectShader.Set("material.shininess", 2);

    // Light source
    SpotLight light = SpotLight{camPos,
                                camDir,
                                15.0f,
                                25.0f,
                                glm::vec3(0.15f, 0.15f, 0.15f),
                                glm::vec3(1.0f, 0.75f, 0.7f),
                                0.0045f, 0.000075f};

    // Z-Buffer
    glEnable(GL_DEPTH_TEST);

    // Model
    stbi_set_flip_vertically_on_load(true);
    glfwSetWindowTitle(window, "LearnOpenGL - Loading model...");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
    Model model = Model("src/assets/backpack/backpack.obj", [] { glfwPollEvents(); });
    glfwSetWindowTitle(window, "LearnOpenGL");

    // Render loop
    constexpr auto targetFrameTime = std::chrono::duration<double>(1.0 / 60.0);
    while (!glfwWindowShouldClose(window))
    {
        const auto frameStart = std::chrono::steady_clock::now();

        // Clear background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update delta time
        updateDt();

        // Input managing
        processInput(window);

        // Transformations + coordinate systems
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        camDir = -glm::normalize(camPos - camTrg);
        camDirRt = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), -camDir));
        camDirUp = glm::normalize(glm::cross(-camDir, camDirRt));

        view = glm::lookAt(camPos,
                           camTrg,
                           camDirUp);

        projection = glm::perspective(glm::radians(camFov), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 10000.0f);

        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::scale(modelMat, glm::vec3(25.0f));

        objectShader.Use();
        objectShader.Set("light", light);
        objectShader.Set("viewPos", camPos);

        objectShader.Set("view", view);
        objectShader.Set("projection", projection);
        objectShader.Set("model", modelMat);

        light.pos = camPos;
        light.dir = camDir;

        // Drawing
        model.Draw(objectShader);

        // Rendering and callback events
        glfwSwapBuffers(window);
        glfwPollEvents();
        const auto frameTime = std::chrono::steady_clock::now() - frameStart;
        if (frameTime < targetFrameTime)
        {
            std::this_thread::sleep_for(targetFrameTime - frameTime);
        }
    }

    glfwTerminate();
    return 0;
}

// Prepare resize callback for opengl viewport
void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    glViewport(0, 0, width, height);
}

// Prepare mouse movement callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) return;

    if (!mouseMoved)
    {
        lastMousePos = glm::vec2(xpos, ypos);
        mouseMoved = true;
        return;
    }

    mouseMoved = true;

    glm::vec2 offset = glm::vec2(glm::vec2(xpos, ypos) - lastMousePos);    
    offset *= mouseSensitivity;

    camRot.x += offset.y;
    camRot.y -= offset.x;
    
    camRot.x = camRot.x < glm::radians(-79.9f) ? glm::radians(-79.9f) : camRot.x > glm::radians(79.9f) ? glm::radians(79.9f) : camRot.x;
    camRot.y = camRot.y > glm::radians(360.0f) ? glm::radians(360.0f) : camRot.y;

    glm::vec3 dir;
    dir.x = std::cos(camRot.x) * std::sin(camRot.y);
    dir.y = std::sin(camRot.x);
    dir.z = -std::cos(camRot.x) * std::cos(camRot.y);
    camTrg = glm::normalize(dir) + camPos;

    lastMousePos = glm::vec2(xpos, ypos);
}

// Prepare scroll callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camFov = camFov - yoffset;
    camFov = camFov < 1.0f ? 1.0f : camFov > 180.0f ? 180.0f : camFov;
}

// Input function
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetCursorPos(window, width/2.0f, height/2.0f);
        mouseMoved = false;
    }
    
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // Movement
    glm::vec3 dir;
    dir.x = std::cos(camRot.x) * std::sin(camRot.y);
    dir.y = std::sin(camRot.x);
    dir.z = -std::cos(camRot.x) * std::cos(camRot.y);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {    
        camPos -= camDirRt * camSpd * dt;
        camTrg -= camDirRt * camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camPos += camDirRt * camSpd * dt;
        camTrg += camDirRt * camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camPos += camDirUp * camSpd * dt;
        camTrg += camDirUp * camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camPos -= camDirUp * camSpd * dt;
        camTrg -= camDirUp * camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
        camPos -= camDir * camSpd * dt;
        camTrg -= camDir * camSpd * dt;
    }
        
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
        camPos += camDir * camSpd * dt;
        camTrg += camDir * camSpd * dt;
    }
}

// Delta time function
void updateDt()
{
    dt = glfwGetTime() - lastFrame;
    lastFrame = glfwGetTime(); 
}