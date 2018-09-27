#pragma once
#include "socketClass.h"

class server {
public:
	mainSocket mainSock;
	char fileName[256] = { NULL };
	
	bool init();
	void startServer();
	void startFileTransfer();
	void chkFileTransfer(SOCKET);
	void endServer();
	void errorPrint(char*);

};