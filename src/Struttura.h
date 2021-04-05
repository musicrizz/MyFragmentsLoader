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

using namespace std;
namespace fs = std::filesystem;

//----------------------------------------------------------------
//-----INPUT PARAMETERS
fs::path vertex_shader_file = "resources/default/vertex_default.glsl",//default value
		fragments_folder = "";
bool recursive = false;

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
//for polling the status
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
//----------------DEFAULT ERROR PROGRAM------------------------------

struct DEFAULT_PROGRAM {
private:
	unsigned int vaos = -1;

public:
	const string NAME = "ERROR_ROGRAM_musicrizz";

	void init()  {
//		try{
//			ShaderMap::createProgram(NAME , "resources/default/vertex.glsl", "resources/default/fragment.glsl");
//			programs.push_back({NAME, NULL, "", false});
//		}catch (ShaderException &e) {
//			cerr<<"ERROR LOADING DEFAULT PROGRAM : "<<e.what()<<endl;
//			exit(-1);
//		}
//
//		glGenVertexArrays(1, vaos);
//		glGenBuffers(BUFFERS_NUM, buffers);
//
//		const char* _img_bg = "resources/default/CompilerError.jpg";
//
//		glActiveTexture(GL_TEXTURE1);
//		unsigned int sampler;
//		glGenSamplers(1, &sampler);
//		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glBindTexture(GL_TEXTURE_2D, textures[T_TEXT]);
//		glBindSampler(T_TEXT, sampler);
//		glUniform1i(ShaderMap::getUniformLocation("char_texture"), T_TEXT);
	}

	void display()  {
		ShaderMap::useProgram(NAME);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

} _default_program;











#endif /* STRUTTURA_H_ */
