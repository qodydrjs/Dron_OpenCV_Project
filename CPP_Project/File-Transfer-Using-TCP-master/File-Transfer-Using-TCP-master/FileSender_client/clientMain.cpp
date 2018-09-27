// clientMain.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "clientMain.h"
#include "File.h"

time_t start_time, end_time;
float gap;
extern File file;

void WINAPI dataRecv(LPVOID);

void client::init() {
	start_time = clock(); // 시작 시간 측정

	// WinSock initialization
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		errorPrint("WSAStartup Error");

	user.nameSock.initSocket();
	user.sizeSock.initSocket();
	user.dataSock.initSocket();
}

void client::startProgram() {
	init();
	user.dataSock.setSockOptions();

	// Connect - 서버 측에 연결 요청
	int retval;
	retval = connect(user.nameSock.sock,
		(SOCKADDR*)&user.nameSock.sockAddr, user.nameSock.addrSize);
	retval = connect(user.sizeSock.sock,
		(SOCKADDR*)&user.sizeSock.sockAddr, user.sizeSock.addrSize);
	if ((retval = connect(user.dataSock.sock,
		(SOCKADDR*)&user.dataSock.sockAddr, user.dataSock.addrSize)) == SOCKET_ERROR)
		errorPrint("Connect Error");

	// 파일 이름 받기
	char fName[_MAX_FILE_LENGTH] = { NULL };
	recv(user.nameSock.sock, fName, sizeof(fName), 0);
	file.setFileName(fName);

	// 파일 크기 받기
	int fSize;
	recv(user.sizeSock.sock, (char*)&fSize, sizeof(fSize), 0);
	file.setFileSize(fSize);
	closesocket(user.nameSock.sock);
	closesocket(user.sizeSock.sock);

	file.hFile = file.saveFile();
	file.hFileMapping = file.mappingFile();
	file.pbFile = file.createMapView();

	startFileRecv();
}

void client::startFileRecv() {
	user.hThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)dataRecv,
		(SOCKET*)user.dataSock.sock,
		0,
		&user.hThreadID
	);
	WaitForSingleObject(user.hThread, INFINITE);
	puts("File Data Recving Finish");

	chkFileTransfer(user.dataSock.sock);
}

void client::chkFileTransfer(SOCKET dataSock) {
	// 받은 파일 사이즈 전송
	file.recvFileSize = GetFileSize(file.hFile, NULL);
	int retval = send(dataSock, (char*)&file.recvFileSize, sizeof(file.recvFileSize), 0);
	endProgram();
}

void client::endProgram() {
	end_time = clock(); // 종료 시간 측정
	gap = (float)(end_time - start_time) / (CLOCKS_PER_SEC); // 총 걸린 시간
	printf("\nTime : %.3f s\n\n", gap);
	printf("Received File Size : %d bytes\n", file.recvFileSize);
	if (file.getFileSize() == file.recvFileSize)
		puts("File Receiving Complete!");
	else
		puts("File Receiving Error, plese retry");

	CloseHandle(file.hFileMapping);
	CloseHandle(file.hFile);
	closesocket(user.dataSock.sock);
	WSACleanup();
}

void client::errorPrint(char* message) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", message, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

char* client::getServerIP() {
	return serverIP;
}

void client::setServerIP(char* sIP) {
	strcpy(serverIP, sIP);
}

int client::getServerPORT() {
	return serverPORT;
}

void client::setServerPORT(int sPORT) {
	serverPORT = sPORT;
}

void WINAPI dataRecv(LPVOID arg) {
	SOCKET dataSock = (SOCKET)arg;
	SOCKADDR_IN dataAddr;
	int dataAddrSz = sizeof(dataAddr);
	int retval;

	getpeername(dataSock, (SOCKADDR*)&dataAddr, &dataAddrSz);

	puts("File Data Recving Start");
	do {
		retval = recv(dataSock, file.pbFile, BUFSIZE, 0);
		file.pbFile += retval;
		if (retval == 0) break;
		else if (retval == SOCKET_ERROR) break;
	} while (retval > 0);
}