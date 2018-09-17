#include "main.h"
#include <windows.h>

void*  recv_msg(SOCKET* arg);
void*  send_msg(SOCKET* arg);

int main(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	SOCKADDR_IN target;
	SOCKET s;
	int err;
	int bytesSent;
	char buf[50];

	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);

	if (err != 0)
	{
		printf("WSASTartup error %ld", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	target.sin_family = AF_INET;
	target.sin_port = htons(SERVER_PORT);
	target.sin_addr.s_addr = inet_addr(IPAddress);

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		perror("SOCKET");
		WSACleanup();
		return 0;
	}

	if (connect(s, reinterpret_cast<SOCKADDR*>(&target), sizeof(target))
		== SOCKET_ERROR)
	{
		perror("Connect()");
		WSACleanup();
		return 0;
	}

	printf("send......\n");
	bytesSent = send(s, "1111", strlen("1111"), 0);


	std::thread t1(recv_msg, &s);
	std::thread t2(send_msg, &s);
	t1.join();

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
			cout << "Sending \"" << sRand << " to client" << endl;
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
