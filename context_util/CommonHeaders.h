//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * CommonHeaders.h
 *
 *  Created on: Nov 18, 2020
 *      Author: musicrizz
 */

#pragma once
#ifndef COMMONHEADERS_H_
#define COMMONHEADERS_H_

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include <thread>
#include <mutex>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Set to 1 to enable OpenGl Debug Message
#define DEVELOPEMENT 0


#endif /* COMMONHEADERS_H_ */
