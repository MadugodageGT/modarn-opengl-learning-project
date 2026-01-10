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
#include "Model.h"




//structure to save vertices and indices of a Sphere
struct Sphere {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

//vector to save grid vertices
std::vector<float> gridVertices;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void generateGrid(int size, float spacing);
Sphere createSphere(float radius, unsigned int sectorCount, unsigned int stackCount);




//setttings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;




//camera 
float lastX = SCR_WIDTH/ 2.0f;
float lastY = SCR_HEIGHT/ 2.0f;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));




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


	glm::vec3 lightPos(0.3f, 0.3f, -0.5f);

	//shader
	Shader ourShader("model.vert", "model.frag");


	//grid shader
	Shader gridShader("grid.vert", "grid.frag");


	//light shader
	Shader lightShader("light.vert", "light.frag");	


	glEnable(GL_DEPTH_TEST);


	//_____________________________________________________________________________________________

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

		//_______________________________plane and  light spher___________________________________________



		// Loaded model

		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

		ourShader.use();

		view = camera.GetViewMatrix();
		model = glm::scale(model, glm::vec3(5.0f));
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

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


//draw sphere
Sphere createSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {

	Sphere sphere;
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord
	
	float sectorStep = 2 * glm::pi<float>() / sectorCount;
	float stackStep = glm::pi<float>() / stackCount;
	float sectorAngle, stackAngle;

	for (unsigned int i = 0; i <= stackCount; ++i)
	{
		stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (unsigned int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			sphere.vertices.push_back(x);
			sphere.vertices.push_back(y);
			sphere.vertices.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			sphere.vertices.push_back(nx);
			sphere.vertices.push_back(ny);
			sphere.vertices.push_back(nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			sphere.vertices.push_back(s);
			sphere.vertices.push_back(t);
		}
	}

	// indices
	unsigned int k1, k2;
	for (unsigned int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			if (i != 0)
			{
				sphere.indices.push_back(k1);
				sphere.indices.push_back(k2);
				sphere.indices.push_back(k1 + 1);
			}

			if (i != (stackCount - 1))
			{
				sphere.indices.push_back(k1 + 1);
				sphere.indices.push_back(k2);
				sphere.indices.push_back(k2 + 1);
			}
		}
	}

	return sphere;

}


//callback function to adjust viewport on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, width, height);
}



//handle inputs
void processInput(GLFWwindow* window) {

	static bool gKeyWasPressed = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

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
	float yoffset = lastY - ypos; // reversed

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
