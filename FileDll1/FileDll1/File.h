#pragma once

#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
#include <windows.h>
#include <thread>
#include <string>
#include <process.h>
#include "../common/export_define.h"

#define _MAX_FILE_LENGTH 256
#define _MAX_PATH_LENGTH 1024

class EXPORTDLL File {
private:
	char fileName[_MAX_FILE_LENGTH];
	int fileSize;
	char filePath[_MAX_PATH_LENGTH];

public:
	HANDLE hFile;
	HANDLE hFileMapping;
	char* pbFile;

	char* getFileName();
	void setFileName(char*);

	int getFileSize();
	void setFileSize(int);

	char* getFilePath();
	void setFilePath(char*);

	inline void reset() const
	{

	}

	HANDLE loadFile(const char*);
	HANDLE mappingFile();
};
