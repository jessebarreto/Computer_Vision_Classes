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
 * Requisito 4
 * Repita o procedimento desenvolvido no Requisito 3, em que ao invés de abrir um arquivo de vídeo,
 * a aplicação abra o streaming de vídeo de uma câmera USB conectada ao computador e realize todos
 * os procedimentos solicitados no requisito 3.
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

template <class T, class U, class V>
class Tuple
{
public:
    T data1;

    U data2;

    V data3;

    Tuple(T d1, U d2, V d3)
    {
        data1 = d1;
        data2 = d2;
        data3 = d3;
    }
};

static void mouseHandler( int event, int x, int y, int flag, void* tuple_ptr)
{
    if( event != cv::EVENT_LBUTTONDOWN )
        return;

    cv::Point curPos = cv::Point(x,y);
    bool isGray = static_cast<Tuple<bool, cv::Mat*, std::vector<cv::Vec3b>*>*>(tuple_ptr)->data1;
    cv::Mat* image = static_cast<Tuple<bool, cv::Mat*, std::vector<cv::Vec3b>*>*>(tuple_ptr)->data2;
    std::vector<cv::Vec3b>* colours = static_cast<Tuple<bool, cv::Mat*, std::vector<cv::Vec3b>*>*>(tuple_ptr)->data3;

    std::cout << "Mouse Position: " << curPos << "\t";
    cv::Vec3b pixel = image->at<cv::Vec3b>(curPos);;

    if (isGray) {;
        std::cout << "Grayscale Image: I: [" << static_cast<int>(pixel[0]) << "]"<< std::endl;
    } else {
        std::cout << "Color Image: RGB: [" << static_cast<int>(pixel[2]) << ", "
                                           << static_cast<int>(pixel[1]) << ", "
                                           << static_cast<int>(pixel[0]) << "]" << std::endl;
    }

    colours->push_back(pixel);
}

static void paintColoursToRed(cv::Mat* image_ptr, std::vector<cv::Vec3b>* colours)
{
    static const cv::Vec3b RED = cv::Vec3b(0, 0, 255);
    if (colours->size() == 0) {
        return;
    }

    cv::Mat mask;
    uchar lowr, lowg, lowb;
    uchar higr, higg, higb;
    uchar range = uchar(13);

    for (size_t n = 0; n < colours->size(); n++)
    {
        lowb = colours->at(n)[0] < range ? 0 : colours->at(n)[0] - range;
        lowg = colours->at(n)[1] < range ? 0 : colours->at(n)[1] - range;
        lowr = colours->at(n)[2] < range ? 0 : colours->at(n)[2] - range;
        higb = colours->at(n)[0] > std::numeric_limits<uchar>::max() - range ? std::numeric_limits<uchar>::max() : colours->at(n)[0] + range;
        higg = colours->at(n)[1] > std::numeric_limits<uchar>::max() - range ? std::numeric_limits<uchar>::max() : colours->at(n)[1] + range;
        higr = colours->at(n)[2] > std::numeric_limits<uchar>::max() - range ? std::numeric_limits<uchar>::max() : colours->at(n)[2] + range;
        cv::inRange(*image_ptr, cv::Scalar(lowb, lowg, lowr), cv::Scalar(higb, higg, higr), mask);
        image_ptr->setTo(RED, mask);
    }
}

int main(int argc, char **argv)
{
    cv::VideoCapture video(0);
    if (!video.isOpened()) {
        std::cout << "[ERROR] Could not open video stream from default camera." << std::endl;
        return -1;
    }

    cv::namedWindow("Projeto 1 - Requisito 4", CV_WINDOW_NORMAL);
    cv::resizeWindow("Projeto 1 - Requisito 4", 640, 480);

    cv::Mat* frame = new cv::Mat();
    std::vector<cv::Vec3b> colours;
    while(true) {
        video >> *frame;
        if (frame->empty()) {
            video.set(CV_CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        if (cv::waitKey(30) >= 0) {
            break;
        }

        bool isGray = (frame->channels() == 1);
        if (isGray)
            cv::cvtColor(*frame, *frame, CV_GRAY2BGR);

        Tuple<bool, cv::Mat*, std::vector<cv::Vec3b>*> tuple3(isGray, frame, &colours);
        cv::setMouseCallback("Projeto 1 - Requisito 4", mouseHandler, &tuple3);
        paintColoursToRed(frame, &colours);
        cv::imshow("Projeto 1 - Requisito 4", *frame);
    }

    return 0;
}
