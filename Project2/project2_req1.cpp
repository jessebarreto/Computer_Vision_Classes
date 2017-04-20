/******************************************************************************************
 * Universidade de Brasília – UnB
 * Instituto de Ciências Exatas – IE
 * Departamento de Ciência da Computação – CIC
 * Visao Computacional – Professor Fabio Vidal
 *
 * Projeto 2 – Objetivo
 * Esta atividade tem como objetivo principal avaliar os aspectos da calibração de câmeras,
 * realizando o desenvolvimento de uma "régua visual".
 *
 * Requisito 1
 * Desenvolva uma aplicação em OpenCV que a partir da utilização de uma entrada (clique de mouse),
 * seja capaz de capturar a posição inicial - xi,yi (primeiro clique de mouse) e a posição final xf,yf
 * (segundo clique do mouse) e permita desenhar uma linha na tela e ainda, calcule o comprimento desta linha em pixels.
 * O comprimento deverá ser dado pela distância Euclidiana Bidimensional
 * (https://pt.wikipedia.org/wiki/Dist%C3%A2ncia_euclidiana#Dist.C3.A2ncia_bidimensional).
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/

#include "projeto2.h"

#define WIN_NAME "Projeto 2 - Requisito 1"
#define DEFAULT_IMAGE "lena512color.tiff"

// Mouse Handler
static void mouseHandlerDrawLine( int event, int x, int y, int, void* requisitoData)
{
    if( event != cv::EVENT_LBUTTONDOWN )
        return;

    Projeto2 *data = static_cast<Projeto2*>(requisitoData);

    // Multiple Points Logic
    if (data->initFlag) {
        std::pair<cv::Point*, cv::Point*> linha(new cv::Point(x,y), NULL);
        data->lines.push_back(linha);
        data->initFlag = false;
        std::cout << "Fetch Point1: " << *linha.first << std::endl;
    } else {
        std::pair<cv::Point*, cv::Point*> &linha = data->lines.back();
        linha.second = new cv::Point(x,y);
        data->initFlag = true;
        std::cout << "Draw Line: " << *linha.first << " to " << *linha.second
                  << " Length: " << cv::norm(*linha.first - *linha.second) << std::endl;
        cv::line(*data->image, *linha.first, *linha.second, data->lineColor);
        data->updateWindow();
    }
}

int main(int argc, char **argv)
{
    std::string fileName;
    if (argc < 2) {
        fileName = DEFAULT_IMAGE;
        std::cout << "[WARNING] Parameter not found - Opening default image at "
                  << fileName << std::endl;
    } else {
        fileName = argv[1];
    }

    cv::Mat *image = new cv::Mat(cv::imread(fileName, CV_LOAD_IMAGE_ANYCOLOR));
    if (image->empty()) {
        std::cout << "[Error] Could not open image at " << fileName << std::endl;
        return -1;
    }

    Projeto2* requisito1 = new Projeto2(image, WIN_NAME);
    cv::namedWindow(WIN_NAME, CV_WINDOW_NORMAL);
    cv::resizeWindow(WIN_NAME, 640, 480);
    cv::imshow(WIN_NAME, *image);
    cv::setMouseCallback(WIN_NAME, mouseHandlerDrawLine, requisito1);
    cv::waitKey(0);
    cv::destroyAllWindows();
    delete requisito1;

    return 0;
}
