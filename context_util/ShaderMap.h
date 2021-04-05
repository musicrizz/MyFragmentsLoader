//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * ShaderMap.h
 *
 *  Created on: Nov 21, 2020
 *      Author: gio
 */
#pragma once
#ifndef SHADERMAP_H_
#define SHADERMAP_H_

#include <unordered_map>

#include "CommonHeaders.h"
#include "ShaderException.h"
#include "Program.h"

class ShaderMap {
private:
	static Program* current;
	static std::unordered_map<std::string, Program*> map;

	ShaderMap();
	ShaderMap(const ShaderMap &other) = delete;
	ShaderMap(ShaderMap &&other) = delete;
	ShaderMap& operator=(const ShaderMap &other) = delete;
	ShaderMap& operator=(ShaderMap &&other) = delete;
public:

	virtual ~ShaderMap();

	static bool createProgram(std::string name, const char* vertexPath, const char* fragmentPath);

	static void useProgram(std::string name);
	static void program_null();//set glUseProgram to 0;

	static Program* getProgram(std::string name);
	static unsigned int getProgramID(std::string name);

	static Program* getCurrentProgram();
	static unsigned int getCurrentProgramID();

	static int getCountPrograms();

	static int getUniformLocation(std::string program, std::string var_name);
	static int getUniformLocation(std::string var_name);

	static int getUniformBlockIndex(std::string program, std::string var_name);
	static int getUniformBlockIndex(std::string var_name);

	static void bindingUniformBlocks(std::string var_name, unsigned int biding_point);

	static int getAttributeLocation(std::string program, std::string var_name);
	static int getAttributeLocation(std::string var_name);

};

#endif /* SHADERMAP_H_ */
