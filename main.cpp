#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Camera.h"
#include "OrbitCamera.h"
#include "Model.h"

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
OrbitCamera camera(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 45.0f, 30.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool rightMousePressed = false;
bool middleMousePressed = false;

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int LoadTexture(const char* path);

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Simple Box", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    // Cube vertices with normals and texture coordinates
    float vertices[] = {
        // ---------- Back face (-Z)
        -0.5f,-0.5f,-0.5f,  0,0,-1,  0,0,
         0.5f,-0.5f,-0.5f,  0,0,-1,  1,0,
         0.5f, 0.5f,-0.5f,  0,0,-1,  1,1,
         0.5f, 0.5f,-0.5f,  0,0,-1,  1,1,
        -0.5f, 0.5f,-0.5f,  0,0,-1,  0,1,
        -0.5f,-0.5f,-0.5f,  0,0,-1,  0,0,

        // ---------- Front face (+Z)
        -0.5f,-0.5f, 0.5f,  0,0,1,   0,0,
         0.5f,-0.5f, 0.5f,  0,0,1,   1,0,
         0.5f, 0.5f, 0.5f,  0,0,1,   1,1,
         0.5f, 0.5f, 0.5f,  0,0,1,   1,1,
        -0.5f, 0.5f, 0.5f,  0,0,1,   0,1,
        -0.5f,-0.5f, 0.5f,  0,0,1,   0,0,

        // ---------- Left face (-X)
        -0.5f, 0.5f, 0.5f, -1,0,0,   1,0,
        -0.5f, 0.5f,-0.5f, -1,0,0,   1,1,
        -0.5f,-0.5f,-0.5f, -1,0,0,   0,1,
        -0.5f,-0.5f,-0.5f, -1,0,0,   0,1,
        -0.5f,-0.5f, 0.5f, -1,0,0,   0,0,
        -0.5f, 0.5f, 0.5f, -1,0,0,   1,0,

        // ---------- Right face (+X)
         0.5f, 0.5f, 0.5f,  1,0,0,   1,0,
         0.5f, 0.5f,-0.5f,  1,0,0,   1,1,
         0.5f,-0.5f,-0.5f,  1,0,0,   0,1,
         0.5f,-0.5f,-0.5f,  1,0,0,   0,1,
         0.5f,-0.5f, 0.5f,  1,0,0,   0,0,
         0.5f, 0.5f, 0.5f,  1,0,0,   1,0,

         // ---------- Bottom face (-Y)
         -0.5f,-0.5f,-0.5f,  0,-1,0,  0,1,
          0.5f,-0.5f,-0.5f,  0,-1,0,  1,1,
          0.5f,-0.5f, 0.5f,  0,-1,0,  1,0,
          0.5f,-0.5f, 0.5f,  0,-1,0,  1,0,
         -0.5f,-0.5f, 0.5f,  0,-1,0,  0,0,
         -0.5f,-0.5f,-0.5f,  0,-1,0,  0,1,

         // ---------- Top face (+Y)
         -0.5f, 0.5f,-0.5f,  0,1,0,   0,1,
          0.5f, 0.5f,-0.5f,  0,1,0,   1,1,
          0.5f, 0.5f, 0.5f,  0,1,0,   1,0,
          0.5f, 0.5f, 0.5f,  0,1,0,   1,0,
         -0.5f, 0.5f, 0.5f,  0,1,0,   0,0,
         -0.5f, 0.5f,-0.5f,  0,1,0,   0,1
    };

    // Setup cube VAO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Load shader
    Shader shader("blending.vert", "blending.frag");


    //load textures
    unsigned int grassTexture = LoadTexture("assets/textures/grass/grass.png");


  

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


        // Activate shader
        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);


        shader.setInt("texture1",0);


        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);

        // Create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // Draw cube
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (rightMousePressed) {
        camera.ProcessOrbitRotation(xoffset, yoffset);
    }
    else if (middleMousePressed) {
        camera.ProcessPan(xoffset, yoffset);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        rightMousePressed = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        middleMousePressed = (action == GLFW_PRESS);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessZoom(yoffset);
}

unsigned int LoadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (!data)
    {
        std::cout << "Failed to load texture: " << path << "\n";
        return 0;
    }

    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
        format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}
