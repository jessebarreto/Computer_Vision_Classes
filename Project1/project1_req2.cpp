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

template <class T, class U>
class Tuple2
{
public:
    Tuple2(T d1, U d2)
    {
        data1 = d1;
        data2 = d2;
    }

    T data1;

    U data2;
};


static void mouseHandler( int event, int x, int y, int flag, void* tuple_ptr)
{
    if( event != cv::EVENT_LBUTTONDOWN )
        return;

    cv::Point curPos = cv::Point(x,y);
    bool isGray = static_cast<Tuple2<bool, cv::Mat*>*>(tuple_ptr)->data1;
    cv::Mat* image = static_cast<Tuple2<bool, cv::Mat*>*>(tuple_ptr)->data2;


    std::cout << "Mouse Position: " << curPos << "\t";
    cv::Vec3b pixel(0,0,0);

    if (isGray) {
        pixel = image->at<cv::Vec3b>(curPos);
        std::cout << "Grayscale Image: I: [" << static_cast<int>(pixel[0]) << "]"<< std::endl;
    } else {
        pixel = image->at<cv::Vec3b>(curPos);
        std::cout << "Color Image: RGB: [" << static_cast<int>(pixel[2]) << ", "
                                           << static_cast<int>(pixel[1]) << ", "
                                           << static_cast<int>(pixel[0]) << "]" << std::endl;
    }

    cv::Mat mask;
    uchar range = 13;
    uchar lowb = pixel[0] < range ? 0 : pixel[0] - range;
    uchar lowg = pixel[1] < range ? 0 : pixel[1] - range;
    uchar lowr = pixel[2] < range ? 0 : pixel[2] - range;
    uchar higb = pixel[0] > std::numeric_limits<uchar>::max() - range ? std::numeric_limits<uchar>::max() : pixel[0] + range;
    uchar higg = pixel[1] > std::numeric_limits<uchar>::max() - range ? std::numeric_limits<uchar>::max() : pixel[1] + range;
    uchar higr = pixel[2] > std::numeric_limits<uchar>::max() - range ? std::numeric_limits<uchar>::max() : pixel[2] + range;
    cv::inRange(*image, cv::Scalar(lowb, lowg, lowr), cv::Scalar(higb, higg, higr), mask);
    image->setTo(cv::Vec3b(0, 0, 255), mask);
    cv::imshow("Projeto 1 - Requisito 2", *image);
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

    cv::namedWindow("Projeto 1 - Requisito 2", CV_WINDOW_NORMAL);
    cv::resizeWindow("Projeto 1 - Requisito 2", 640, 480);
    cv::imshow("Projeto 1 - Requisito 2", image);

    bool isGray = (image.channels() == 1);
    if (isGray)
        cv::cvtColor(image, image, CV_GRAY2BGR);

    Tuple2<bool, cv::Mat*> tupla(isGray, &image);
    cv::setMouseCallback("Projeto 1 - Requisito 2", mouseHandler, &tupla);

    cv::waitKey(0);
    return 0;
}
