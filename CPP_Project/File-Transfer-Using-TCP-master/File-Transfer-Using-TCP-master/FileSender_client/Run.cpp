// Run.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Run.h"
#include "File.h"

client c;
File file;

int main(int argc, char* argv[])
{
	if (argc != 3) {
		fputs("	Options : ~/FileSender_client.exe [serverIP] [serverPORT] \n", stderr);
		exit(0);
	}
	else {
		c.setServerIP(argv[1]);
		c.setServerPORT(atoi(argv[2]));
	}
	
	c.startProgram();
    return 0;
}

