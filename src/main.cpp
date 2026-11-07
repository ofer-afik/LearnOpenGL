// DEBUG
#define DEBUG true

// Image Loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// C++ Inclusions
#include <iostream>
#include <cmath>

// Include GLAD first, because it includes multiple definitions required for next header files
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM Inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 800;

glm::vec3 cubePositions[10] {
    25.0f * glm::vec3( 3.2f,  4.0f,  -0.8f),
    25.0f * glm::vec3(-1.5f, -2.2f,  -2.5f),
    25.0f * glm::vec3(-4.8f, -2.0f,  -3.3f),
    25.0f * glm::vec3( 2.4f, -0.4f,  -3.5f),
    25.0f * glm::vec3(-1.7f,  6.0f,  -4.5f), 
    25.0f * glm::vec3( 1.3f, -2.0f,  -2.5f), 
    25.0f * glm::vec3( 1.5f,  2.0f,  -2.5f),
    25.0f * glm::vec3( 1.5f,  0.2f,  -1.5f),
    25.0f * glm::vec3(-1.3f,  1.0f,  -1.5f),
    25.0f * glm::vec3( 0.0f,  0.0f,   0.0f),
};


// Camera variables
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 40.0f);
glm::vec3 camTrg = glm::vec3(0.0f, 0.0f, 39.0f);
glm::vec2 camRot = glm::vec2(0.0f, 0.0f);
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

// Vertex data
float vertices[180] = {
    -10.0f, -10.0f, -10.0f,  0.0f, 0.0f,
     10.0f, -10.0f, -10.0f,  1.0f, 0.0f,
     10.0f,  10.0f, -10.0f,  1.0f, 1.0f,
     10.0f,  10.0f, -10.0f,  1.0f, 1.0f,
    -10.0f,  10.0f, -10.0f,  0.0f, 1.0f,
    -10.0f, -10.0f, -10.0f,  0.0f, 0.0f,
    
    -10.0f, -10.0f,  10.0f,  0.0f, 0.0f,
    10.0f, -10.0f,  10.0f,  1.0f, 0.0f,
    10.0f,  10.0f,  10.0f,  1.0f, 1.0f,
    10.0f,  10.0f,  10.0f,  1.0f, 1.0f,
    -10.0f,  10.0f,  10.0f,  0.0f, 1.0f,
    -10.0f, -10.0f,  10.0f,  0.0f, 0.0f,
    
    -10.0f,  10.0f,  10.0f,  1.0f, 0.0f,
    -10.0f,  10.0f, -10.0f,  1.0f, 1.0f,
    -10.0f, -10.0f, -10.0f,  0.0f, 1.0f,
    -10.0f, -10.0f, -10.0f,  0.0f, 1.0f,
    -10.0f, -10.0f,  10.0f,  0.0f, 0.0f,
    -10.0f,  10.0f,  10.0f,  1.0f, 0.0f,

     10.0f,  10.0f,  10.0f,  1.0f, 0.0f,
     10.0f,  10.0f, -10.0f,  1.0f, 1.0f,
     10.0f, -10.0f, -10.0f,  0.0f, 1.0f,
     10.0f, -10.0f, -10.0f,  0.0f, 1.0f,
     10.0f, -10.0f,  10.0f,  0.0f, 0.0f,
     10.0f,  10.0f,  10.0f,  1.0f, 0.0f,

    -10.0f, -10.0f, -10.0f,  0.0f, 1.0f,
     10.0f, -10.0f, -10.0f,  1.0f, 1.0f,
     10.0f, -10.0f,  10.0f,  1.0f, 0.0f,
     10.0f, -10.0f,  10.0f,  1.0f, 0.0f,
    -10.0f, -10.0f,  10.0f,  0.0f, 0.0f,
    -10.0f, -10.0f, -10.0f,  0.0f, 1.0f,

    -10.0f,  10.0f, -10.0f,  0.0f, 1.0f,
     10.0f,  10.0f, -10.0f,  1.0f, 1.0f,
     10.0f,  10.0f,  10.0f,  1.0f, 0.0f,
     10.0f,  10.0f,  10.0f,  1.0f, 0.0f,
    -10.0f,  10.0f,  10.0f,  0.0f, 0.0f,
    -10.0f,  10.0f, -10.0f,  0.0f, 1.0f
};
float dt;
float lastFrame;

// Main function
int main()
{
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Shaders
    Shader shader = Shader("src/assets/shaders/shader.vs", "src/assets/shaders/shader.fs");

    // Z-Buffer
    glEnable(GL_DEPTH_TEST);

    // Textures
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *imgSrc = stbi_load("src/assets/textures/final_render.jpg", &width, &height, &nrChannels, 0);
#if DEBUG
    if (imgSrc)
    {
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgSrc);
        glGenerateMipmap(GL_TEXTURE_2D);
#if DEBUG
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
#endif
    stbi_image_free(imgSrc);

    shader.Use();
    shader.setInt("tex0", 0);

    // Buffers
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);



    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update delta time
        updateDt();

        // Input managing
        processInput(window);

        // Transformations + coordinate systems
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        glm::vec3 camDir = -glm::normalize(camPos - camTrg);
        glm::vec3 camDirRt = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), -camDir));
        glm::vec3 camDirUp = glm::normalize(glm::cross(-camDir, camDirRt));

        view = glm::lookAt(camPos,
                           camTrg,
                           camDirUp);
        projection = glm::perspective(glm::radians(camFov), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 10000.0f);
        // Draw the triangle
        shader.Use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(vao);
        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cubePositions[i]));
            model = glm::rotate(model,
                                glm::radians(i * 20.0f + 45.0f),
                                glm::normalize(glm::vec3(1.0f, 0.3f, 0.5f)));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // Rendering and callback events
        glfwSwapBuffers(window);
        glfwPollEvents();
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

    bool shouldReturn;
    if (!mouseMoved) shouldReturn = true;
    mouseMoved = true;

    if (shouldReturn)
    {
        lastMousePos = glm::vec2(xpos, ypos);
        return;
    }

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
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camPos.x -= camSpd * dt;
        camTrg.x -= camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camPos.x += camSpd * dt;
        camTrg.x += camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camPos.y += camSpd * dt;
        camTrg.y += camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camPos.y -= camSpd * dt;
        camTrg.y -= camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
        camPos.z += camSpd * dt;
        camTrg.z += camSpd * dt;
    }
        
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
        camPos.z -= camSpd * dt;
        camTrg.z -= camSpd * dt;
    }
}

// Delta time function
void updateDt()
{
    dt = glfwGetTime() - lastFrame;
    lastFrame = glfwGetTime(); 
}