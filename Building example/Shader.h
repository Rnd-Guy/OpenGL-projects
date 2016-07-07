#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GLEW/glew.h> // Include glew to get all the required OpenGL headers

// to use the shader class to load external shaders:
// 1. call constructor
//		Shader shaderName("path/to/shader.vert", "path/to/shader.frag");
// 2. use shader program by calling the .use() function
// while (...) {
//     ourShader.use();
//     glUniform1f(glGetUniformLocation(ourShader.program, "someUniform"), 1.0f);
//     DrawStuff();
// }


class Shader
{
public:
	// The program ID
	GLuint program;
	// Constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {

		// 1. Retrieve vertex/fragment source code from filepath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensures ifstream objects can throw exceptions
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		try {
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream into GLchar array
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e){
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();

		// 2: Compile shaders
		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[512];

		// Vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		};
		
		// Fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		};

		// Shader program
		this->program = glCreateProgram();
		glAttachShader(this->program, vertex);
		glAttachShader(this->program, fragment);
		glLinkProgram(this->program);
		// Print linking errors if any
		glGetProgramiv(this->program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		};

		// Delete shaders as they have been linked now and no longer necesary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
		
		
		;
	// Use the program
	void use() { glUseProgram(this->program); }
};