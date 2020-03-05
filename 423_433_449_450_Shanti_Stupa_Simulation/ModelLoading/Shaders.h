#pragma once
#include<gl\glew.h>
#include<string>
#include<iostream>
class Shaders
{
public:
	Shaders();
	void LoadFromFile(GLuint Shader, const std::string &fileName);
	void compileShader(GLuint Shader, const std::string &source);
	void useProgram();
	void unUseProgram();
	void createProgram();
	GLuint getProgramID();
	~Shaders();
private:
	enum ShaderType { vertexShader = 0, fragmentShader = 1 };
	GLuint _shaders[2];
	GLuint tot_shaders;
	GLuint _program;
};

