//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * OpenGLContext.h
 *
 *  Created on: Nov 30, 2020
 *      Author: gio
 */
#pragma once
#ifndef OPENGLCONTEXT_H_
#define OPENGLCONTEXT_H_

#include <unordered_map>

#include "CommonHeaders.h"
#include "ShaderMap.h"

/*
 * Create a map of OpenglContex with GLFW
 */
class OpenGLContext {

private:

	static std::string current_context;
	//Context Map
	static std::unordered_map<std::string, GLFWwindow*> context_map;

	//CallBacks Maps
	static void reshape(GLFWwindow*, int width, int height);
	static std::unordered_map<std::string, void(*)(GLFWwindow*, int,int)> reshape_callback_map;

	static void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	static std::unordered_map<std::string, void(*)(GLFWwindow*, int, int, int, int)> keyboard_callback_map;

	static void mouseCursorPos(GLFWwindow* window, double xpos, double ypos);
	static std::unordered_map<std::string, void(*)(GLFWwindow*, double, double)> mouseCursorPos_callback_map;

	static void mouseScroll(GLFWwindow* window, double xoffset, double yoffset);
	static std::unordered_map<std::string, void(*)(GLFWwindow*, double, double)> mouseScroll_callback_map;

	static void mouseCursorEnter(GLFWwindow* window, int entered);
	static std::unordered_map<std::string, void(*)(GLFWwindow*, int)> mouseCursorEnter_callback_map;

	static void mouseButton(GLFWwindow* window, int button, int action, int mods);
	static std::unordered_map<std::string, void(*)(GLFWwindow*, int, int, int)> mouseButton_callback_map;

	static void initOpengl(std::string name);

	OpenGLContext();
	OpenGLContext(const OpenGLContext &other) = delete;
	OpenGLContext(OpenGLContext &&other) = delete;
	OpenGLContext& operator=(const OpenGLContext &other) = delete;
	OpenGLContext& operator=(OpenGLContext &&other) = delete;
public:


	virtual ~OpenGLContext();

	static void createContex(std::string name, unsigned int width, unsigned int height, GLFWmonitor* monitor = NULL,
										bool visible = true, bool decorated = true, bool trasparentFrameBuffer = false);

	static void makecurrent(std::string name);
	static void releaseContex();

	static GLFWwindow* getCurrent();
	static GLFWwindow* getContext(std::string name);

	static void swapBuffers();

	static void destroy();
	static void destroy(std::string name);
	static void destroyAll();

	static void visible();
	static void visible(std::string name);
	static bool isVisible();
	static bool isVisible(std::string name);
	static void hide();
	static void hide(std::string name);

	static void setUserPointer(std::string name, void* p);
	static void setUserPointer(void* p);
	static void* getUserPointer(std::string name);
	static void* getUserPointer();

	static void setWindowSize(int w, int h);
	static void setWindowSize(std::string name, int w, int h);

	static void setWindowPosition(int x, int y);
	static void setWindowPosition(std::string name, int x, int y);
	static glm::ivec2 getWindowPosition();
	static glm::ivec2 getWindowPosition(std::string name);

	static void setWindowShouldClose(std::string name, int val);
	static void setWindowShouldClose(int val);

	static void setReshape(void (*pf)(GLFWwindow*, int width, int height));
	static void setReshape(std::string name, void (*pf)(GLFWwindow*, int width, int height));

	static void setKeyboard(void (*pf)(GLFWwindow*, int key, int scancode, int action, int mods));
	static void setKeyboard(std::string name, void (*pf)(GLFWwindow*, int key, int scancode, int action, int mods));

	static void setMouseCursorPos(void (*pf)(GLFWwindow* window, double xpos, double ypos));
	static void setMouseCursorPos(std::string name, void (*pf)(GLFWwindow* window, double xpos, double ypos));

	static void setMouseScroll(void(*pf)(GLFWwindow* window, double xoffset, double yoffset));
	static void setMouseScroll(std::string name, void(*pf)(GLFWwindow* window, double xoffset, double yoffset));

	static void setMouseCursorEnter(void(*pf)(GLFWwindow* window, int entered));
	static void setMouseCursorEnter(std::string name, void (*pf)(GLFWwindow* window, int entered));

	static void setMouseButton(void(*pf)(GLFWwindow* window, int button, int action, int mods));
	static void setMouseButton(std::string name, void(*pf)(GLFWwindow* window, int button, int action, int mods));

};

#endif /* OPENGLCONTEXT_H_ */




