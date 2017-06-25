/*******************************************************************************************
 * Universidade de Brasília – UnB
 * Instituto de Ciências Exatas – IE
 * Departamento de Ciência da Computação – CIC
 * Visao Computacional – Professor Fabio Vidal
 *
 * Projeto 7 – Objetivo
 * Esta atividade tem como objetivo principal o desenvolvimento de melhorias para a detecção de movimento em imagens
 * sequenciais utilizando fluxo óptico.
 *
 * Requisito 1
 * A partir dos três arquivos de vídeo disponibilizados na pasta "videos-transito" e utilizando o processo de
 * segmentação de movimento descrito no artigo "Motion segmentation in sequential images
 * based on the differential optical flow" (disponibilizado na seção da tarefa, na página principal do Moodle da
 * disciplina) implemente a metodologia proposta e realize a segmentação de movimento seguindo a técnica apresentada
 * neste referido artigo.
 *
 * Requisito 2
 * Compare os resultados (melhorias ou pioras) desta versão desenvolvida em relação à técnica clássica de Background
 * Subtraction (http://docs.opencv.org/trunk/d1/dc5/tutorial_background_subtraction.html). Realize as análises sob o
 * ponto de vista da precisão do processo de detecção de veículos, bem como argumentando sobre a sua precisão na
 * detecção, tempo de execução e acurácia.
 *
 * Requisito 3
 * Descreva os resultados alcançados no relatório.
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 *********************************************************************************************
*/

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <iostream>
#include <chrono>

#include "project7.h"
                                                            //  a   d   e   t       or  a   d   e   t
#define DEFAULT_FILE1 "Data/highwayI_raw.avi"               //  1.5 20 25   35      or  1   15  15  49
#define DEFAULT_FILE2 "Data/Bsb-EPGU.mov"                   //  55  23  25  80      or  1   12  12  34
#define DEFAULT_FILE3 "Data/Bsb-EixoRodoviarioSul.mov"      //  10  12  12  35      or  66  44  44  98
#define DEFAULT_FILE4 "Data/viptraffic.mp4"                 //  1   10  15  28      or  15  5   5   36

#define DEFAULT_VIDEO_FILE DEFAULT_FILE4
#define VIDEO_SPEED_RATE 1 // Can not be zero

#define OPTICAL_FLOW_ALGORITHM_HS 1

#define ADAPTATIVE_BILATERAL_FILTER 0
#define BILATERAL_FILTER 0
#define BLUR_FILTER 1
#define BLUR_BOX_SIZE 15

#define USE_OTSU 0
#define USE_BHS 1

#define USE_MORPH_OPEN_CLOSE 1

#define ORIGINAL_WIN_NAME "Original Video"
#define FLOW_WIN_NAME "Optical Flow Video"
#define FILTERED_FLOW_WIN_NAME "Filtered Optical Flow Video"
#define NORMALIZED_FLOW_WIN_NAME "Normalized Optical Flow Values"
#define BINARY_NORMAL_WIN_NAME "Binary Normal Flow Video"
#define BINARY_MORPH_WIN_NAME "Binary Morphed Flow Video"
#define RESULT_WIN_NAME "Result Video"

#define DEFAULT_ALPHA 8000
#define DEFAULT_DILATE_BOX_SIZE 15
#define DEFAULT_ERODE_BOX_SIZE 30

static int alphaTrack = 1000;
static double globalHSAlpha = alphaTrack / 1000;
static int globalDilateSize = DEFAULT_DILATE_BOX_SIZE;
static int globalErodeSize = DEFAULT_ERODE_BOX_SIZE;
static bool globalTurnOnDilate = false;
static bool globalTurnOnErode = false;

static void trackBarHandler(int , void *);

int main(int argc, char **argv)
{
    // Open Video File
    std::string fileName;
    if (argc < 2) {
        fileName = DEFAULT_VIDEO_FILE;
        std::cout << "[WARNING] Parameter not found - Opening default video file at "
                  << fileName << std::endl;
    } else {
        fileName = argv[1];
    }
    cv::VideoCapture video(fileName);
    if (!video.isOpened()) {
        std::cout << "[ERROR] Could not open video file at " << fileName << std::endl;
        return -1;
    }

    // Run Video
    cv::namedWindow(ORIGINAL_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(ORIGINAL_WIN_NAME, 640, 480);
    cv::namedWindow(FLOW_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(FLOW_WIN_NAME, 640, 480);
    cv::namedWindow(FILTERED_FLOW_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(FILTERED_FLOW_WIN_NAME, 640, 480);
    cv::namedWindow(NORMALIZED_FLOW_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(NORMALIZED_FLOW_WIN_NAME, 640, 480);
    cv::namedWindow(BINARY_NORMAL_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(BINARY_NORMAL_WIN_NAME, 640, 480);
    cv::namedWindow(BINARY_MORPH_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(BINARY_MORPH_WIN_NAME, 640, 480);
    cv::namedWindow(RESULT_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(RESULT_WIN_NAME, 640, 480);
    cv::createTrackbar("HS Alpha", RESULT_WIN_NAME, &alphaTrack, 200000, trackBarHandler);
    cv::createTrackbar("Dilate Size", RESULT_WIN_NAME, &globalDilateSize, 100, trackBarHandler);
    cv::createTrackbar("Erode Size", RESULT_WIN_NAME, &globalErodeSize, 100, trackBarHandler);

    // Collection of Frames
    cv::Mat frame; // Current Frame
    cv::Mat currentFrame; // Current Frame in Grayscale
    cv::Mat previousFrame; // Previous Frame in Grayscale
    cv::Mat totalFlow; // Flow values at X,Y Directions and its variances
    cv::Mat flowX, flowY; // Flow values at X and Y separated
    cv::Mat blurredFlowX, blurredFlowY; // blurred Flow values at X and Y separated
    cv::Mat flowFrame, blurFlowFrame; // Flow Grid Frames
    cv::Mat normalFlow; // Normalized Flow Values
    cv::Mat flowMag, flowAng;
    cv::Mat binaryNormalFlow, binaryMorphFlow; // Image Binarized
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat resultImage;

    // First Frame
    video >> previousFrame;
    cv::cvtColor(previousFrame, previousFrame, CV_BGR2GRAY);

    for (;;) {
        // Read the frames from video
        video >> frame;
        cv::cvtColor(frame, currentFrame, CV_BGR2GRAY);

        // Loop the video
        if (currentFrame.empty()) {
            video.set(CV_CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        // Exit if user press space
        if (cv::waitKey((1/VIDEO_SPEED_RATE) * 30) >= 0) {
            break;
        }

        // Start Timer
        auto start = std::chrono::high_resolution_clock::now();

        // Obtain Motion Map
#if OPTICAL_FLOW_ALGORITHM_HS
        motionMapHSImage(flowFrame, frame,  previousFrame, currentFrame, globalHSAlpha, &flowX, &flowY, &totalFlow);
#else
        // Obtain Motion Map
        motionMapFImage(flowFrame, frame, previousFrame, currentFrame, &totalFlow);
        std::vector<cv::Mat> flows;
        cv::split(totalFlow, flows);
        flowX = flows[0].clone();
        flowY = flows[1].clone();
#endif

        // Filter the Motion Map
#if ADAPTATIVE_BILATERAL_FILTER
        cv::Mat flowX8b, flowY8b;
        flowX.convertTo(flowX8b, CV_8U);
        flowY.convertTo(flowY8b, CV_8U);
        cv::adaptiveBilateralFilter(flowX8b, blurredFlowX, cv::Size(BLUR_BOX_SIZE, BLUR_BOX_SIZE), 50);
        cv::adaptiveBilateralFilter(flowY8b, blurredFlowY, cv::Size(BLUR_BOX_SIZE, BLUR_BOX_SIZE), 50);
        blurredFlowX.convertTo(blurredFlowX, CV_32F);
        blurredFlowY.convertTo(blurredFlowY, CV_32F);
#elif BLUR_FILTER
        cv::blur(flowX, blurredFlowX, cv::Size(BLUR_BOX_SIZE, BLUR_BOX_SIZE));
        cv::blur(flowY, blurredFlowY, cv::Size(BLUR_BOX_SIZE, BLUR_BOX_SIZE));
#elif BILATERAL_FILTER
        cv::bilateralFilter(flowX, blurredFlowX, 4, 50, 50, cv::BORDER_DEFAULT);
        cv::bilateralFilter(flowY, blurredFlowY, 4, 50, 50, cv::BORDER_DEFAULT);
#endif
        drawOpticalFlowImage(blurFlowFrame, blurredFlowX, blurredFlowY, frame, 5, CV_RGB(255, 0, 0));

        // Normalize Magnitude
        cv::cartToPolar(blurredFlowX, blurredFlowY, flowMag, flowAng, true);
        cv::normalize(flowMag, normalFlow, 0, 255, cv::NORM_MINMAX, CV_8U);

        // Binaryze Magnitude based on Flow Map Histogram
#if USE_OTSU
        cv::threshold(normalFlow, binaryNormalFlow, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
#elif USE_BHS
        balancedHistogramThreshold(normalFlow, binaryNormalFlow);
#else
        cv::threshold(normalFlow, binaryNormalFlow, 200, 255, cv::THRESH_BINARY);
#endif

        // Morphologic Filter - Dilate and Erode
#if USE_MORPH_OPEN_CLOSE
        if (globalTurnOnErode && globalTurnOnDilate) {
            cv::dilate(binaryNormalFlow, binaryMorphFlow,
                       cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(globalDilateSize,
                                                                             globalDilateSize)));
            cv::erode(binaryMorphFlow, binaryMorphFlow,
                       cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(globalErodeSize,
                                                                             globalErodeSize)));
        } else if (!globalTurnOnDilate && globalTurnOnErode) {
            cv::erode(binaryNormalFlow, binaryMorphFlow,
                       cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(globalErodeSize,
                                                                             globalErodeSize)));
        } else if (globalTurnOnDilate && !globalTurnOnErode) {
            cv::dilate(binaryNormalFlow, binaryMorphFlow,
                       cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(globalDilateSize,
                                                                             globalDilateSize)));
        } else {
            binaryMorphFlow = binaryNormalFlow.clone();
        }
#else
        if (!globalTurnOffDilate) {
            cv::dilate(binaryNormalFlow, binaryMorphFlow,
                       cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(globalDilateSize,
                                                                             globalDilateSize)));
            cv::morphologyEx(binaryMorphFlow, binaryMorphFlow, cv::MORPH_CLOSE,
                             cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(globalMorphBoxSize,
                                                                                   globalMorphBoxSize)));
        } else {
            cv::morphologyEx(binaryNormalFlow, binaryMorphFlow, cv::MORPH_CLOSE,
                             cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(globalMorphBoxSize,
                                                                                   globalMorphBoxSize)));
        }
        cv::morphologyEx(binaryMorphFlow, binaryMorphFlow, cv::MORPH_OPEN,
                         cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(globalMorphBoxSize,
                                                                               globalMorphBoxSize)));
#endif
        // Find Cars Contours
        cv::findContours(binaryMorphFlow, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        // Draw Contours
        cv::cvtColor(currentFrame, resultImage, CV_GRAY2BGR);
        cv::drawContours(resultImage, contours, -1, cv::Scalar(0, 255, 255));
        for (int i = 0; i < contours.size(); i++) {
            cv::rectangle(resultImage, boundingRect(cv::Mat(contours[i])), cv::Scalar(255, 0, 0));
        }


        // Show Images
        cv::imshow(ORIGINAL_WIN_NAME, frame);
        cv::imshow(FLOW_WIN_NAME, flowFrame);
        cv::imshow(FILTERED_FLOW_WIN_NAME, blurFlowFrame);
        cv::imshow(NORMALIZED_FLOW_WIN_NAME, normalFlow);
        cv::imshow(BINARY_NORMAL_WIN_NAME, binaryNormalFlow);
        cv::imshow(BINARY_MORPH_WIN_NAME, binaryMorphFlow);
        cv::imshow(RESULT_WIN_NAME, resultImage);

        // Update Image
        previousFrame = currentFrame.clone();

        // Print Time
        auto end = std::chrono::high_resolution_clock::now();
        auto dt = 1.e-6 * std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        std::clog << dt << " ms elapsed" << std::endl;
    }

    return 0;
}

void trackBarHandler(int , void *)
{
    alphaTrack = std::max(alphaTrack, 1);
    globalHSAlpha = alphaTrack / 1000;

    globalTurnOnDilate = !(globalDilateSize < 2);
    globalTurnOnErode = !(globalErodeSize < 2);
}
