/******************************************************************************************
 * Universidade de Brasília – UnB
 * Instituto de Ciências Exatas – IE
 * Departamento de Ciência da Computação – CIC
 * Visao Computacional – Professor Fabio Vidal
 *
 * Projeto 1 – Objetivo
 * Esta atividade tem como objetivo principal a exploração e desenvolvimento de algoritmos
 * na ferramenta OpenCV (http://opencv.org/).
 * Observação: Todos os requisitos desta atividade deverão ser elaborados utilizando a
 * versão 2.x (ou superior) do ferramenta OpenCV.
 *
 * Requisito 1
 * Elabore uma aplicação utilizando OpenCV que abra um arquivo de imagem (tipo JPG) e que
 * permita ao usuário clicar (botão esquerdo do mouse) sobre um ponto na área da imagem
 * na tela e após realizado o clique mostre no terminal a coordenadado ponto (x,y) na
 * imagem, informando os valores do pixel RGB, quando a imagem for colorida ou mostrando
 * o valor da intensidade o pixel quando a imagem for em nível de cinza (grayscale).
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/


#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;

static void mouseHandler( int event, int x, int y, int flag, void* image_ptr)
{
    if( event != cv::EVENT_LBUTTONDOWN )
        return;

    cv::Point curPos = cv::Point(x,y);
    cv::Mat* image = static_cast<cv::Mat*>(image_ptr);
    std::cout << "Mouse Position: " << curPos << "\t";
    cv::Vec3b pixel = image->at<cv::Vec3b>(curPos);
    if (image->channels() == 1) {
        std::cout << "Grayscale Image: I: [" << static_cast<int>(pixel[0]) << "]"<< std::endl;
    } else {
        std::cout << "Color Image: RGB: [" << static_cast<int>(pixel[2]) << ", "
                                           << static_cast<int>(pixel[1]) << ", "
                                           << static_cast<int>(pixel[0]) << "] " << std::endl;
    }
}

int main(int argc, char **argv)
{
    std::string fileName;
    if (argc < 2) {
        fileName = "lena.jpg";
        std::cout << "[WARNING] Parameter not found - Opening default image at "
                  << fileName << std::endl;
    } else {
        fileName = argv[1];
    }

    cv::Mat image = cv::imread(fileName, CV_LOAD_IMAGE_ANYCOLOR);
    if (image.empty()) {
        std::cout << "[Error] Could not open image at " << fileName << std::endl;
        return -1;
    }

    cv::namedWindow("Projeto 1 - Requisito 1", CV_WINDOW_NORMAL);
    cv::resizeWindow("Projeto 1 - Requisito 1", 640, 480);
    cv::imshow("Projeto 1 - Requisito 1", image);

    cv::setMouseCallback("Projeto 1 - Requisito 1", mouseHandler, &image);

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
