// File.h

#pragma once

class File {
private:
	char fileName[_MAX_FILE_LENGTH];
	int fileSize;

public:
	HANDLE hFile;
	HANDLE hFileMapping;
	char* pbFile;
	int recvFileSize;

	char* getFileName();
	void setFileName(char*);

	int getFileSize();
	void setFileSize(int);
	
	HANDLE saveFile();
	HANDLE mappingFile();
	char* createMapView();
};
