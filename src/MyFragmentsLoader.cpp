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
	cout<<"USAGE: MyFragmentsLoader -f <fragment_shaders_folder> \n"
			"\t -f   : folder where are located all fragment shaders \n"
			"\t -r   : (optional) set recursive search in subfolder of fragment folder \n"
			"\t -t   : (optional) folder of texture images. ('will be loaded in alphabetically order and ca be accessed in fragment with []' - no recursive in sub-forlder)\n"
			"\t -v   : (optional) your vertex shader file that will be common to all fragments. ('overwrite the default') \n";
}

void keyboard(GLFWwindow* window, int key, int scancose, int action, int mods);
void mouseCallback(GLFWwindow* window, double y, double x);

void updateViewPort();
void updateTime();

static const char *CONTEXT="CONTEX_BASE",
				  *FPS_TIME="FPS_TIME";

bool flag_update_viewport=true;

int main(int argc, char **argv) {

	if(argc > 8 || argc < 3)  {
		usage();
		return -1;
	}

	START_EASYLOGGINGPP(argc, argv);
	el::Configurations log_conf("conf/logger.conf");
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

	OpenGLContext::setMouseCursorPos(mouseCallback);
	OpenGLContext::setKeyboard(keyboard);

	loadFragmentFiles(fragments_folder);

	//CREATE DEFAULT PROGRAM FOR DISPLAY COMPILATION ERROR in position 0
	_base_system.createDefaultProgram();

	//Create a program for each fragment file
	for(const auto &f : fragments_map)  {
		string name = f.first.substr(f.first.find_last_of("/\\")+1);
		LOG(DEBUG)<<"File to load : "<<name<<endl;

		try{
			ShaderMap::createProgram( name , vertex_shader_file.c_str(), f.first.c_str());
			//set Binding point for uniform_buffer
			ShaderMap::getProgram(name)->setBindingPoint(_base_system.uniform_binding_point);
			programs.push_back({name, f.first, "", false});
		}catch (ShaderException &e) {
			LOG(DEBUG)<<"Error program creation : "<< f.first << " :" <<e.what()<<endl;
			programs.push_back({name, f.first, string(e.what()), true});
		}
	}

	if (programs.size() == 0)  {
		cerr<<"ERROR NO PROGRAM LOADED"<<endl;
		exit(-1);
	}

	current_program = 1;

	for(auto p : programs)  {
		LOG(DEBUG)<<"Struttura : "<<p.name<<" \n\t "<<p.path<<" \n\t "<<p.error<<" \n\t "<<p.error_status<<endl;
	}

	_base_system.initOpenGLBuffers();

	glfwGetFramebufferSize(OpenGLContext::getCurrent(), &viewport_w, &viewport_h);
	viewport_aspect = float(viewport_h) / float(viewport_w);
	flag_update_viewport = true;

	glfwSwapInterval(1);

	cout<<OpenGLerror::check("Finish setUp Opengl");
	glfwSetWindowTitle(OpenGLContext::getCurrent(), programs[current_program].name.c_str());

	while (!glfwWindowShouldClose(OpenGLContext::getCurrent())) {

		if (TempoMap::getElapsedMill(FPS_TIME) >= 30) {

			if(!programs[current_program].error_status)  {
				_base_system.displayProgram(programs[current_program].name);
			}else{
				_base_system.displayDefaultProgram();
			}

			OpenGLContext::swapBuffers();

			TempoMap::updateStart(FPS_TIME);


		}

		glfwPollEvents(); //  It MUST be in the main thread

		//this_thread::yield();

	}

	OpenGLContext::destroyAll();

	glfwTerminate();

	fprintf(stdout, " MAIN FINISH .");

	exit(EXIT_SUCCESS);

}


void updateViewPort() {
	if (flag_update_viewport) {
		_base_system.updateViewPort();
		flag_update_viewport = false;
	}
}

void updateTime()   {
	_base_system.updateTime();
}

void mouseCallback(GLFWwindow* window, double x, double y)  {
	_base_system.updateMouse(x,y);
}

void keyboard(GLFWwindow* window, int key, int scancose, int action, int mods)  {
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);

		}
		break;
	case GLFW_KEY_KP_ADD:
		if (action == GLFW_PRESS) {
			if(current_program < programs.size()-1)  {
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
