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

HANDLE File::saveFile() {
	HANDLE hd = CreateFile
	(
		TEXT(getFileName()),   // ���ϸ�
		GENERIC_WRITE | GENERIC_READ,   // �б�
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (hd == INVALID_HANDLE_VALUE) {
		puts("Cannot Save File");
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
		PAGE_READWRITE,   // ���ϼӼ��� ����
		0,      // dwMaximumSizeHigh
		fileSize,    // dwMaximumSizeLow
		NULL
	);
	return hfm;
}

char* File::createMapView() {
	char* pbf = (char*)MapViewOfFile
	(
		hFileMapping,
		FILE_MAP_WRITE,
		0,  // ���� ������
		0,
		getFileSize()
	);

	return pbf;
}