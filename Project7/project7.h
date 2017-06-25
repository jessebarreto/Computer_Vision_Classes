#ifndef PROJECT7_H
#define PROJECT7_H

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/legacy/legacy.hpp>

#include <limits>

// Functions Definitions
void drawOpticalFlowImage (cv::Mat& flowImage, const cv::Mat& flowXY, const cv::Mat &original, int step, const cv::Scalar& color);
void drawOpticalFlowImage(cv::Mat& flowImage, const cv::Mat &flowX, const cv::Mat &flowY,
                                 const cv::Mat &original, int step, const cv::Scalar &color);
static void calculateFlowHS(cv::Mat *result, cv::Mat *resultX, cv::Mat *resultY, const cv::Mat *previous,
                            const cv::Mat *current, int iterations, double alpha);
void motionMapHSImage(cv::Mat &flowImage, const cv::Mat &original,  const cv::Mat &previousFrame,
                           const cv::Mat &currentFrame, double alpha, cv::Mat *velocityX, cv::Mat *velocityY,
                           cv::Mat *flow);
void motionMapFImage(cv::Mat &flowImage, const cv::Mat &original, const cv::Mat &previousFrame,
                            const cv::Mat &currentFrame, cv::Mat *flow);
void calculateHistogram(const cv::Mat &source, cv::Mat &histogram);
void balancedHistogramThreshold(const cv::Mat &source, cv::Mat &destination, int criterio = 5);
int getHistogramWeight(int begin, int end, const cv::Mat &histogram);
void setDefaultMorphKernel(cv::Mat &kernel, int size, int type = 0);

// Functions
void drawOpticalFlowImage (cv::Mat& flowImage, const cv::Mat& flowXY, const cv::Mat &original, int step, const cv::Scalar& color)
{
    flowImage = original.clone();
    for(int y = 0; y < flowXY.rows; y += step) {
        for(int x = 0; x < flowXY.cols; x += step)
        {
            const cv::Point2f& fxy = flowXY.at< cv::Point2f>(y, x);
            cv::line(flowImage, cv::Point(x,y), cv::Point(std::round(x + fxy.x), std::round(y + fxy.y)), color);
        }
    }
}

void drawOpticalFlowImage(cv::Mat& flowImage, const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &original,
                                 int step, const cv::Scalar &color)
{
    flowImage = original.clone();
    for(int y = 0; y < original.rows; y += step) {
        for(int x = 0; x < original.cols; x += step)
        {
            const float& fx = flowX.at<float>(y, x);
            const float& fy = flowY.at<float>(y, x);
            cv::line(flowImage, cv::Point(x,y), cv::Point(std::round(x + fx), std::round(y + fy)), color);
        }
    }
}

void calculateFlowHS(cv::Mat *result, cv::Mat *resultX, cv::Mat *resultY, const cv::Mat *previous,
                            const cv::Mat *current, int iterations, double alpha)
{
    static CvMat *flowX = cvCreateMat(previous->rows, previous->cols, CV_32FC1);
    static CvMat *flowY = cvCreateMat(previous->rows, previous->cols, CV_32FC1);
    static CvMat *prev = cvCreateMat(previous->rows, previous->cols, CV_8UC1);
    static CvMat *next = cvCreateMat(previous->rows, previous->cols, CV_8UC1);
    static CvTermCriteria endCriteria = {CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, iterations, 1e-10};

    previous->copyTo(cv::Mat(prev));
    current->copyTo(cv::Mat(next));

    cvCalcOpticalFlowHS(prev, next, false, flowX, flowY, alpha, endCriteria);
    *resultX = cv::Mat(flowX);
    *resultY = cv::Mat(flowY);

    // Merge Flows Together
    std::vector<cv::Mat> flows;
    flows.push_back(*resultX);
    flows.push_back(*resultY);
    cv::merge(flows, *result);
}

void motionMapHSImage(cv::Mat &flowImage, const cv::Mat &original,  const cv::Mat &previousFrame,
                    const cv::Mat &currentFrame, double alpha, cv::Mat *velocityX, cv::Mat *velocityY, cv::Mat *flow)
{
    calculateFlowHS(flow, velocityX, velocityY, &previousFrame, &currentFrame, 100, alpha);
    drawOpticalFlowImage(flowImage, *flow, original, 5, CV_RGB(0, 0, 255));
}

void motionMapFImage(cv::Mat &flowImage, const cv::Mat &original, const cv::Mat &previousFrame,
                     const cv::Mat &currentFrame, cv::Mat *flow)
{
    // Calculate flow using Farnebäck's algorithm and some parameters that seem to work the best
    double pyr_scale = 0.8;
    double poly_sigma = (previousFrame.rows + previousFrame.cols) / 1000.0;
    int levels = 100;
    int winsize = (previousFrame.rows + previousFrame.cols)/100;
    int iterations = 7;
    int poly_n = (poly_sigma < 1.5 ? 5 : 7);
    int flags = 0;

    cv::calcOpticalFlowFarneback(previousFrame, currentFrame, *flow, pyr_scale, levels, winsize, iterations, poly_n,
                                 poly_sigma, flags);
    drawOpticalFlowImage(flowImage, *flow, original, 10, CV_RGB(0, 0, 255));
}

void calculateHistogram(const cv::Mat &source, cv::Mat &histogram)
{
    int histogramSize = 256;
    float range[] = {0, 256};
    const float* histogramRange = { range };
    int grayChannels = 0;
    cv::calcHist(&source, 1, &grayChannels, cv::Mat(), histogram, 1, &histogramSize, &histogramRange);
}

int getHistogramWeight(int begin, int end, const cv::Mat &histogram)
{
    int range = end -begin;
    int sum = 0;
    for(int i = begin; i < end; i++) {
        sum += static_cast<int>(histogram.at<uchar>(0, i));
    }
    return sum / range;
}

void balancedHistogramThreshold(const cv::Mat &source, cv::Mat &destination, int criterio)
{
    cv::Mat histogram;
    calculateHistogram(source, histogram);

    int begin = 0, end = 256, center = 128;

    int leftWeight = getHistogramWeight(begin, center + 1, histogram);
    int rightWeight = getHistogramWeight(center + 1, end, histogram);
    int newCenter;
    int diff = 8000;

    int cnt = 0;
    do {
        newCenter = (leftWeight + rightWeight) / 2;
        diff = std::abs(newCenter - center);
        center = newCenter;
        leftWeight = getHistogramWeight(begin, center + 1, histogram);
        rightWeight = getHistogramWeight(center + 1, end, histogram);
        cnt++;
    } while(diff >= criterio && cnt < 200);

// METHOD 1
//    for (;begin <= end;) {
//        if (rightWeight > leftWeight) {
//            rightWeight = static_cast<int>(histogram.at<uchar>(0, end--));
//            if ((begin + end) / 2 < center) {
//                rightWeight += static_cast<int>(histogram.at<uchar>(0, center));
//                leftWeight -= static_cast<int>(histogram.at<uchar>(0, center));
//                center--;
//            }
//        } else {
//            leftWeight = static_cast<int>(histogram.at<uchar>(0, begin++));
//            if ((begin + end) / 2 >= center) {
//                leftWeight += static_cast<int>(histogram.at<uchar>(0, center + 1));
//                rightWeight -= static_cast<int>(histogram.at<uchar>(0, center + 1));
//                center++;
//            }
//        }
//    }

    cv::threshold(source, destination, center, 255, cv::THRESH_BINARY);
}

void setDefaultMorphKernel(cv::Mat &kernel, int size, int type)
{
    if (type = 0) {
        kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(size, size));
    } else {
        kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(size, size));
    }
}

#endif // PROJECT7_H
