#pragma once

class sockets {
public:
	SOCKET sock;
	SOCKADDR_IN sockAddr;
};

class mainSocket : public sockets {
public:
	void initMainSocket();
	void socketBind();
	void setSockOptions();
};

class subSocket : public sockets {
public:
	int addrSize;
	void acceptSock();
};