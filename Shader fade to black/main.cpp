// GLEW must be included first
#define GLEW_STATIC
#include <GLEW/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

// Image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// C++ includes
#include <iostream>

// Shader class
#include "Shader.h"

// Camera class
#include "Camera.h"

// temporary globals
bool lockCursor = true; // (un)lock cursor in window by pressing C
float count = 0;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
double GLVer = 3.3;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat currentFrame = 0.0f;

//camera 
Camera camera;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

// input
bool keys[1024];

// light
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // only supports 3.1 on intel hd 3000
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // requires 3.3+ for this line to work
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // asks if the window should be resized by the user
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr); // creates a window, or returns nullptr if error
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window 3.3" << std::endl;
		std::cout << "Attempting to create GLFW window 3.1" << std::endl;
		glfwTerminate();
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // only supports 3.1 on intel hd 3000
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
		GLVer = 3.1;

		if (window == nullptr)
		{
			std::cout << "Failed to create GLFW window 3.1" << std::endl;
			std::cout << "Attempting to create GLFW window 2.1" << std::endl;
			glfwTerminate();
			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); 
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
			GLVer = 2.1;

			if (window == nullptr)
			{
				std::cout << "Failed to create GLFW window 2.1" << std::endl;
				glfwTerminate();
				return -1;
			}
		}
	}
	glfwMakeContextCurrent(window); // makes the window the main context for draw operations

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // locks cursor to window (for fps controls)

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE; // use modern techniques for managine opengl functionality, mostly for 3.3+
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height); // gets size of screen
	glViewport(0, 0, width, height); 

	Shader exampleShader("exampleShader.vert", "exampleShader.frag");

	GLfloat vertices[] = {
		// Position(x,y,z),  TexCoord(x,y)
		-1.0f,	-1.0f,	0.0f,		0.0f, 0.0f,
		-1.0f,	1.0f,	0.0f,		0.0f, 1.0f,
		1.0f,	1.0f,	0.0f,		1.0f, 1.0f,
		-1.0f,	-1.0f,	0.0f,		0.0f, 0.0f,
		1.0f,	-1.0f,	0.0f,		1.0f, 0.0f,
		1.0f,	1.0f,	0.0f,		1.0f, 1.0f,
					
		
	}; // can use an EBO to use only 24 vertices instead of 36


	// indices useful for EBOs, especially when reusing vertices
	//GLint indices[] = {
	//	0,1,3,
	//	1,2,3
	//};


	GLuint VBO; // vertex buffer object
	glGenBuffers(1, &VBO); // generate buffer ID
	GLuint VAO; // vertex array object
	glGenVertexArrays(1, &VAO); // generate vertex ID
	//GLuint EBO; // element buffer object to avoid storing repeated vertices
	//glGenBuffers(1, &EBO); 

	// can initialise more than one at a time using GLuint VAOs[2] and glGenVertexArrays(2,VAOs)
	//GLuint VAO2;
	//glGenVertexArrays(1, &VAO2);


	// initialisation code
	// 1: bind vertex array object
	glBindVertexArray(VAO);
	// 2: copy vertices array in buffer for opengl
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// GL_STATIC_DRAW = data that is unlikely to change
		// GL_DYNAMIC_DRAW = data that is likely to change a lot
		// GL_STREAM_DRAW = data will change every time it is drawn
	// 2.5: copy index array in elemennt buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 3: set vertex position attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 3.5: set vertex color or texture attribute pointers
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 4: unbind VAO (NOT the EBO)
	glBindVertexArray(0);


	// create texture buffer
	stbi_set_flip_vertically_on_load(1); // flips images so that 0.0 is at the bottom left for openGL
	GLuint texture;
	glGenTextures(1, &texture); // generate texture ID
	glBindTexture(GL_TEXTURE_2D, texture); // bind texture so that all GL_TEXTURE_2D operations act on this texture
	// set texture wrapping/filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image
	int stbi_x, stbi_y, stbi_n;
	// stores the size of image and number of components. stbi_n: 1 = grey, 2 = grey alpha, 3 = rgb, 4 = rgba
	unsigned char *image = stbi_load("buildings.png", &stbi_x, &stbi_y, &stbi_n, 0);
	// check if image loads properly
	if (image == NULL) {
		std::cout << "ERROR::IMAGE_LOAD::FAILED\n" << stbi_failure_reason() << std::endl;
	}
	else {
		std::cout << "x = " << stbi_x << "\ny = " << stbi_y << "\nn = " << stbi_n << std::endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stbi_x, stbi_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D); // create mipmap for currently binded texture
	stbi_image_free(image); 

	glEnable(GL_BLEND); // process alpha channels
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		// time update
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// movement update
		//movement();

		// Render
		// Clear the colorbuffer
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(135.0 / 255.0, 206.0 / 255.0, 235.0 / 255.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Activate shader (cannot send uniform data before using the shader)
		exampleShader.use();

		// send data to shader
		count += deltaTime*200;
		glUniform1f(glGetUniformLocation(exampleShader.program, "count"), count);

		// draw triangles
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// unbind VAO after use
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS) keys[key] = true;
	else if (action == GLFW_RELEASE) keys[key] = false;
	//if (key == GLFW_KEY_C && action == GLFW_PRESS) {
	//	if (lockCursor == false) {
	//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//		lockCursor = true;
	//	}
	//	else {
	//		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//		lockCursor = false;
	//	}
	//}
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		count = 0;
	}

}

void movement() {
	if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);  
	if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);  
	if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime); 
	if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime); 
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static bool firstMouse = true; // initialised only once, avoids camera starting in a random direction on first frame
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX; 
	GLfloat yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;
	
	//camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//camera.ProcessMouseScroll(yoffset);
}