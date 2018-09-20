
#include "File.h"
#include "stdafx.h"
#include <fstream>
#include<algorithm>
#include <string>

#define BUF_SIZE 100
#define MAX_CLNT 256

#define SERVER_PORT 5000

File file;

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

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //���� ���̺귯�� �ʱ�ȭ
		ErrorHandling("WSAStartup() error!");


	hMutex = CreateMutex(NULL, FALSE, NULL);
	//hServSock = socket(AF_INET, SOCK_STREAM, 0); //���ϻ��� //(AF_INET, SOCK_STREAM, 0)
	hServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //���ϻ��� //(AF_INET, SOCK_STREAM, 0)

	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");


	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERVER_PORT);
	//servAddr.sin_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) //���Ͽ� IP�ּҿ� PORT ��ȣ �Ҵ�
		ErrorHandling("bind() error");


	if (listen(hServSock, 5) == SOCKET_ERROR) //listen �Լ�ȣ���� ���ؼ� ������ ������ ���� �������� �ϼ�
		ErrorHandling("listen() error");

	//file Load
	file.hFile = file.loadFile("C:\\image\\shape.png");
	file.setFileSize(GetFileSize(file.hFile, NULL));
	file.hFileMapping = file.mappingFile();

	while (1)
	{
		clntAdrSz = sizeof(clntAddr);
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAdrSz); //Ŭ���̾�Ʈ �����û �����ϱ� ���� accept�Լ� ȣ��

		if (hClntSock == INVALID_SOCKET)
			ErrorHandling("accept() error");
		std::cout << "accept()" << std::endl;


		WaitForSingleObject(hMutex, INFINITE);
		clntSocks[clntCnt++] = hClntSock;
		ReleaseMutex(hMutex);

		//hThread = (std::thread)_beginthreadex(NULL, 0, HandleClnt, (void *)&hClntSock, 0, NULL);
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void *)&hClntSock, 0, NULL);
		printf("Connected client IP: %s \n", inet_ntoa(clntAddr.sin_addr));



		//send(hClntSock, message, sizeof(message), 0); //send�Լ� ȣ���� ���ؼ� ����� Ŭ���̾�Ʈ�� �����͸� ����

		//std::thread t1(send_msg1, &hClntSock);
		//std::thread t2(send_msg, &hClntSock);

		//t1.join();

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
unsigned WINAPI HandleClnt(void * arg) {
	SOCKET hClntSock = *((SOCKET*)arg);
	int strLen = 0, i;
	char msg[BUF_SIZE];
	
	while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != 0) {
		//file Load
		

			std::cout << "���� : " << std::endl;
			//Token '$','^','@'
			char buf_start[5] = { '#','i','$','^','@' };

			//Image Size byte�� ��ȯ 
			char c_size[100];
			std::string str_size = std::to_string(file.getFileSize());
			str_size.append("$^@");
			std::strncpy(c_size, str_size.c_str(), str_size.length());
			//string �� char[] �� ��ȯ �ϴ� �Լ� 
			//strs.c_str();

			//���۽�ȣ + �̹��� ������ ��ħ
			//char buf_Start_ImageSize[sizeof(buf_start) + )];
			////std::copy header =  #include<algorithm> 
			//std::copy(buf_start, buf_start+ sizeof(buf_start), buf_Start_ImageSize);
			//std::copy(buf_image_size, buf_image_size + sizeof(buf_image_size), buf_Start_ImageSize + sizeof(buf_start) );
			SendMsg(buf_start, sizeof(buf_start)); //�̹��� ���� Flag ����
			SendMsg(c_size, str_size.length()); //�̹��� ������ ����
			//char buf_image_size2[4] = buf_image_size;
		/*	*/
		/*	for (int i = 0; i < 3; i++) {
				if (i == 0)msg[i] = '4';
				if (i == 1)msg[i] = '1';
				if (i == 2)msg[i] = '\0';
			}*/
			//SendMsg(buf_image_size, sizeof(buf_image_size));
			//std::cout << "int size : " << sizeof(int)<<std::endl;

			//Sleep(100);
		/*	std::ifstream files("C:\\image\\bike.bmp", std::ifstream::binary);
			files.seekg(0, std::ifstream::beg);
			int n = 0;
			while (files.tellg() != -1)
			{

				char *p = new char[1024];
				memset(p, 0, 1024);
				files.read(p, 1024);

				n = send(hClntSock, p, 1024, 0);
				if (n < 0) {

				}
				else {

				}
				delete p;*/

				//}

			std::cout << "���� ������ ���� : " << std::endl;
			std::cout << "���ϻ����� : " << file.getFileSize() << std::endl;


			SOCKADDR_IN dataAddr;
			int dataAddrSz = sizeof(dataAddr);
			int retval;

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
				//SendMsg(file.pbFile, dwBytesInBlock);
				retval = send(hClntSock, file.pbFile, dwBytesInBlock, 0);
				if (retval != dwBytesInBlock) {
					while (1) {
						retval = send(hClntSock, file.pbFile, dwBytesInBlock, 0);
						if (retval != 0) break;
					}
				}

				// �並 �� �����Ƿ�, �並 �����Ѵ�.
				UnmapViewOfFile(file.pbFile);

				// ������ �� ���� ���� ũ�� ����
				qwFileOffset += dwBytesInBlock;
				qwFileSize -= dwBytesInBlock;
			}
			//Sleep(100);
			std::cout << "�������� �Ϸ� : " << std::endl;

			/*		for (int i = 0; i < 3; i++) {
						if (i == 0)msg[i] = '$';
						if (i == 1)msg[i] = 'i';
						if (i == 2)msg[i] = '\0';
					}
					SendMsg(msg, 3);*/
					//file Load
	}
	

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
	for (i = 0; i < clntCnt; i++) {
		send(clntSocks[i], msg, len, 0);
	}
	ReleaseMutex(hMutex);
}

