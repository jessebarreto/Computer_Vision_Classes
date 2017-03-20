#include "cv.h"
#include "highgui.h"

int main()
{
        cvNamedWindow( "Video Example", CV_WINDOW_AUTOSIZE );
        CvCapture* capture = cvCreateFileCapture( "../../Data/Week1/traffic_chaos.mp4" );
        IplImage* frame;
        while(1) {
                frame = cvQueryFrame( capture );
                if( !frame ) break;
                cvShowImage( "Video Example", frame );
                char c = cvWaitKey(33);
                if( c == 27 ) break;
        }
        cvReleaseCapture( &capture );
        cvDestroyWindow( "Video Example" );
        return 0;
}
