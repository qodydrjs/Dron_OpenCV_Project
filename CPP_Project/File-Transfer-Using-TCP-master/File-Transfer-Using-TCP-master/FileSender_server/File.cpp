// FileControl 클래스에서 사용되는 데이터(value)에 대한
// 객체를 생성하여 코드를 캡슐화하고 정보를 은닉한다.

#include "stdafx.h"
#include "File.h"

char* File::getFileName() {
	return fileName;
}

void File::setFileName(char* fName) {
	strcpy(fileName, fName);
}

int File::getFileSize() {
	return fileSize;
}

void File::setFileSize(int fSize) {
	fileSize = fSize;
}

HANDLE File::loadFile() {
	HANDLE hd = CreateFile
	(
		TEXT(getFileName()),   // 파일명
		GENERIC_READ,   // 읽기
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL
	);
	if (hd == INVALID_HANDLE_VALUE) {
		puts("Cannot Load File");
		exit(0);
	}

	return hd;
}

HANDLE File::mappingFile() {
	// 파일 매핑
	HANDLE hfm = CreateFileMapping
	(
		hFile,   // 맵 시킬 파일의 핸들
		NULL,   // 보안 설정
		PAGE_READONLY,   // 파일속성과 맞춤
		0,      // dwMaximumSizeHigh
		0,    // dwMaximumSizeLow
		NULL
	);
	return hfm;
}