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

// Vector to save grid vertices
std::vector<float> gridVertices;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void generateGrid(int size, float spacing);
void processUI(int& windowWidth, int& windowHeight, glm::vec3 &hitPoint, glm::vec2& hitUV);
std::vector<unsigned char> generateTexture();

bool intersectRayTriangle(
    const glm::vec3& orig,
    const glm::vec3& dir,
    const glm::vec3& v0,
    const glm::vec3& v1,
    const glm::vec3& v2,
    float& t,
    float& outU,
    float& outV);

bool PickModel(
    const Model& model,
    const glm::mat4& modelMatrix,
    const glm::vec3& ray_origin,
    const glm::vec3& ray_dir,
    glm::vec3& outHitPoint,
    glm::vec2& outHitUV);

// Settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float PANEL_WIDTH = 350.0f; // Right panel width

// Camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isRightMousePressed = false;
bool isMiddleMousePressed = false;
bool isLeftMousePressed = false;

OrbitCamera camera(glm::vec3(0.0f, 0.0f, 0.0f), 12.0f, 45.0f, 30.0f);

// Delta time variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// Grid visibility
bool gridVisible = true;

// Viewport dimensions
int viewportWidth = SCR_WIDTH - PANEL_WIDTH;
int viewportHeight = SCR_HEIGHT;

// UI state
static float bgColor[3] = { 0.1f, 0.1f, 0.1f };
static float gridColor[3] = { 0.6f, 0.6f, 0.6f };
static float modelScale = 5.0f;
static float modelPosY = -0.85f;
static bool showWireframe = false;

//Generated image diamentions
const int IMG_WIDTH = 2048;
const int IMG_HEIGHT = 2048;




int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Viewport", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Generate grid
    generateGrid(20, 1.0f);

    unsigned int gridVAO, gridVBO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    // Load model
    Model ourModel("assets/models/sample_model_obj/24_12_2024.obj");

    //load texture to openGL
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    auto textureData = generateTexture();

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        IMG_WIDTH,
        IMG_HEIGHT,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        textureData.data()
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



    std::cout << textureID << std::endl;


    // Shaders
    Shader ourShader("model.vert", "model.frag");
    Shader gridShader("grid.vert", "grid.frag");
	Shader debugShader("debugRay.vert", "debugRay.frag");

    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();


        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;






        processInput(window);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Get current window size
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        // Calculate viewport size (left side)
        viewportWidth = windowWidth - PANEL_WIDTH;
        viewportHeight = windowHeight;

        // Set viewport for 3D rendering (left side only)
        glViewport(0, 0, viewportWidth, viewportHeight);
        glScissor(0, 0, viewportWidth, viewportHeight);
        glEnable(GL_SCISSOR_TEST);

        glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        float aspect = (float)viewportWidth / (float)viewportHeight;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Distance), aspect, 0.1f, 100.0f);

        // Draw grid
        if (gridVisible) {
            gridShader.use();
            gridShader.setMat4("view", view);
            gridShader.setMat4("model", model);
            gridShader.setMat4("projection", projection);
            gridShader.setVec3("gridColor", glm::vec3(gridColor[0], gridColor[1], gridColor[2]));

            glBindVertexArray(gridVAO);
            glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
            glBindVertexArray(0);
        }

        // Draw model
        if (showWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        ourShader.use();

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureID);

        ourShader.setInt("generated_texture", 2);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, modelPosY, 0.0f));
        model = glm::scale(model, glm::vec3(modelScale));

        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        ourModel.Draw(ourShader);

        if (showWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }




        //ray picking variables
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);


        //ray casting code
        float NDC_X = (2.0f * mouseX) / viewportWidth - 1.0f;
        float NDC_Y = 1.0f - (2.0f * mouseY) / viewportHeight;
        float NDC_Z = -1;

		glm::vec4 ray_clip = glm::vec4(NDC_X, NDC_Y, NDC_Z, 1.0f);
		glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

		glm::vec3 ray_wor = glm::vec3(glm::inverse(view) * ray_eye);

		ray_wor = glm::normalize(ray_wor);

		//std::cout << "Ray Direction: (" << ray_wor.x << ", " << ray_wor.y << ", " << ray_wor.z << ")\n";

		glm::vec3 ray_origin = camera.GetPosition();

		glm::vec3 hitPoint(0.0f);
        glm::vec2 hitUV(0.0f);
        
        if (isLeftMousePressed)
        {
           // glm::vec3 hitPoint;
            if (PickModel(ourModel, model, ray_origin, ray_wor, hitPoint, hitUV))
            {
                //annotations.push_back(hitPoint);
				//std::cout << "Hit Point: (" << hitPoint.x << ", " << hitPoint.y << ", " << hitPoint.z << ")\n";
            }
        }



        //Render UI
		processUI(windowWidth, windowHeight, hitPoint, hitUV);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);


    ourShader.~Shader();
	gridShader.~Shader();
	debugShader.~Shader();


    glfwTerminate();
    return 0;
}

void generateGrid(int size, float spacing) {
    gridVertices.clear();
    float half = size * spacing * 0.5f;

    for (int i = 0; i <= size; i++) {
        float pos = -half + i * spacing;

        gridVertices.push_back(pos);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(-half);
        gridVertices.push_back(pos);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(half);

        gridVertices.push_back(-half);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(pos);
        gridVertices.push_back(half);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(pos);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Viewport will be set in the main loop
}

void processInput(GLFWwindow* window) {
    static bool gKeyWasPressed = false;

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gKeyWasPressed) {
        gridVisible = !gridVisible;
        gKeyWasPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
        gKeyWasPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGuiIO& io = ImGui::GetIO();

    // Only process camera movement if mouse is over viewport (left side)
    if (xpos > viewportWidth || io.WantCaptureMouse) {
        return;
    }

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (isRightMousePressed) {
        camera.ProcessOrbitRotation(xoffset, yoffset);
    }
    else if (isMiddleMousePressed) {
        camera.ProcessPan(xoffset, yoffset);
    }

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse)
        return;

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Only allow camera control if mouse is in viewport area
    if (xpos > viewportWidth)
        return;

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS)
            isRightMousePressed = true;
        else if (action == GLFW_RELEASE)
            isRightMousePressed = false;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS)
            isMiddleMousePressed = true;
        else if (action == GLFW_RELEASE)
            isMiddleMousePressed = false;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS)
            isLeftMousePressed = true;
        else if (action == GLFW_RELEASE)
            isLeftMousePressed = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse)
        return;

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Only allow zoom if mouse is in viewport area
    if (xpos > viewportWidth)
        return;

    camera.ProcessZoom(yoffset);
}

void processUI(int &windowWidth, int &windowHeight, glm::vec3 &hitPoint, glm::vec2 &hitUV) {

    glDisable(GL_SCISSOR_TEST);

    // Reset viewport for UI rendering
    glViewport(0, 0, windowWidth, windowHeight);

    // Right side panel (properties)
    ImGui::SetNextWindowPos(ImVec2(viewportWidth, 0));
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH, windowHeight));
    ImGui::Begin("Properties", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("PROPERTIES");
    ImGui::Separator();
    ImGui::Spacing();

    // Scene Settings
    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::ColorEdit3("Background", bgColor);
        ImGui::Spacing();
    }

    // Grid Settings
    if (ImGui::CollapsingHeader("Grid", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Show Grid", &gridVisible);
        if (gridVisible) {
            ImGui::ColorEdit3("Grid Color", gridColor);
        }
        ImGui::Spacing();
    }

    // Model Transform
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SliderFloat("Scale", &modelScale, 0.1f, 20.0f);
        ImGui::SliderFloat("Height", &modelPosY, -5.0f, 5.0f);
        if (ImGui::Button("Reset Transform")) {
            modelScale = 5.0f;
            modelPosY = -0.85f;
        }
        ImGui::Spacing();
    }

    // Display Settings
    if (ImGui::CollapsingHeader("Display", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Wireframe", &showWireframe);
        ImGui::Spacing();
    }

    // Camera Info
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Distance: %.2f", camera.Distance);
        if (ImGui::Button("Reset Camera")) {
            camera = OrbitCamera(glm::vec3(0.0f, 0.0f, 0.0f), 12.0f, 45.0f, 30.0f);
        }
        ImGui::Spacing();
    }

    // Controls Info
    if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextWrapped("Right Mouse: Orbit camera");
        ImGui::TextWrapped("Middle Mouse: Pan camera");
        ImGui::TextWrapped("Scroll: Zoom in/out");
        ImGui::TextWrapped("G: Toggle grid");
        ImGui::TextWrapped("ESC: Exit");
    }

    if (ImGui::CollapsingHeader("Debug Info", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextWrapped("Hit Point");
        ImGui::TextWrapped("x: %.2f", hitPoint.x);
        ImGui::TextWrapped("y: %.2f", hitPoint.y);
        ImGui::TextWrapped("z: %.2f", hitPoint.z);
        ImGui::TextWrapped("Hit UV");
        ImGui::TextWrapped("x: %.2f", hitUV.x);
        ImGui::TextWrapped("y: %.2f", hitUV.y);
  
    }

    ImGui::End();

    // Viewport info overlay (top-left corner of 3D viewport)
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Viewport Info", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav);
    ImGui::Text("Frame: %d", (int)glfwGetTime());
    ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}


bool intersectRayTriangle(
    const glm::vec3& orig,
    const glm::vec3& dir,
    const glm::vec3& v0,
    const glm::vec3& v1,
    const glm::vec3& v2,
    float& t,
    float& outU,
    float& outV)
{
    const float EPSILON = 1e-7f;

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;

    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);
    if (fabs(a) < EPSILON) return false;

    float f = 1.0f / a;
    glm::vec3 s = orig - v0;

    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0f || u + v > 1.0f) return false;

    float tempT = f * glm::dot(edge2, q);
    if (tempT > EPSILON)
    {
        t = tempT;
        outU = u;
        outV = v;
        return true;
    }

    return false;
}



bool PickModel(
    const Model& model,
    const glm::mat4& modelMatrix,
    const glm::vec3& ray_origin,
    const glm::vec3& ray_dir,
    glm::vec3& outHitPoint,
    glm::vec2& outHitUV)
{
    float closestT = FLT_MAX;
    bool hit = false;

    for (const Mesh& mesh : model.meshes)
    {
        for (size_t i = 0; i < mesh.indices.size(); i += 3)
        {
            glm::vec3 v0 = mesh.vertices[mesh.indices[i]].Position;
            glm::vec3 v1 = mesh.vertices[mesh.indices[i + 1]].Position;
            glm::vec3 v2 = mesh.vertices[mesh.indices[i + 2]].Position;

            glm::vec2 uv0 = mesh.vertices[mesh.indices[i]].TexCoords;
            glm::vec2 uv1 = mesh.vertices[mesh.indices[i + 1]].TexCoords;
            glm::vec2 uv2 = mesh.vertices[mesh.indices[i + 2]].TexCoords;


            // MODEL → WORLD (CRITICAL)
            v0 = glm::vec3(modelMatrix * glm::vec4(v0, 1.0f));
            v1 = glm::vec3(modelMatrix * glm::vec4(v1, 1.0f));
            v2 = glm::vec3(modelMatrix * glm::vec4(v2, 1.0f));

            float t, u, v;

            if (intersectRayTriangle(ray_origin, ray_dir, v0, v1, v2, t, u, v))
            {
                if (t < closestT)
                {
                    closestT = t;

                    float w = 1.0f - u - v;

                    glm::vec2 hitUV =
                        w * uv0 +
                        u * uv1 +
                        v * uv2;

                    outHitPoint = ray_origin + t * ray_dir;

                    outHitUV = glm::vec2(hitUV.x, hitUV.y);

                    //std::cout << "Hit UV: "
                    //    << hitUV.x << ", "
                    //    << hitUV.y << "\n";

                    hit = true;
                }
            }

        }
    }

    return hit;
}





std::vector<unsigned char> generateTexture()
{
    std::vector<unsigned char> data(IMG_WIDTH * IMG_HEIGHT * 4); // RGBA

    for (int y = 0; y < IMG_HEIGHT; y++)
    {
        for (int x = 0; x < IMG_WIDTH; x++)
        {
            int index = (y * IMG_WIDTH + x) * 4;

            // Simple gradient
            data[index + 0] = (unsigned char)((float)x / IMG_WIDTH * 255);  // R
            data[index + 1] = (unsigned char)((float)y / IMG_HEIGHT * 255); // G
            data[index + 2] = 128;                                      // B

            if (x < 1000)
                data[index + 3] = 255;                        // A
            else
                data[index + 3] = 0;
        }
    }

    return data;
}

