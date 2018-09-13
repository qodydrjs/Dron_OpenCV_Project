// socketClass.cpp : 소켓에 대한 속성 및 메서드 정의 클래스.
//

#include "stdafx.h"
#include "socketClass.h"
#include "clientMain.h"

extern client c;

void clientSocket::initSocket() {
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		c.errorPrint("Create Socket Error");

	addrSize = sizeof(sockAddr);
	ZeroMemory(&sockAddr, addrSize);
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(c.getServerIP());
	sockAddr.sin_port = htons(c.getServerPORT());
}

void clientSocket::setSockOptions() {
	// 소켓 옵션 설정 - 수신 버퍼 크기 설정
	int retval, recvBuf = BUFSIZE;
	retval = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&recvBuf, sizeof(recvBuf));
	printf("Recv buffer size : %d bytes\n", recvBuf);

	// Nagle 알고리즘
	int opt_val = TRUE;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)opt_val, sizeof(opt_val));
}