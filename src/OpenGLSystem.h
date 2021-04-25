/*
 * OpenGLSystem.h
 *
 *  Created on: Apr 24, 2021
 *      Author: gio
 */
#pragma once
#ifndef OPENGLSYSTEM_H_
#define OPENGLSYSTEM_H_

#include "../context_util/util.h"
#include "FileStructure.h"

using namespace std;

class OpenGLSystem {
private:
	//dafault program name
	const string DEFAULT_PGR_NAME = "DEFAULT_ROGRAM_MyFragmentsLoader";

	const char* DEFAULT_VERTEX_SHADER = "resources/default/vertex_default.glsl";
	const char*	DEFAULT_FRAGMT_SHADER = "resources/default/fragment_default.glsl";
	const char* DEFAULT_TEXTURE_IMG = "resources/default/CompilerError.jpg";

	const string default_vertex_attrib_name = "position";
	const string default_uv_coord_attrib_name = "_uv_coord";

	int default_texture_array_loc;
	const string default_texture_base_name = "texture_img[";

	//Binding point for uniform_buffer 'CommonUniform' ,shared among all fragments
	const unsigned int uniform_binding_point = 1;
	const string common_uniform_name = "CommonUniform";

	enum VAO {V, VAOS_NUM};
	unsigned int vaos[VAOS_NUM];

	enum BUFFERS {B_VERTEX, UNIFORM, BUFFERS_NUM};
	unsigned int buffers[BUFFERS_NUM];

	unsigned int* textures;
	unsigned int texture_img_file_size;

	int viewport_w, viewport_h;
	float viewport_aspect;
	bool flag_update_viewport;
	unsigned int zoom_scroll;

	void activeTextureUnits();
	pair<GLenum,GLenum> getFormat(int nrChannels);

	OpenGLSystem(const OpenGLSystem &other) = delete;
	OpenGLSystem(OpenGLSystem &&other) = delete;
	OpenGLSystem& operator=(const OpenGLSystem &other) = delete;
	OpenGLSystem& operator=(OpenGLSystem &&other) = delete;

public:
	OpenGLSystem();
	virtual ~OpenGLSystem();

	int getViewportW();
	void setViewportW(int w);

	int getViewportH();
	void setViewportH(int h);

	float getViewportAspect();
	void setViewportAspect(float aspect);

	void setFlagUpdateViewport(bool flag);

	void serTextureImgFileSize(unsigned int num);

	unsigned int getUniformBindingPoint();
	string getCommonUniformName();

	void createDefaultProgram();
	void initOpenGLBuffers();

	void displayDefaultProgram();
	void displayProgram(string name);

	void updateViewPort();
	void updateMousePosition(double x, double y);
	void updateMouseScroll(double x, double y);
	void updateTime();
	void updateDeltaTime(int delta);
};

#endif /* OPENGLSYSTEM_H_ */
