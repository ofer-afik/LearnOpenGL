// DEBUG
#define DEBUG true

// C++ Inclusions
#include <iostream>
#include <cmath>

// Include GLAD first, because it includes multiple definitions required for next header files
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// Vertex data
float vertices[18] = {
    // aPos                 // aColor
    -0.25f, -0.2f, 0.0f,    1.0f, 0.25f, 0.25f,
    0.25f, -0.2f, 0.0f,     0.25f, 1.0f, 0.25f,
    0.0f,  0.2f, 0.0f,      0.25f, 0.25f, 1.0f
};

unsigned int indices[3] = {
    0, 1, 2
};

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
    Shader shader = Shader("src/shaders/shader.vs", "src/shaders/shader.fs");

    // Buffers
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear background
        glClear(GL_COLOR_BUFFER_BIT);

        // Input managing
        processInput(window);

        // Update triangle vbo data
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Draw the triangle
        shader.Use();
        float t = glfwGetTime();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

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

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        for (int i = 0; i < 18; i += 6)
        {
            vertices[i] -= 0.1f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        for (int i = 0; i < 18; i += 6)
        {
            vertices[i] += 0.1f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        for (int i = 1; i < 18; i += 6)
        {
            vertices[i] += 0.1f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        for (int i = 1; i < 18; i += 6)
        {
            vertices[i] -= 0.1f;
        }
    }
}