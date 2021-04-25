/*
 * OpenGLSystem.cpp
 *
 *  Created on: Apr 24, 2021
 *      Author: gio
 */

#include "OpenGLSystem.h"


OpenGLSystem::OpenGLSystem() { // @suppress("Class members should be properly initialized")
	default_texture_array_loc = -1;
	texture_img_file_size = 0;
	viewport_w = 0;
	viewport_h = 0;
	viewport_aspect = 0.0f;
	zoom_scroll = 1;
	flag_update_viewport = true;
}

OpenGLSystem::~OpenGLSystem() {
	// TODO Auto-generated destructor stub flag Initialized
}

int OpenGLSystem::getViewportW() {
	return viewport_w;
}
void OpenGLSystem::setViewportW(int w) {
	viewport_w = w;
}

int OpenGLSystem::getViewportH() {
	return viewport_h;
}
void OpenGLSystem::setViewportH(int h) {
	viewport_h = h;
}

float OpenGLSystem::getViewportAspect() {
	return viewport_aspect;
}

void OpenGLSystem::setViewportAspect(float aspect)  {
	viewport_aspect = aspect;
}

void OpenGLSystem::setFlagUpdateViewport(bool flag) {
	this->flag_update_viewport = flag;
}

void OpenGLSystem::serTextureImgFileSize(unsigned int size) {
	this->texture_img_file_size = size;
}

unsigned int OpenGLSystem::getUniformBindingPoint() {
	return this->uniform_binding_point;
}
string OpenGLSystem::getCommonUniformName() {
	return this->common_uniform_name;
}

void OpenGLSystem::createDefaultProgram() {
	try {
		//this default program is used for display error
		ShaderMap::createProgram(DEFAULT_PGR_NAME, DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMT_SHADER);
		FileStructure::getInstance()->insertProgramFile(0, DEFAULT_PGR_NAME, "", "");
	} catch (ShaderException &e) {
		cerr << "ERROR LOADING DEFAULT PROGRAM : " << e.what() << endl;
		exit(-1);
	}
}

void OpenGLSystem::initOpenGLBuffers() {
	LOG(DEBUG)<< "--------- Init openGl buffers and loading textures-------------------\n";

	ShaderMap::useProgram(FileStructure::getInstance()->getProgramName(0));

	glGenVertexArrays(VAOS_NUM, vaos);
	glGenBuffers(BUFFERS_NUM, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 6, NULL, GL_STATIC_DRAW);
	glm::vec4 *v = (glm::vec4*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		//coordinate to draw a simple quad with triangle fan
		*v++ = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
		*v++ = glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);
		*v++ = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		*v++ = glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
		//uv-coordinate are 4 vec2 but i use this pointer then 2 vec4
		*v++ = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		*v = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	LOG(DEBUG) << OpenGLerror::check("creation buffer vertex");

	int vtx_loc = ShaderMap::getAttributeLocation(default_vertex_attrib_name);
	int uv_loc = ShaderMap::getAttributeLocation(default_uv_coord_attrib_name);
	LOG(DEBUG) << "GLSL var location -> verteex : " << vtx_loc << ", uv: "<< uv_loc;

	glBindVertexArray(vaos[V]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
		glVertexAttribPointer(vtx_loc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), reinterpret_cast<void*>(0));
		glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), reinterpret_cast<void*>(sizeof(glm::vec4) * 4));
		glEnableVertexAttribArray(vtx_loc);
		glEnableVertexAttribArray(uv_loc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	LOG(DEBUG) << OpenGLerror::check("creation buffer VAO");

	ShaderMap::bindingUniformBlocks(common_uniform_name, uniform_binding_point);
	glBindBufferBase(GL_UNIFORM_BUFFER, uniform_binding_point, buffers[UNIFORM]);
	LOG(DEBUG) << OpenGLerror::check("Binding uniform Buffer");

	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
	glBufferData(GL_UNIFORM_BUFFER, 28, NULL, GL_DYNAMIC_DRAW); // allocate 20 bytes of memory
		//init mouse pos e scroll
		glBufferSubData(GL_UNIFORM_BUFFER, 8, 8, glm::value_ptr(glm::vec2(0.0f, 0.0f)));	//mouse position init 0.0
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, reinterpret_cast<void*>(&zoom_scroll));		//zoom_scroll init to 1
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	LOG(DEBUG) << OpenGLerror::check("CREAZIOEN GL_UNIFORM_BUFFER : ") << endl;

	//Creatio TExture

	//todo doc
	texture_img_file_size = FileStructure::getInstance()->getTexImg_vec().size();
	textures = new unsigned int[texture_img_file_size + 1];
	glGenTextures(texture_img_file_size + 1, textures);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//if not set the image that are not multiple of 2 will crash the application

	//First load default texture for base default program
	int w, h, nrChannels;
	unsigned char *data = stbi_load(DEFAULT_TEXTURE_IMG, &w, &h, &nrChannels,0);
	if (!data) {
		//LOG(ERROR)<<"ERROR LOAD DEFAULT IMAGE Texture";
		cerr << "ERROR LOAD DEFAULT IMAGE Texture";
		exit(-1);
	}
	unsigned int img_size = sizeof(unsigned char) * w * h * nrChannels;
	LOG(DEBUG) << "Image default w: " << w << ", h: " << h << ", ch: "<< nrChannels << ", size: " << img_size << std::endl;

	default_texture_array_loc = ShaderMap::getUniformLocation(default_texture_base_name + "0]");
	LOG(DEBUG) << "Texture location " << (default_texture_base_name + "0]")<< " : " << default_texture_array_loc;

	if (default_texture_array_loc < 0) {
		cerr<< "BAD DEFAULT FRAGMENT SHADERS texture_img[0] not valid location \n";
		exit(-1);
	}

	pair<GLenum, GLenum> format = getFormat(nrChannels);
	int lev = mipmapsLevels(w, h, 24);
	glBindTexture(GL_TEXTURE_2D, textures[texture_img_file_size]);
	glTexStorage2D(GL_TEXTURE_2D, lev, format.first, w, h);
	 glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format.second,GL_UNSIGNED_BYTE, data);
	 glGenerateMipmap(GL_TEXTURE_2D);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lev - 1);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	LOG(DEBUG) << OpenGLerror::check("Creazione Default Texture") << std::endl;
	stbi_image_free(data);

	//Load all texture in the vector texture_files
	int index = 0;
	for (auto &f : FileStructure::getInstance()->getTexImg_vec()) {
		if (!f.data) {
			LOG(DEBUG) << "ERROR LOAD IMAGE: " << to_string(index);
			index++;
			continue;
		}
		img_size = sizeof(unsigned char) * f.w * f.h * f.nrChannles;

		format = getFormat(f.nrChannles);
		lev = mipmapsLevels(f.w, f.h, 24);
		glBindTexture(GL_TEXTURE_2D, textures[index++]);
		 glTexStorage2D(GL_TEXTURE_2D, lev, format.first, f.w, f.h);
		  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, f.w, f.h, format.second, GL_UNSIGNED_BYTE, f.data);
		  glGenerateMipmap(GL_TEXTURE_2D);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lev - 1);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		LOG(DEBUG) << "TEXTURE: " << to_string(index) << OpenGLerror::check(" Creazione Texture") << std::endl;
		stbi_image_free(f.data);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(default_texture_array_loc, 0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	glDepthMask(true);
	glClearDepth(1.0f);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glColorMask(true, true, true, true);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(vaos[V]);
}

void OpenGLSystem::displayDefaultProgram() {
	ShaderMap::useProgram(DEFAULT_PGR_NAME);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[texture_img_file_size]);
	glUniform1i(default_texture_array_loc, 1);

	updateViewPort();

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//ShaderMap::program_null();
}

void OpenGLSystem::displayProgram(string name) {
	ShaderMap::useProgram(name);

	activeTextureUnits();

	updateViewPort();
	updateTime();

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

}

void OpenGLSystem::activeTextureUnits()  {
	for(unsigned int i = 0; i < texture_img_file_size; i++)  {
		int loc = ShaderMap::getUniformLocation((default_texture_base_name+to_string(i)+"]"));
		if(loc > 0)  {
			glActiveTexture(GL_TEXTURE1+i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glUniform1i(loc, i+1);
		}
	}
}

void OpenGLSystem::updateViewPort() {
	if (flag_update_viewport) {
		glViewport(0, 0, viewport_w, viewport_h);
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 8, glm::value_ptr(glm::ivec2(viewport_w, viewport_h)));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		flag_update_viewport = false;
	}
}

void OpenGLSystem::updateMousePosition(double x, double y) {
	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
	glBufferSubData(GL_UNIFORM_BUFFER, 8, 8,
			glm::value_ptr(glm::vec2(float(x) / float(viewport_w), float(viewport_h - y) / float(viewport_h))));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLSystem::updateMouseScroll(double x, double y) {
	zoom_scroll += float(y);
	zoom_scroll < 1 ? zoom_scroll = 1 : 0;
	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, reinterpret_cast<void*>(&zoom_scroll));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLSystem::updateTime() {
	float time = float(glfwGetTime());
	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
	glBufferSubData(GL_UNIFORM_BUFFER, 20, 4, reinterpret_cast<void*>(&time));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLSystem::updateDeltaTime(int delta) {
	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
	glBufferSubData(GL_UNIFORM_BUFFER, 24, 4, reinterpret_cast<void*>(&delta));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

pair<GLenum,GLenum> OpenGLSystem::getFormat(int nrChannels)  {
	GLenum internalFormat, format;
	switch(nrChannels)  {
		case 1: internalFormat = GL_R8; format = GL_RED;break;
		//TODO add case 2
		case 3: internalFormat = GL_RGB32F; format = GL_RGB;break;
		case 4: internalFormat = GL_RGBA32F; format = GL_RGBA;break;
		default: {
			LOG(INFO)<<"Not supported image";
			return pair<GLenum,GLenum> (0, 0);
		}//TODO
	}
	return pair<GLenum,GLenum> (internalFormat, format);
}
