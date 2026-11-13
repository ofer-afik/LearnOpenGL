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
    glm::vec3( 80.0f, 100.0f, -20.0f),
    glm::vec3(-37.5f, -55.0f, -62.5f),
    glm::vec3(-120.0f, -50.0f, -82.5f),
    glm::vec3( 60.0f, -10.0f, -87.5f),
    glm::vec3(-42.5f, 150.0f, -112.5f),
    glm::vec3( 32.5f, -50.0f, -62.5f),
    glm::vec3( 37.5f,  50.0f, -62.5f),
    glm::vec3( 37.5f,   5.0f, -37.5f),
    glm::vec3(-32.5f,  25.0f, -37.5f),
    glm::vec3(  0.0f,   0.0f,   0.0f),
};
glm::vec4 objectColor = glm::vec4(1.0f, 0.5f, 0.31f, 1.0f);

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

float vertices[288] = {
    // Back face
    -10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    // Front face
    -10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    // Left face
    -10.0f,  10.0f,  10.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -10.0f,  10.0f, -10.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -10.0f, -10.0f, -10.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -10.0f, -10.0f, -10.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -10.0f, -10.0f,  10.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -10.0f,  10.0f,  10.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

    // Right face
     10.0f,  10.0f,  10.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     10.0f,  10.0f, -10.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     10.0f, -10.0f, -10.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     10.0f, -10.0f, -10.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     10.0f, -10.0f,  10.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     10.0f,  10.0f,  10.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

    // Bottom face
    -10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
     10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    -10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    -10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

    // Top face
    -10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
     10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    -10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
};

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
    Shader lightShader = Shader("src/assets/shaders/lightShader/lightShader.vs", "src/assets/shaders/lightShader/lightShader.fs");

    // Textures
    Light light = Light{glm::vec3(75.0f, 25.0f, 200.0f),
                        glm::vec3(0.1f, 0.1f, 0.1f),
                        glm::vec3(1.0f),
                        0.0045f, 0.000075f};

    Material material;
    material.shininess = 76.0f;

    glGenTextures(1, &material.diff);
    glBindTexture(GL_TEXTURE_2D, material.diff);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    int width; int height; int nrChannels;
    unsigned char *data = stbi_load("src/assets/textures/images/emerald_in_gold_diff.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &material.spec);
    glBindTexture(GL_TEXTURE_2D, material.spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    data = stbi_load("src/assets/textures/images/emerald_in_gold_spec.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &material.shininess);
    glBindTexture(GL_TEXTURE_2D, material.shininess);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    data = stbi_load("src/assets/textures/images/emerald_in_gold_shine.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Z-Buffer
    glEnable(GL_DEPTH_TEST);

    // Buffers
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

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

        camDir = -glm::normalize(camPos - camTrg);
        camDirRt = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), -camDir));
        camDirUp = glm::normalize(glm::cross(-camDir, camDirRt));

        view = glm::lookAt(camPos,
                           camTrg,
                           camDirUp);
        projection = glm::perspective(glm::radians(camFov), (float)WINDOW_WIDTH/(float)WINDOW_HEIGHT, 0.1f, 10000.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diff);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.spec);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, material.shininess);

        objectShader.Use();
        objectShader.Set("material", material, GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2);
        objectShader.Set("light", light);
        objectShader.Set("viewPos", camPos);

        objectShader.Set("view", view);
        objectShader.Set("projection", projection);

        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cubePositions[i]));
            model = glm::rotate(model,
                                glm::radians(i * 20.0f + 45.0f),
                                glm::normalize(glm::vec3(1.0f, 0.3f, 0.5f)));
            objectShader.Set("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        light.pos = glm::vec3(std::cos(glfwGetTime()) * glm::length(glm::vec2(light.pos.x, light.pos.z)),
                              light.pos.y,
                              std::sin(glfwGetTime()) * glm::length(glm::vec2(light.pos.x, light.pos.z)));

        lightShader.Use();
        lightShader.Set("light", light);
        lightShader.Set("view", view);
        lightShader.Set("projection", projection);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light.pos);
        lightShader.Set("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        

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