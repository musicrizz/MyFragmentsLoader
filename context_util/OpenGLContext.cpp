//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * OpenglCotext.cpp
 *
 *  Created on: Dec 3, 2020
 *      Author: gio
 */
#include "OpenGLContext.h"
#include "OpenGLerror.h"

std::string OpenGLContext::current_context;
std::unordered_map<std::string,  GLFWwindow*> OpenGLContext::context_map;

std::unordered_map<std::string, void (*)(GLFWwindow*, int, int)> OpenGLContext::reshape_callback_map;
std::unordered_map<std::string, void (*)(GLFWwindow*, int, int, int, int)> OpenGLContext::keyboard_callback_map;
std::unordered_map<std::string, void (*)(GLFWwindow*, double, double)> OpenGLContext::mouseCursorPos_callback_map;
std::unordered_map<std::string, void (*)(GLFWwindow*, double, double)> OpenGLContext::mouseScroll_callback_map;
std::unordered_map<std::string, void (*)(GLFWwindow*, int)> OpenGLContext::mouseCursorEnter_callback_map;
std::unordered_map<std::string, void (*)(GLFWwindow*, int, int, int)> OpenGLContext::mouseButton_callback_map;
OpenGLContext::OpenGLContext() {
	current_context = "";
}

void OpenGLContext::createContex(std::string name, unsigned int width, unsigned int height, GLFWmonitor *monitor, bool visible, bool decorated, bool trasparentFrameBuffer) {

	if(name.size() == 0) return;

	current_context = "";
	GLFWwindow* window;

	visible   ? glfwWindowHint(GLFW_VISIBLE,   GLFW_TRUE) : glfwWindowHint(GLFW_VISIBLE,   GLFW_FALSE);
	decorated ? glfwWindowHint(GLFW_DECORATED, GLFW_TRUE) : glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	trasparentFrameBuffer ? glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE) : glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);

	window = glfwCreateWindow(width, height, name.c_str(), monitor, NULL);

	if (window == NULL) {
	        fprintf(stderr,"Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
	        glfwTerminate();
	        exit(EXIT_FAILURE);
	}

	glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetKeyCallback(window,         keyboard);
	glfwSetCursorPosCallback(window,   mouseCursorPos);
	glfwSetScrollCallback(window,      mouseScroll);
	glfwSetCursorEnterCallback(window, mouseCursorEnter);
	glfwSetMouseButtonCallback(window, mouseButton);

	glfwMakeContextCurrent(window);
	initOpengl(name);
	glfwMakeContextCurrent(NULL);

	context_map.insert(std::pair<std::string, GLFWwindow*>(name, window));

}

OpenGLContext::~OpenGLContext() {
	for(auto c : context_map)  {
		if(c.second) glfwDestroyWindow(c.second);
	}
}

void OpenGLContext::makecurrent(std::string name) {
	if(name.size() == 0)  {
		glfwMakeContextCurrent(NULL);
		return;
	}
	try {
		GLFWwindow* w = context_map.at(name);
		current_context = name;
		glfwMakeContextCurrent(w);
		glfwSwapInterval(1);
	} catch (const std::out_of_range &oor) {}
}

void OpenGLContext::releaseContex() {
	glfwMakeContextCurrent(NULL);
}

GLFWwindow* OpenGLContext::getCurrent() {
	try {
		return context_map.at(current_context);
	} catch (const std::out_of_range &oor) {}
	return 0;
}

GLFWwindow* OpenGLContext::getContext(std::string name) {
	try {
		return context_map.at(name);
	} catch (const std::out_of_range &oor) {}
	return nullptr;
}

void OpenGLContext::swapBuffers() {
	glfwSwapBuffers(OpenGLContext::getCurrent());
}

void OpenGLContext::destroy()   {
	glfwDestroyWindow(getCurrent());
	context_map.erase(current_context);
	current_context = "";
	releaseContex();
}

void OpenGLContext::destroy(std::string name)   {
	glfwDestroyWindow(getContext(name));
	context_map.erase(name);
}

void OpenGLContext::destroyAll()   {
	for(auto c : context_map)   {
		glfwDestroyWindow(c.second);
	}
	context_map.clear();
	current_context = "";
}

void OpenGLContext::setUserPointer(std::string name, void* p) {
	GLFWwindow *w = getContext(name);
	if (w) {
		try {
			glfwSetWindowUserPointer(w, p);
		} catch (const std::out_of_range &oor) {}
	}
}
void OpenGLContext::setUserPointer(void *p) {
	GLFWwindow *w = getCurrent();
	if (w) {
		try {
			glfwSetWindowUserPointer(w, p);
		} catch (const std::out_of_range &oor) {}
	}
}

void* OpenGLContext::getUserPointer(std::string name) {
	GLFWwindow *w = getContext(name);
	if (w) {
		try {
			return glfwGetWindowUserPointer(w);
		} catch (const std::out_of_range &oor) {
		}
	}
	return nullptr;
}

void* OpenGLContext::getUserPointer() {
	GLFWwindow *w = getCurrent();
	if (w) {
		try {
			return glfwGetWindowUserPointer(w);
		} catch (const std::out_of_range &oor) {
		}
	}
	return nullptr;
}

void OpenGLContext::visible() {
	if(glfwGetWindowAttrib(getCurrent(), GLFW_VISIBLE)) return;
	glfwShowWindow(getCurrent());
}

void OpenGLContext::visible(std::string name) {
	if(glfwGetWindowAttrib(getContext(name), GLFW_VISIBLE)) return;
	glfwShowWindow(getContext(name));
}

bool OpenGLContext::isVisible()  {
	return glfwGetWindowAttrib(getCurrent(), GLFW_VISIBLE);
}

bool OpenGLContext::isVisible(std::string name)  {
	return glfwGetWindowAttrib(getContext(name), GLFW_VISIBLE);
}

void OpenGLContext::hide() {
	if (glfwGetWindowAttrib(getCurrent(), GLFW_VISIBLE)) {
		glfwHideWindow(getCurrent());
	}
}

void OpenGLContext::hide(std::string name)  {
	if (glfwGetWindowAttrib(getCurrent(), GLFW_VISIBLE)) {
		glfwHideWindow(getContext(name));
	}
}

void OpenGLContext::setWindowPosition(int x, int y)   {
	GLFWwindow* w = getCurrent();
	if(w) {
		glfwSetWindowPos(w, x, y);
	}
}

void OpenGLContext::setWindowPosition(std::string name, int x, int y)   {
	GLFWwindow* w = getContext(name);
	if(w) {
		glfwSetWindowPos(w, x, y);
	}
}

void OpenGLContext::setWindowSize(int w, int h) {
	GLFWwindow *win = getCurrent();
	if (win) {
		glfwSetWindowSize(win, w, h);
	}
}

void OpenGLContext::setWindowSize(std::string name, int w, int h)  {
	GLFWwindow *win = getContext(name);
	if (win) {
		glfwSetWindowSize(win, w, h);
	}
}

glm::ivec2 OpenGLContext::getWindowPosition()   {
	int x, y;
	glfwGetWindowPos(getCurrent(), &x, &y);
	return glm::ivec2(x,y);

}

glm::ivec2 OpenGLContext::getWindowPosition(std::string name)   {
	int x, y;
	glfwGetWindowPos(getContext(name), &x, &y);
	return glm::ivec2(x,y);
}

void OpenGLContext::setWindowShouldClose(std::string name, int val) {
	GLFWwindow *win = getContext(name);
	if (win) {
		glfwSetWindowShouldClose(getCurrent(), val);
	}
}
void OpenGLContext::setWindowShouldClose(int val) {
	glfwSetWindowShouldClose(getCurrent(), val);
}

//-----------------------------------------------
//RESHAPE CALLBACK

void OpenGLContext::setReshape(void (*pf)(GLFWwindow*, int width, int height)) {
	if (current_context.size() > 0) {
		setReshape(current_context, pf);
	}
}

void OpenGLContext::setReshape(std::string name, void (*pf)(GLFWwindow*, int width, int height)) {
	reshape_callback_map.insert(std::pair<std::string, void (*)(GLFWwindow*, int, int)>(name, pf));
}

void OpenGLContext::reshape(GLFWwindow*, int width, int height) {
	if (reshape_callback_map.empty())	return;
	GLFWwindow *cx = getCurrent();
	if (cx) {
		try {
			reshape_callback_map.at(current_context)(cx, width, height);
		} catch (const std::out_of_range &oor) {}
	}
}

//-----------------------------------------------

//KEYBOARD CALLBACK
void OpenGLContext::setKeyboard(void (*pf)(GLFWwindow*, int key, int scancode, int action, int mods))  {
	if (current_context.size() > 0) {
		setKeyboard(current_context, pf);
	}
}

void OpenGLContext::setKeyboard(std::string name, void (*pf)(GLFWwindow*, int key, int scancode, int action, int mods)) {
	keyboard_callback_map.insert(std::pair<std::string, void(*)(GLFWwindow*, int, int, int, int)>(name, pf));
}

void OpenGLContext::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (keyboard_callback_map.empty())	return;
	GLFWwindow *cx = getCurrent();
	if (cx) {
		try {
			keyboard_callback_map.at(current_context)(cx, key, scancode, action, mods);
		} catch (const std::out_of_range &oor) {}
	}
}

//-------------------------------------------------------------------
// MOUSE CURSOR CLLBAKS
void OpenGLContext::setMouseCursorPos(void (*pf)(GLFWwindow* window, double xpos, double ypos)) {
	if (current_context.size() > 0) {
		setMouseCursorPos(current_context, pf);
	}
}
void OpenGLContext::setMouseCursorPos(std::string name, void (*pf)(GLFWwindow* window, double xpos, double ypos)){
	mouseCursorPos_callback_map.insert(std::pair<std::string, void(*)(GLFWwindow* window, double xpos, double ypos)>(name, pf));
}

void OpenGLContext::mouseCursorPos(GLFWwindow* window, double xpos, double ypos){
	if (mouseCursorPos_callback_map.empty())	return;
	GLFWwindow *cx = getCurrent();
	if (cx) {
		try {
			mouseCursorPos_callback_map.at(current_context)(cx, xpos, ypos);
		} catch (const std::out_of_range &oor) {}
	}
}
//-----------------------------------------------------------------------------------
//MOUSE SCROLL CALLBAK
void OpenGLContext::setMouseScroll(void(*pf)(GLFWwindow* window, double xoffset, double yoffset)) {
	if (current_context.size() > 0) {
		setMouseScroll(current_context, pf);
	}
}
void OpenGLContext::setMouseScroll(std::string name, void(*pf)(GLFWwindow* window, double xoffset, double yoffset))  {
	mouseScroll_callback_map.insert( std::pair<std::string, void(*)(GLFWwindow* window, double xoffset, double yoffset)> (name, pf));
}

void OpenGLContext::mouseScroll(GLFWwindow* window, double xoffset, double yoffset){
	if (mouseScroll_callback_map.empty())	return;
	GLFWwindow *cx = getCurrent();
	if (cx) {
		try {
			mouseScroll_callback_map.at(current_context)(cx, xoffset, yoffset);
		} catch (const std::out_of_range &oor) {}
	}
}

//-----------------------------------------------------------------------------------
//MOUSE CURSOR ENTERED CALLNAK
void OpenGLContext::setMouseCursorEnter(void(*pf)(GLFWwindow* window, int entered)) {
	if (current_context.size() > 0) {
		setMouseCursorEnter(current_context, pf);
	}
}

void OpenGLContext::setMouseCursorEnter(std::string name, void (*pf)(GLFWwindow* window, int entered)) {
	mouseCursorEnter_callback_map.insert(std::pair<std::string, void(*)(GLFWwindow* window, int entered)>(name, pf));
}

void OpenGLContext::mouseCursorEnter(GLFWwindow* window, int entered){
	if (mouseCursorEnter_callback_map.empty())	return;
	GLFWwindow *cx = getCurrent();
	if (cx) {
		try {
			mouseCursorEnter_callback_map.at(current_context)(cx, entered);
		} catch (const std::out_of_range &oor) {}
	}
}

//-----------------------------------------------------------------------------------
//MOUSE BUTTON CALLBAK
void OpenGLContext::setMouseButton(void(*pf)(GLFWwindow* window, int button, int action, int mods)) {
	if (current_context.size() > 0) {
		setMouseButton(current_context, pf);
	}
}

void OpenGLContext::setMouseButton(std::string name, void(*pf)(GLFWwindow* window, int button, int action, int mods)) {
	mouseButton_callback_map.insert(std::pair<std::string, void(*)(GLFWwindow* window, int button, int action, int mods)>(name, pf));
}

void OpenGLContext::mouseButton(GLFWwindow* window, int button, int action, int mods){
	if (mouseButton_callback_map.empty())	return;
	GLFWwindow *cx = getCurrent();
	if (cx) {
		try {
			mouseButton_callback_map.at(current_context)(cx, button, action, mods);
		} catch (const std::out_of_range &oor) {}
	}
}

//---------------------------------------------------------------

void OpenGLContext::initOpengl(std::string name) {

	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, " GLEW INIT ERROR !!!  . \n");
		exit(EXIT_FAILURE);
	}

#if DEVELOPEMENT
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(DebugMessageCallback, 0);
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR,
				GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		fprintf(stdout, " OpenGL DEBUG MODE ENABLED for context %s . \n", name.c_str());
	}
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

}

