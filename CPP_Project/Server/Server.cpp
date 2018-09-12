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

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //���� ���̺귯�� �ʱ�ȭ
		 ErrorHandling("WSAStartup() error!");
		//std::cout << "WSAStartup() error!" << std::endl;

	hServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //���ϻ��� //(AF_INET, SOCK_STREAM, 0)

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");
		//std::cout << "socket() error" << std::endl;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) //���Ͽ� IP�ּҿ� PORT ��ȣ �Ҵ�
		ErrorHandling("bind() error");
		//std::cout << "bind() error" << std::endl;

	if (listen(hServSock, 5) == SOCKET_ERROR) //listen �Լ�ȣ���� ���ؼ� ������ ������ ���� �������� �ϼ�
		ErrorHandling("listen() error");
		//std::cout << "listen() error" << std::endl;
	while (1) 
	{
		szClntAddr = sizeof(clntAddr);
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr); //Ŭ���̾�Ʈ �����û �����ϱ� ���� accept�Լ� ȣ��

		if (hClntSock == INVALID_SOCKET)
			ErrorHandling("accept() error");
		//std::cout << "accept() error" << std::endl;

		send(hClntSock, message, sizeof(message), 0); //send�Լ� ȣ���� ���ؼ� ����� Ŭ���̾�Ʈ�� �����͸� ����
		closesocket(hClntSock);
	}
		closesocket(hServSock);
		WSACleanup(); //���α׷� ���� ���� �ʱ�ȭ�� ���� ���̺귯�� ����

		return 0;
	
}

void ErrorHandling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);

}