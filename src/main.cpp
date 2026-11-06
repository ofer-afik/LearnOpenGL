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

#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 800

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
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

glm::vec3 cubePositions[10] {
    10.0f * glm::vec3( 0.0f,  0.0f, 0.0f),
    10.0f * glm::vec3( 2.0f,  5.0f, -15.0f),
    10.0f * glm::vec3(-1.5f, -2.2f, -2.5f),
    10.0f * glm::vec3(-3.8f, -2.0f, -12.3f),
    10.0f * glm::vec3( 2.4f, -0.4f, -3.5f),
    10.0f * glm::vec3(-1.7f,  3.0f, -7.5f), 
    10.0f * glm::vec3( 1.3f, -2.0f, -2.5f), 
    10.0f * glm::vec3( 1.5f,  2.0f, -2.5f),
    10.0f * glm::vec3( 1.5f,  0.2f, -1.5f),
    10.0f * glm::vec3(-1.3f,  1.0f, -1.5f)
};

// Camera variables
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 40.0f);
glm::vec3 camTrg = glm::vec3(0.0f, 0.0f, 0.0f);
float camSpd = 100.0;

// Delta time variable
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
    GLFWwindow* window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Define viewport for OpenGL
    int min = std::min(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    glViewport(std::max(INITIAL_WINDOW_WIDTH - INITIAL_WINDOW_HEIGHT, 0) / 2, std::max(INITIAL_WINDOW_HEIGHT - INITIAL_WINDOW_WIDTH, 0) / 2, min, min);

    // Callbacks
    glfwSetWindowSizeCallback(window, framebuffer_size_callback);

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
        projection = glm::perspective(glm::radians(90.0f), (float)INITIAL_WINDOW_WIDTH/(float)INITIAL_WINDOW_HEIGHT, 0.1f, 1000.0f);
        // Draw the triangle
        shader.Use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(vao);
        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(cubePositions[1]));
            model = glm::rotate(model,
                                glm::radians((float)sin(glfwGetTime()) * i * 20.0f),
                                glm::normalize(glm::vec3((float)i * (float)i, (float)i, 1.0f)));
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
    int min = std::min(width, height);
    glViewport(std::max(width - height, 0) / 2, std::max(height - width, 0) / 2, min, min);
}

// Input function
void processInput(GLFWwindow *window)
{
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // << >>
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
        camTrg.y -= camSpd * dt;
        camPos.y -= camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
        camTrg.z += camSpd * dt;
        camPos.z += camSpd * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
        camTrg.z -= camSpd * dt;
        camPos.z -= camSpd * dt;
    }
}

// Delta time function
void updateDt()
{
    dt = glfwGetTime() - lastFrame;
    lastFrame = glfwGetTime(); 
}