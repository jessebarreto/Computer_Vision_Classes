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
 * Requisito 4
 * Apresente os resultados obtidos e compare os valores calculados para cada região e a distância da lente da câmera,
 * em relação às saídas "raw" e "undistorted". Discuta os principais elementos causadores das variações das medições
 * obtidas com a esperada (medida real do objeto avaliado). Avalie a característica do hardware da câmera e quais
 * fatores podem ser os causadores destas possíveis diferenças. Proponha/discuta quais fatores podem ser
 * melhorados/aprimorados para que esta medição seja feita com a maior precisão possível.
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/

#include "projeto2.h"

#define RAW_WINDOW_NAME "Raw Video"
#define UNDISTORTED_WINDOW_NAME "Undistort"

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

    return 0;
}
