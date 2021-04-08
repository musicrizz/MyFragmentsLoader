/*
 * Struttura.h
 *
 *  Created on: Mar 31, 2021
 *      Author: gio
 */

#ifndef STRUTTURA_H_
#define STRUTTURA_H_

#include "../context_util/util.h"
#include <filesystem>
#include <map>
#include <vector>
#include <iomanip>
#include <string>

#include "../third_party_lib/image/stb_image.h"

using namespace std;
namespace fs = std::filesystem;

//----------------------------------------------------------------
//-----INPUT PARAMETERS
fs::path vertex_shader_file = "resources/default/vertex_default.glsl",//default value
		 fragments_folder   = "",
		 textures_folder    = "";
bool recursive = false,
	 flag_load_Texture = false;

bool file_order (string a, string b) {
	return a.substr(a.find_last_of("/\\")+1) < (b.substr(b.find_last_of("/\\")+1));
}
bool(*file_order_pt)(string,string) = file_order;
map<string, fs::file_time_type, bool(*)(string,string)> fragments_map (file_order_pt);

bool setVertexFile(char* v) {
	vertex_shader_file = v;
	return fs::is_regular_file(vertex_shader_file);
}

bool setFragmentFolder(char* v) {
	fragments_folder = v;
	return fs::is_directory(fragments_folder);
}

bool setTextureFolder(char* v) {
	textures_folder = v;
	if(fs::is_directory(textures_folder)) {
		flag_load_Texture = true;
		return true;
	}
	return false;
}

void loadFragmentFiles(fs::path dir)  {
	for (const auto &f : fs::directory_iterator(dir)) {
		if (f.is_regular_file()) {
			fragments_map.insert( std::pair<string, fs::file_time_type>(f.path().string(),fs::last_write_time(f)) );
		}else if(recursive && f.is_directory()) {
			loadFragmentFiles(f);
		}
	}
}

//-----------------------------------------------------------
//-----------PROGRAMS FILES-------------------------------------------------

//represent a program files loaded or not
//for monitoring the status
struct PROGRAM_FILE {
public:
	string name,
		   path,
	       error;
	bool error_status;
};

int current_program = 0;
vector<PROGRAM_FILE> programs;

//-------------------------------------------------
//----------------------------------------------

int viewport_w, viewport_h;
float viewport_aspect;

struct BASE_OPENGL {
private:
	//dafault program name
	const string NAME = "DEFAULT_ROGRAM_MyFragmentsLoader";

	enum VAO {V, VAOS_NUM};
	unsigned int vaos[VAOS_NUM];

	enum BUFFERS {B_VERTEX, UNIFORM, BUFFERS_NUM};
	unsigned int buffers[BUFFERS_NUM];

	enum TEXTURES {T_NULL = 0, T_1, T_2, T_3, T_4, T_5, T_DEFAULT, TEXTURES_NUM};
	unsigned int textures[TEXTURES_NUM];

public:

	//Binding point for uniform_buffer 'CommonUniform' ,shared among all fragments
	const unsigned int uniform_binding_point = 1;

	void createDefaultProgram()  {
		try{
			//this default program is used for display error
			ShaderMap::createProgram(NAME , "resources/default/vertex_default.glsl", "resources/default/fragment_default.glsl");
			programs.push_back({NAME, "", "", false});
		}catch (ShaderException &e) {
			cerr<<"ERROR LOADING DEFAULT PROGRAM : "<<e.what()<<endl;
			exit(-1);
		}
	}

	void initOpenGL()  {
		LOG(DEBUG)<<"--------- Init openGl buffer loading -------------------\n";

		ShaderMap::useProgram(programs[0].name);

		glGenVertexArrays(VAOS_NUM, vaos);
		glGenBuffers(BUFFERS_NUM, buffers);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*6, NULL, GL_STATIC_DRAW);
		glm::vec4* v = (glm::vec4*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
			//coordinate to draw a simple quad with triangle fan
			*v++ = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
			*v++ = glm::vec4( 1.0f, -1.0f, 0.0f, 1.0f);
			*v++ = glm::vec4( 1.0f,  1.0f, 0.0f, 1.0f);
			*v++ = glm::vec4(-1.0f,  1.0f, 0.0f, 1.0f);
			//uv-coordinate are 4 vec2 but i use this pointer then 2 vec4
			*v++ = glm::vec4( 0.0f,  0.0f, 1.0f, 0.0f);
			*v   = glm::vec4( 1.0f,  1.0f, 0.0f, 1.0f);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		LOG(DEBUG)<<OpenGLerror::check("creation buffer vertex");

		unsigned int vtx_loc = ShaderMap::getAttributeLocation("position");
		glBindVertexArray(vaos[V]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
			glVertexAttribPointer(vtx_loc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), reinterpret_cast<void*>(0));

			glEnableVertexAttribArray(vtx_loc);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		LOG(DEBUG)<<OpenGLerror::check("creation buffer VAO");

		ShaderMap::bindingUniformBlocks("CommonUniform", uniform_binding_point);
		glBindBufferBase(GL_UNIFORM_BUFFER, uniform_binding_point, buffers[UNIFORM]);

		LOG(DEBUG)<<OpenGLerror::check("Binding uniform Buffer");

		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferData(GL_UNIFORM_BUFFER, 20, NULL, GL_DYNAMIC_DRAW); // allocate 20 bytes of memory
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		LOG(DEBUG)<<OpenGLerror::check("CREAZIOEN GL_UNIFORM_BUFFER : ")<<endl;

		//Creation textures
		glGenTextures(TEXTURES_NUM, textures);

		unsigned int sampler;
		glGenSamplers(1, &sampler);
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_set_flip_vertically_on_load(true);
		const char* img = "resources/default/CompilerError.jpg";
		int w, h, nrChannels;
		unsigned char *data = stbi_load(img, &w, &h, &nrChannels, 0);
		if(!data) {
			LOG(DEBUG)<<"ERROR LOAD DEFAULT TEXTURE";
			exit(-1);
		}
		unsigned int img_size = sizeof(unsigned char) * w * h * nrChannels;
		LOG(DEBUG)<<"Image\n w: "<<w<<"\n h: "<<h<<"\n ch: "<<nrChannels<<"\n size: "<<img_size<<std::endl;

		GLenum internalFormat, format;
		switch(nrChannels)  {
			case 1: internalFormat = GL_R8; format = GL_RED;break;
			case 3: internalFormat = GL_RGB32F; format = GL_RGB;break;
			case 4: internalFormat = GL_RGBA32F; format = GL_RGBA;break;
			default: //TODO
		}

		int lev = mipmapsLevels(w, h, 16);

		stbi_image_free(data);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, T_NULL);


		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0, 1.0);
		glDepthMask(true);
		glClearDepth(1.0f);
		glEnable(GL_LINE_SMOOTH);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glColorMask(true, true, true, true);
		//	//glEnable(GL_BLEND);
		//	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(vaos[V]);
	}

	void displayDefaultProgram()  {

	}

	void displayProgram(string name) {

	}

	void updateViewPort() {
		glViewport(0, 0, viewport_w, viewport_h);
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, 8, glm::value_ptr(glm::ivec2(viewport_w, viewport_h)));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void updateTime()  {
		float time = float(glfwGetTime());
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, reinterpret_cast<void*>(&time) );
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void updateMouse(double x, double y)  {
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferSubData(GL_UNIFORM_BUFFER, 8, 8, glm::value_ptr(glm::vec2(float(x)/float(viewport_w), float(viewport_h-y)/float(viewport_h))));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

} _base_system;











#endif /* STRUTTURA_H_ */
