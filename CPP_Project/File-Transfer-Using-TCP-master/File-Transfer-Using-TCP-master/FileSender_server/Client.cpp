// Client.cpp : 접속한 사용자에 대한 정보 저장 클래스.
//

#include "stdafx.h"
#include "client.h"

clients::clients() {
	nameSock.acceptSock();
	sizeSock.acceptSock();
	dataSock.acceptSock();

	strcpy(cIP, inet_ntoa(dataSock.sockAddr.sin_addr));
	cStartPort = ntohs(nameSock.sockAddr.sin_port);
	cEndPort = ntohs(dataSock.sockAddr.sin_port);

	printf("Client Connect from IP : %s, PORT : %d-%d\n\n", cIP, cStartPort, cEndPort);
}