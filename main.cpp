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

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram(); // create a shader program
	
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader); // delete the vertex shader as it is no longer needed
	glDeleteShader(fragmentShader); // delete the fragment shader as it is no longer needed

	// Define the vertices of the triangle
	GLfloat vertices[] = {
		// positions
		-0.5f, -0.5f, 0.0f, // bottom left
		 0.5f, -0.5f, 0.0f, // bottom right
		 0.0f,  0.5f, 0.0f  // top
	}; //since we here use normalized coordionate system. coordinates should within [-1.0, 1.0] range


	GLuint VAO, VBO; // Vertex Buffer Object and Vertex Array Object
	glGenVertexArrays(1, &VAO); // generate a vertex array object
	glGenBuffers(1, &VBO); //

	glBindVertexArray(VAO); // bind the vertex array object

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind the vertex buffer object

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy the vertex data to the buffer

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // specify the layout of the vertex data
	glEnableVertexAttribArray(0); // enable the vertex attribute at index 0

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the vertex buffer object
	glBindVertexArray(0); // unbind the vertex array object


	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set the clear color to a dark blue
	//glClear(GL_COLOR_BUFFER_BIT); // clear the color buffer
	//glfwSwapBuffers(window); // swap the front and back buffers to display the cleared color

	//MAIN WHILE LOOP
	while (!glfwWindowShouldClose(window)) // keep the window open until it is closed by the user
	{

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT); 
		glUseProgram(shaderProgram); // use the shader program	
		glBindVertexArray(VAO); // bind the vertex array object
		// draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 3); // draw the triangle using the vertex data in the VBO

		glfwSwapBuffers(window); // swap the front and back buffers


		glfwPollEvents();  // process events like keyboard input, mouse movement, etc. (GLFW events)
	}

	// free resourses 
	glDeleteVertexArrays(1, &VAO); // delete the vertex array object
	glDeleteBuffers(1, &VBO); // delete the vertex buffer object
	glDeleteProgram(shaderProgram); // delete the shader program

	glfwDestroyWindow(window); // destroy the window before exiting
	glfwTerminate();
	return 0;
}
