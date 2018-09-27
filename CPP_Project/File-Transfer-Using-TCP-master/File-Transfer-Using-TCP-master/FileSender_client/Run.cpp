// Run.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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

