#include "project5.h"

Project5::Project5() :
    _defaultImageFileName("fruits.jpg"),
    _sourceWindowName("Source Image"),
    _sourceGrayWindowName("Gray Image"),
    _GLCM0WindowName("GLCM 0 degrees Image"),
    _GLCM45WindowName("GLCM 45 degrees Image"),
    _directionsOffset{cv::Point2i(1, 0), // 0 degrees
                      cv::Point2i(1, 1), // 45 degrees
                      cv::Point2i(0, 1), // 90 degrees
                      cv::Point2i(-1, 1), // 135 degrees
                      cv::Point2i(-1, 0), // 180 degrees
                      cv::Point2i(-1, -1), // 225 degrees
                      cv::Point2i(0, -1), // 270 degrees
                      cv::Point2i(1, -1)} // 315 degrees
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
    case -45:
    case 315:
        return _directionsOffset[7];
    case -90:
    case 270:
        return _directionsOffset[6];
    case -135:
    case 225:
        return _directionsOffset[5];
    case -180:
    case 180:
        return _directionsOffset[4];
    case -225:
    case 135:
        return _directionsOffset[3];
    case -270:
    case 90:
        return _directionsOffset[2];
    case -315:
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

void Project5::setSourceImage(cv::Mat &sourceImage)
{
    _sourceImage = sourceImage;
    cv::cvtColor(_sourceImage, _grayImage, CV_BGR2GRAY, CV_8UC1);
}

void Project5::_calculateGLCM(int distance, int angle, cv::Mat &GLCMResult)
{
    cv::Point2i offset = getOffsetDirections(angle);

    // Create an empty GLCM Matrix
    GLCMResult = cv::Mat::zeros(256, 256, CV_32SC1);


    // Offsets in rows and cols
    int xoffset = offset.x * distance;
    int yoffset = offset.y * distance;

    // Pixels
    int pixelRow, pixelCol;

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
            GLCMResult.at<int32_t>(pixelRow, pixelCol)++;
        }
    }
}

void Project5::calculateGLCMs()
{
    _calculateGLCM(1, 0, _0degreesGLCM);
    _calculateGLCM(1, 45, _45degreesGLCM);
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

