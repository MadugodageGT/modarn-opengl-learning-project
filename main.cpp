#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"	
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";




float triangleA[] = {
	-0.5f, -0.5f, 0.0f, // left
	-0.1f, -0.5f, 0.0f, // right
	-0.3f, 0.5f, 0.0f,// top
};


float triangleB[] = {
	0.1f, -0.5f, 0.0f, // left
	0.5f, -0.5f, 0.0f, // right
	0.3f, 0.5f, 0.0f // top
};



float vertices[] = {
0.5f, 0.5f, 0.0f, // top right
0.5f, -0.5f, 0.0f, // bottom right
-0.5f, -0.5f, 0.0f, // bottom left
-0.5f, 0.5f, 0.0f // top left
};

unsigned int indices[] = {
	0,1,3,
	1,2,3
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "test window", NULL, NULL);

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



	//SETUP for the triangle 1
	//_______________________________________________________________________________________________
	unsigned int VAO, VBO;// EBO;
	
	glGenVertexArrays(1, &VAO); // generate VAO ID


	glGenBuffers(1, &VBO); // generate vbo ID
	//glGenBuffers(1, &EBO); // generate EBO

	glBindVertexArray(VAO); // bind the VAO

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind the buffer to the GL_ARRAY_BUFFER target
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleA), triangleA, GL_STATIC_DRAW); //copy user defined vertex data to the target

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//_______________________________________________________________________________________________

	
	
	//SETUP for the triangle 2
	//_______________________________________________________________________________________________
	unsigned int VAO2, VBO2;

	glGenVertexArrays(1, &VAO2);

	glGenBuffers(1, &VBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleB), triangleB, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	//_______________________________________________________________________________________________





	//shaders
	//_______________________________________________________________________________________________
	///vertex shader
	unsigned int vertexShader; //create the vertex shader object
	vertexShader = glCreateShader(GL_VERTEX_SHADER); 

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //attach the shader source code to the shade
	glCompileShader(vertexShader); // compile the shader program
	

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

	}

	//fragment shader
	unsigned int fragmentShader; //create the fragment shader object
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

	}
	
	
	//shader program
	unsigned int shaderProgram; 
	shaderProgram = glCreateProgram(); //create the shader program object

	glAttachShader(shaderProgram, vertexShader); //link the vertex shader to the shader program
	glAttachShader(shaderProgram, fragmentShader); //link the fragment shader to the shader program
	glLinkProgram(shaderProgram); 

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

	}

	glUseProgram(shaderProgram); //activate the shader program
	glDeleteShader(vertexShader); //delete the shader objects once linked
	glDeleteShader(fragmentShader);


	//_______________________________________________________________________________________________



	//main rendering loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

		glUseProgram(shaderProgram);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES,0,3);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(VBO2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);


	glfwTerminate();
	return 0;
}






//callback function to adjust viewport on window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, width, height);
}

//handle inputs
void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {

		glfwSetWindowShouldClose(window, true);
	}
}



