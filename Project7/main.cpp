/*******************************************************************************************
 * Universidade de Brasília – UnB
 * Instituto de Ciências Exatas – IE
 * Departamento de Ciência da Computação – CIC
 * Visao Computacional – Professor Fabio Vidal
 *
 * Projeto 5 – Objetivo
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
#include <iostream>

#define DEFAULT_VIDEO_FILE "Data/highwayI_raw.avi"

#define ORIGINAL_WIN_NAME "Original Video"

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
    cv::Mat *frame = new cv::Mat();
    for (;;) {
        video >> *frame;
        if (frame->empty()) {
            video.set(CV_CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        if (cv::waitKey(30) >= 0) {
            break;
        }

        cv::imshow(ORIGINAL_WIN_NAME, *frame);
    }

	return 0;
}
