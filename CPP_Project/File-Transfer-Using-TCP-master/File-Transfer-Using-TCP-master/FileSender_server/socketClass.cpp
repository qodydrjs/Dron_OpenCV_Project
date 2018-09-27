// socketClass.cpp : 사용할 소켓에 대한 속성 및 메서드 정의 클래스.
//

#include "stdafx.h"
#include "socketClass.h"
#include "serverMain.h"

extern server sv;

void mainSocket::initMainSocket() {
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		sv.errorPrint("Create Socket Error");

	ZeroMemory(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(PORT);
}

void mainSocket::socketBind() {
	int retval;
	if ((retval = bind(sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr))) == SOCKET_ERROR) {
		sv.errorPrint("Socket Bind Error");
	}
}

void mainSocket::setSockOptions() {
	int retval;

	int sendBuf, sendBufSize = sizeof(sendBuf);
	if ((retval = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuf, &sendBufSize)) == SOCKET_ERROR)
		sv.errorPrint("getSockOpt Error");
	printf("Before send buffer size : %d bytes\n", sendBuf);

	sendBuf = BUFSIZE;
	if ((retval = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuf, sendBufSize)) == SOCKET_ERROR)
		sv.errorPrint("getSockOpt Error");

	if ((retval = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuf, &sendBufSize)) == SOCKET_ERROR)
		sv.errorPrint("getSockOpt Error");
	printf("After send buffer size : %d bytes\n", sendBuf);
}

void subSocket::acceptSock() {
	addrSize = sizeof(sockAddr);
	while (1) {
		if ((sock = accept(sv.mainSock.sock, (SOCKADDR*)&sockAddr, &addrSize)) == INVALID_SOCKET)
			sv.errorPrint("Accept Error");
		else break;
	}
}