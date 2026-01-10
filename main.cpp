#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



#include "Shader.h"
#include "Camera.h"
#include "OrbitCamera.h"
#include "Model.h"



//vector to save grid vertices
std::vector<float> gridVertices;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void generateGrid(int size, float spacing);





//setttings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;




//camera 
float lastX = SCR_WIDTH/ 2.0f;
float lastY = SCR_HEIGHT/ 2.0f;
bool firstMouse = true;
bool isRightMousePressed = false; // check wether right mouse is pressed
OrbitCamera camera(glm::vec3(0.0f, -0.85f, 0.0f), 5.0f, 45.0f, 30.0f);




//delta time variables
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;



//grid visibility
bool gridVisible = false;





//main function
int main() {


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "test window", NULL, NULL);

	if (window == NULL) {

		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



	//______________________________________________________________________________________________

	generateGrid(20, 1.0f);

	unsigned int gridVAO, gridVBO;

	glGenVertexArrays(1, &gridVAO);
	glGenBuffers(1, &gridVBO);

	glBindVertexArray(gridVAO);

	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
	glBufferData(GL_ARRAY_BUFFER,
		gridVertices.size() * sizeof(float),
		gridVertices.data(),
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);



	// load models______________________________________________________________________________________

	Model ourModel("assets/models/sample_model_obj/24_12_2024.obj");

	//______________________________________________________________________________________________


	//shader
	Shader ourShader("model.vert", "model.frag");


	//grid shader
	Shader gridShader("grid.vert", "grid.frag");



	glEnable(GL_DEPTH_TEST);


	//_____________________________________________________________________________________________

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//main rendering loop__________________________________________________________________________
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput(window);
		



		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		glm::mat4 projection;

		//_______________________________draw grid___________________________________________
		if (gridVisible) {
			gridShader.use();

			gridShader.setMat4("view", view);
			gridShader.setMat4("model", model);
			gridShader.setMat4("projection", projection);

			gridShader.setVec3("gridColor", glm::vec3(0.6f));


			glBindVertexArray(gridVAO);
			glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
			glBindVertexArray(0);
		}


		// _________________________Loaded model__________________________________________

		ourShader.use();

		view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.85f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f));
		projection = glm::perspective(glm::radians(camera.Distance), 1920.0f/1080, 0.1f, 100.0f);

		ourShader.setMat4("model", model);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);

		ourModel.Draw(ourShader);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	

	glDeleteVertexArrays(1, &gridVAO);
	glDeleteBuffers(1, &gridVBO);


	ourShader.~Shader();
	gridShader.~Shader();

	glfwTerminate();
	return 0;
}



//generate grid
void generateGrid(int size, float spacing)
{
	gridVertices.clear();

	float half = size * spacing * 0.5f;

	for (int i = 0; i <= size; i++)
	{
		float pos = -half + i * spacing;

		// Lines parallel to Z (along X)
		gridVertices.push_back(pos);
		gridVertices.push_back(0.0f);
		gridVertices.push_back(-half);

		gridVertices.push_back(pos);
		gridVertices.push_back(0.0f);
		gridVertices.push_back(half);

		// Lines parallel to X (along Z)
		gridVertices.push_back(-half);
		gridVertices.push_back(0.0f);
		gridVertices.push_back(pos);

		gridVertices.push_back(half);
		gridVertices.push_back(0.0f);
		gridVertices.push_back(pos);
	}
}


//callback function to adjust viewport on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, width, height);
}



//handle inputs
void processInput(GLFWwindow* window) {

	static bool gKeyWasPressed = false;


	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gKeyWasPressed)
	{
		gridVisible = !gridVisible;   // toggle once
		gKeyWasPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
	{
		gKeyWasPressed = false;
	}

	

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {

		glfwSetWindowShouldClose(window, true);
	}

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	// Only rotate when right mouse button is held
	if (isRightMousePressed)
	{
		camera.ProcessOrbitRotation(xoffset, yoffset);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			isRightMousePressed = true;
		else if (action == GLFW_RELEASE)
			isRightMousePressed = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessZoom(yoffset);
}
