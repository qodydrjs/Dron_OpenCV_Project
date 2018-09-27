#pragma once
#include "File.h"

class fileControl {
public:
	char fName[_MAX_FILE_LENGTH];
	int fSize;
	char fPath[_MAX_PATH_LENGTH];

	File getFileInfo(char*);
	int chkFileSize();
};