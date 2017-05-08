#include "project3.h"

Project3::Project3(cv::Mat *imageLeft, cv::Mat *imageRight, int windowSize, int dispNumber) :
    _imageLeft(imageLeft),
    _imageRight(imageRight)
{
    _cam_baseline = 120;
    _cam_focus = 25;

    cv::cvtColor(*_imageLeft, _grayLeft, CV_BGR2GRAY);
    cv::cvtColor(*_imageRight, _grayRight, CV_BGR2GRAY);

    _minWindowSize = 5;
    _flagChanged = false;

    setMembers(windowSize, dispNumber);
}

void Project3::setMembers(int windowSize, int dispNumber)
{
    if (windowSize != _windowSize || dispNumber != _displacementNumber) {
        _flagChanged = true;
        _windowSize = windowSize;
        _displacementNumber = dispNumber;

        // O template tem que ser impar
        _windowSize += _windowSize % 2 ? 0 : 1;
        _windowSize = _windowSize < _minWindowSize ? _minWindowSize : _windowSize;

        // Disp Number deve ser multiplo de 16
        if (_displacementNumber < 16) {
            _displacementNumber = 16;
        } else if (_displacementNumber % 16 != 0) {
            _displacementNumber -= dispNumber % 16;
        }
    } else {
        _flagChanged = false;
    }
}

void Project3::calculateDepthMap()
{
    if (_flagChanged) {
        std::cout << "Initializing Depth Calculation (WindowSize: " << _windowSize << ")" << std::endl;

        // Border Size
        int borderSize = _windowSize/2;

        // Depth Matrix
        cv::Mat depth = cv::Mat::zeros(_grayRight.rows, _grayRight.cols, CV_16UC1);

        // Create SAD Values
        cv::Mat minSAD(_grayLeft.rows, _grayLeft.cols, CV_16UC1);
        minSAD.setTo(std::numeric_limits<int16_t>::max());

        // Shifted Right
        cv::Mat grayRShift;

        cv::Mat winL, winR, winAbs;

        // Perform SAD and Save Minimum
        for (int d = 0; d < _displacementNumber; d++) {
            std::cout << "Displacement: " << d << " of " << _displacementNumber << std::endl;
            // Shift Right
            grayRShift = cv::Mat::zeros(_grayRight.rows, _grayRight.cols, CV_8UC1);
            _grayRight(cv::Rect(d, 0, _grayRight.cols - d, _grayRight.rows)).copyTo(grayRShift(cv::Rect(0, 0 ,_grayRight.cols - d, _grayRight.rows)));
            for (int row = borderSize; row < _grayLeft.rows - borderSize; row++) {
                for (int col = borderSize; col < _grayLeft.cols - borderSize; col++) {
                    winL = _grayLeft(cv::Rect(col - borderSize, row - borderSize, _windowSize, _windowSize));
                    winR = grayRShift(cv::Rect(col - borderSize, row - borderSize, _windowSize, _windowSize));
                    cv::absdiff(winL, winR, winAbs);
                    uint16_t curSAD = uint16_t(cv::sum(winAbs)[0]);
                    if (curSAD < minSAD.at<int16_t>(row,col)) {
                        minSAD.at<uint16_t>(row,col) = curSAD;
                        depth.at<uint16_t>(row, col) = d + 1;
                    }
                }
            }
        }

        // Calculate Depth Map Based on SAD values
        for (int row = borderSize; row < _grayLeft.rows - borderSize; row++) {
            for (int col = borderSize; col < _grayLeft.cols - borderSize; col++) {
                if (depth.at<uint16_t>(row,col) != 0) {
                    depth.at<uint16_t>(row,col) = _cam_baseline * _cam_focus / depth.at<uint16_t>(row,col);
                }
            }
        }

        std::cout << "Done!" << std::endl;
        normalizeDepthMap(depth);
    }
}

void Project3::calculateAbsDiff(const uint16_t& pixelLeft, const uint16_t& pixelRight, uint16_t& ADresult)
{
    ADresult = std::abs(pixelLeft - pixelRight);
}

void Project3::normalizeDepthMap(const cv::Mat& disparities)
{
    cv::normalize(disparities, _depthImage, 0, 255, CV_MINMAX, CV_8U);
}

cv::Mat& Project3::getDepthImage()
{
    return _depthImage;
}

int Project3::getWindowSize()
{
    return _windowSize;
}
