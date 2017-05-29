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

    // Not Normalized Gray Level Co-ocurrence Matrix
    cv::Mat _raw0degreesGLCM, _raw45degreesGLCM;

    // Calculate raw GLCM given the offset
    void _calculateGLCM(int distance, int angle, cv::Mat& rawGLCM, cv::Mat &normGLCM);
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

    cv::Mat &getRawGLCM0();

    cv::Mat &getRawGLCM45();

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

Project5::Project5() :
    _defaultImageFileName("wine.jpg"),
    _sourceWindowName("Source Image"),
    _sourceGrayWindowName("Gray Image"),
    _GLCM0WindowName("GLCM 0 degrees Image"),
    _GLCM45WindowName("GLCM 45 degrees Image"),
    _directionsOffset{cv::Point2i(0, 1), // 0 degrees
                      cv::Point2i(-1, 1)} // 45 degrees
{

}

const std::string &Project5::getDefaultImageFileName()
{
    return _defaultImageFileName;
}

const std::string &Project5::getWindowsNames(unsigned windowName)
{
    switch (windowName) {
    case 3:
        return _GLCM45WindowName;
    case 2:
        return _GLCM0WindowName;
    case 1:
        return _sourceGrayWindowName;
    case 0:
    default:
        return _sourceWindowName;
    }
}

const cv::Point2i &Project5::getOffsetDirections(int angle)
{
    switch (angle) {
    case 45:
        return _directionsOffset[1];
    case 0:
    default:
        return _directionsOffset[0];
    }
}

cv::Mat &Project5::getSourceImage()
{
    return _sourceImage;
}

cv::Mat &Project5::getGraySourceImage()
{
    return _grayImage;
}

cv::Mat &Project5::getGLCM0()
{
    return _0degreesGLCM;
}

cv::Mat &Project5::getGLCM45()
{
    return _45degreesGLCM;
}

cv::Mat &Project5::getRawGLCM0()
{
    return _raw0degreesGLCM;
}

cv::Mat &Project5::getRawGLCM45()
{
    return _raw45degreesGLCM;
}

void Project5::setSourceImage(cv::Mat &sourceImage)
{
    _sourceImage = sourceImage;
    cv::cvtColor(_sourceImage, _grayImage, CV_BGR2GRAY, CV_8UC1);
}

void Project5::_calculateGLCM(int distance, int angle, cv::Mat &rawGLCM, cv::Mat &normGLCM)
{
    cv::Point2i offset = getOffsetDirections(angle);

    // Create an empty GLCM Matrix
    cv::Mat GLCM = cv::Mat::zeros(256, 256, CV_32FC1);


    // Offsets in rows and cols
    int xoffset = offset.y * distance;
    int yoffset = offset.x * distance;

    // Pixels
    int pixelRow, pixelCol;

    // Number of Image elements
    unsigned pixels = 0;

    // Fill the GLCM Matrix
    for (int row = 0; row < _grayImage.rows; row++) {
        for (int col = 0; col < _grayImage.cols; col++) {
            // If a pixel is outside of the image, it is ignored
            if (row + xoffset >= _grayImage.rows || row + xoffset < 0 ||
                    col + yoffset >= _grayImage.cols || col + yoffset < 0) {
                continue;
            }

            // Color Origin and Destiny
            pixelRow = static_cast<int>(_grayImage.at<uint8_t>(row, col));
            pixelCol = static_cast<int>(_grayImage.at<uint8_t>(row + xoffset, col + yoffset));

            // Count this color in the GLCM matrix
            GLCM.at<float>(pixelRow, pixelCol)++;

            // Count number of pixels
            pixels++;
        }
    }

    rawGLCM = GLCM;

    // Normalize
    normGLCM = GLCM / static_cast<float>(pixels);
}


void Project5::calculateGLCMs()
{
    _calculateGLCM(1, 0, _raw0degreesGLCM, _0degreesGLCM);
    _calculateGLCM(1, 45, _raw45degreesGLCM, _45degreesGLCM);
}

void Project5::calculateAndSaveGLCMs()
{
    calculateGLCMs();

    // Save GLCMs
    saveMatrixXML(_0degreesGLCM, "GLCM_0", "GLCM_0degrees", false);
    saveMatrixXML(_45degreesGLCM, "GLCM_45", "GLCM_45degrees", false);
}

void Project5::saveMatrixXML(const cv::Mat &matrix, const std::string &matrixName, const std::string &fileName,
                             bool appendFlag)
{
    std::string fileNameExtension = fileName;
    fileNameExtension.append(".xml");
    cv::FileStorage *file;
    if (!appendFlag)
        file = new cv::FileStorage(fileNameExtension, cv::FileStorage::WRITE);
    else
        file = new cv::FileStorage(fileNameExtension, cv::FileStorage::APPEND);

    if (!file->isOpened())
        return;

    *file << matrixName << matrix;
    file->release();

    std::cout << "Matrix: " << matrixName << " saved!" << std::endl;
    delete file;
}

#endif // PROJECT5_H
