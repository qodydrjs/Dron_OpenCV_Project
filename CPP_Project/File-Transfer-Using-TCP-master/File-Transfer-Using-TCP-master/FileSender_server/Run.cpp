// Run.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Run.h"
#include "File.h"

server sv;
File file;

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));
	if (argc != 2) {
		fputs("	Options : ~/FileSender_server.exe [fileName]\n",stderr);
		exit(0);
	}
	
	file.setFileName(argv[1]);
	sv.startServer();
	return 0;
}

