#pragma once
#include "socketClass.h"

class receiver {
public:
	char cIP[16];
	u_short cStartPort;
	u_short cEndPort;

	clientSocket nameSock;
	clientSocket sizeSock;
	clientSocket dataSock;

	DWORD hThreadID;
	HANDLE hThread;
};