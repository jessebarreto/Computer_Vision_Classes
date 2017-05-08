#include <opencv2/opencv.hpp>

#ifndef PROJECT3_H
#define PROJECT3_H

const std::string WINDOW_NAME_L("Image Left");
const std::string WINDOW_NAME_R("Image Right");
const std::string WINDOW_NAME_D("Image Depth");

const std::string DEFAULT_IMG_FILE_L("aloeL.png");
const std::string DEFAULT_IMG_FILE_R("aloeR.png");

class Project3
{
private:
    uint16_t _cam_baseline; //mm
    uint16_t _cam_focus; //mm

    // Original Images
    cv::Mat* _imageLeft;
    cv::Mat* _imageRight;

    // Grayscale Images
    cv::Mat _grayLeft;
    cv::Mat _grayRight;

    // Depth Image
    cv::Mat _depthImage;

    // Template Size
    int _windowSize;
    int _minWindowSize;

    // Number of Disparities to be Shifted
    int _displacementNumber;

    // Flag to parameters change
    bool _flagChanged;

public:
    // Constructor
    Project3(cv::Mat* imageLeft, cv::Mat* imageRight, int windowSize, int dispNumber);

    // Set windows and shift disparities number
    void setMembers(int windowSize, int dispNumber);

    // Calculate depth map
    void calculateDepthMap();

    // Calculate Absolute Difference
    void calculateAbsDiff(const uint16_t &pixelLeft, const uint16_t &pixelRight, uint16_t &ADresult);

    // Normalize Depth Map
    void normalizeDepthMap(const cv::Mat &disparities);

    // Returns the Depth Image
    cv::Mat& getDepthImage();

    // Returns Size of Window
    int getWindowSize();
};

#endif // PROJECT3_H
