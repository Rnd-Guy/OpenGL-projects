#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GLEW/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader class
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ONLY_PNG
//#define STBI_ONLY_JPEG
#include "stb_image.h"
#include "Camera.h"

// temporary globals
bool lockCursor = false; // lock cursor in window by pressing C


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void movement();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
double GLVer = 3.3;

//float toMix = 0.2;
//float c_x = 0;
//float c_y = 0;
//float c_z = -3;

void update();

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat currentFrame = 0.0f;


//camera 
Camera camera;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];

// light
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.1" << std::endl;
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
		std::cout << "Starting to create GLFW window 3.1" << std::endl;
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
			std::cout << "Starting to create GLFW window 2.1" << std::endl;
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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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

	//Shader testShader("lighting.vert", "lighting.frag");
	Shader lightingShader("lighting.vert", "lighting.frag");
	Shader lampShader("lamp.vert", "lamp.frag");

	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};



	GLint indices[] = {
		0,1,3,
		1,2,3
	};


	GLuint VBO; // vertex buffer object
	glGenBuffers(1, &VBO); // generate buffer ID
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	GLuint EBO; // element buffer object to avoid storing repeated vertices
	glGenBuffers(1, &EBO);

	GLuint lightingVAO;
	glGenVertexArrays(1, &lightingVAO);


	// can initialise more than one at a time using GLuint VAOs[2] and glGenVertexArrays(2,VAOs)
	//GLuint VAO2;
	//glGenVertexArrays(1, &VAO2);

	// initialisation code
	// 1: bind vertex array object
	//glBindVertexArray(VAO);
	//glBindVertexArray(lightingVAO);
	glBindVertexArray(VAO);
	// 2: copy vertices array in buffer for opengl


	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 2.5: copy index array in elemennt buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// GL_STATIC_DRAW = data that is unlikely to change
		// GL_DYNAMIC_DRAW = data that is likely to change a lot
		// GL_STREAM_DRAW = data will change every time it is drawn


	// 3: set vertex position attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 3.5: set vertex normal attribute pointers
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 4: unbind VAO (NOT the EBO)
	glBindVertexArray(0);

	// bind other vao
	glBindVertexArray(lightingVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);



	


	// load texture
	
	// matrix goodness!

	
	glEnable(GL_DEPTH_TEST); // required for z-buffer to work


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		// time update
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;





		movement();
		// Render
	

		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		lightingShader.use();
		GLuint objectColorLoc = glGetUniformLocation(lightingShader.program, "objectColor");
		GLuint lightColorLoc = glGetUniformLocation(lightingShader.program, "lightColor");

		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);



		// matrix
		glm::mat4 model;
		model = glm::mat4();
		
		//model = glm::translate(model, lightPos);
		//model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 view;
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.zoom), (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(lightingShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glUniform3f(glGetUniformLocation(lightingShader.program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(lightingShader.program, "viewPos"), camera.position.x, camera.position.y, camera.position.z);

		// draw triangle
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 36);



		lampShader.use();
		// change lamp position
		lightPos = glm::vec3(sin(glfwGetTime()*glm::radians(45.0f)), 1.0f, cos(glfwGetTime()*glm::radians(45.0f)));
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(glGetUniformLocation(lampShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(lightingVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);




		glBindVertexArray(0);



		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		//glUseProgram(shaderProgram2);
		//glBindVertexArray(VAO2);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);

		


		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
bool upP = false, downP = false, leftP = false, rightP = false, shiftP = false, ctrlP = false;
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS) keys[key] = true;
	else if (action == GLFW_RELEASE) keys[key] = false;
	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		if (lockCursor == false) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			lockCursor = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			lockCursor = false;
		}
	}
	

}

void update() {

}

void movement() {
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);  //cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);  //cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);  //cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);  //cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static bool firstMouse = true;
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX; 
	GLfloat yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}