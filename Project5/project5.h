#ifndef PROJECT5_H
#define PROJECT5_H

#include <opencv2/opencv.hpp>

class Project5
{
    // Default image file name
    const std::string _defaultImageFileName;

    // Default windows names
    const std::string _sourceWindowName;
    const std::string _sourceGrayWindowName;
    const std::string _GLCM0WindowName;
    const std::string _GLCM45WindowName;

    // GLCM Offsets
    const cv::Point2i _directionsOffset[8];

    // Source Image
    cv::Mat _sourceImage;

    // Gray Image
    cv::Mat _grayImage;

    // Normalized Gray Level Co-ocurrence Matrix
    cv::Mat _0degreesGLCM, _45degreesGLCM;

    // Calculate GLCM given the offset
    void _calculateGLCM(int distance, int angle, cv::Mat& GLCMResult);
public:
    // Constructor
    Project5();

    // Return default image file name
    const std::string &getDefaultImageFileName();

    // Return default windows names
    const std::string &getWindowsNames(unsigned windowName);

    // Return offset directions
    const cv::Point2i &getOffsetDirections(int angle);

    // Return source image
    cv::Mat &getSourceImage();

    // Return source gray image
    cv::Mat &getGraySourceImage();

    // Return GLCM 0 degrees
    cv::Mat &getGLCM0();

    // Return GLCM 45 degrees
    cv::Mat &getGLCM45();

    // Set the source image
    void setSourceImage(cv::Mat& sourceImage);

    // Calculate both Gray-level Co-Ocurrence Matrices (0 and +45 degrees)
    void calculateGLCMs();

    // Calculate and Save both GLCMs (0 and +45 degrees)
    void calculateAndSaveGLCMs();

    // Save a matrix in a XML file
    void saveMatrixXML(const cv::Mat &matrix, const std::string &matrixName, const std::string &fileName,
                       bool appendFlag);
};

#endif // PROJECT5_H
