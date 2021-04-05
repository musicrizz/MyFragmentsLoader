//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * ShaderMap.cpp
 *
 *  Created on: Nov 21, 2020
 *      Author: gio
 */

#include "ShaderMap.h"

Program* ShaderMap::current;
std::unordered_map<std::string, Program*> ShaderMap::map;
ShaderMap::ShaderMap() {
}

ShaderMap::~ShaderMap() {
	//useless
//	for(auto p : map)  {
//		if(p.second) {
//			glDeleteProgram(p.second->getID());
//			delete p.second;
//		}
//	}
//	map.clear();
}

bool ShaderMap::createProgram(std::string name, const char *vertexPath, const char *fragmentPath) {
	try {
		Program* p = new Program(name, vertexPath, fragmentPath);
		if (p->getID() > 0) {
			map.insert(std::pair<std::string, Program*>(name, p));
			return true;
		}
	} catch (ShaderException &e) {
		//fprintf(stderr,"Create Program %s ShaderException: \n %s \n", name.c_str(), e.what());
		throw e;
	}

	return false;
}

void ShaderMap::useProgram(std::string name)  {
	Program* pgr = getProgram(name);
	if(pgr->getID() > 0) {
		glUseProgram(pgr->getID());
		current = pgr;
	}
}

Program* ShaderMap::getProgram(std::string name)  {
	try {
		return map.at(name);
	} catch (const std::out_of_range &oor) {}
	return nullptr;
}

unsigned int ShaderMap::getProgramID(std::string name) {
	return getProgram(name)->getID();
}

void ShaderMap::program_null()  {
	glUseProgram(0);
}

Program* ShaderMap::getCurrentProgram()  {
	return current;
}

unsigned int ShaderMap::getCurrentProgramID()  {
	return current->getID();
}

int ShaderMap::getCountPrograms()  {
	return map.size();
}

int ShaderMap::getUniformLocation(std::string program, std::string var_name)   {
	int p = getProgram(program)->getID();
	if(p > 0)  {
		return glGetUniformLocation(p, var_name.c_str());
	}
	return -1;
}

int ShaderMap::getUniformLocation(std::string var_name)   {
	if(current->getID() > 0)  {
		return glGetUniformLocation(current->getID(), var_name.c_str());
	}
	return -1;
}

int ShaderMap::getUniformBlockIndex(std::string program, std::string var_name) {
	int p = getProgram(program)->getID();
	if (p > 0) {
		return glGetUniformBlockIndex(p, var_name.c_str());
	}
	return -1;
}

int ShaderMap::getUniformBlockIndex(std::string var_name) {
	if (current->getID() > 0) {
		return glGetUniformBlockIndex(current->getID(), var_name.c_str());
	}
	return -1;
}

void ShaderMap::bindingUniformBlocks(std::string var_name, unsigned int biding_point)   {
	for(auto p : map)  {
		if(p.second->getBindingPoint() == biding_point) {
			glUniformBlockBinding(p.second->getID(), getUniformBlockIndex(p.first, var_name), biding_point);
		}
	}
}

int ShaderMap::getAttributeLocation(std::string program, std::string var_name) {
	int p = getProgram(program)->getID();
	if (p > 0) {
		return glGetAttribLocation(p, var_name.c_str());
	}
	return -1;
}

int ShaderMap::getAttributeLocation(std::string var_name) {
	if (current->getID()) {
		return glGetAttribLocation(current->getID(), var_name.c_str());
	}
	return -1;
}


