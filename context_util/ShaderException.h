//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * ShaderException.h
 *
 *  Created on: Nov 17, 2020
 *      Author: gio
 */
#pragma once
#ifndef SHADEREXCEPTION_H_
#define SHADEREXCEPTION_H_

#include <exception>
#include <cstring>

class ShaderException: public std::exception {
private:
	char* problem = 0;
public:
	ShaderException(const char* p) {
		problem = new char[strlen(p)];
		memcpy(problem, p, strlen(p));
	}

	~ShaderException() {
		if(problem)delete problem;
	}

	ShaderException(const ShaderException &other){
		problem = new char[strlen(other.problem)];
		memcpy(problem, other.problem, strlen(other.problem));
	}

	ShaderException(ShaderException &&other) {
		problem = std::move(other.problem);
		other.problem=0;
	}

	ShaderException& operator=(const ShaderException &other) = delete;
	ShaderException& operator=(ShaderException &&other) = delete;

	virtual const char* what() const throw () {
		return problem;
	}
};

#endif /* SHADEREXCEPTION_H_ */
