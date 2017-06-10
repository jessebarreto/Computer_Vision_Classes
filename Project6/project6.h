#ifndef PROJECT6_H
#define PROJECT6_H

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <vector>

#define ASCII_EXIT_CHAR static_cast<char>(27)

class HarrisDetector
{
    cv::Mat _harrisCornerStrength;

    // Harris Image of Threshold Corners
    cv::Mat _harrisCornerThreshold;

    // Harris Neighbourhood Size for derivative smoothing
    int _harrisNeighbourhood;

    // Harris Aperture for Gradient Computation
    int _harrisAperture;

    // Harris Parameter
    double _harrisParameter;

    // Harris Calculated Threshold Value
    double _harrisThreshold;

    // Harris Image for Local Maxima
    cv::Mat _harrisLocalMax;

    // Harris Maximum Strength for Threshold Computation
    double _harrisMaxStrength;

    // Harris Size of Neighbourhod for non-max supression
    int _harrisNonMaxSize;

    // Harris Kernel for non-max supression
    cv::Mat _harrisKernel;
public:
    // default constructor
    HarrisDetector(double harrisParameter = 0.01, double threshldValue = 0.01, double maxStrength = 0.0,
                   int aperture = 3, int neighbourhoodSize = 3, int nonMaxSize = 3) :
        _harrisNeighbourhood(neighbourhoodSize),
        _harrisAperture(aperture),
        _harrisParameter(harrisParameter),
        _harrisThreshold(threshldValue),
        _harrisMaxStrength(maxStrength),
        _harrisNonMaxSize(nonMaxSize)
    {
        setLocalMaxWindowSize();
    }

    // Sets Kernel
    void setLocalMaxWindowSize()
    {
        _harrisKernel.create(_harrisNonMaxSize, _harrisNonMaxSize, CV_8U);
    }

    // Compute Harris Corners
    void detectHarrisCorners(const cv::Mat &image)
    {
        cv::cornerHarris(image, _harrisCornerStrength, _harrisNeighbourhood, _harrisAperture, _harrisParameter,
                         cv::BORDER_DEFAULT);

        double minStrength;
        cv::minMaxLoc(_harrisCornerStrength, &minStrength, &_harrisMaxStrength);

        // Local Maximum detection
        cv::Mat dilatedImage;
        cv::dilate(_harrisCornerStrength, dilatedImage, cv::Mat());
        cv::compare(_harrisCornerStrength, dilatedImage, _harrisLocalMax, cv::CMP_EQ);
    }

    // Get the corner map from the computed Harris Values
    cv::Mat getCornersMap(double qualityLevel)
    {
        cv::Mat cornerMap;

        // Threshold the Corner Strength as a Percentage of the maximum strength
        _harrisThreshold = qualityLevel * _harrisMaxStrength;
        cv::threshold(_harrisCornerStrength, _harrisCornerThreshold, _harrisThreshold, 255, cv::THRESH_BINARY);

        // Convert to 8-bit image
        _harrisCornerThreshold.convertTo(cornerMap, CV_8U);

        // non-max supression
        cv::bitwise_and(cornerMap, _harrisLocalMax, cornerMap);

        return cornerMap;
    }

    void getCorners(std::vector<cv::KeyPoint> &keypoints, const cv::Mat &cornerMap) {
        keypoints.clear();
        // Iterates over the pixels to obtain all features
        for (int row = 0; row < cornerMap.rows; row++) {
            const uchar *rowPointer = cornerMap.ptr<uchar>(row);
            for (int col = 0; col < cornerMap.cols; col++) {
                if (rowPointer[col]) {
                    keypoints.push_back(cv::KeyPoint(static_cast<float>(col), static_cast<float>(row), 1.0f));
                }
            }
        }
    }

    void detect(const cv::Mat &image, std::vector<cv::KeyPoint> &keypoints)
    {
        detectHarrisCorners(image);
        cv::Mat cornerMap(getCornersMap(_harrisThreshold));
        getCorners(keypoints, cornerMap);
    }
};

class MouseHandler
{
private:
    cv::Point *_pointBegin;
    cv::Point *_pointEnd;
    cv::Mat *_frame;
    cv::Mat _roiImage;
    bool _isTemplateReady;

public:
    MouseHandler(cv::Mat* frame) :
        _pointBegin(NULL),
        _pointEnd(NULL),
        _frame(frame),
        _isTemplateReady(false)
    {
        if (_frame == NULL) {
            std::exit(-1);
        }
    }

    bool isReady()
    {
        return _isTemplateReady;
    }

    static void run(int event, int x, int y, int flag, void *rawMouseHandlerPtr)
    {
        MouseHandler *mouseHandler = static_cast<MouseHandler *>(rawMouseHandlerPtr);
        if (event == cv::EVENT_LBUTTONDOWN) {
            if (mouseHandler->_pointBegin == NULL) {
                mouseHandler->_pointBegin = new cv::Point(x, y);
            } else {
                int minX = std::min(x, mouseHandler->_pointBegin->x);
                int minY = std::min(y, mouseHandler->_pointBegin->y);
                int maxX = minX == x ? mouseHandler->_pointBegin->x : x;
                int maxY = minY == y ? mouseHandler->_pointBegin->y : y;

                mouseHandler->_pointBegin->x = minX;
                mouseHandler->_pointBegin->y = minY;
                if (mouseHandler->_pointEnd == NULL) {
                    mouseHandler->_pointEnd = new cv::Point(maxX, maxY);
                } else {
                    mouseHandler->_pointEnd->x = maxX;
                    mouseHandler->_pointEnd->y = maxY;
                }
            }
        } else if (event == cv::EVENT_RBUTTONDOWN) {
            if (mouseHandler->_pointBegin != NULL && mouseHandler->_pointEnd != NULL) {
                mouseHandler->_roiImage = mouseHandler->_frame->operator ()(cv::Rect(*mouseHandler->_pointBegin,
                                                                                     *mouseHandler->_pointEnd));
                mouseHandler->_isTemplateReady = true;
            }
        } else {
            return;
        }

    }

    ~MouseHandler()
    {
        if (_pointBegin) {
            delete _pointBegin;
        }

        if (_pointEnd) {
            delete _pointEnd;
        }
    }

    void drawRectangle(cv::Mat &image)
    {
        if (_pointBegin == NULL || _pointEnd == NULL) {
            return;
        }

        cv::rectangle(image, *_pointBegin - cv::Point(1, 1), *_pointEnd + cv::Point(1, 1),
                      cv::Scalar(255, 0, 0), 1, 8);
    }

    cv::Mat &getTemplateImage()
    {
        return _roiImage;
    }
};

class Project6
{
    // Video capture reference
    cv::VideoCapture *_videoStream;

    // Current Frame
    cv::Mat _frame;
    cv::Mat _grayFrame;

    // Template Image
    cv::Mat _template;
    cv::Mat _grayTemplate;

    // Surf Detector
    cv::SURF _surfDetector;

    // Harris Detector
    HarrisDetector _harrisDetector;

    // Surf Extractor
    cv::SurfDescriptorExtractor _surfExtractor;

    // Flann Based Matcher
    cv::FlannBasedMatcher _flannMatcher;

    // Matches
    cv::vector<cv::DMatch> _surfMatches, _harrisMatches;

    // Init Status
    bool _isSurfInit, _isTemplateInit, _isHarrisInit;

    // Images with Matches
    cv::Mat _surfMatchesImage, _harrisMatchesImage;

    // Window Windows Names
    std::vector<std::string> _windowsNames;

public:
    // Default Constructor
    Project6(cv::VideoCapture *videoStream);

    // Set SurfDetector
    void setSurfDetector(double surfHessianThreshold);

    // Set HarrisDetector
    void setHarrisDetector(double parameter, double threshold, double maxStrength,
                           int aperture, int neighboursSize, int nonMaxSize);

    // Set Template
    void setTemplate();


    // Check if it is initialized.
    bool isInit();

    // Tracks the template
    bool trackTemplate(int numberOfKeypoints);

    // Checks for next frame and update the current frame
    bool hasNextFrame();

    // Clear keypoints
    void clearKeyPoints(std::vector<cv::KeyPoint> &keypoints, int numberOfElements);

    // Find Good Matches
    void findGoodMatches(const std::__cxx11::string &msg, std::vector<cv::DMatch> &matches, const cv::Mat &descriptors);
};

#endif // PROJECT6_H
