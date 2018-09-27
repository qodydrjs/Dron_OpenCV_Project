#pragma once

class sockets {
public:
	SOCKET sock;
	SOCKADDR_IN sockAddr;
	int addrSize;
};

class clientSocket : public sockets {
public:
	void initSocket();
	void setSockOptions();
};