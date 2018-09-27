#pragma once

class File {
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

	HANDLE loadFile();
	HANDLE mappingFile();
};
