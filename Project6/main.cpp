/*
 **********************************************************************************************
 * Universidade de Brasília – UnB
 * Instituto de Ciências Exatas – IE
 * Departamento de Ciência da Computação – CIC
 * Visao Computacional – Professor Fabio Vidal
 *
 * Projeto 6 – Objetivo
 * Esta atividade tem como objetivo principal a exploração e desenvolvimento de algoritmos para análise e comparativo
 * do processo de reconhecimentos de objetos em vídeo.
 *
 * Requisito 1
 * Utilizando como referência a implementação proposta no Capítulo 8 de [1], implemente um código de detecção e
 * reconhecimento usando um descritor de Harris e usando o método de detecção invariante a escala do descritor SURF,
 * conforme apresentado neste capítulo da referência [1].
 *
 * Modifique o algoritmo para que a imagem do objeto a ser detectado seja proveniente de uma entrada de vídeo capturado
 * pela webcam instalada no computador. A dinâmica desta implementação deverá seguir a seguinte idéia: Inicializar a
 * câmera -> Apresentar o objeto a ser reconhecido à camera (capture uma imagem de template) -> Depois retire o objeto
 * do campo de visão da câmera e coloque ele em outro lugar/ambiente -> inicie o processo de reconhecimento em laço
 * capturando as imagens e procurando o objeto em cada imagem apresentada.
 *
 * Esta dinâmica de reconhecimento será computada com o número de pontos chaves (detectados) em cada imagem. Faça a
 * variação da quantidade de pontos chaves utilizados começando com o valor 1 e até 1000 (com passos incrementais de
 * 10 em 10 pontos)em seguida compare a quantidade de pontos detectados a partir de cada um dos algoritmos, observando
 * aquele que melhor detectou/reconheceu o objeto (conseguiu detectar ou não o objeto). Discuta os resultados também na
 * abordagem de custo computacional desta variação de pontos.
 *
 * [1] - Laganière, R.. OpenCV 2 Computer Vision Application Programming Cookbook - Packt Pub Limited, 2011
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 *********************************************************************************************
*/

#include "project6.h"

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

#define USE_CAM 1
#define DEFAULT_VIDEO "viptraffic.mp4"

int main(int argc, char **argv)
{
    /*************** Start Camera ***************/
    // Open Video
    cv::VideoCapture videoStream;
    if (argc < 2 && USE_CAM) {
        videoStream = cv::VideoCapture(0);
        std::cout << "[INFO] Opening Standard Video Device ID=0" << std::endl;
    } else {
        std::string fileName = argc >= 2 ? argv[1] : DEFAULT_VIDEO;
        videoStream = cv::VideoCapture(fileName);
        std::cout << "[INFO] Opening Video File at " << fileName << std::endl;
    }

    // Validate Video Stream
    if (!videoStream.isOpened()) {
        std::cout << "[ERROR] Could not open video stream" << std::endl;
        return 1;
    }

    // Set Project
    Project6 project(&videoStream);
    project.setHarrisDetector(0.01, 0.01, 0.0, 3, 3, 3);
    project.setSurfDetector(400.0);

    // Identify Object
    project.setTemplate();

    // Loop to Identify
    for (int i = 10; i < 1000; i+=10) {
        if (!project.trackTemplate(i)) {
            break;
        }
    }

    return 0;
}
