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
			"\t-f   : specify folder where are located all fragment shaders \n"
			"\t-r   : (optional) set recursive search in subfolder of fragment folder \n"
			"\t-v   : (optional) specify your vertex shader file that will common to all fragment . ('overwrite the default') \n";
}

void keyboard(GLFWwindow* window, int key, int scancose, int action, int mods);
void mouseCallback(GLFWwindow* window, double y, double x);

void updateViewPort();
void updateTime();

int viewport_w, viewport_h;
float viewport_aspect;

enum VAO {V, VAOS_NUM};
unsigned int vaos[VAOS_NUM];

enum BUFFERS {B_VERTEX, UNIFORM, BUFFERS_NUM};
unsigned int buffers[BUFFERS_NUM];

static const char *CONTEXT="CONTEX_BASE",
				  *FPS_TIME="FPS_TIME";

bool flag_update_viewport=true;

int main(int argc, char **argv) {

	if(argc > 6 || argc < 3)  {
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
			cerr<<"ERROR : "<<argv[i+1]<<" is not a valid folder"<<endl;
			usage();
			return -1;
		}
		if(strcmp(argv[i], "-v") == 0 && !setVertexFile(argv[i+1])) {
			cerr<<"ERROR : "<<argv[i+1]<<" is not a valid file"<<endl;
			usage();
			return -1;
		}
		if(strcmp(argv[i], "-r") == 0 ) recursive = true;
	}

	if(vertex_shader_file.empty() || fragments_folder.empty())  {
		usage();
		return -1;
	}

	LOG(DEBUG)<<"Input parameter : "<<vertex_shader_file.c_str()<<" - "<<fragments_folder.c_str()<<endl;

	if(fs::is_regular_file(vertex_shader_file))  {
		LOG(DEBUG)<<"OK default vertex is regular";
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
	_default_program.init();

	//Binding point for uniform_buffer 'CommonUniform' shared among all fragments
	unsigned int uniform_binding_point = 2;

	//Program Name
	string name;
	for(const auto &f : fragments_map)  {
		try{
			name = f.first.substr(f.first.find_last_of("/\\")+1);
			LOG(DEBUG)<<"File to load : "<<name<<endl;
			ShaderMap::createProgram( name , vertex_shader_file.c_str(), f.first.c_str());
			ShaderMap::getProgram(name)->setBindingPoint(uniform_binding_point);
			programs.push_back({name, f.first, "", false});
		}catch (ShaderException &e) {
			LOG(DEBUG)<<"Error program creation : "<< f.first << " :" <<e.what()<<endl;
			programs.push_back({name, f.first, string(e.what()), true});
		}
	}

	if (programs.size() == 0)  {
		cerr<<"ERRORE NESSUN PROGRAMMMYA CARICATO"<<endl;
		exit(-1);
	}

	current_program = 0;

	for(auto p : programs)  {
		LOG(DEBUG)<<"Struttura : "<<p.name<<" \n\t "<<p.path<<" \n\t "<<p.error<<" \n\t "<<p.error_status<<endl;
	}

	LOG(DEBUG)<<"--------- Inizio caricamento openGl-------------------\n";

	ShaderMap::useProgram(programs[current_program].name);
	LOG(DEBUG)<<"Use program : "<<programs[current_program].name <<endl;

	glGenVertexArrays(VAOS_NUM, vaos);
	glGenBuffers(BUFFERS_NUM, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*4, NULL, GL_STATIC_DRAW);
	glm::vec4* v = (glm::vec4*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
		*v++ = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
		*v++ = glm::vec4( 1.0f, -1.0f, 0.0f, 1.0f);
		*v++ = glm::vec4( 1.0f,  1.0f, 0.0f, 1.0f);
		*v   = glm::vec4(-1.0f,  1.0f, 0.0f, 1.0f);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	LOG(DEBUG)<<OpenGLerror::check("creation buffer vertex");

	glBindVertexArray(vaos[V]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[B_VERTEX]);
		glVertexAttribPointer(ShaderMap::getAttributeLocation("position"), 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(ShaderMap::getAttributeLocation("position"));
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
//
	glfwGetFramebufferSize(OpenGLContext::getCurrent(), &viewport_w, &viewport_h);
	viewport_aspect = float(viewport_h) / float(viewport_w);
	flag_update_viewport = true;

	glfwSwapInterval(1);

	cout<<OpenGLerror::check("Finish setUp Opengl");

	glBindVertexArray(vaos[V]);

	while (!glfwWindowShouldClose(OpenGLContext::getCurrent())) {

		if (TempoMap::getElapsedMill(FPS_TIME) >= 30) {

			ShaderMap::useProgram(programs[current_program].name);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			updateViewPort();

			updateTime();

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			OpenGLContext::swapBuffers();

			TempoMap::updateStart(FPS_TIME);

			ShaderMap::program_null();
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
		glViewport(0, 0, viewport_w, viewport_h);
		glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, 8, glm::value_ptr(glm::ivec2(viewport_w, viewport_h)));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		flag_update_viewport = false;
	}
}

void updateTime()   {
	float time = float(glfwGetTime());
	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, reinterpret_cast<void*>(&time) );
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void mouseCallback(GLFWwindow* window, double x, double y)  {
	glBindBuffer(GL_UNIFORM_BUFFER, buffers[UNIFORM]);
		glBufferSubData(GL_UNIFORM_BUFFER, 8, 8, glm::value_ptr(glm::vec2(float(x)/float(viewport_w), float(viewport_h-y)/float(viewport_h))));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
				current_program = 0;
			}
		}
		break;
	case GLFW_KEY_KP_SUBTRACT:
		if (action == GLFW_PRESS) {
			if(current_program > 1)  {
				current_program--;
			}else{
				current_program = programs.size()-1;
			}
		}
		break;
	}
}
