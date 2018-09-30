#pragma once
#include<iostream>
#include<Windows.h>
#include <tchar.h> 
#include<string>
#include <sqlext.h>
#include <string.h>
#include<fstream>
#include "../common/export_define.h"
using namespace std;

#define TEXTSIZE  12000
#define MAXBUFLEN 256


class EXPORTDLL mssqlx64
{
private:
	SQLHENV henv = SQL_NULL_HENV;
	SQLHDBC hdbc1 = SQL_NULL_HDBC;
	SQLHSTMT hstmt1 = SQL_NULL_HSTMT;
	RETCODE retcode;

public:
	mssqlx64();
	~mssqlx64();
	void geterror(SQLHSTMT hstmt);
	void Cleanup();
	int ImagefileUpload(byte* ImageFile, int size);
};

