//============================================================================
// Author      : Giovanni Grandinetti
// Version     : 0.1
// Copyright   : Giovanni Grandinetti
//============================================================================
/*
 * OpenGLerror.h
 *
 *  Created on: Nov 20, 2020
 *      Author: gio
 */
#pragma once
#ifndef OPENGLERROR_H_
#define OPENGLERROR_H_

#include "CommonHeaders.h"

class OpenGLerror {
private:
	OpenGLerror();
public:
	static std::string check(std::string operation) {
		std::string error = "";
		switch (glGetError()) {
		case GL_NO_ERROR:
			error.append(operation.append(" : . ok \n"));
			break;

		case GL_INVALID_ENUM:
			error.append(operation.append(" : GL_INVALID_ENUM \n"));
			break;

		case GL_INVALID_VALUE:
			error.append(operation.append(" : GL_INVALID_VALUE \n"));
			break;

		case GL_INVALID_OPERATION:
			error.append(operation.append(" : GL_INVALID_OPERATION \n"));
			break;

		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error.append(
					operation.append(" : GL_INVALID_FRAMEBUFFER_OPERATION \n"));
			break;

		case GL_OUT_OF_MEMORY:
			error.append(operation.append(" : GL_OUT_OF_MEMORY \n"));
			break;

		case GL_STACK_UNDERFLOW:
			error.append(operation.append(" : GL_STACK_UNDERFLOW \n"));
			break;

		case GL_STACK_OVERFLOW:
			error.append(operation.append(" : GL_STACK_OVERFLOW \n"));
			break;

		default:
			error.append(operation.append(" : UNKNOWN GLenum !!\n"));
			break;
		}

		return error;
	}

};

inline void GLAPIENTRY DebugMessageCallback(
		GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar *message,
		const void *userParam) {

	if (type != GL_DEBUG_TYPE_ERROR || id == 131169 || id == 131185 || id == 131218 || id == 131204)
		return;

	std::string debug_type, debug_severity, debug_source;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		debug_source = "Source: API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		debug_source = "Source: Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		debug_source = "Source: Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		debug_source = "Source: Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		debug_source = "Source: Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		debug_source = "Source: Other";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		debug_type = "Type: Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		debug_type = "Type: Deprecated Behaviour";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		debug_type = "Type: Undefined Behaviour";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		debug_type = "Type: Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		debug_type = "Type: Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		debug_type = "Type: Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		debug_type = "Type: Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		debug_type = "Type: Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		debug_type = "Type: Other";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		debug_severity = "Severity: high";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		debug_severity = "Severity: medium";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		debug_severity = "Severity: low";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		debug_severity = "Severity: notification";
		break;
	}

	fprintf(stderr,
			"OpenGL DEBUG: %u \n\t %s \n\t %s \n\t %s \n\t message = %s\n", id,
			debug_source.c_str(), debug_type.c_str(), debug_severity.c_str(),
			message);

	/*std::string debug = "OpenGL DEBUG: ";
	 debug.append(std::to_string(id)).append("\n");
	 debug.append("\t").append(debug_source).append("\n");
	 debug.append("\t").append(debug_type).append("\n");
	 debug.append("\t").append(debug_severity).append("\n");
	 debug.append("\t").append(message).append("\n");
	 debug.append("---\n");
	 (userParam)();*/
}

#endif /* OPENGLERROR_H_ */


