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

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <chrono>

                                                            //  a   d   e   t       or  a   d   e   t
#define DEFAULT_FILE1 "Data/highwayI_raw.avi"               //  1.5 20 25   35      or  1   15  15  49
#define DEFAULT_FILE2 "Data/Bsb-EPGU.mov"                   //  55  23  25  80      or  1   12  12  34
#define DEFAULT_FILE3 "Data/Bsb-EixoRodoviarioSul.mov"      //  10  12  12  35      or  66  44  44  98
#define DEFAULT_FILE4 "Data/viptraffic.mp4"                 //  1   10  15  28      or  15  5   5   36

#define DEFAULT_VIDEO_FILE DEFAULT_FILE2
#define VIDEO_SPEED_RATE 1 // Can not be zero

#define MOG2 1
#define MOG 0

#define BLUR_BOX_SIZE 15

#define ORIGINAL_WIN_NAME "Original Video"
#define FOREGROUND_MASK_WIN_NAME "Foreground Mask"
#define BLUR_FOREGROUND_MASK_WIN_NAME "Blurred Foreground Mask"
#define RESULT_WIN_NAME "Result Video"

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
    cv::namedWindow(FOREGROUND_MASK_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(FOREGROUND_MASK_WIN_NAME, 640, 480);
//    cv::namedWindow(BLUR_FOREGROUND_MASK_WIN_NAME, CV_WINDOW_NORMAL);
//    cv::resizeWindow(BLUR_FOREGROUND_MASK_WIN_NAME, 640, 480);
    cv::namedWindow(RESULT_WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(RESULT_WIN_NAME, 640, 480);


    // Collection of Frames
    cv::Mat frame; // Current Frame
    cv::Mat foregroundMask, blurForegroundMask; //Foreground Mask
    cv::Ptr<cv::BackgroundSubtractor> ptrSubtractor; //MOG2 Background subtractor
    std::vector<std::vector<cv::Point>> contours; // Car Contours
    cv::Mat hierarchy; // Contours Hierarchy
    cv::Mat resultFrame;


    // Subtractor Object
#if MOG2
    ptrSubtractor = new cv::BackgroundSubtractorMOG2(1, 50 , false);
#elif MOG
    ptrSubtractor = new cv::BackgroundSubtractorMOG();
#else
    ptrSubtractor = new cv::BackgroundSubtractorGMG();
#endif
    // Time Setup
    for (;;) {
        // Read the frames from video
        video >> frame;

        // Loop the video
        if (frame.empty()) {
            video.set(CV_CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        // Exit if user press space
        if (cv::waitKey((1/VIDEO_SPEED_RATE) * 30) >= 0) {
            break;
        }

        // Count Time
        auto start = std::chrono::high_resolution_clock::now();

        // Create a foreground Mask
        ptrSubtractor->operator()(frame, foregroundMask);

        // Blur Foreground Mask
        blurForegroundMask = foregroundMask.clone();
//        cv::blur(blurForegroundMask, blurForegroundMask, cv::Size(BLUR_BOX_SIZE, BLUR_BOX_SIZE));

        // Obtain Car Countours
        cv::findContours(blurForegroundMask, contours, hierarchy, CV_RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Draw Car Contours
        resultFrame = frame.clone();
        for (size_t idx = 0; idx < contours.size(); idx++) {
            cv::Rect rect = cv::boundingRect(contours[idx]);
            if (rect.width > 10 && rect.width < frame.cols * 0.75 && rect.height > 10 && rect.height < frame.rows * 0.75) {
                int cx = rect.width / 2;
                int cy = rect.height / 2;
                cv::drawContours(resultFrame, contours, idx, CV_RGB(255, 0 ,0));
                cv::rectangle(resultFrame, rect, CV_RGB(0, 255, 0));
            }

        }

        // Show Images
        cv::imshow(ORIGINAL_WIN_NAME, frame);
        cv::imshow(FOREGROUND_MASK_WIN_NAME, foregroundMask);
//        cv::imshow(BLUR_FOREGROUND_MASK_WIN_NAME, foregroundMask);
        cv::imshow(RESULT_WIN_NAME, resultFrame);

        // Print Time
        auto end = std::chrono::high_resolution_clock::now();
        auto dt = 1.e-6 * std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        std::clog << dt << " ms elapsed" << std::endl;
    }

    return 0;
}

