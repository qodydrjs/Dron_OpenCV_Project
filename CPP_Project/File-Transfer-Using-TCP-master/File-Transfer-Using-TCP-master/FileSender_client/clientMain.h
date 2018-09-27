#pragma once
#include "socketClass.h"
#include "receiveObject.h"

class client {
private:
	receiver user;
	char serverIP[16];
	int serverPORT;

public:
	void init();
	void startProgram();
	void startFileRecv();
	void chkFileTransfer(SOCKET);
	void endProgram();

	void errorPrint(char*);
	
	char* getServerIP();
	void setServerIP(char*);
	int getServerPORT();
	void setServerPORT(int);
};