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
 * Requisito 2
 * Repita o procedimento desenvolvido no Requisito 1, de forma a criar uma rotina que além
 * de atender os itens do Requisito 1, após o clique com o botão esquerdo, mude a cor na
 * imagem aberta em tela de todos os pixels da imagem que foi selecionado com valores
 * variando em 5% do valor do pixel que foi clicado para a cor vermelha. Observem que isto
 * deve estar preparado para imagens coloridas e também em nível de cinza.
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

static void mouseHandler( int event, int x, int y, int flag, void* image_ptr)
{
    if( event != cv::EVENT_LBUTTONDOWN ) {
        return;
    }

    cv::Point curPos = cv::Point(x,y);
    cv::Mat* image = static_cast<cv::Mat*>(image_ptr);
    std::cout << "Mouse Position: " << curPos << "\t";
    cv::Vec3b pixel = image->at<cv::Vec3b>(curPos);
    bool isGray = pixel[0] == pixel[1] && pixel[1] == pixel[2];
    if (!isGray) {
        std::cout << "Color Image: RGB:" << pixel << std::endl;
    } else {
        std::cout << "Grayscale Image: I:" << static_cast<int>(pixel[0]) << std::endl;
    }


    cv::Vec3b pixel_diff;
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            cv::absdiff(image->at<cv::Vec3b>(i, j), pixel, pixel_diff);
            if (pixel_diff[0] < 13 && pixel_diff[1] < 13 && pixel_diff[2] < 13) {
                image->at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);

            }
        }
    }
    cv::imshow("Projeto 1 - Requisito 2", *image);
}

int main(int argc, char **argv)
{
    std::string fileName;
    if (argc < 2) {
        fileName = "../../Data/CuteDoge.jpg";
        std::cout << "[WARNING] Parameter not found - Opening default image at "
                  << fileName << std::endl;
    } else {
        fileName = argv[1];
    }

    cv::Mat image = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);
    if (image.empty()) {
        std::cout << "[Error] Could not open image at " << fileName << std::endl;
        return -1;
    }

    cv::namedWindow("Projeto 1 - Requisito 2", CV_WINDOW_NORMAL);
    cv::resizeWindow("Projeto 1 - Requisito 2", 640, 480);
    cv::imshow("Projeto 1 - Requisito 2", image);

    cv::setMouseCallback("Projeto 1 - Requisito 2", mouseHandler, &image);

    cv::waitKey(0);
    return 0;
}

