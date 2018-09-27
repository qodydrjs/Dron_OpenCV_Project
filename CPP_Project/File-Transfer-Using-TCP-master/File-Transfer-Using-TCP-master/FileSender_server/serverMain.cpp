// serverMain.cpp : serverMain Class.
//

#include "stdafx.h"
#include "serverMain.h"
#include "Client.h"
#include "File.h"
#include <iostream>

extern File file;
void WINAPI dataSending(LPVOID);

bool server::init() {
	// WinSock initialization
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;
	mainSock.initMainSocket();
	mainSock.socketBind();

	file.hFile = file.loadFile();
	file.setFileSize(GetFileSize(file.hFile, NULL));
	file.hFileMapping = file.mappingFile();

	return true;
}

void server::startServer() {
	if(init() == false)
		exit(0);
	mainSock.setSockOptions();

	// ������ �����·� �����Ѵ�.
	int retval = listen(mainSock.sock, 0);
	if (retval == SOCKET_ERROR) {
		errorPrint("Socket Listen Error");
	}
	
	// Ŭ���̾�Ʈ�� ������ ��ٸ�
	puts("\nWaiting for Client...\n");
	startFileTransfer();
	endServer();
}

void server::startFileTransfer() {
	// Accept - Ŭ���̾�Ʈ ������ �޾Ƶ��� = Ŭ���̾�Ʈ�� ���� �� ���� ����
	clients clnt;
	clients* pclnt;
	int retval;
	//
	retval = send(clnt.nameSock.sock, "#i", sizeof("#i"), 0);
	std::cout << "Read : " << retval << std::endl;
	//// ���� �̸� ����
	//char fnm[_MAX_FILE_LENGTH] = { NULL };
	//strcpy(fnm, file.getFileName());
	//retval = send(clnt.nameSock.sock, fnm, sizeof(fnm), 0);
	//std::cout << "Read : " << retval << std::endl;
	//
	//// ���� ũ�� ����

	unsigned int fsz = file.getFileSize();
	retval = send(clnt.sizeSock.sock, (char*)&fsz, sizeof(fsz), 0);
	std::cout << "Read : " << fsz << std::endl;
	// ���� ������ ����
	pclnt = &clnt;
	puts("file data sending start");
	clnt.hDataThread = CreateThread
	(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)dataSending,
		(clients*)pclnt,
		0,
		&pclnt->hDataThreadID
	);
	WaitForSingleObject(pclnt->hDataThread, INFINITE);


	std::cout << "Read : " << retval << std::endl;
	//image end flag
	//retval = send(clnt.nameSock.sock, "$i", sizeof("$i"), 0);
	std::cout << "Read : " << retval << std::endl;
	//int a;
	//std::cin >> a;
	puts("File Sengind Finish !!!!!!!!!!!!");
	//shutdown(pclnt->dataSock.sock, SD_SEND);

	//chkFileTransfer(pclnt->dataSock.sock);
}

void server::chkFileTransfer(SOCKET dataSock) {
	// Ŭ���̾�Ʈ���� ���� ���� ������
	unsigned int sendSize;
	int retval;
	if ((retval = recv(dataSock, (char*)&sendSize, sizeof(sendSize), 0)) == SOCKET_ERROR)
		errorPrint("Recv Respone Message Error");

	// Ŭ���̾�Ʈ ������� ���� ������ �� - ������ ����, �ٸ��� ����
	if (sendSize != file.getFileSize()) puts("File Data Sending Error, plese retry");
	else puts("\nFile Data Sending Complete!");
	closesocket(dataSock);
}

void server::endServer() {
	CloseHandle(file.hFileMapping);
	CloseHandle(file.hFile);
	closesocket(mainSock.sock);
	WSACleanup();
}

void server::errorPrint(char* message) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", message, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void WINAPI dataSending(LPVOID arg) {
	clients* pclnt = (clients*)arg;
	SOCKADDR_IN dataAddr;
	int dataAddrSz = sizeof(dataAddr);
	int retval;

	getpeername(pclnt->dataSock.sock, (SOCKADDR*)&dataAddr, &dataAddrSz);

	DWORD dwFileSizeHigh;
	__int64 qwFileSize = GetFileSize(file.hFile, &dwFileSizeHigh);

	__int64 qwFileOffset = 0;

	while (qwFileSize > 0) {
		DWORD dwBytesInBlock = BUFSIZE;
		if (qwFileSize < BUFSIZE) {
			dwBytesInBlock = qwFileSize;
		}

		file.pbFile = (char*)MapViewOfFile
		(
			file.hFileMapping,
			FILE_MAP_READ,
			(DWORD)(qwFileOffset >> 32),  // ���� ������
			(DWORD)(qwFileOffset & 0xFFFFFFFF),
			dwBytesInBlock
		);
		// ��������
		retval = send(pclnt->dataSock.sock, file.pbFile, dwBytesInBlock, 0);
		if (retval != dwBytesInBlock) {
			while (1) {
				retval = send(pclnt->dataSock.sock, file.pbFile, dwBytesInBlock, 0);
				if (retval != 0) break;
			}
		}
		// �並 �� �����Ƿ�, �並 �����Ѵ�.
		UnmapViewOfFile(file.pbFile);

		// ������ �� ���� ���� ũ�� ����
		qwFileOffset += dwBytesInBlock;
		qwFileSize -= dwBytesInBlock;
	}
}
