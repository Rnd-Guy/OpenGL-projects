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
bool wireframeMode = false; // show wireframe in window by pressing F
bool lockCamera = false; // lock camera direction by pressing P

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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // locks cursor to window (for fps controls)

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
		// Position(x,y,z),  Colour(r,g,b)
		// 128,64,0, door
		// 128,255,255, glass
		// 128,128,128, wall
		// shader divides value by 255 to get the correct color required

		// vertices based on picture
		// walls
		//   front face
		0,0,0,				128,128,128,	//0
		0.25,0,0,			128,128,128,
		0.75,0,0,			128,128,128,
		1.25,0,0,			128,128,128,
		1.35,0,0,			128,128,128,	// bottom left of door
		1.65,0,0,			128,128,128,	// bottom right of door
		1.75,0,0,			128,128,128,
		2.25,0,0,			128,128,128,
		2.75,0,0,			128,128,128,
		3,0,0,				128,128,128,	// 9

		0.25,0.25,0,		128,128,128,	// 10
		0.75,0.25,0,		128,128,128,
		2.25,0.25,0,		128,128,128,
		2.75,0.25,0,		128,128,128,
		
		0.25,0.75,0,		128,128,128,
		0.75,0.75,0,		128,128,128,
		2.25,0.75,0,		128,128,128,
		2.75,0.75,0,		128,128,128,

		1.35,0.85,0,		128,128,128,	// top left of door
		1.65,0.85,0,		128,128,128,	// top right of door

		0.25,1.25,0,		128,128,128,	// 20
		0.75,1.25,0,		128,128,128,
		1.25,1.25,0,		128,128,128,
		1.35,1.25,0,		128,128,128,	// above door left
		1.65,1.25,0,		128,128,128,	// above door right
		1.75,1.25,0,		128,128,128,
		2.25,1.25,0,		128,128,128,
		2.75,1.25,0,		128,128,128,

		0.25,1.75,0,		128,128,128,
		0.75,1.75,0,		128,128,128,
		1.25,1.75,0,		128,128,128,	// 30
		1.75,1.75,0,		128,128,128,
		2.25,1.75,0,		128,128,128,
		2.75,1.75,0,		128,128,128,

		0.25,2.25,0,		128,128,128,
		0.75,2.25,0,		128,128,128,
		1.25,2.25,0,		128,128,128,
		1.75,2.25,0,		128,128,128,
		2.25,2.25,0,		128,128,128,
		2.75,2.25,0,		128,128,128,

		0.25,2.75,0,		128,128,128,	// 40
		0.75,2.75,0,		128,128,128,
		1.25,2.75,0,		128,128,128,
		1.75,2.75,0,		128,128,128,
		2.25,2.75,0,		128,128,128,
		2.75,2.75,0,		128,128,128,

		0,3,0,				128,128,128,	// 46
		0.25,3,0,			128,128,128,
		0.75,3,0,			128,128,128,
		1.25,3,0,			128,128,128,
		1.75,3,0,			128,128,128,	// 50
		2.25,3,0,			128,128,128,
		2.75,3,0,			128,128,128,
		3,3,0,				128,128,128,	// 53

		//   right face

		//3, 0, 0, already included as vertex 9
		3, 0, -0.25,		128,128,128,
		3, 0, -0.75,		128,128,128,
		3, 0, -1.25,		128,128,128,
		3, 0, -1.75,		128,128,128,
		3, 0, -2.25,		128,128,128,
		3, 0, -2.75,		128,128,128,
		3, 0, -3,			128,128,128,	// 60

		3, 0.25,-0.25,		128,128,128,
		3, 0.25,-0.75,		128,128,128,
		3, 0.25,-1.25,		128,128,128,
		3, 0.25,-1.75,		128,128,128,
		3, 0.25,-2.25,		128,128,128,
		3, 0.25,-2.75,		128,128,128,
		
		3, 0.75, -0.25,		128,128,128,
		3, 0.75, -0.75,		128,128,128,
		3, 0.75, -1.25,		128,128,128,
		3, 0.75, -1.75,		128,128,128,	// 70
		3, 0.75, -2.25,		128,128,128,
		3, 0.75, -2.75,		128,128,128,

		3, 1.25, -0.25,		128,128,128,
		3, 1.25, -0.75,		128,128,128,
		3, 1.25, -1.25,		128,128,128,
		3, 1.25, -1.75,		128,128,128,
		3, 1.25, -2.25,		128,128,128,
		3, 1.25, -2.75,		128,128,128,

		3, 1.75, -0.25,		128,128,128,
		3, 1.75, -0.75,		128,128,128,	// 80
		3, 1.75, -1.25,		128,128,128,
		3, 1.75, -1.75,		128,128,128,
		3, 1.75, -2.25,		128,128,128,
		3, 1.75, -2.75,		128,128,128,

		3, 2.25, -0.25,		128,128,128,
		3, 2.25, -0.75,		128,128,128,
		3, 2.25, -1.25,		128,128,128,
		3, 2.25, -1.75,		128,128,128,
		3, 2.25, -2.25,		128,128,128,
		3, 2.25, -2.75,		128,128,128,	// 90

		3, 2.75, -0.25,		128,128,128,
		3, 2.75, -0.75,		128,128,128,
		3, 2.75, -1.25,		128,128,128,
		3, 2.75, -1.75,		128,128,128,
		3, 2.75, -2.25,		128,128,128,
		3, 2.75, -2.75,		128,128,128,

		// 3, 3, 0, already included as vertex 53
		3, 3, -0.25,		128,128,128,
		3, 3, -0.75,		128,128,128,
		3, 3, -1.25,		128,128,128,
		3, 3, -1.75,		128,128,128,	// 100
		3, 3, -2.25,		128,128,128,
		3, 3, -2.75,		128,128,128,
		3, 3, -3,			128,128,128,	// 103

		//  back face

		//3   , 0, -3, already included as vertex 60
		2.75, 0, -3,		128,128,128,
		2.25, 0, -3,		128,128,128,
		1.75, 0, -3,		128,128,128,
		1.25, 0, -3,		128,128,128,
		0.75, 0, -3,		128,128,128,
		0.25, 0, -3,		128,128,128,
		0   , 0, -3,		128,128,128,	// 110

		2.75, 0.25, -3,		128,128,128,
		2.25, 0.25, -3,		128,128,128,
		1.75, 0.25, -3,		128,128,128,
		1.25, 0.25, -3,		128,128,128,
		0.75, 0.25, -3,		128,128,128,
		0.25, 0.25, -3,		128,128,128,
					
		2.75, 0.75, -3,		128,128,128,
		2.25, 0.75, -3,		128,128,128,
		1.75, 0.75, -3,		128,128,128,
		1.25, 0.75, -3,		128,128,128,	// 120
		0.75, 0.75, -3,		128,128,128,
		0.25, 0.75, -3,		128,128,128,
					
		2.75, 1.25, -3,		128,128,128,
		2.25, 1.25, -3,		128,128,128,
		1.75, 1.25, -3,		128,128,128,
		1.25, 1.25, -3,		128,128,128,
		0.75, 1.25, -3,		128,128,128,
		0.25, 1.25, -3,		128,128,128,
					
		2.75, 1.75, -3,		128,128,128,
		2.25, 1.75, -3,		128,128,128,	// 130
		1.75, 1.75, -3,		128,128,128,
		1.25, 1.75, -3,		128,128,128,
		0.75, 1.75, -3,		128,128,128,
		0.25, 1.75, -3,		128,128,128,

		2.75, 2.25, -3,		128,128,128,
		2.25, 2.25, -3,		128,128,128,
		1.75, 2.25, -3,		128,128,128,
		1.25, 2.25, -3,		128,128,128,
		0.75, 2.25, -3,		128,128,128,
		0.25, 2.25, -3,		128,128,128,	// 140
					
		2.75, 2.75, -3,		128,128,128,
		2.25, 2.75, -3,		128,128,128,
		1.75, 2.75, -3,		128,128,128,
		1.25, 2.75, -3,		128,128,128,
		0.75, 2.75, -3,		128,128,128,
		0.25, 2.75, -3,		128,128,128,

		// 3,3, -3, already included as vertex 103
		2.75, 3, -3,		128,128,128,
		2.25, 3, -3,		128,128,128,
		1.75, 3, -3,		128,128,128,
		1.25, 3, -3,		128,128,128,	// 150
		0.75, 3, -3,		128,128,128,
		0.25, 3, -3,		128,128,128,
		0, 3, -3,			128,128,128,	// 153

		//   left face

		// 0, 0, -3, already included as vertex 110
		0, 0, -2.75,		128,128,128,
		0, 0, -2.25,		128,128,128,
		0, 0, -1.75,		128,128,128,
		0, 0, -1.25,		128,128,128,
		0, 0, -0.75,		128,128,128,
		0, 0, -0.25,		128,128,128,
		// 0, 0, 0, already included as vertex 0

		0, 0.25, -2.75,		128,128,128,	// 160
		0, 0.25, -2.25,		128,128,128,
		0, 0.25, -1.75,		128,128,128,
		0, 0.25, -1.25,		128,128,128,
		0, 0.25, -0.75,		128,128,128,
		0, 0.25, -0.25,		128,128,128,

		0, 0.75, -2.75,		128,128,128,
		0, 0.75, -2.25,		128,128,128,
		0, 0.75, -1.75,		128,128,128,
		0, 0.75, -1.25,		128,128,128,
		0, 0.75, -0.75,		128,128,128,	// 170
		0, 0.75, -0.25,		128,128,128,

		0, 1.25, -2.75,		128,128,128,
		0, 1.25, -2.25,		128,128,128,
		0, 1.25, -1.75,		128,128,128,
		0, 1.25, -1.25,		128,128,128,
		0, 1.25, -0.75,		128,128,128,
		0, 1.25, -0.25,		128,128,128,

		0, 1.75, -2.75,		128,128,128,
		0, 1.75, -2.25,		128,128,128,
		0, 1.75, -1.75,		128,128,128,	// 180
		0, 1.75, -1.25,		128,128,128,
		0, 1.75, -0.75,		128,128,128,
		0, 1.75, -0.25,		128,128,128,

		0, 2.25, -2.75,		128,128,128,
		0, 2.25, -2.25,		128,128,128,
		0, 2.25, -1.75,		128,128,128,
		0, 2.25, -1.25,		128,128,128,
		0, 2.25, -0.75,		128,128,128,
		0, 2.25, -0.25,		128,128,128,

		0, 2.75, -2.75,		128,128,128,	// 190
		0, 2.75, -2.25,		128,128,128,
		0, 2.75, -1.75,		128,128,128,
		0, 2.75, -1.25,		128,128,128,
		0, 2.75, -0.75,		128,128,128,
		0, 2.75, -0.25,		128,128,128,

		// 0, 3, -3, already included as vertex 153
		0, 3, -2.75,		128,128,128,
		0, 3, -2.25,		128,128,128,
		0, 3, -1.75,		128,128,128,
		0, 3, -1.25,		128,128,128,
		0, 3, -0.75,		128,128,128,	// 200
		0, 3, -0.25,		128,128,128,
		// 0, 3, 0, already included as vertex 46

		// windows

		0.25, 0.25, 0,		128,255,255,	// 202
		0.75, 0.25, 0,		128,255,255,
		2.25, 0.25, 0,		128,255,255,
		2.75, 0.25, 0,		128,255,255,

		0.25, 0.75, 0,		128,255,255,	
		0.75, 0.75, 0,		128,255,255,
		2.25, 0.75, 0,		128,255,255,
		2.75, 0.75, 0,		128,255,255,

		0.25, 1.25, 0,		128,255,255,	// 210
		0.75, 1.25, 0,		128,255,255,
		1.25, 1.25, 0,		128,255,255,
		1.75, 1.25, 0,		128,255,255,
		2.25, 1.25, 0,		128,255,255,	
		2.75, 1.25, 0,		128,255,255,

		0.25, 1.75, 0,		128,255,255,	
		0.75, 1.75, 0,		128,255,255,
		1.25, 1.75, 0, 		128,255,255,
		1.75, 1.75, 0,		128,255,255,
		2.25, 1.75, 0,		128,255,255,	// 220
		2.75, 1.75, 0,		128,255,255,

		0.25, 2.25, 0,		128,255,255,	
		0.75, 2.25, 0,		128,255,255,
		1.25, 2.25, 0,		128,255,255,
		1.75, 2.25, 0,		128,255,255,
		2.25, 2.25, 0,		128,255,255,	
		2.75, 2.25, 0,		128,255,255,

		0.25, 2.75, 0,		128,255,255,	
		0.75, 2.75, 0,		128,255,255,
		1.25, 2.75, 0,		128,255,255,	// 230
		1.75, 2.75, 0,		128,255,255,
		2.25, 2.75, 0,		128,255,255,	
		2.75, 2.75, 0,		128,255,255,

				 //   right face

		3, 0.25, -0.25,		128,255,255,	
		3, 0.25, -0.75,		128,255,255,
		3, 0.25, -1.25,		128,255,255,
		3, 0.25, -1.75,		128,255,255,
		3, 0.25, -2.25,		128,255,255,	
		3, 0.25, -2.75,		128,255,255,

		3, 0.75, -0.25,		128,255,255,	// 240
		3, 0.75, -0.75,		128,255,255,
		3, 0.75, -1.25,		128,255,255,
		3, 0.75, -1.75,		128,255,255,
		3, 0.75, -2.25,		128,255,255,	
		3, 0.75, -2.75,		128,255,255,

		3, 1.25, -0.25,		128,255,255,	
		3, 1.25, -0.75,		128,255,255,
		3, 1.25, -1.25,		128,255,255,
		3, 1.25, -1.75,		128,255,255,
		3, 1.25, -2.25,		128,255,255,	// 250
		3, 1.25, -2.75,		128,255,255,

		3, 1.75, -0.25,		128,255,255,	
		3, 1.75, -0.75,		128,255,255,
		3, 1.75, -1.25,		128,255,255,
		3, 1.75, -1.75,		128,255,255,
		3, 1.75, -2.25,		128,255,255,	
		3, 1.75, -2.75,		128,255,255,

		3, 2.25, -0.25,		128,255,255,	
		3, 2.25, -0.75,		128,255,255,
		3, 2.25, -1.25,		128,255,255,	// 260
		3, 2.25, -1.75,		128,255,255,
		3, 2.25, -2.25,		128,255,255,	
		3, 2.25, -2.75,		128,255,255,

		3, 2.75, -0.25,		128,255,255,	
		3, 2.75, -0.75,		128,255,255,
		3, 2.75, -1.25,		128,255,255,
		3, 2.75, -1.75,		128,255,255,
		3, 2.75, -2.25,		128,255,255,	
		3, 2.75, -2.75,		128,255,255,

				  //  back face

		2.75, 0.25, -3,		128,255,255,	// 270
		2.25, 0.25, -3,		128,255,255,
		1.75, 0.25, -3,		128,255,255,
		1.25, 0.25, -3,		128,255,255,
		0.75, 0.25, -3,		128,255,255,	
		0.25, 0.25, -3,		128,255,255,

		2.75, 0.75, -3,		128,255,255,	
		2.25, 0.75, -3,		128,255,255,
		1.75, 0.75, -3,		128,255,255,
		1.25, 0.75, -3,		128,255,255,
		0.75, 0.75, -3,		128,255,255,	// 280
		0.25, 0.75, -3,		128,255,255,

		2.75, 1.25, -3,		128,255,255,	
		2.25, 1.25, -3,		128,255,255,
		1.75, 1.25, -3,		128,255,255,
		1.25, 1.25, -3,		128,255,255,
		0.75, 1.25, -3,		128,255,255,	
		0.25, 1.25, -3,		128,255,255,

		2.75, 1.75, -3,		128,255,255,	
		2.25, 1.75, -3,		128,255,255,
		1.75, 1.75, -3,		128,255,255,	// 290
		1.25, 1.75, -3,		128,255,255,
		0.75, 1.75, -3,		128,255,255,	
		0.25, 1.75, -3,		128,255,255,

		2.75, 2.25, -3,		128,255,255,	
		2.25, 2.25, -3,		128,255,255,
		1.75, 2.25, -3,		128,255,255,
		1.25, 2.25, -3,		128,255,255,
		0.75, 2.25, -3,		128,255,255,	
		0.25, 2.25, -3,		128,255,255,

		2.75, 2.75, -3,		128,255,255,	// 300
		2.25, 2.75, -3,		128,255,255,
		1.75, 2.75, -3,		128,255,255,
		1.25, 2.75, -3,		128,255,255,
		0.75, 2.75, -3,		128,255,255,	
		0.25, 2.75, -3,		128,255,255,

				  // left face

		0, 0.25, -2.75,		128,255,255,	
		0, 0.25, -2.25,		128,255,255,
		0, 0.25, -1.75,		128,255,255,
		0, 0.25, -1.25,		128,255,255,
		0, 0.25, -0.75,		128,255,255,	// 310
		0, 0.25, -0.25,		128,255,255,

		0, 0.75, -2.75,		128,255,255,	
		0, 0.75, -2.25,		128,255,255,
		0, 0.75, -1.75,		128,255,255,
		0, 0.75, -1.25,		128,255,255,
		0, 0.75, -0.75,		128,255,255,	
		0, 0.75, -0.25,		128,255,255,

		0, 1.25, -2.75,		128,255,255,	
		0, 1.25, -2.25,		128,255,255,
		0, 1.25, -1.75,		128,255,255,	// 320
		0, 1.25, -1.25,		128,255,255,
		0, 1.25, -0.75,		128,255,255,	
		0, 1.25, -0.25,		128,255,255,

		0, 1.75, -2.75,		128,255,255,	
		0, 1.75, -2.25,		128,255,255,
		0, 1.75, -1.75,		128,255,255,
		0, 1.75, -1.25,		128,255,255,
		0, 1.75, -0.75,		128,255,255,	
		0, 1.75, -0.25,		128,255,255,

		0, 2.25, -2.75,		128,255,255,	// 330
		0, 2.25, -2.25,		128,255,255,
		0, 2.25, -1.75,		128,255,255,
		0, 2.25, -1.25,		128,255,255,
		0, 2.25, -0.75,		128,255,255,	
		0, 2.25, -0.25,		128,255,255,

		0, 2.75, -2.75,		128,255,255,	
		0, 2.75, -2.25,		128,255,255,
		0, 2.75, -1.75,		128,255,255,
		0, 2.75, -1.25,		128,255,255,
		0, 2.75, -0.75,		128,255,255,	// 340
		0, 2.75, -0.25,		128,255,255,

		// door
		1.35,0,0,			128,64,0,		// 342
		1.65,0,0,			128,64,0,
		1.35,0.85,0,		128,64,0,
		1.65,0.85,0,		128,64,0,		// 345

	}; 


	// indices useful for EBOs, especially when reusing vertices
	GLint indices[] = {
		// front face walls
		0,46,47,		//1
		0,1,47,

		1,10,11,
		1,2,11,
		14,20,21,
		14,15,21,
		28,34,35,
		28,29,35,
		40,47,48,
		40,41,48,		//10

		2,48,49,
		2,3,49,

		// around door
		3,22,23,
		3,4,23,
		18,23,24,
		18,19,24,
		5,24,25,
		5,6,25,

		30,36,37,
		30,31,37,		//20
		42,49,50,
		42,43,50,

		6,50,51,
		6,7,51,

		7,12,13,
		7,8,13,
		16,26,27,
		16,17,27,
		32,38,39,
		32,33,39,		//30
		44,51,52,
		44,45,52,

		8,52,53,
		8,9,53,

		// right wall

		9,53,97,
		9,54,97,
		
		54,61,62,
		54,55,62,
		67,73,74,
		67,68,74,
		79,85,86,
		79,80,86,
		91,97,98,
		91,92,98,

		55,98,99,
		55,56,99,

		56,63,64,
		56,57,64,
		69,75,76,
		69,70,76,
		81,87,88,
		81,82,88,
		93,99,100,
		93,94,100,

		57,100,101,
		57,58,101,

		58,65,66,
		58,59,66,
		71,77,78,
		71,72,78,
		83,89,90,
		83,84,90,
		95,101,102,
		95,96,102,
		
		59,102,103,
		59,60,103,

		// back wall

		60,103,147,
		60,104,147,

		104,111,112,
		104,105,112,
		117,123,124,
		117,118,124,
		129,135,136,
		129,130,136,
		141,147,148,
		141,142,148,

		105,148,149,
		105,106,149,

		106,113,114,
		106,107,114,
		119,125,126,
		119,120,126,
		131,137,138,
		131,132,138,
		143,149,150,
		143,144,150,

		107,150,151,
		107,108,151,

		108,115,116,
		108,109,116,
		121,127,128,
		121,122,128,
		133,139,140,
		133,134,140,
		145,151,152,
		145,146,152,

		109,152,153,
		109,110,153,

		// left wall

		110,153,196,
		110,154,196,

		154,160,161,
		154,155,161,
		166,172,173,
		166,167,173,
		178,184,185,
		178,179,185,
		190,196,197,
		190,191,197,

		155,197,198,
		155,156,198,

		156,162,163,
		156,157,163,
		168,174,175,
		168,169,175,
		180,186,187,
		180,181,187,
		192,198,199,
		192,193,199,

		157,199,200,
		157,158,200,

		158,164,165,
		158,159,165,
		170,176,177,
		170,171,177,
		182,188,189,
		182,183,189,
		194,200,201,
		194,195,201,

		159,201,46,
		159,0,46,

		// bottom

		0,110,60,
		0,9,60,

		// top

		46,153,103,
		46,53,103,

		// windows
		// front wall

		202,206,207,
		202,203,207,
		210,216,217,
		210,211,217,
		222,228,229,
		222,223,229,

		212,218,219,
		212,213,219,
		224,230,231,
		224,225,231,

		204,208,209,
		204,205,209,
		214,220,221,
		214,215,221,
		226,232,233,
		226,227,233,

		// right wall

		234,240,241,
		234,235,241,
		246,252,253,
		246,247,253,
		258,264,265,
		258,259,265,

		236,242,243,
		236,237,243,
		248,254,255,
		248,249,255,
		260,266,267,
		260,261,267,

		238,244,245,
		238,239,245,
		250,256,257,
		250,251,257,
		262,268,269,
		262,263,269,

		// back wall

		270,276,277,
		270,271,277,
		282,288,289,
		282,283,289,
		294,300,301,
		294,295,301,

		272,278,279,
		272,273,279,
		284,290,291,
		284,285,291,
		296,302,303,
		296,297,303,

		274,280,281,
		274,275,281,
		286,292,293,
		286,287,293,
		298,304,305,
		298,299,305,

		// left wall

		306,312,313,
		306,307,313,
		318,324,325,
		318,319,325,
		330,336,337,
		330,331,337,

		308,314,315,
		308,309,315,
		320,326,327,
		320,321,327,
		332,338,339,
		332,333,339,

		310,316,317,
		310,311,317,
		322,328,329,
		322,323,329,
		334,340,341,
		334,335,341,
		
		// door

		342,344,345,
		342,343,345


	};


	GLuint VBO; // vertex buffer object
	glGenBuffers(1, &VBO); // generate buffer ID
	GLuint VAO; // vertex array object
	glGenVertexArrays(1, &VAO); // generate vertex ID
	GLuint EBO; // element buffer object to avoid storing repeated vertices
	glGenBuffers(1, &EBO); 


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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 3: set vertex position attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 3.5: set vertex color or texture attribute pointers
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// 4: unbind VAO (NOT the EBO)
	glBindVertexArray(0);


	
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
		// movement update
		movement();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Activate shader (cannot send uniform data before using the shader)
		exampleShader.use();

		// send data to shader
		// model : position in world coordinates
		glm::mat4 model;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		//model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(glGetUniformLocation(exampleShader.program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		// view : what the camera sees
		glm::mat4 view;
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(exampleShader.program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		// projection : projecting into 2d window
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.zoom), (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(exampleShader.program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		// draw triangles
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(vertices), GL_UNSIGNED_INT, 0);

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
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		if (wireframeMode == false) {
			wireframeMode = true;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
		}
		else {
			wireframeMode = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // wireframe mode
		}
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		if (lockCamera == true) lockCamera = false;
		else lockCamera = true;
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
	if (lockCamera == false) {
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}