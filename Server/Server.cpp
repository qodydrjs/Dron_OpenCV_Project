#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
#include <windows.h>
#include <thread>
#include <string>
#include <process.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

#define SERVER_PORT 5000

void ErrorHandling(const char *message);
unsigned WINAPI HandleClnt(void *arg);
void SendMsg(char *msg, int len);

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

int main(int argc, char *argv[])
{

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	int clntAdrSz;
	HANDLE hThread;

	/*if (argc != 2)
	{
	printf("Usage:%s <port>\n", argv[0]);
	exit(1);
	}*/

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //소켓 라이브러리 초기화
		ErrorHandling("WSAStartup() error!");


	hMutex = CreateMutex(NULL, FALSE, NULL);
	//hServSock = socket(AF_INET, SOCK_STREAM, 0); //소켓생성 //(AF_INET, SOCK_STREAM, 0)
	hServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //소켓생성 //(AF_INET, SOCK_STREAM, 0)

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");


	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERVER_PORT);
	//servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) //소켓에 IP주소와 PORT 번호 할당
		ErrorHandling("bind() error");


	if (listen(hServSock, 5) == SOCKET_ERROR) //listen 함수호출을 통해서 생성한 소켓을 서버 소켓으로 완성
		ErrorHandling("listen() error");

	while (1)
	{
		clntAdrSz = sizeof(clntAddr);
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAdrSz); //클라이언트 연결요청 수락하기 위해 accept함수 호출

		if (hClntSock == INVALID_SOCKET)
			ErrorHandling("accept() error");
		std::cout << "accept()" << std::endl;


		WaitForSingleObject(hMutex, INFINITE);
		clntSocks[clntCnt++] = hClntSock;
		ReleaseMutex(hMutex);

		//hThread = (std::thread)_beginthreadex(NULL, 0, HandleClnt, (void *)&hClntSock, 0, NULL);
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void *)&hClntSock, 0, NULL);
		printf("Connected client IP: %s \n", inet_ntoa(clntAddr.sin_addr));



		//send(hClntSock, message, sizeof(message), 0); //send함수 호출을 통해서 연결된 클라이언트에 데이터를 전송

		//std::thread t1(send_msg1, &hClntSock);
		//std::thread t2(send_msg, &hClntSock);

		//t1.join();

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
unsigned WINAPI HandleClnt(void * arg) {
	SOCKET hClntSock = *((SOCKET*)arg);
	int strLen = 0, i;
	char msg[BUF_SIZE];

	while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != 0)
		SendMsg(msg, strLen);

	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clntCnt; i++)
	{
		if (hClntSock == clntSocks[i])
		{
			while (i++ < clntCnt - 1)
				clntSocks[i] = clntSocks[i + 1];
			break;
		}
	}
	clntCnt--;
	ReleaseMutex(hMutex);
	closesocket(hClntSock);
	return 0;

}
void SendMsg(char * msg, int len)
{
	int i;
	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clntCnt; i++)
		send(clntSocks[i], msg, len, 0);
	ReleaseMutex(hMutex);
}

