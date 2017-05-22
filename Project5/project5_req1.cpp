/*******************************************************************************************
 * Universidade de Brasília – UnB
 * Instituto de Ciências Exatas – IE
 * Departamento de Ciência da Computação – CIC
 * Visao Computacional – Professor Fabio Vidal
 *
 * Projeto 5 – Objetivo
 * Esta atividade tem como objetivo principal a exploração e desenvolvimento de algoritmos para a obtenção da
 * matriz GLCM, como apresentado em sala de aula.
 *
 * Requisito 1
 * Escreva um programa utilizando linguagem C/C++ e a biblioteca OpenCV (não utilizando as funções pré-definidas para
 * obtenção da GLCM implementadas na biblioteca OpenCV previamente) que receba como parâmetro uma imagem de teste
 * (fruit.jpg e wine.jpg, que estão disponíveis no arquivo .zip da atividade) e realize a obtenção da matriz GLCM,
 * salvando as duas matrizes resultantes em arquivos XMLs separados, na direção horizontal e 45 graus positivos,
 * neste caso em que o offset entre os pixels analisados sejam  [0 1] e [-1 1]
 * (vide: http://www.mathworks.com/help/images/ref/graycomatrix.html?requestedDomain=www.mathworks.com, seção offset),
 * ou seja, o par analisado é o pixel que está na mesma linha, ao lado direito da mesma linha do pixel em análise,
 * para a horizontal e para a 45 graus é o pixel que está no canto superior direito do pixel de análise.
 *
 * Para dúvidas sobre a obtenção da matriz GLCM, utilizem o tutorial a seguir: http://www.fp.ucalgary.ca/mhallbey/tutorial.htm
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 *********************************************************************************************
*/

#include "project5.h"

int main(int argc, char **argv)
{
    // Main Project Object
    Project5 projeto;

    cv::Mat sourceImage; // Source Image
    std::string fileName; // Image File Name

    // Load Image
    if (argc < 2) {
        fileName = projeto.getDefaultImageFileName();
    } else {
        fileName = argv[1];
    }

    // Load Image from Disk
    sourceImage = cv::imread(fileName, CV_LOAD_IMAGE_ANYCOLOR);
    if (sourceImage.empty()) {
        std::cout << "[Error] Could not open image at "
                  << fileName << std::endl;
        return 1;
    }

    // Sets the source image
    projeto.setSourceImage(sourceImage);

    // Sets the windows
    cv::namedWindow(projeto.getWindowsNames(0), CV_WINDOW_NORMAL);
    cv::namedWindow(projeto.getWindowsNames(1), CV_WINDOW_NORMAL);
    cv::namedWindow(projeto.getWindowsNames(2), CV_WINDOW_NORMAL);
    cv::namedWindow(projeto.getWindowsNames(3), CV_WINDOW_NORMAL);

    // Update the windows gray and colourful
    cv::imshow(projeto.getWindowsNames(0), projeto.getSourceImage());
    cv::imshow(projeto.getWindowsNames(1), projeto.getGraySourceImage());

    // Calculate GLCMs and Save them
    projeto.calculateAndSaveGLCMs();

    // Shows GLCMs as images
    cv::imshow(projeto.getWindowsNames(2), projeto.getGLCM0());
    cv::imshow(projeto.getWindowsNames(3), projeto.getGLCM45());

    // Wait any key to leave the program.
    cv::waitKey(0);

    return 0;
}
