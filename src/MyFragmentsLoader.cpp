//============================================================================
// Name        : ShadersBookExample.cpp
// Author      : Giovanni Grandinetti
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "OpenGLSystem.h"
#include "FileStructure.h"

//using namespace std;
//namespace fs = std::filesystem;

#define ELPP_THREAD_SAFE
INITIALIZE_EASYLOGGINGPP



void usage()  {
	cout<<"Usage:  \n"
			"   options: \n"
			"   -f   : folder where are located all fragment shaders (mandatory) \n\n"

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
				  *FPS_TIME="FPS_TIME",
				  *DELTA_TIME="DELTA_TIME",
				  *FILE_POLLING_TIMER="FILE_POLLING_TIMER";

FileStructure* _file_structure = nullptr;
int current_program = 0;

int main(int argc, char **argv) {

	if(argc > 8 || argc < 3)  {
		usage();
		return -1;
	}

	START_EASYLOGGINGPP(argc, argv);
	el::Configurations log_conf("config/logger.conf");
	el::Loggers::reconfigureLogger("default", log_conf);
	el::Loggers::reconfigureAllLoggers(log_conf);

	_file_structure = FileStructure::getInstance();

	fs::path tmp;
	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-f") == 0 && !_file_structure->checkFragmentFolder( string(argv[i+1])) ) {
			cerr<<"ERROR : "<<argv[i+1]<<" is not a valid fragments folder"<<endl;
			usage();
			return -1;
		}
		if(strcmp(argv[i], "-t") == 0 && !_file_structure->checkTextureFolder( string(argv[i+1])) ) {
			cerr<<"ERROR : "<<argv[i+1]<<" is not a valid textures folder"<<endl;
			usage();
			return -1;
		}
		if(strcmp(argv[i], "-v") == 0 && !_file_structure->checkVertexFile( string(argv[i+1])) ) {
			cerr<<"ERROR : "<<argv[i+1]<<" is not a valid vertex file"<<endl;
			usage();
			return -1;
		}
		if(strcmp(argv[i], "-r") == 0 ) _file_structure->setRecursiveSearch(true);
	}

	if(_file_structure->getVertxShaderFile().empty() || _file_structure->getFragmentShaderFolder().empty())  {
		cerr<<"MMMMMMMMMMMMMMMMMMMMMMM\n";
		usage();
		return -1;
	}

	if (!glfwInit()) {
		fprintf(stderr, "GLEW INTI ERROR");
		exit(EXIT_FAILURE);
	}

	//Load fragments and texture if exists
	_file_structure->loadFragmentFiles(_file_structure->getFragmentShaderFolder());
	_file_structure->loadTextureFiles();

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

	OpenGLSystem* _opengl_system = new OpenGLSystem();

	OpenGLContext::createContex(CONTEXT, 600, 600, NULL, true, true, false);
	OpenGLContext::makecurrent(CONTEXT);
	OpenGLContext::setUserPointer(_opengl_system);

	OpenGLContext::setReshape(CONTEXT, [](GLFWwindow* window, int width, int height){
		OpenGLSystem* p = (OpenGLSystem*)glfwGetWindowUserPointer(window);
		p->setViewportW(width);
		p->setViewportH(height);
		p->setFlagUpdateViewport(true);
	});

	OpenGLContext::setMouseCursorPos([](GLFWwindow* window, double x, double y){
		OpenGLSystem* p = (OpenGLSystem*)glfwGetWindowUserPointer(window);
		p->updateMousePosition(x,y);
	});

	OpenGLContext::setMouseScroll([](GLFWwindow* window, double x, double y){
		OpenGLSystem* p = (OpenGLSystem*)glfwGetWindowUserPointer(window);
		p->updateMouseScroll(x, y);
	});

	OpenGLContext::setKeyboard(keyboardHandler);

	//CREATE DEFAULT PROGRAM FOR DISPLAY COMPILATION ERROR in position 0
	_opengl_system->createDefaultProgram();

	//Create a program for each fragment file
	current_program = 1;
	for(const auto &f : _file_structure->getFragmets_map())  {

		string name = f.first.substr(f.first.find_last_of("/\\")+1);
		LOG(DEBUG)<<"File to load : "<<name<<endl;

		try{
			ShaderMap::createProgram( name , _file_structure->getVertxShaderFile().c_str(), f.first.c_str());
			//set Binding point for uniform_buffer
			ShaderMap::getProgram(name)->setBindingPoint(_opengl_system->getUniformBindingPoint());
			_file_structure->insertProgramFile(current_program++, name, f.first, "");

		}catch (ShaderException &e) {
			LOG(INFO)<<"Error program creation : "<< f.first << " :" <<e.what()<<endl;
			_file_structure->insertProgramFile(current_program++, name, f.first, string(e.what()), true, false);
		}
	}

	if (_file_structure->getProgramsSize() < 2)  {
		cerr<<"ERROR NO PROGRAM LOADED"<<endl;
		exit(-1);
	}

	current_program = 1;

#if DEVELOPEMENT
	for(auto p : _file_structure->getPrograms_map())  {
		LOG(DEBUG)<<"Struttura : "<<p.second.name<<" \n\t "<<p.second.path<<" \n\t "<<p.second.error<<" \n\t "<<p.second.error_status<<endl;
	}
#endif

	_opengl_system->initOpenGLBuffers();

	{
	int w,h;
	glfwGetFramebufferSize(OpenGLContext::getCurrent(), &w, &h);
	_opengl_system->setViewportW(w);
	_opengl_system->setViewportH(h);
	_opengl_system->setFlagUpdateViewport(true);
	}

	glfwSwapInterval(1);

	glfwSetWindowTitle(OpenGLContext::getCurrent(), _file_structure->getProgramName(current_program).c_str());

	//pollingFragmentFiles will be executed in another thread created by TempoMap
	TempoMap::createTimer(FILE_POLLING_TIMER, pollingFragmentFiles, 500);

	int delta_time = 0;
	int fps = 0;
	while (!glfwWindowShouldClose(OpenGLContext::getCurrent())) {

		if (TempoMap::getElapsedMill(FPS_TIME) >= 33) {

			std::unique_lock<std::timed_mutex> lk(_file_structure->getTimedMutex_ref(), std::defer_lock);
			if(!lk.try_lock_for(std::chrono::microseconds(500)))continue;

			TempoMap::updateStart(DELTA_TIME);

			if(_file_structure->isProgramModified(current_program))  {
				try{
					LOG(INFO)<<"Program "<<_file_structure->getProgramName(current_program) <<" IS MODIFIED !";
					_file_structure->setProgramModified(current_program, false);

					ShaderMap::deleteProgram(_file_structure->getProgramName(current_program));

					ShaderMap::createProgram(_file_structure->getProgramName(current_program),
											 _file_structure->getVertxShaderFile().c_str(),
											 _file_structure->getProgramPath(current_program).c_str());

					ShaderMap::getProgram(_file_structure->getProgramName(current_program))
								->setBindingPoint(_opengl_system->getUniformBindingPoint());

					ShaderMap::bindingUniformBlocks(_opengl_system->getCommonUniformName(),
													_opengl_system->getUniformBindingPoint());

					ShaderMap::bindingUniformBlocksForSingleProgram(_file_structure->getProgramName(current_program),
														_opengl_system->getCommonUniformName(),
														_opengl_system->getUniformBindingPoint());

					_file_structure->setProgramError(current_program, "");
					_file_structure->setProgramErrorStatus(current_program, false);

					LOG(INFO)<<"Program "<<_file_structure->getProgramName(current_program) <<"SUCCESIFUL COMPILED :)";

				}catch (ShaderException &e) {
					LOG(INFO)<<"Error program Re-Compilation : "<< _file_structure->getProgramName(current_program) << " :" <<e.what()<<endl;
					_file_structure->setProgramError(current_program, e.what());
					_file_structure->setProgramErrorStatus(current_program, true);
					_file_structure->setProgramModified(current_program, false);
				}
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if(!_file_structure->isProgramErrorStatus(current_program))  {
				_opengl_system->displayProgram(_file_structure->getProgramName(current_program));
			}else{
				_opengl_system->displayDefaultProgram();
			}

			lk.unlock();

			OpenGLContext::swapBuffers();

			delta_time = TempoMap::getElapsedMill(DELTA_TIME);

			TempoMap::updateStart(FPS_TIME);fps++;

		}
		//Perfect !
//		if(TempoMap::getElapsedSeconds("DEBUG_TIME_DELTA_AND_FPS") > 1)  {
//			LOG(DEBUG)<<"Delta :"<<delta_time<<", FPS : "<<fps<<endl;
//			fps=0;
//			TempoMap::updateStart("DEBUG_TIME_DELTA_AND_FPS");
//		}

		glfwPollEvents(); //  It MUST be in the main thread
//
		//this_thread::yield();
		this_thread::sleep_for(std::chrono::milliseconds(1));

	}

	OpenGLContext::destroyAll();

	glfwTerminate();

	fprintf(stdout, " BYe Bye . musicrizz . \n");

	exit(EXIT_SUCCESS);

}

void pollingFragmentFiles()  {
	//LOG(DEBUG)<<"CHECK IF FRAGMENTS ARE MODIFIED";
	_file_structure->checkFilesModified(_file_structure->getFragmentShaderFolder());
	_file_structure->updateModifiedFragment();
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
			if(current_program < _file_structure->getProgramsSize()-1)  {
				current_program++;
			}else{
				current_program = 1;
			}
			glfwSetWindowTitle(window, _file_structure->getProgramName(current_program).c_str());
		}
		break;
	case GLFW_KEY_KP_SUBTRACT:
		if (action == GLFW_PRESS) {
			if(current_program > 1)  {
				current_program--;
			}else{
				current_program = _file_structure->getProgramsSize()-1;
			}
			glfwSetWindowTitle(window, _file_structure->getProgramName(current_program).c_str());
		}
		break;
	}
}
