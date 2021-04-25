/*
 * FileStructure.cpp
 *
 *  Created on: Apr 24, 2021
 *      Author: gio
 */

#include "FileStructure.h"

FileStructure* FileStructure::instance = nullptr;
std::once_flag FileStructure::instance_flag_once;

FileStructure* FileStructure::getInstance()  {
	std::call_once(instance_flag_once, []()-> FileStructure* {
		return FileStructure::instance = new FileStructure();
	});
	return instance;
}


