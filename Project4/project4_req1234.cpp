/******************************************************************************************
 * Universidade de Brasília – UnB
 * Instituto de Ciências Exatas – IE
 * Departamento de Ciência da Computação – CIC
 * Visao Computacional – Professor Fabio Vidal
 *
 * Projeto 4 – Objetivo
 * Esta atividade tem como objetivo principal a exploração e desenvolvimento de algoritmos para análise e comparativo
 * do processo de detecção de bordas em imagens.
 *
 * Requisito 1
 * A partir da referência de documentação da biblioteca OpenCV
 * (vide os links para cada tipo de detector de bordas proposto acima) desenvolva um algiritmo que a partir de uma
 * imagem de entrada realize o cálculo dos três tipos de detectores de bordas propostos, gerando 3(três) imagens
 * binárias de saída (caso o descritor não forneça diretamente uma imagem binarizada aplique uma técnica de binarização,
 * por exemplo algoritmo de Otsu - http://docs.opencv.org/2.4/doc/tutorials/imgproc/threshold/threshold.html) para cada
 * uma das saídas de cada detector de borda e salve as imagens resultantes (binárias) em disco.
 *
 * Requisito 2
 * Aplique o código desenvolvido no Requisito 1 para as imagens contidas no arquivo compactado edges-images/src
 * (Obs.: As imagens estão em formato .gif e é fortemente recomendado a conversão para .jpg), lembrando de salvar as
 * imagens binarizadas geradas em disco.
 *
 * Requisito 3
 * Para cada imagem da pasta ./src existe um arquivo de ground truth (GT)
 * (mais info: http://en.wikipedia.org/wiki/Ground_truth) na pasta ./gt. Estes arquivos da pasta ./gt possuem a
 * informação de referência GT para testar se o detector de borda está de acordo com a borda real da imagem de entrada
 * (os números no nome das imagens da pasta src são os mesmos da pasta gt com a adição dos caracteres "gt"). As imagens
 * da pasta ./gt são imagens .gif, então como no item do Requisito 1 e 2, desenvolva um código em OpenCV que abra (ou
 * transforme) as imagens da pasta ./gt como imagens binárias.
 *
 * Requisito 4
 * Para cada imagem de saída e para cada um dos detectores de borda propostos e com a imagem de GT fornecida (da mesma
 * imagem), realize o cálculo de um "score" para comparar os resultados obtidos pelo processo de detecção de borda
 * (assumindo a borda com o valor de pixel 0(zero)) desenvolvido, a saber:
 * Para cada pixel da imagem binarizada (ImBinSaida) de saída usando um detector de borda X (DBx)
 * crie um contador (DBxContador) em que quando o valor do pixel da imagem binarizada de GT (ImBinGT) for igual a da
 * imagem de entrada binarizada (ImBinSaida) denominado de DBxContadorHit, e outro contador denominado DBxContadorMiss
 * quando o valor do pixel destas duas imagens forem diferentes.
 * Após realizado para todos os pixels da imagem em questão, calcule a precisão (PrecImDBx) para imagem analisa e para
 * cada tipo de detector de borda, conforme as equações a seguir:
 *
 * PrecImDBx = DBxContadorHit / (DBxContadorHit + DBxContadorMiss)
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/

#include <vector>

#include "project4.h"

int main()
{
    // Main project objects
    Project4 projeto(100); // Threshold

    cv::Mat originalImage, groundTruthImage;
    std::string sourceImageName, groundTruthImageName;
    std::string destImageName;

    // Set Windows
    cv::namedWindow(projeto.getImageName(Project4::Original), CV_WINDOW_NORMAL);
    cv::namedWindow(projeto.getImageName(Project4::Sobel), CV_WINDOW_NORMAL);
    cv::namedWindow(projeto.getImageName(Project4::Canny), CV_WINDOW_NORMAL);
    cv::namedWindow(projeto.getImageName(Project4::Laplace), CV_WINDOW_NORMAL);
    cv::namedWindow(projeto.getImageName(Project4::GroundTruth), CV_WINDOW_NORMAL);

    // Set Saving Parameters
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);

    // Calculate borders for every image in src
    for (unsigned it = 0; it < 6; it++) {
        // Load Image File Name
        sourceImageName =   projeto.getDefaultImagesSrcsDirName() + projeto.getDefaultImageName(it) +
                            projeto.getImageFormatName(Project4::JPG);
        groundTruthImageName =  projeto.getDefaultImagesGtDirName() + projeto.getDefaultImageName(it) +
                                projeto.getImageName(Project4::GroundTruth) +
                                projeto.getImageFormatName(Project4::JPG);
        std::cout << "[INFO] Opening default images at " << sourceImageName << groundTruthImageName << std::endl;

        // Load image source from disk
        originalImage = cv::imread(sourceImageName, CV_LOAD_IMAGE_ANYCOLOR);
        if (originalImage.empty()) {
            std::cout << "[Error] Could not open image at "
                      << sourceImageName << std::endl;
            return 1;
        }
        groundTruthImage = cv::imread(groundTruthImageName, CV_LOAD_IMAGE_ANYCOLOR);
        if (groundTruthImage.empty()) {
            std::cout << "[Error] Could not open image at "
                      << groundTruthImageName << std::endl;
            return 1;
        }


        // Calculate
        projeto.setSourceImage(originalImage);
        projeto.setGroundTruthImage(groundTruthImage);
        projeto.calculateBorders(it);

        // Update Windows
        cv::imshow(projeto.getImageName(Project4::Original), projeto.getOriginal(it));
        cv::imshow(projeto.getImageName(Project4::Sobel), projeto.getSobel(it));
        cv::imshow(projeto.getImageName(Project4::Canny), projeto.getCanny(it));
        cv::imshow(projeto.getImageName(Project4::Laplace), projeto.getLaplace(it));
        cv::imshow(projeto.getImageName(Project4::GroundTruth), projeto.getGroundTruth(it));

        // Save images results in disk
        destImageName = projeto.getDefaultImagesDstsDirName() +
                                    projeto.getDefaultImageName(it) +
                                    projeto.getImageName(Project4::Sobel) +
                                    projeto.getImageFormatName(Project4::JPG);
        cv::imwrite(destImageName, projeto.getSobel(it), compression_params);
        destImageName = projeto.getDefaultImagesDstsDirName() +
                                        projeto.getDefaultImageName(it) +
                                        projeto.getImageName(Project4::Laplace) +
                                        projeto.getImageFormatName(Project4::JPG);
        cv::imwrite(destImageName, projeto.getLaplace(it), compression_params);
        destImageName = projeto.getDefaultImagesDstsDirName() +
                                        projeto.getDefaultImageName(it) +
                                        projeto.getImageName(Project4::Canny) +
                                        projeto.getImageFormatName(Project4::JPG);
        cv::imwrite(destImageName, projeto.getCanny(it), compression_params);

        // Calculate Scores
        projeto.calculateScore(it);

        // Update source image pressing any key
        cv::waitKey(0);
    }

    std::cout << "Saving Resultados Scores (%): " << std::endl;
    std::cout << projeto.getScores() << std::endl;
    projeto.saveMatrixXML(projeto.getScores(), "Scores", "scores", false);

    return 0;
}
