//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * Program.cpp
 *
 *  Created on: Nov 17, 2020
 *      Author: gio
 */

#include "Program.h"
Program::Program()  {
	this->name = "";
	this->ID = -1;
	this->_binding_point = -1;
}

Program::Program(std::string name, const char *vertexPath, const char *fragmentPath) {
	this->name = name;
	this->ID = 0;
	this->_binding_point = -1;

	std::string vertex_src, fragment_src;

	vertex_src = loadsource(vertexPath);
	fragment_src = loadsource(fragmentPath);

	unsigned int vertexID = compileShader(GL_VERTEX_SHADER, vertex_src.c_str(), vertexPath);
	unsigned int fragmentID = compileShader(GL_FRAGMENT_SHADER, fragment_src.c_str(), fragmentPath);

	unsigned int tmp_prg_ID = glCreateProgram();
	glAttachShader(tmp_prg_ID, vertexID);
	glAttachShader(tmp_prg_ID, fragmentID);
	glLinkProgram(tmp_prg_ID);
	checkLinkedProgram(tmp_prg_ID);

	this->ID = tmp_prg_ID;
	glDetachShader(tmp_prg_ID, vertexID);
	glDetachShader(tmp_prg_ID, fragmentID);
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

}

unsigned int Program::compileShader(GLenum type, const char *src, const char *path) {
	std::string shader_type;
	switch (type) {
	case GL_VERTEX_SHADER:
		shader_type = "VERTEX_SHADER";
		break;
	case GL_FRAGMENT_SHADER:
		shader_type = "FRAGMENT_SHADER";
		break;
	default : return 0;
	}
	unsigned int id = glCreateShader(type);
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);
	checkCompiledShader(path, id, shader_type);
	return id;
}

std::string Program::loadsource(const char *source) {
	std::stringstream result;
	try {
		std::ifstream in;
		in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		in.open(source);
		result << in.rdbuf();
		in.close();
	} catch (std::ifstream::failure &e) {
		std::string error = "Eccezine lettura File : ";
		error.append(source).append(" : ");
		error.append(e.what());
		throw ShaderException(error.c_str());
	}
	return result.str();
}

Program::~Program() {}

Program::Program(const Program &other) {
	this->ID = other.ID;
	this->name = other.name;
	this->_binding_point = other._binding_point;
}

Program& Program::operator=(const Program &other) {
	if (this != &other) {
		this->ID = other.ID;
		this->name = other.name;
		this->_binding_point = other._binding_point;
	}
	return *this;
}

unsigned int Program::getID() {
	return this->ID;
}

bool Program::isLinked() {
	return this->ID > 0;
}

bool Program::checkCompiledShader(const GLchar *shaderFileName, GLuint shaderID, std::string type) {
	GLint compiled;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
		GLchar *log = new GLchar[len + 1];
		glGetShaderInfoLog(shaderID, len, &len, log);
		std::string error;
		error.append(type).append(" : ").append(shaderFileName)
				.append(" Log : ").append(log);
		throw ShaderException(error.c_str());
	}
	return true;
}

bool Program::checkLinkedProgram(unsigned int pId) {
	GLint linked;
	glGetProgramiv(pId, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLsizei len;
		glGetProgramiv(pId, GL_INFO_LOG_LENGTH, &len);
		GLchar *log = new GLchar[len + 1];
		glGetProgramInfoLog(pId, len, &len, log);
		std::string error = "Program " + this->name + "Link Fail : " + log;
		throw ShaderException(error.c_str());
	}
	return true;
}

void Program::setBindingPoint(unsigned int p)   {
	this->_binding_point = p;
}

unsigned int Program::getBindingPoint()  {
	return this->_binding_point;
}












