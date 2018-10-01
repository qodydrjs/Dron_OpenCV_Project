
//#include "File.h"
#include "stdafx.h"
#include <fstream>
#include<algorithm>
#include <string>

#include "include/File.h"
#pragma comment(lib,"include/FileDll1.lib")
#include<iostream>
#include<Windows.h>
#include <tchar.h> 
#include<string>
#include <sqlext.h>
#include <string.h>
#include<fstream>

//server
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

/////////////sql


void geterror(SQLHSTMT hstmt)
{
	SQLSMALLINT     HandleType;
	SQLHANDLE       Handle;
	SQLSMALLINT     RecNumber;
	SQLCHAR         SQLState[500] = "SELECT * FROM imageTable3 \WHERE number = (SELECT MAX(number)as number from imageTable3)";
	SQLINTEGER      NativeErrorPtr;
	SQLCHAR         MessageText[SQL_MAX_MESSAGE_LENGTH] = "";
	SQLSMALLINT     BufferLength = 0;
	SQLSMALLINT     TextLengthPtr;

	SQLSTATE state;

	SQLRETURN retcode;

	int i = 1;
	while ((retcode = SQLGetDiagRecA(SQL_HANDLE_STMT, hstmt, i,
		SQLState, &NativeErrorPtr, MessageText, sizeof(MessageText), &TextLengthPtr)) != SQL_NO_DATA)
	{
		std::cout << SQLState << std::endl;
		i++;
	}
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	int clntAdrSz;
	HANDLE hThread;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //소켓 라이브러리 초기화
		ErrorHandling("WSAStartup() error!");

	hMutex = CreateMutex(NULL, FALSE, NULL);
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

		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void *)&hClntSock, 0, NULL);
		printf("Connected client IP: %s \n", inet_ntoa(clntAddr.sin_addr));
	}
	closesocket(hServSock);
	WSACleanup(); //프로그램 종료 전에 초기화한 소켓 라이브러리 해제
	return 0;
}

void ErrorHandling(const char *message) //에러 출력 메시지
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);

}

unsigned WINAPI HandleClnt(void * arg) { // 
	SOCKET hClntSock = *((SOCKET*)arg);
	int strLen = 0, i;
	char msg[BUF_SIZE];


	while ((strLen = recv(hClntSock, msg, sizeof(msg), 0)) != 0) {

			
		char buf_end[4] = { '$','d','d','!' };

		while (true) {

			SQLHENV henv;
			SQLHDBC hdbc;
			SQLHSTMT hstmt;
			SQLRETURN retcode;

			SQLCHAR OutConnStr[255];
			SQLSMALLINT OutConnStrLen;

			HWND desktopHandle = GetDesktopWindow();   // desktop's window handle

			// Allocate environment handle
			retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

			// Set the ODBC version environment attribute
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

				// Allocate connection handle
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

					// Set login timeout to 5 seconds
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

						retcode = SQLConnect(hdbc, (UCHAR*)"mssql", SQL_NTS, (UCHAR*)"sa", SQL_NTS, (UCHAR*)"123qwe", SQL_NTS);

						// Allocate statement handle
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

							SQLINTEGER cbfname = SQL_NTS, cbfdata = SQL_NTS;

							char name[5000] = "";
							char data[5000] = "";

							retcode = SQLExecDirectA(hstmt, (SQLCHAR*)"SELECT * FROM imageTable3 \WHERE number = (SELECT MAX(number)as number from imageTable3)", SQL_NTS);

							std::ofstream fout;
							fout.open("C:\\temp\\img02.jpg", std::ios::binary);

							while ((retcode = SQLFetch(hstmt)) != SQL_NO_DATA)
							{
								while ((retcode = SQLGetData(hstmt, 1, SQL_C_CHAR, name, sizeof(name), &cbfname) != SQL_NO_DATA))
								{
									SQLINTEGER NumBytes = (cbfname > 5000) || (cbfname == SQL_NO_TOTAL) ? 5000 : cbfname;
								}

								while ((retcode = SQLGetData(hstmt, 2, SQL_C_BINARY, data, sizeof(data), &cbfdata)) != SQL_NO_DATA)
								{
									SQLINTEGER NumBytes = (cbfdata > 5000) || (cbfdata == SQL_NO_TOTAL) ? 5000 : cbfdata;

									fout.write(data, 5000);
								}
							}
							fout.close();

							// Process data
							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
								SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
							}

							SQLDisconnect(hdbc);
						}

						SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
					}
				}
				SQLFreeHandle(SQL_HANDLE_ENV, henv);
			}

			try
			{
				file.hFile = file.loadFile("C:\\temp\\img02.jpg");
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
				ReleaseMutex(hMutex);
				SendMsg(buf_end, sizeof(buf_end)); //이미지 END Flag 전송
				std::cout << "파일전송 완료 : " << std::endl;
				CloseHandle(file.hFile);
				CloseHandle(file.hFileMapping);
				Sleep(500);
			}
			catch (const std::exception&)
			{
				std::cout << "File IO Error" << std::endl;
			}

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

