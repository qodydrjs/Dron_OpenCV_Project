#include "stdafx.h"
#include <windows.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vlc/vlc.h>
#include <process.h>
#include <thread>

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "mssqlx64.h"
#pragma comment(lib,"mssqlx64.lib")


using namespace cv;
using namespace std;

struct ctx
{
	Mat* image;
	HANDLE mutex;
	uchar*    pixels;
};
// define output video resolution
//#define VIDEO_WIDTH     1920
//#define VIDEO_HEIGHT    1080

#define VIDEO_WIDTH     400
#define VIDEO_HEIGHT    300

unsigned WINAPI HandleClnt(void * arg);

///////////////sql
//
#define TEXTSIZE  12000
#define MAXBUFLEN 256

SQLHENV henv = SQL_NULL_HENV;
SQLHDBC hdbc1 = SQL_NULL_HDBC;
SQLHSTMT hstmt1 = SQL_NULL_HSTMT;


void geterror(SQLHSTMT hstmt)
{
	SQLSMALLINT     HandleType;
	SQLHANDLE       Handle;
	SQLSMALLINT     RecNumber;
	SQLCHAR         SQLState[500] = "INSERT INTO imageTable3 (name, data) VALUES ('file2', ?)";
	SQLINTEGER      NativeErrorPtr;
	SQLCHAR         MessageText[SQL_MAX_MESSAGE_LENGTH] = "";
	SQLSMALLINT     BufferLength = 0;
	SQLSMALLINT     TextLengthPtr;

	SQLSTATE state;
	SQLRETURN retcode;

	int i = 1;
	while ((retcode = SQLGetDiagRecA(SQL_HANDLE_STMT, hstmt, i, SQLState, &NativeErrorPtr, MessageText, sizeof(MessageText), &TextLengthPtr)) != SQL_NO_DATA)
	{
		cout << SQLState << endl;
		i++;
	}
}

void Cleanup()
{
	if (hstmt1 != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	if (hdbc1 != SQL_NULL_HDBC) {
		SQLDisconnect(hdbc1);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc1);
	}

	if (henv != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
}
////////////////////




/** Global variables */
String face_cascade_name;
CascadeClassifier face_cascade;
String window_name = "Face detection";

Mat frame;

HANDLE hThread;

mssqlx64 mssql;

char buf[256];

void *lock(void *data, void**p_pixels)
{
	struct ctx *ctx = (struct ctx*)data;

	WaitForSingleObject(ctx->mutex, INFINITE);

	// pixel will be stored on image pixel space
	*p_pixels = ctx->pixels;
	return NULL;
}

void display(void *data, void *id) {
	(void)data;
	assert(id == NULL);
}

void unlock(void *data, void *id, void *const *p_pixels) {

	// get back data structure 
	struct ctx *ctx = (struct ctx*)data;

	/* VLC just rendered the video, but we can also render stuff */
	// show rendered image
	imshow("test", *ctx->image);

	frame = ctx->image->clone();

	std::vector<Rect> faces;
	Mat frame_gray;

	cv::cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	boolean checkfalg = false;

	//for (size_t i = 0; i < faces.size(); i++)
	//{
	//	cv::Point lb(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
	//	cv::Point tr(faces[i].x, faces[i].y);
	//	cv::rectangle(frame, lb, tr, cv::Scalar(0, 255, 0), 3, 4, 0);

	//	checkfalg = true;
	//}

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2),
			0, 0, 360, Scalar(0, 0, 255), 4, 8, 0);

		checkfalg = true;
	}



	if (checkfalg) {

		sprintf(buf, "c:/temp/img01.jpg", 0);
		std::cout << buf << std::endl;
		imwrite(buf, frame);


		///////////////////

		RETCODE retcode;
		// SQLBindParameter variables.
		SQLLEN cbTextSize, lbytes;
		// SQLParamData variable.
		PTR pParmID;
		// SQLPutData variables.
		string s = "C:\\temp\\img01.jpg"; // any large image file

		ifstream fin;
		fin.open(s.c_str(), ios::binary | ios::ate);

		size_t sz = fin.tellg();
		fin.seekg(0, ios::beg);

		char *ptr = new char[sz];

		fin.read(ptr, sz);

		fin.close();

		SDWORD cbBatch = sz;

		// Allocate the ODBC environment and save handle.
		retcode = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);
		if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS))
		{
			printf("SQLAllocHandle(Env) Failed\n\n");
			Cleanup();
			//return(9);
		}

		// Notify ODBC that this is an ODBC 3.0 app.
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3,
			SQL_IS_INTEGER);
		if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS))
		{
			printf("SQLSetEnvAttr(ODBC version) Failed\n\n");
			Cleanup();
			//return(9);
		}

		// Allocate ODBC connection handle and connect.
		retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc1);
		if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS))
		{
			printf("SQLAllocHandle(hdbc1) Failed\n\n");
			Cleanup();
			//return(9);
		}

		retcode = SQLConnectW(hdbc1, (SQLWCHAR*)L"mssql", SQL_NTSL, (SQLWCHAR*)L"sa", SQL_NTSL, (SQLWCHAR*)L"123qwe", SQL_NTSL);


		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
		{
			printf("SQLConnect() Failed\n\n");
			Cleanup();
			//return(9);
		}

		// Allocate statement handle.
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
		{
			printf("SQLAllocHandle(hstmt1) Failed\n\n");
			Cleanup();
			//return(9);
		}

		// Set parameters based on total data to send.

		lbytes = (SDWORD)sz;
		cbTextSize = SQL_LEN_DATA_AT_EXEC(lbytes);

		// Bind the parameter marker.

		SQLSMALLINT   NumParams2, DataType2, DecimalDigits2, Nullable2;

		//x86
		//SQLUINTEGER   ParamSize2;
		//x64
		SQLULEN   ParamSize2;
		retcode = SQLPrepareA(hstmt1, (SQLCHAR*)"INSERT INTO imageTable3 (name, data) VALUES ('file2', ?)", SQL_NTS);

		retcode = SQLDescribeParam(hstmt1, 1, &DataType2, &ParamSize2, &DecimalDigits2, &Nullable2);

		retcode = SQLBindParameter(hstmt1, 1, SQL_PARAM_INPUT, SQL_C_BINARY,
			DataType2, ParamSize2, 0, (VOID *)1, 0, &cbTextSize);

		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
			printf("SQLBindParameter Failed\n\n");
			Cleanup();
			//return(9);
		}

		// Execute the command.
		retcode = SQLExecDirectA(hstmt1, (UCHAR*)"INSERT INTO imageTable3 (name, data) VALUES ('file2', ?)", SQL_NTS);

		if ((retcode != SQL_SUCCESS) && (retcode != SQL_NEED_DATA) && (retcode != SQL_SUCCESS_WITH_INFO))
		{
			printf("SQLExecDirect Failed\n\n");
			Cleanup();
			//return(9);
		}

		// Check to see if NEED_DATA; if yes, use SQLPutData.

		retcode = SQLParamData(hstmt1, &pParmID);

		if (retcode == SQL_NEED_DATA)
		{
			while (lbytes > cbBatch)
			{
				retcode = SQLPutData(hstmt1, (SQLCHAR*)ptr, cbBatch);

				geterror(hstmt1);

				lbytes -= cbBatch;
				ptr += cbBatch;
			}
			// Put final batch.
			retcode = SQLPutData(hstmt1, ptr, lbytes);
		}

		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
		{
			printf("SQLParamData Failed\n\n");
			Cleanup();
			//return(9);
		}

		// Make final SQLParamData call.
		retcode = SQLParamData(hstmt1, &pParmID);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
		{
			printf("Final SQLParamData Failed\n\n");
			Cleanup();
			//return(9);
		}
		checkfalg = false;

		// Clean up.
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
		SQLDisconnect(hdbc1);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc1);
		SQLFreeHandle(SQL_HANDLE_ENV, henv);


		///////////////////


		delete[] ptr;

	}
	//hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void *)&frame, 0, NULL);
	ReleaseMutex(ctx->mutex);
}

int main()
{
	//mssql
	//mssql.Cleanup();

	//face_cascade_name = "D:\\OpenCV-VLC-master\\OpenCV_VLC\\OpenCV_VLC\\haarcascade_frontalface_alt.xml";
		face_cascade_name = "C:\\project\\Dron_OpenCV_Project\\OpenCV-VLC-master_final\\OpenCV_VLC\\OpenCV_VLC\\haarcascade_frontalface_alt.xml";

	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };

	// VLC pointers
	libvlc_instance_t *vlcInstance;
	libvlc_media_player_t *mp;
	libvlc_media_t *media;

	const char * const vlc_args[] = {
		"-I", "dummy", // Don't use any interface
		"--ignore-config", // Don't use VLC's config
		"--extraintf=logger", // Log anything
		"--verbose=2", // Be much more verbose then normal for debugging purpose
	};

	vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);

	// Read a distant video stream
	media = libvlc_media_new_location(vlcInstance, "rtsp://192.168.99.1/media/stream2");
	// Read a local video file
	//media = libvlc_media_new_path(vlcInstance, "d:\\a.mp4");

	mp = libvlc_media_player_new_from_media(media);

	libvlc_media_release(media);

	struct ctx* context = (struct ctx*)malloc(sizeof(*context));
	context->mutex = CreateMutex(NULL, FALSE, NULL);

	context->image = new Mat(VIDEO_HEIGHT, VIDEO_WIDTH, CV_8UC3);
	context->pixels = (unsigned char *)context->image->data;
	// show blank image
	imshow("test", *context->image);

	libvlc_video_set_callbacks(mp, lock, unlock, display, context);
	libvlc_video_set_format(mp, "RV24", VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WIDTH * 24 / 8); // pitch = width * BitsPerPixel / 8


	int ii = 0;
	int key = 0;
	while (key != 27)
	{
		ii++;
		if (ii > 5)
		{
			libvlc_media_player_play(mp);
		}
		float fps = libvlc_media_player_get_fps(mp);
		printf("fps:%f\r\n", fps);
		key = waitKey(100); // wait 100ms for Esc key
	}

	libvlc_media_player_stop(mp);


	return 0;
}

unsigned WINAPI HandleClnt(void * arg) { // 

	Mat frm = *((Mat*)arg);
	std::vector<Rect> faces;
	Mat frame_gray;

	cv::cvtColor(frm, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Point lb(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
		cv::Point tr(faces[i].x, faces[i].y);
		cv::rectangle(frame_gray, lb, tr, cv::Scalar(0, 255, 0), 3, 4, 0);

		sprintf(buf, "c:/temp/img01.jpg", 0);
		std::cout << buf << std::endl;
		imwrite(buf, frame_gray);
	}

	

	return 0;
}