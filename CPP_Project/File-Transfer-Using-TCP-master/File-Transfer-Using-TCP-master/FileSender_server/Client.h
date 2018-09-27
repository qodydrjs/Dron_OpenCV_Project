#pragma once
#include "socketClass.h"

class clients {
public:
	char cIP[16];
	u_short cStartPort;
	u_short cEndPort;

	subSocket nameSock;
	subSocket sizeSock;
	subSocket dataSock;

	DWORD hDataThreadID;
	HANDLE hDataThread;

	clients();
};