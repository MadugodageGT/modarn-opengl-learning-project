#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";



int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // since we use the modern functions of openGL we use core

	GLfloat vertices[] = {
		// positions
		-0.5f, -0.5f, 0.0f, // bottom left
		 0.5f, -0.5f, 0.0f, // bottom right
		 0.0f,  0.5f, 0.0f  // top
	}; //since we here use normalized coordionate system. coordinates should within [-1.0, 1.0] range

	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Window", NULL, NULL);

	if(window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // make the window's context current


	gladLoadGL(); // load GLAD so that it can manage OpenGL function pointers

	glViewport(0, 0, 800, 800); // set the viewport to the size of the window

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set the clear color to a dark blue
	glClear(GL_COLOR_BUFFER_BIT); // clear the color buffer
	glfwSwapBuffers(window); // swap the front and back buffers to display the cleared color

	while (!glfwWindowShouldClose(window)) // keep the window open until it is closed by the user
	{
		glfwPollEvents();  // process events like keyboard input, mouse movement, etc.
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
