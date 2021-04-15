/*
 * Struttura.h
 *
 *  Created on: Mar 31, 2021
 *      Author: Giovanni Grandinetti
 */

#ifndef STRUTTURA_H_
#define STRUTTURA_H_

#include "../context_util/util.h"
#include <filesystem>
#include <map>
#include <set>
#include <iomanip>
#include <string>
#include <utility>

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

static std::timed_mutex polling_mutex;
bool flag_files_modified = false;

//--
bool file_order (string a, string b) {
	return a.substr(a.find_last_of("/\\")+1) < (b.substr(b.find_last_of("/\\")+1));
}
bool(*file_order_pt)(string,string) = file_order;
//This map have a key = file_name and a value of time of modified end bool flag for modification
//Ordered per file_name
map<string, std::pair<fs::file_time_type, bool>, bool(*)(string, string)> fragments_map (file_order_pt);
//--

set<string> texture_files;

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
			fragments_map.insert(
				std::pair<string, std::pair<fs::file_time_type,bool> >
				(f.path().string(), std::pair<fs::file_time_type, bool>(fs::last_write_time(f), false))
			);
		}else if(recursive && f.is_directory()) {
			loadFragmentFiles(f);
		}
	}
}

void loadTextureFiles(fs::path dir)  {
	for (const auto &f : fs::directory_iterator(dir)) {
		if (f.is_regular_file()) {
			texture_files.insert(f.path().string());
		}
	}
}

pair<GLenum,GLenum> getFormat(int nrChannels)  {
	GLenum internalFormat, format;
	switch(nrChannels)  {
		case 1: internalFormat = GL_R8; format = GL_RED;break;
		case 3: internalFormat = GL_RGB32F; format = GL_RGB;break;
		case 4: internalFormat = GL_RGBA32F; format = GL_RGBA;break;
		default: {
			LOG(ERROR)<<"Not supported image";
			return pair<GLenum,GLenum> (0, 0);
		}//TODO
	}
	return pair<GLenum,GLenum> (internalFormat, format);
}

//--------------------------------------------------------------
//-----------PROGRAMS FILES-------------------------------------------------

//represent a program files loaded or not
//for monitoring the status
struct PROGRAM_FILE {
public:
	string name,
		   path,
	       error;
	bool error_status = false,
		 modified = false;
};

int current_program = 0;
map<int,PROGRAM_FILE> programs;

void checkFilesModified(fs::path dir)  {
	//LOG(DEBUG)<<"CHECK IF FRAGMENTS ARE MODIFIED ";
	for (const auto &f : fs::directory_iterator(dir)) {
		if (f.is_regular_file()) {

			try{
				if(fragments_map.at(f.path().string()).first != fs::last_write_time(f))  {
					fragments_map.at(f.path().string()).second = true;
					fragments_map.at(f.path().string()).first = fs::last_write_time(f);
					flag_files_modified = true;
				}
			}catch (...) {}

		}else if(recursive && f.is_directory()) {
			checkFilesModified(f);
		}
	}
}

void updateModifiedFragment()  {
	if(flag_files_modified)  {
		std::lock_guard<std::timed_mutex> lk(polling_mutex);
		//LOG(DEBUG)<<"UPDATE FRAGMNETS PRGRAM";
		int pos = 1;
		for(auto& f : fragments_map)  {
			if(f.second.second) {
				programs[pos].modified=true;
				f.second.second=false;
			}
			pos++;
		}
		flag_files_modified = false;
	}
}

//-------------------------------------------------
//----------------------------------------------

int viewport_w, viewport_h;
float viewport_aspect;

struct BASE_OPENGL {
private:
	//dafault program name
	const string DEFAULT_PGR_NAME = "DEFAULT_ROGRAM_MyFragmentsLoader";

	const char* DEFAULT_VERTEX_SHADER = "resources/default/vertex_default.glsl";
	const char*	DEFAULT_FRAGMT_SHADER = "resources/default/fragment_default.glsl";
	const char*DEFAULT_TEXTURE_IMG = "resources/default/CompilerError.jpg";

	const string default_vertex_attrib_name = "position";
	const string default_uv_coord_attrib_name = "_uv_coord";

	int default_texture_array_loc;
	const string default_texture_base_name = "texture_img[";

	unsigned int zoom_scroll = 1;

	enum VAO {V, VAOS_NUM};
	unsigned int vaos[VAOS_NUM];

	enum BUFFERS {B_VERTEX, UNIFORM, BUFFERS_NUM};
	unsigned int buffers[BUFFERS_NUM];

	unsigned int* textures;

	void activeTextureUnits()  {
		for(int i = 0; i < (int)texture_files.size(); i++)  {
			int loc = ShaderMap::getUniformLocation((default_texture_base_name+to_string(i)+"]"));
			if(loc > 0)  {
				glActiveTexture(GL_TEXTURE1+i);
				glBindTexture(GL_TEXTURE_2D, textures[i]);
				glUniform1i(loc, i+1);
			}
		}
	}

public:
	bool flag_update_viewport = true;

	//Binding point for uniform_buffer 'CommonUniform' ,shared among all fragments
	const unsigned int uniform_binding_point = 1;
	const string common_uniform_name = "CommonUniform";

	void createDefaultProgram()  {
		try{
			//this default program is used for display error
			ShaderMap::createProgram(DEFAULT_PGR_NAME , DEFAULT_VERTEX_SHADER, DEFAULT_FRAGMT_SHADER);
			programs.insert( std::pair<int,PROGRAM_FILE>(0, {DEFAULT_PGR_NAME, "", "", false}) );
		}catch (ShaderException &e) {
			cerr<<"ERROR LOADING DEFAULT PROGRAM : "<<e.what()<<endl;
			exit(-1);
		}

	}

	void initOpenGLBuffers()  {
		LOG(DEBUG)<<"--------- Init openGl buffers and loading textures-------------------\n";

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

		int vtx_loc = ShaderMap::getAttributeLocation(default_vertex_attrib_name);
		int uv_loc  = ShaderMap::getAttributeLocation(default_uv_coord_attrib_name);
		LOG(DEBUG)<<"GLSL var location -> verteex : "<<vtx_loc<<", uv: "<<uv_loc;

		glBindVertexArray(vaos[V]);
		glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
			glVertexAttribPointer(vtx_loc, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), reinterpret_cast<void*>(0));
			glVertexAttribPointer(uv_loc,  2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), reinterpret_cast<void*>(sizeof(glm::vec4)*4));
			glEnableVertexAttribArray(vtx_loc);
			glEnableVertexAttribArray(uv_loc);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		LOG(DEBUG)<<OpenGLerror::check("creation buffer VAO");

		ShaderMap::bindingUniformBlocks(common_uniform_name, uniform_binding_point);
		glBindBufferBase(GL_UNIFORM_BUFFER, uniform_binding_point, buffers[UNIFORM]);
		LOG(DEBUG)<<OpenGLerror::check("Binding uniform Buffer");

		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferData(GL_UNIFORM_BUFFER, 28, NULL, GL_DYNAMIC_DRAW); // allocate 20 bytes of memory
			//init mouse pos e scroll
			glBufferSubData(GL_UNIFORM_BUFFER, 8, 8, glm::value_ptr(glm::vec2(0.0f, 0.0f)));//mouse position init 0.0
			glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, reinterpret_cast<void*>(&zoom_scroll) );//zoom_scroll init to 1
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		LOG(DEBUG)<<OpenGLerror::check("CREAZIOEN GL_UNIFORM_BUFFER : ")<<endl;

		//Creation textures
		if(flag_load_Texture)  {
			LOG(DEBUG)<<"LOAD IMAGES FILES PATH";
			loadTextureFiles(textures_folder);
		}

		textures = new unsigned int[texture_files.size()+1];
		glGenTextures(texture_files.size()+1, textures);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//if not set the image that are not multiple of 2 will crash the application

		stbi_set_flip_vertically_on_load(true);

		//First load default texture for base program
		int w, h, nrChannels;
		unsigned char *data = stbi_load(DEFAULT_TEXTURE_IMG, &w, &h, &nrChannels, 0);
		if(!data) {
			//LOG(ERROR)<<"ERROR LOAD DEFAULT IMAGE Texture";
			cerr<<"ERROR LOAD DEFAULT IMAGE Texture";
			exit(-1);
		}
		unsigned int img_size = sizeof(unsigned char) * w * h * nrChannels;
		LOG(DEBUG)<<"Image default w: "<<w<<", h: "<<h<<", ch: "<<nrChannels<<", size: "<<img_size<<std::endl;

		default_texture_array_loc = ShaderMap::getUniformLocation(default_texture_base_name+"0]");
		LOG(DEBUG)<<"Texture location "<< (default_texture_base_name+"0]") << " : " <<default_texture_array_loc;

		if(default_texture_array_loc < 0)  {
			cerr<<"BAD DEFAULT FRAGMENT SHADERS texture_img[0] not valid location \n";
			exit(-1);
		}

		pair<GLenum, GLenum> format = getFormat(nrChannels);
		int lev = mipmapsLevels(w, h, 24);
		glBindTexture(GL_TEXTURE_2D, textures[texture_files.size()]);
		    glTexStorage2D(GL_TEXTURE_2D, lev, format.first, w, h);
		    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format.second, GL_UNSIGNED_BYTE, data);
		    glGenerateMipmap(GL_TEXTURE_2D);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lev-1);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		LOG(DEBUG)<<OpenGLerror::check("Creazione Default Texture")<<std::endl;

		stbi_image_free(data);

		//Load all texture in the vector texture_files
		int index = 0;
		for(auto &f : texture_files)  {
			data = stbi_load(f.c_str(), &w, &h, &nrChannels, 0);
			if(!data) {
				LOG(DEBUG)<<"ERROR LOAD IMAGE: "<<f;
				index++;
				continue;
			}
			img_size = sizeof(unsigned char) * w * h * nrChannels;
			LOG(DEBUG)<<"Image "<<f<<", w: "<<w<<", h: "<<h<<", ch: "<<nrChannels<<", size: "<<img_size<<std::endl;
			format = getFormat(nrChannels);
			lev = mipmapsLevels(w, h, 24);
			glBindTexture(GL_TEXTURE_2D, textures[index++]);
				glTexStorage2D(GL_TEXTURE_2D, lev, format.first, w, h);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format.second, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, lev-1);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
			LOG(DEBUG)<<"TEXTURE: "<<f<<OpenGLerror::check(" Creazione Texture")<<std::endl;
			stbi_image_free(data);
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

	void displayDefaultProgram()  {
		ShaderMap::useProgram(DEFAULT_PGR_NAME);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[texture_files.size()]);
		glUniform1i(default_texture_array_loc, 1);

		updateViewPort();

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		//ShaderMap::program_null();
	}

	void displayProgram(string name) {
		ShaderMap::useProgram(name);

		activeTextureUnits();

		updateViewPort();
		updateTime();

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	}

	void updateViewPort() {
		if(flag_update_viewport)  {
			glViewport(0, 0, viewport_w, viewport_h);
			glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, 8, glm::value_ptr(glm::ivec2(viewport_w, viewport_h)));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			flag_update_viewport = false;
		}
	}

	void updateMousePosition(double x, double y)  {
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferSubData(GL_UNIFORM_BUFFER, 8, 8, glm::value_ptr(glm::vec2(float(x)/float(viewport_w), float(viewport_h-y)/float(viewport_h))));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void updateMouseScroll(double x, double y)  {
		zoom_scroll += float(y);
		zoom_scroll < 1 ? zoom_scroll = 1 : 0;
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, reinterpret_cast<void*>(&zoom_scroll) );
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void updateTime()  {
		float time = float(glfwGetTime());
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferSubData(GL_UNIFORM_BUFFER, 20, 4, reinterpret_cast<void*>(&time) );
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void updateDeltaTime()  {
//		float time = float(glfwGetTime());
//		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
//			glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, reinterpret_cast<void*>(&time) );
//		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}



} _base_system;











#endif /* STRUTTURA_H_ */
