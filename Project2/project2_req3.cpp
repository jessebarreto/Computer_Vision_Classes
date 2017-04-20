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
 * Requisito 3 (Criando a régua visual)
 * Terminado o processo de calibração e utilizando a imagem de vídeo das saídas "raw" e "undistorted" definidas pela
 * aplicação e algum objeto com dimensão (altura ou largura) conhecida faça o seguinte:
 *
 * 1) Divida a região do vídeo em um grid de 8 quadrados (Dica: utilize as funções de desenhar linha na tela do openCV,
 * tendo como exemplo de divisão a imagem disponível neste
 * link: http://photos1.blogger.com/blogger/3641/220/1600/quadrados3.jpg).
 * Este grid é solicitado para única e exclusivamente servir como orientação na imagem.
 * Se possível, crie um ponto de referência no centro de cada célula do grid para que no próximo passo o objeto possa
 * ser posicionado mais facilmente na imagem. Posicione no centro de cada um dos 8 quadrados(células do grid) o objeto
 * a uma distância de 0,15m, 0,8m, 1,5m e 3m respectivamente da lente da câmera (lembrando de após a calibração a câmera
 * deverá estar afixada e deve-se evitar a todo custo todo e qualquer movimento, principalmente de lentes).
 *
 * 2) Com a aplicação desenvolvida, meça em pixels a maior dimensão deste objeto (utilize sempre esta dimensão em todas
 * as medidas). Transforme este valor em pixel (assumindo o tamanho do pixel quadrado para simplificar) e utilizando o
 * valor médio do pixel obtido a partir da média dos parâmetros referentes às dimensões do pixel no processo de
 * calibração. Repita a medição de cada posição 3 vezes e calcule a média da medida obtida para cada célula.
 *
 * 3) Faça uma tabela consolidada (já utilizando os valores médios para não ocupar muito espaço no relatório) com estes
 * resultados, como indicado abaixo, para os valores medidos na imagem de saída da janela "raw" e "undistorted".
 * Distância	0,15m	0,8m	1,5m	3m
 * Célula 1
 * Célula 2
 * Célula 3
 * Célula 4
 * Célula 5
 * Célula 6
 * Célula 7
 * Célula 8
 * Célula 9
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/

#include "projeto2.h"

#define RAW_WINDOW_NAME "Raw Video"
#define UNDISTORTED_WINDOW_NAME "Undistort"
#define NUMBER_OF_CELLS 9

const std::vector<int> Distances = {
    15,
    80,
    150,
    300
};

// Mouse Handler
static void mouseHandler( int event, int x, int y, int, void* requisitoData)
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
    }
}

int main(int argc, char **argv)
{
    cv::VideoCapture video(0);
    if (!video.isOpened()) {
        std::cout << "[ERROR] Could not open camera." << std::endl;
        return -1;
    }

    // Project Objects
    cv::Mat* rawImage = new cv::Mat();
    cv::Mat* undistortedImage = new cv::Mat();
    video >> *rawImage;
    Projeto2 requisito2Raw(rawImage, RAW_WINDOW_NAME);
    Projeto2 requisito2Undistorted(undistortedImage, UNDISTORTED_WINDOW_NAME, cv::Scalar(0, 255, 0));

    // Load Matrices
    std::cout << "Loading Camera Parameters" << std::endl;
    std::string intrinsicsName("Intrinsics");
    std::string distortionName("Distortion");
    cv::Mat intr, dist;
    requisito2Raw.loadMatrixXML(intr, intrinsicsName, intrinsicsName);
    requisito2Raw.loadMatrixXML(dist, distortionName, distortionName);

    // Calibrator
    CameraCalibrator calibrator(intr, dist, requisito2Raw.image->size());

    // Start Windows
    cv::namedWindow(RAW_WINDOW_NAME);
    cv::namedWindow(UNDISTORTED_WINDOW_NAME);

    std::string LinesXMLFile("LinesXML");
    for (unsigned distanceIndex = 0; distanceIndex < Distances.size(); distanceIndex++) {
        for (int cellNumber = 1; cellNumber <= NUMBER_OF_CELLS; ) {
            if (!video.isOpened()) {
                std::cout << "[ERROR] Could not open camera." << std::endl;
                return -1;
            }
            video >> *requisito2Raw.image;

            if (rawImage->empty()) {
                std::cout << "[ERROR] Camera Disconnected!" << std::endl;
                cv::destroyAllWindows();
                return -1;
            }

            if (cv::waitKey(30) >= 0) {
                std::cout << "[MESSAGE] Finish Distance: " << Distances[distanceIndex] << std::endl;
                cv::destroyAllWindows();
                break;
            }

            cv::setMouseCallback(RAW_WINDOW_NAME, mouseHandler, &requisito2Raw);
            cv::setMouseCallback(UNDISTORTED_WINDOW_NAME, mouseHandler, &requisito2Undistorted);
            requisito2Raw.drawGrid();
            requisito2Raw.drawLines();

            calibrator.undistortImageFromCamera(*requisito2Raw.image, *requisito2Undistorted.image);

            requisito2Undistorted.drawLines();

            requisito2Raw.updateWindow();
            requisito2Undistorted.updateWindow();

            if (requisito2Raw.lines.size() > 3 && requisito2Undistorted.lines.size() >= 3)
            {
                // Print the mean of every cell
                std::cout << "Save Lines -  Cell: " << cellNumber << " Distance: "
                          << Distances[distanceIndex] << std::endl;

                std::string LinesXMLFileName(LinesXMLFile);
                LinesXMLFileName.append("Cell");
                LinesXMLFileName.append(std::to_string(cellNumber));
                requisito2Raw.saveLinesXML(LinesXMLFileName, Distances[distanceIndex]);
                LinesXMLFileName.append("Und");
                requisito2Undistorted.saveLinesXML(LinesXMLFileName, Distances[distanceIndex]);

                cellNumber++;
                requisito2Raw.clearLines();
                requisito2Undistorted.clearLines();
            }
        }
    }

    // Destroy Windows
    cv::destroyAllWindows();

    return 0;
}
