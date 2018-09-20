
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
	file.hFile = file.loadFile("C:\\image\\shape.png");
	file.setFileSize(GetFileSize(file.hFile, NULL));
	file.hFileMapping = file.mappingFile();

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
		//file Load
		

			std::cout << "시작 : " << std::endl;
			//Token '$','^','@'
			char buf_start[5] = { '#','i','$','^','@' };

			//Image Size byte로 변환 
			char c_size[100];
			std::string str_size = std::to_string(file.getFileSize());
			str_size.append("$^@");
			std::strncpy(c_size, str_size.c_str(), str_size.length());
			//string 을 char[] 로 변환 하는 함수 
			//strs.c_str();

			//시작신호 + 이미지 사이즈 합침
			//char buf_Start_ImageSize[sizeof(buf_start) + )];
			////std::copy header =  #include<algorithm> 
			//std::copy(buf_start, buf_start+ sizeof(buf_start), buf_Start_ImageSize);
			//std::copy(buf_image_size, buf_image_size + sizeof(buf_image_size), buf_Start_ImageSize + sizeof(buf_start) );
			SendMsg(buf_start, sizeof(buf_start)); //이미지 시작 Flag 전송
			SendMsg(c_size, str_size.length()); //이미지 사이즈 전송
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

			std::cout << "파일 사이즈 전송 : " << std::endl;
			std::cout << "파일사이즈 : " << file.getFileSize() << std::endl;


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

				// 뷰를 다 썼으므로, 뷰를 해제한다.
				UnmapViewOfFile(file.pbFile);

				// 오프셋 및 남은 파일 크기 갱신
				qwFileOffset += dwBytesInBlock;
				qwFileSize -= dwBytesInBlock;
			}
			//Sleep(100);
			std::cout << "파일전송 완료 : " << std::endl;

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

