#include <iostream>
#include "highgui.h"
#include "cv.h"

using namespace std;

int main()
{
    IplImage* img = cvLoadImage( "../../Data/Week1/harold.png", CV_LOAD_IMAGE_COLOR);
    cvNamedWindow( "Hide Your Pain For The 1st Commit", CV_WINDOW_AUTOSIZE );
    cvShowImage( "Hide Your Pain For The 1st Commit", img );
    cvWaitKey(0);
    cvReleaseImage( &img );
    cvDestroyWindow( "Hide Your Pain For The 1st Commit" );

    cout << "First Commit Test for our CMake Module!" << endl;
    return 0;
}
