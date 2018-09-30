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

#define VIDEO_WIDTH     500
#define VIDEO_HEIGHT    300

unsigned WINAPI HandleClnt(void * arg);

/** Global variables */
String face_cascade_name;
CascadeClassifier face_cascade;
String window_name = "Face detection";

Mat frame;

HANDLE hThread;

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

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++)
	{
		cv::Point lb(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
		cv::Point tr(faces[i].x, faces[i].y);
		cv::rectangle(frame, lb, tr, cv::Scalar(0, 255, 0), 3, 4, 0);

	/*	sprintf(buf, "c:/temp/img01.jpg", 0);
		std::cout << buf << std::endl;
		imwrite(buf, frame);*/

		int size = frame.rows*frame.cols;
		byte *bytes;
		bytes = (byte*)malloc(size);
		std::memcpy(bytes, frame.data, size * sizeof(byte));
		delete bytes;
		

	}
	//hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void *)&frame, 0, NULL);
	ReleaseMutex(ctx->mutex);
}

int main()
{
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