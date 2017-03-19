#include "highgui.h"
#include "cv.h"
#include "stdio.h"

int main( int argc, char **argv )
{
    CvCapture *capture = 0;
    IplImage *frame = 0;
    int key = 0;

    cvNamedWindow("WebCam Test", 1);
    capture = cvCaptureFromCAM(-1);

    while(key != 'q') {
        frame = cvQueryFrame(capture);
        cvShowImage("WebCam Test", frame);
        key = cvWaitKey(1);
    }
    cvReleaseCapture(&capture);
    cvDestroyWindow("WebCam Test");

    return 0;
}
