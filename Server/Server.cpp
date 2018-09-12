#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
#include <windows.h>

#define SERVER_PORT 5000

void ErrorHandling(const char *message);

int main(int argc, char *argv[])
{

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	int szClntAddr;
	char message[] = "Hello World!";
	if (argc != 2)

	{
		printf("Usage:%s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //소켓 라이브러리 초기화
		 ErrorHandling("WSAStartup() error!");
		//std::cout << "WSAStartup() error!" << std::endl;

	hServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //소켓생성 //(AF_INET, SOCK_STREAM, 0)

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");
		//std::cout << "socket() error" << std::endl;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) //소켓에 IP주소와 PORT 번호 할당
		ErrorHandling("bind() error");
		//std::cout << "bind() error" << std::endl;

	if (listen(hServSock, 5) == SOCKET_ERROR) //listen 함수호출을 통해서 생성한 소켓을 서버 소켓으로 완성
		ErrorHandling("listen() error");
		//std::cout << "listen() error" << std::endl;
	while (1) 
	{
		szClntAddr = sizeof(clntAddr);
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr); //클라이언트 연결요청 수락하기 위해 accept함수 호출

		if (hClntSock == INVALID_SOCKET)
			ErrorHandling("accept() error");
		//std::cout << "accept() error" << std::endl;

		send(hClntSock, message, sizeof(message), 0); //send함수 호출을 통해서 연결된 클라이언트에 데이터를 전송
		closesocket(hClntSock);
	}
		closesocket(hServSock);
		WSACleanup(); //프로그램 종료 전에 초기화한 소켓 라이브러리 해제

		return 0;
	
}

void ErrorHandling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);

}