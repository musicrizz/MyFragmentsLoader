//============================================================================
// Name        : ShadersBookExample.cpp
// Author      : Giovanni Grandinetti
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "../third_party_lib/logging/easylogging++.h"
#include "Struttura.h"

using namespace std;
namespace fs = std::filesystem;

#define ELPP_THREAD_SAFE
INITIALIZE_EASYLOGGINGPP

void usage()  {
	cout<<"Usage:  \n"
			"   options: \n"
			"   -f   : folder where are located all fragment shaders \n\n"
			"   -r   : set recursive search in subfolder of fragment folder \n\n"
			"   -t   : folder of texture images. \n"
			"           The images will be loaded in alphabetically order \n "
			"           (no recursive  search in sub-folder) \n "
			"           they can be accessed in the fragment with texture_img[ ] \n\n"
			"   -v   :  your vertex shader file that will be common to all fragments. \n"
			"           ('overwrite the default') use the template in resources folder for help\n\n\n"
			"Examples: \n"
			"   MyFragmentsLoader -f <fragment_shaders_folder> -r \n"
			"   MyFragmentsLoader -f <fragment_shaders_folder> -r -t <texture_folder> \n\n";
}

void pollingFragmentFiles();

void keyboardHandler(GLFWwindow* window, int key, int scancose, int action, int mods);

static const char *CONTEXT="CONTEX_BASE",
				  *FPS_TIME="FPS_TIME";

bool flag_update_viewport=true;

int main(int argc, char **argv) {

	if(argc > 8 || argc < 3)  {
		usage();
		return -1;
	}

	START_EASYLOGGINGPP(argc, argv);
	el::Configurations log_conf("config/logger.conf");
	el::Loggers::reconfigureLogger("default", log_conf);
	el::Loggers::reconfigureAllLoggers(log_conf);

	fs::path tmp;
	cout<<"argc : "<<argc<<endl;
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-f") == 0 && !setFragmentFolder(argv[i+1])) {
			cerr<<"ERROR : "<<argv[i+1]<<" is not a valid fragments folder"<<endl;
			usage();
			return -1;
		}
		if(strcmp(argv[i], "-t") == 0 && !setTextureFolder(argv[i+1])) {
			cerr<<"ERROR : "<<argv[i+1]<<" is not a valid textures folder"<<endl;
			usage();
			return -1;
		}
		if(strcmp(argv[i], "-v") == 0 && !setVertexFile(argv[i+1])) {
			cerr<<"ERROR : "<<argv[i+1]<<" is not a valid vertex file"<<endl;
			usage();
			return -1;
		}
		if(strcmp(argv[i], "-r") == 0 ) recursive = true;
	}

	if(vertex_shader_file.empty() || fragments_folder.empty())  {
		usage();
		return -1;
	}

	if(fs::is_regular_file(vertex_shader_file))  {
		LOG(DEBUG)<<"OK vertex shader is regular files";
	}else{
		LOG(DEBUG)<<"Problem default vertex";
		return 0;
	}

	if (!glfwInit()) {
		fprintf(stderr, "GLEW INTI ERROR");
		exit(EXIT_FAILURE);
	}

	glfwSetErrorCallback([](int error, const char *description) {
		fprintf(stderr, "GLFW_ERROR: %u :  %s ", error, description);
	});

	GLFWmonitor *primary = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(primary);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 0);

	//You can set DEVELOPMENT in contex_util/CommonHeaders.h
#if DEVELOPEMENT
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	OpenGLContext::createContex(CONTEXT, 600, 600, NULL, true, true, false);
	OpenGLContext::makecurrent(CONTEXT);

	OpenGLContext::setReshape(CONTEXT, [](GLFWwindow* window, int width, int height){
		viewport_w = width;
		viewport_h = height;
		viewport_aspect = float(height) / float(width);
		flag_update_viewport = true;
	});

	OpenGLContext::setMouseCursorPos([](GLFWwindow* window, double x, double y){
		_base_system.updateMousePosition(x,y);
	});

	OpenGLContext::setMouseScroll([](GLFWwindow* window, double x, double y){
		_base_system.updateMouseScroll(x, y);
	});

	OpenGLContext::setKeyboard(keyboardHandler);

	loadFragmentFiles(fragments_folder);

	//CREATE DEFAULT PROGRAM FOR DISPLAY COMPILATION ERROR in position 0
	_base_system.createDefaultProgram();

	//Create a program for each fragment file
	current_program = 1;
	for(const auto &f : fragments_map)  {
		string name = f.first.substr(f.first.find_last_of("/\\")+1);
		LOG(DEBUG)<<"File to load : "<<name<<endl;

		try{
			ShaderMap::createProgram( name , vertex_shader_file.c_str(), f.first.c_str());
			//set Binding point for uniform_buffer
			ShaderMap::getProgram(name)->setBindingPoint(_base_system.uniform_binding_point);
			programs.insert( std::pair<int,PROGRAM_FILE>(current_program++, {name, f.first, "", false,false}) );
		}catch (ShaderException &e) {
			LOG(DEBUG)<<"Error program creation : "<< f.first << " :" <<e.what()<<endl;
			programs.insert( std::pair<int,PROGRAM_FILE>(current_program++, {name, f.first, string(e.what()), true, false}) );
		}
	}

	if (programs.size() == 0)  {
		cerr<<"ERROR NO PROGRAM LOADED"<<endl;
		exit(-1);
	}

	current_program = 1;

#if DEVELOPEMENT
	for(auto p : programs)  {
		LOG(DEBUG)<<"Struttura : "<<p.second.name<<" \n\t "<<p.second.path<<" \n\t "<<p.second.error<<" \n\t "<<p.second.error_status<<endl;
	}
#endif

	_base_system.initOpenGLBuffers();

	glfwGetFramebufferSize(OpenGLContext::getCurrent(), &viewport_w, &viewport_h);
	viewport_aspect = float(viewport_h) / float(viewport_w);
	flag_update_viewport = true;

	glfwSwapInterval(1);

	glfwSetWindowTitle(OpenGLContext::getCurrent(), programs[current_program].name.c_str());

	//pollingFragmentFiles will be executed in another thread created by TempoMap
	TempoMap::createTimer("POLLING", pollingFragmentFiles, 500);

	while (!glfwWindowShouldClose(OpenGLContext::getCurrent())) {

		if (TempoMap::getElapsedMill(FPS_TIME) >= 20) {

			std::unique_lock<std::timed_mutex> lk(polling_mutex, std::defer_lock);
			if(!lk.try_lock_for(std::chrono::microseconds(500)))continue;

			if(programs[current_program].modified)  {
				try{
					LOG(DEBUG)<<"Program "<<programs[current_program].name <<" IS MODIFIED !";
					programs[current_program].modified = false;
					ShaderMap::deleteProgram(programs[current_program].name);
					ShaderMap::createProgram(programs[current_program].name, vertex_shader_file.c_str(), programs[current_program].path.c_str());
					ShaderMap::getProgram(programs[current_program].name)->setBindingPoint(_base_system.uniform_binding_point);
					ShaderMap::bindingUniformBlocks(_base_system.common_uniform_name, _base_system.uniform_binding_point);
					ShaderMap::bindingUniformBlocksForSingleProgram(programs[current_program].name, "CommonUniform", _base_system.uniform_binding_point);
					programs[current_program].error_status=false;
					programs[current_program].error="";
					LOG(DEBUG)<<"Program "<<programs[current_program].name <<"SUCCESIFUL COMPILED :)";

				}catch (ShaderException &e) {
					LOG(DEBUG)<<"Error program Re-Compilation : "<< programs[current_program].name << " :" <<e.what()<<endl;
					programs[current_program].error_status=true;
					programs[current_program].modified=false;
					programs[current_program].error=e.what();
				}
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if(!programs[current_program].error_status)  {
				_base_system.displayProgram(programs[current_program].name);
			}else{
				_base_system.displayDefaultProgram();
			}

			lk.unlock();

			OpenGLContext::swapBuffers();

			TempoMap::updateStart(FPS_TIME);

		}

		glfwPollEvents(); //  It MUST be in the main thread

		//this_thread::yield();
		this_thread::sleep_for(std::chrono::milliseconds(1));

	}

	OpenGLContext::destroyAll();

	glfwTerminate();

	fprintf(stdout, " BYe Bye . musicrizz .");

	exit(EXIT_SUCCESS);

}

void pollingFragmentFiles()  {
	LOG(DEBUG)<<"CHECK IF FRAGMENTS ARE MODIFIED";
	checkFilesModified(fragments_folder);
	updateModifiedFragment();
}

void keyboardHandler(GLFWwindow* window, int key, int scancose, int action, int mods)  {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);

		}
		break;
	case GLFW_KEY_KP_ADD:
		if (action == GLFW_PRESS) {
			if(current_program < (int)programs.size()-1)  {
				current_program++;
			}else{
				current_program = 1;
			}
			glfwSetWindowTitle(window, programs[current_program].name.c_str());
		}
		break;
	case GLFW_KEY_KP_SUBTRACT:
		if (action == GLFW_PRESS) {
			if(current_program > 1)  {
				current_program--;
			}else{
				current_program = programs.size()-1;
			}
			glfwSetWindowTitle(window, programs[current_program].name.c_str());
		}
		break;
	}
}
