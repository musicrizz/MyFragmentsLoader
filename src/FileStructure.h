/*
 * FileStructure.h
 *
 *  Created on: Apr 24, 2021
 *      Author: gio
 */
#pragma once
#ifndef FILESTRUCTURE_H_
#define FILESTRUCTURE_H_

#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <mutex>
#include <functional>

#include "../third_party_lib/logging/easylogging++.h"
#include "../third_party_lib/image/stb_image.h"

using namespace std;
namespace fs = std::filesystem;

class FileStructure {
private:
	//----------------------------------------------------------------
	fs::path vertex_shader_file;
	fs::path fragments_folder;
	fs::path textures_folder;
	bool recursive;

	//Mutex for sync fileSystem polling and opengl system
	std::timed_mutex polling_mutex;
	bool flag_files_modified = false;


	//***Ordered Map of Fragment shader Sources files
	//This map have a key = file_name and a value of time of modified end bool flag for modification
	//Ordered per file_name
	struct classcomp {
	   bool operator() (const string& a, const string& b) const
	   {return a.substr(a.find_last_of("/\\")+1) < (b.substr(b.find_last_of("/\\")+1));}
	};
	map<string, std::pair<fs::file_time_type, bool>, classcomp> fragments_map;
	//--------------------------------------

	//***represent a program files loaded or not
	//for monitoring the status
	struct PROGRAM_FILE {
	public:
		string name,
			   path,
		       error;
		bool error_status = false,
			 modified = false;
	};

	map<int,PROGRAM_FILE> programs;


	struct TEXTURE_IMG {
		int w;
		int h;
		int nrChannles;
		unsigned char *data;
	};
	vector<TEXTURE_IMG> texture_images;

	static FileStructure* instance;
	static std::once_flag instance_flag_once;
	FileStructure()  {
		vertex_shader_file = "resources/default/vertex_default.glsl";
		fragments_folder   = "";
		textures_folder    = "";
		recursive = false;
	}

	FileStructure(const FileStructure &other) = delete;
	FileStructure(FileStructure &&other) = delete;
	FileStructure& operator=(const FileStructure &other) = delete;
	FileStructure& operator=(FileStructure &&other) = delete;

public:

	static FileStructure* getInstance();

	virtual ~FileStructure() {
		fragments_map.clear();
		programs.clear();
		texture_images.clear();
	}

	bool checkVertexFile(string v) {
		vertex_shader_file = v;
		return fs::is_regular_file(vertex_shader_file);
	}

	bool checkFragmentFolder(string v) {
		fragments_folder = v;
		return fs::is_directory(fragments_folder);
	}

	bool checkTextureFolder(string v) {
		textures_folder = v;
		return fs::is_directory(textures_folder);
	}

	void setRecursiveSearch(bool r)  {
		recursive = r;
	}

	fs::path getVertxShaderFile()  {
		return vertex_shader_file;
	}

	fs::path getFragmentShaderFolder()  {
		return fragments_folder;
	}

	void insertProgramFile(int pos, string pgr_name, string pgr_path, string errors, bool error_staus = false, bool modified = false)  {
		programs.insert(std::pair<int, PROGRAM_FILE>(pos, { pgr_name, pgr_path, errors, error_staus, modified}));
	}

	map<int,PROGRAM_FILE>& getPrograms_map() {
		 return ref(programs);
	}

	string getProgramName(int i)  {
		return programs[i].name;
	}

	string getProgramPath(int i)  {
		return programs[i].path;
	}

	bool isProgramModified(int p)  {
		return programs[p].modified;
	}

	void setProgramModified(int p, bool m)  {
		programs[p].modified = m;
	}

	bool isProgramErrorStatus(int p)   {
		return programs[p].error_status;
	}

	bool setProgramErrorStatus(int p, bool err)   {
		return programs[p].error_status = err;
	}

	string getProgramError(int p)  {
		return programs[p].error;
	}

	string setProgramError(int p, string err)  {
		return programs[p].error = err;
	}

	int getProgramsSize()  {
		return programs.size();
	}

	map<string, std::pair<fs::file_time_type, bool>, classcomp>& getFragmets_map()  {
		return ref(fragments_map);
	}

	vector<TEXTURE_IMG>& getTexImg_vec()  {
		return ref(texture_images);
	}

	std::timed_mutex& getTimedMutex_ref()  {
		return ref(polling_mutex);
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

	void loadTextureFiles()  {
		if(textures_folder.empty()) return;
		stbi_set_flip_vertically_on_load(true);
		int w, h, nrChannels;
		unsigned char *data;
		//alphabetically ordered file name
		set<string> files_name;
		for (const auto &f : fs::directory_iterator(textures_folder)) {
			if (f.is_regular_file())
				files_name.insert(f.path().string());
		}
		for (auto &f : files_name) {
			LOG(DEBUG)<<"Load Texture : "<<f;
			try{
				data = stbi_load(f.c_str(), &w, &h, &nrChannels, 0);
				if(data && nrChannels > 0) {
					texture_images.push_back({w ,h, nrChannels, data});
				}else{
					LOG(INFO)<<"Error to load image : "<<f;
				}
			}catch(...){}
		}
	}

	void checkFilesModified(fs::path dir)  {
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

};

#endif /* FILESTRUCTURE_H_ */
