#ifndef PROJECT7_H
#define PROJECT7_H

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/legacy/legacy.hpp>

static void calculateFlowHS(cv::Mat *result, cv::Mat *resultX, cv::Mat *resultY, const cv::Mat *previous,
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
    static int fromTo[] = {0, 0, 1, 1};
    cv::Mat combinatition[] = {*resultX, *resultY};

    cv::mixChannels(combinatition, 2, result, 1, fromTo, 2);
}

static void calculateFlowReMap(cv::Mat *result, cv::Mat *flowXY, const cv::Mat *image)
{
    for (int x = 0; x < flowXY->cols; x++)
            flowXY->col(x) += cv::Scalar(x, 0);

    for (int y = 0; y < flowXY->rows; y++)
        flowXY->row(y) += cv::Scalar(0, y);

    cv::remap(*image, *result, *flowXY, cv::Mat(), CV_INTER_CUBIC);
}

static void calculateFlowImage(cv::Mat &result, const cv::Mat &flowXY)
{
    static int fromTo[] = {0,0, 1,1, -1,2};
    cv::mixChannels(&flowXY, 1, &result, 1, fromTo, 3);
    result = result*10 + 127;
}

static void getMotionMap(cv::Mat &flowImage, const cv::Mat &previousFrame, const cv::Mat &currentFrame, double alpha,
                      cv::Mat *velocityX, cv::Mat *velocityY, cv::Mat *flow)
{
    static cv::Mat flowXY(previousFrame.rows, previousFrame.cols, CV_32FC2);
    static cv::Mat variance, flowReMaped;

    calculateFlowHS(&flowXY, velocityX, velocityY, &previousFrame, &currentFrame, 100, alpha);
    calculateFlowReMap(&flowReMaped, &flowXY, &previousFrame);
    cv::compare(previousFrame, flowReMaped, variance, cv::CMP_EQ);

    // combine all the values into a single matrix with velocities and variance
    *flow = cv::Mat(previousFrame.rows, previousFrame.cols, CV_32FC4);
    cv::Mat mix[] = {*flow, variance};
    static int fromTo[] = {0,0, 1,1, 2,2, -1,3};
    cv::mixChannels(mix, 2, flow, 1, fromTo, 4);

    flowImage = cv::Mat(previousFrame.rows, previousFrame.cols, CV_32FC4);
    calculateFlowImage(flowImage, flowXY);
}

#endif // PROJECT7_H
