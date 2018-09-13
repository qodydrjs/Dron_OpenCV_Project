// FileControl Ŭ�������� ���Ǵ� ������(value)�� ����
// ��ü�� �����Ͽ� �ڵ带 ĸ��ȭ�ϰ� ������ �����Ѵ�.

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
		TEXT(getFileName()),   // ���ϸ�
		GENERIC_READ,   // �б�
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
	// ���� ����
	HANDLE hfm = CreateFileMapping
	(
		hFile,   // �� ��ų ������ �ڵ�
		NULL,   // ���� ����
		PAGE_READONLY,   // ���ϼӼ��� ����
		0,      // dwMaximumSizeHigh
		0,    // dwMaximumSizeLow
		NULL
	);
	return hfm;
}