#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
#include <windows.h>
#include <string>
#include <thread>

#define SERVER_PORT 5000

void ErrorHandling(const char *message);
void*  recv_msg(SOCKET* arg);
void*  send_msg(SOCKET* arg);


int main(int argc, char *argv[])
{

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	int szClntAddr;
	char message[] = "Hello World!";
	/*if (argc != 2)

	{
		printf("Usage:%s <port>\n", argv[0]);
		exit(1);
	}*/

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
	servAddr.sin_port = htons(SERVER_PORT);
	//servAddr.sin_port = htons(atoi(argv[1]));

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
		
		std::thread t1(recv_msg, &hClntSock);
		std::thread t2(send_msg, &hClntSock);
		t1.join();

		
		
		
	}
		closesocket(hClntSock);
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

void * recv_msg(SOCKET* arg)
{

	SOCKET* sock = reinterpret_cast<SOCKET*>(arg);
	int str_len = 0;
	char buf[50] = { 0, };
	std::string sRand;

	while (1)
	{
		if ((str_len = recv(*sock, buf, 50, 0)) > 0)
		{
			buf[str_len] = 0; // make a string
			sRand = buf;
			std::cout << "Sending \"" << sRand << " to client" << std::endl;
		}
	}
	return 0;
}

void * send_msg(SOCKET * arg)
{
	SOCKET* sock = reinterpret_cast<SOCKET*>(arg);
	int ret;
	struct timeval tv;
	fd_set initset, newset;
	FD_ZERO(&initset);
	FD_SET(_fileno(stdin), &initset);

	char buf[50] = { 0, };
	fputs("Input a Message! \n ", stdout);
	while (1)
	{
		for (int i = 0; i < 50; i++)
			buf[i] = 0;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		newset = initset;
		ret = select(_fileno(stdin) + 1, &newset, NULL, NULL, &tv);
		//__WSAFDIsSet(_fileno(stdin), &newset)
		//FD_ISSET(_fileno(stdin), &newset)
		if (__WSAFDIsSet(_fileno(stdin), &newset))
		{
			fgets(buf, 50, stdin);
			if (!strncmp(buf, "quit\n", 5))
			{
				*sock = -1;
				return NULL;
			}
			if ((send(*sock, buf, strlen(buf), 0)) <= 0)
			{
				*sock = -1;
				return NULL;
			}
		}
	}

	return nullptr;
}
