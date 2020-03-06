#include "Shaders.h"
#include<iostream>
#include<fstream>
#include<string>

Shaders::Shaders()
{
	this->tot_shaders = 0;
	this->_shaders[vertexShader] = 0;
	this->_shaders[fragmentShader] = 0;
}


Shaders::~Shaders()
{
}
void Shaders::LoadFromFile(GLuint Shader, const std::string &fileName)
{
	std::ifstream fp;
	fp.open(fileName.c_str(), std::ios_base::in);
	if (fp) {
		std::string str, buffer;
		while (std::getline(fp, str)) {
			buffer.append(str);
			buffer.append("\r\n");
		};
		compileShader(Shader, buffer);
	}
	else std::cout << "Error loading from file + " << fileName;

}

void Shaders::compileShader(GLuint Shader, const std::string& source)
{
	unsigned int shader;
	shader = glCreateShader(Shader);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, NULL);
	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		std::cout << infoLog << std::endl;
		delete[] infoLog;
	}
	_shaders[tot_shaders++] = shader;
}

void Shaders::useProgram()
{
	glUseProgram(_program);
}

void Shaders::unUseProgram()
{
	glUseProgram(0);
}

void Shaders::createProgram()
{
	_program = glCreateProgram();
	if (_shaders[vertexShader] != 0) {
		glAttachShader(_program, _shaders[vertexShader]);
	}
	if (_shaders[fragmentShader] != 0) {
		glAttachShader(_program, _shaders[fragmentShader]);
	}
	glLinkProgram(_program);
	GLint status;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(_program, infoLogLength, NULL, infoLog);
		std::cout << infoLog << std::endl;
		delete[] infoLog;
	}

	glDeleteShader(_shaders[vertexShader]);
	glDeleteShader(_shaders[fragmentShader]);
}

GLuint Shaders::getProgramID()
{
	return this->_program;
}

