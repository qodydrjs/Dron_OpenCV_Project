
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

	//file Load
	/*file.hFile = file.loadFile("C:\\image\\shape.png");
	file.setFileSize(GetFileSize(file.hFile, NULL));
	file.hFileMapping = file.mappingFile();

	file.hFile = file.loadFile("C:\\image\\girl.jpg");
	file.setFileSize(GetFileSize(file.hFile, NULL));
	file.hFileMapping = file.mappingFile();*/

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
	
	while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != 0) {

		char buf_end[4] = { '$','d','d','!' };
		////////////////////////
		///////////////////////
		while (true) {

			file.hFile = file.loadFile("C:\\image\\girlsgeneration.JPG");
			file.setFileSize(GetFileSize(file.hFile, NULL));
			file.hFileMapping = file.mappingFile();
			std::cout << "시작 : " << std::endl;
			//Token '$','^','@'
			char buf_start[6] = { '#','i','$','d','d','!' };
			printf("%c\n", buf_start[5]);
			printf("%d\n", buf_start[5]);
			//Image Size byte로 변환 
			char c_size[100];
			std::string str_size = std::to_string(file.getFileSize());
			str_size.append("$dd!");
			std::strncpy(c_size, str_size.c_str(), str_size.length());

			SendMsg(buf_start, sizeof(buf_start)); //이미지 시작 Flag 전송
			SendMsg(c_size, str_size.length()); //이미지 사이즈 전송

			std::cout << "파일 사이즈 전송 : " << std::endl;
			std::cout << "파일사이즈 : " << file.getFileSize() << std::endl;


			SOCKADDR_IN dataAddr;
			int dataAddrSz = sizeof(dataAddr);
			int retval;

			DWORD dwFileSizeHigh;
			__int64 qwFileSize = GetFileSize(file.hFile, &dwFileSizeHigh);
			__int64 qwFileOffset = 0;

			WaitForSingleObject(hMutex, INFINITE);
			while (qwFileSize > 0) {
				DWORD dwBytesInBlock = BUFSIZE;
				if (qwFileSize < BUFSIZE) {
					dwBytesInBlock = qwFileSize;
				}

				file.pbFile = (char*)MapViewOfFile
				(
					file.hFileMapping,
					FILE_MAP_READ,
					(DWORD)(qwFileOffset >> 32),  // 상위 오프셋
					(DWORD)(qwFileOffset & 0xFFFFFFFF),
					dwBytesInBlock
				);
				// 파일전송
				//SendMsg(file.pbFile, dwBytesInBlock);
				retval = send(hClntSock, file.pbFile, dwBytesInBlock, 0);
				if (retval != dwBytesInBlock) {
					while (1) {
						retval = send(hClntSock, file.pbFile, dwBytesInBlock, 0);
						if (retval != 0) break;
					}
				}
				//endflag
				//char buf_end[8] = { '$','^','@'};

				// 뷰를 다 썼으므로, 뷰를 해제한다.
				UnmapViewOfFile(file.pbFile);

				// 오프셋 및 남은 파일 크기 갱신
				qwFileOffset += dwBytesInBlock;
				qwFileSize -= dwBytesInBlock;
			}
			ReleaseMutex(hMutex);
			Sleep(100);
			SendMsg(buf_end, sizeof(buf_end)); //이미지 END Flag 전송
			std::cout << "파일전송 완료 : " << std::endl;

			

			file.hFile = file.loadFile("C:\\image\\scene.jpg");
			file.setFileSize(GetFileSize(file.hFile, NULL));
			file.hFileMapping = file.mappingFile();

			//////////////////////////////
			//////////////////////////////

			std::cout << "시작 : " << std::endl;
			//Token '$','^','@'
			char buf_start2[6] = { '#','i','$','d','d','!' };

			//Image Size byte로 변환 
			//char c_size[100];
			str_size = std::to_string(file.getFileSize());
			str_size.append("$dd!");
			std::strncpy(c_size, str_size.c_str(), str_size.length());

			SendMsg(buf_start2, sizeof(buf_start2)); //이미지 시작 Flag 전송
			SendMsg(c_size, str_size.length()); //이미지 사이즈 전송

			std::cout << "파일 사이즈 전송 : " << std::endl;
			std::cout << "파일사이즈 : " << file.getFileSize() << std::endl;


			//SOCKADDR_IN dataAddr;
			dataAddrSz = sizeof(dataAddr);
			retval;

			//DWORD dwFileSizeHigh;
			qwFileSize = GetFileSize(file.hFile, &dwFileSizeHigh);
			qwFileOffset = 0;

			WaitForSingleObject(hMutex, INFINITE);
			while (qwFileSize > 0) {
				DWORD dwBytesInBlock = BUFSIZE;
				if (qwFileSize < BUFSIZE) {
					dwBytesInBlock = qwFileSize;
				}

				file.pbFile = (char*)MapViewOfFile
				(
					file.hFileMapping,
					FILE_MAP_READ,
					(DWORD)(qwFileOffset >> 32),  // 상위 오프셋
					(DWORD)(qwFileOffset & 0xFFFFFFFF),
					dwBytesInBlock
				);
				// 파일전송
				//SendMsg(file.pbFile, dwBytesInBlock);
				retval = send(hClntSock, file.pbFile, dwBytesInBlock, 0);
				if (retval != dwBytesInBlock) {
					while (1) {
						retval = send(hClntSock, file.pbFile, dwBytesInBlock, 0);
						if (retval != 0) break;
					}
				}

				//endflag
				//char buf_start[8] = { '$','^','@','#','e','$','^','@' };
				
				// 뷰를 다 썼으므로, 뷰를 해제한다.
				UnmapViewOfFile(file.pbFile);

				// 오프셋 및 남은 파일 크기 갱신
				qwFileOffset += dwBytesInBlock;
				qwFileSize -= dwBytesInBlock;
			}

			ReleaseMutex(hMutex);
			SendMsg(buf_end, sizeof(buf_end)); //이미지 END Flag 전송
			Sleep(100);
			
			std::cout << "파일전송 완료 : " << std::endl;
		}
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

