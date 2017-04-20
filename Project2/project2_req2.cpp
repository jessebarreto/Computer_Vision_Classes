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
 * Requisito 2
 * A partir do código disponibilizado no aprender, realize a calibração de uma câmera digital
 * (padrão USB, podendo ser a embarcada que acompanha o notebook, por exemplo),
 * gerando os arquivos "xmls" com os parâmetros intrínsecos e distorções provenientes do
 * processo de calibração. Utilize para o desenvolvimento da aplicação de forma mais simples, o
 * programa de calibração disponibilizado no tópico da atividade, incluindo o padrão de calibração.
 * A partir da saída do vídeo na janela "raw" e "undistorted" desta aplicação, acople-a ao código da
 * aplicação desenvolvida no Requisito 1 (utilize uma função de chamada ao sistema pelo mouse -
 * exemplo: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html).
 *
 * Observação
 * Para a calibração, capture pelo menos 25 imagens (spanspots), apresentando o template
 * de tabuleiro de xadrez (lembrando que se for usar o template lembre-se que o tamanho do quadrado é
 * definido de acordo com a precisão impressora utilizada. Utilize uma régua de qualidade para medir
 * o tamanho do quadrado impresso do padrão de referência) passando por todas as áreas visíveis da
 * imagem (explorando as áreas de formas proporcionais). Isto é de fundamental importância para que
 * os parâmetros de distorção da câmera sejam estimados mais precisamente. Repita este procedimento
 * pelo menos 5(cinco) vezes, armazenando cada arquivo xml gerado em separado, e para todos os
 * parâmetros obtidos, tire a média e o desvio padrão para cada parâmetro da calibração obtido.
 * Para ser utilizado nos próximos passos, utilize a média destes parâmetros obtidos em um novo
 * arquivo xml tanto para a distorção (distortion.xml), quanto para os parâmetros intrínsicos
 * (intrisics.xml) . E uma dica importante: realize o processo de calibração em um ambiente com
 * boa iluminação e faça todos os procedimentos no mesmo horário do dia/noite, para evitar que a
 * calibração sofra influência direta da iluminação externa - luz solar.
 *
 * Nome: Jessé Barreto de Barros
 * Matrícula: 17/0067033
 ******************************************************************************************
*/

#include "projeto2.h"

#define RAW_WINDOW_NAME "Raw Video Window"
#define UNDISTORTED_WINDOW_NAME "Undistort Video Window"

#define NUMBER_CALIBRATION 5

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

void calculateMeanStdDevCameraParametersAndSave(Projeto2 &requisito, cv::Mat *intrinsic, cv::Mat *intrinsicStdDev,
                                         cv::Mat *distortion, cv::Mat *distortionStdDev)
{
    std::string intrinsicsName("Intrinsics");
    std::string distortionName("Distortion");
    std::vector<double> k1, k2, p1, p2;
    std::vector<double> fy, fx, cx, cy;
    std::vector<double> mean, stdDev;
    for (int index = 1; index <= NUMBER_CALIBRATION; index++) {
        std::string intrinsicsFileName(intrinsicsName);
        std::string distortionFileName(distortionName);
        intrinsicsFileName.append(std::to_string(index));
        distortionFileName.append(std::to_string(index));

        cv::Mat intr, dist;
        requisito.loadMatrixXML(intr, intrinsicsName, intrinsicsFileName);
        requisito.loadMatrixXML(dist, distortionName, distortionFileName);

        k1.push_back(dist.at<double>(0,0));
        k2.push_back(dist.at<double>(0,1));
        p1.push_back(dist.at<double>(0,2));
        p2.push_back(dist.at<double>(0,3));

        fx.push_back(intr.at<double>(0,0));
        fy.push_back(intr.at<double>(1,1));
        cx.push_back(intr.at<double>(0,2));
        cy.push_back(intr.at<double>(1,2));
    }
    cv::Mat(cv::Mat::zeros(4,1, CV_32F)).copyTo(*distortion);
    cv::Mat(cv::Mat::zeros(4,1, CV_32F)).copyTo(*distortionStdDev);
    cv::meanStdDev(k1, mean, stdDev);
    distortion->at<float>(0,0) = mean.back();
    distortionStdDev->at<float>(0,0) = stdDev.back();
    cv::meanStdDev(k2, mean, stdDev);
    distortion->at<float>(0,1) = mean.back();
    distortionStdDev->at<float>(0,1) = stdDev.back();
    cv::meanStdDev(p1, mean, stdDev);
    distortion->at<float>(0,2) = mean.back();
    distortionStdDev->at<float>(0,2) = stdDev.back();
    cv::meanStdDev(p2, mean, stdDev);
    distortion->at<float>(0,3) = mean.back();
    distortionStdDev->at<float>(0,3) = stdDev.back();

    cv::Mat(cv::Mat::eye(3, 3, CV_32F)).copyTo(*intrinsic);
    cv::Mat(cv::Mat::eye(3, 3, CV_32F)).copyTo(*intrinsicStdDev);;
    cv::meanStdDev(fx, mean, stdDev);
    intrinsic->at<float>(0,0) = mean.back();
    intrinsicStdDev->at<float>(0,0) = stdDev.back();
    cv::meanStdDev(fy, mean, stdDev);
    intrinsic->at<float>(1,1) = mean.back();
    intrinsicStdDev->at<float>(1,1) = stdDev.back();
    cv::meanStdDev(cx, mean, stdDev);
    intrinsic->at<float>(0,2) = mean.back();
    intrinsicStdDev->at<float>(0,2) = stdDev.back();
    cv::meanStdDev(cy, mean, stdDev);
    intrinsic->at<float>(1,2) = mean.back();
    intrinsicStdDev->at<float>(1,2) = stdDev.back();

    // Save Mean and StdDev Camera Parameters
    requisito.saveMatrixXML(*intrinsic, intrinsicsName, intrinsicsName);
    requisito.saveMatrixXML(*intrinsicStdDev, "IntrinsicsStdDev", intrinsicsName, true);
    requisito.saveMatrixXML(*distortion, distortionName, distortionName);
    requisito.saveMatrixXML(*distortionStdDev, "DistortionStdDev", distortionName, true);
}

int main(int argc, char **argv)
{
    cv::VideoCapture video(0);
    if (!video.isOpened()) {
        std::cout << "[ERROR] Could not open camera." << std::endl;
        return -1;
    }

    bool calibrateFlag = true;
    if (argc > 2) {
        if (argv[1] == "--just-undistort-it") {
            calibrateFlag = false;
        }
    }

    // Project Objects
    cv::Mat* rawImage = new cv::Mat();
    cv::Mat* undistortedImage = new cv::Mat();
    video >> *rawImage;
    Projeto2 requisito2Raw(rawImage, RAW_WINDOW_NAME);
    Projeto2 requisito2Undistorted(undistortedImage, UNDISTORTED_WINDOW_NAME, cv::Scalar(0, 255, 0));

    // Start Calibration the desired number of times
    if (calibrateFlag) {
        // Calibrator
        CameraCalibrator calibrator(&video);

        // Start to Calibrate
        for (int index = 1; index <= NUMBER_CALIBRATION; index++) {
            std::cout << "Starting Calibration" << index << " ...." << std::endl;

            // Calibrate
            double rms = 0.0;
            if (calibrator.calibrateCamera(rms) != 0) {
                std::cout << "[FATAL ERROR] Calibration " << index << " could not be finished!" << std::endl;
                return -1;
            }
            std::cout << "Calibration RMS: " << rms << std::endl;

            // Save Matrices
            std::cout << "Saving Camera Parameters" << std::endl;
            std::string intrinsicsName("Intrinsics");
            std::string distortionName("Distortion");
            std::string intrinsicsFileName(intrinsicsName);
            std::string distortionFileName(distortionName);
            intrinsicsFileName.append(std::to_string(index));
            distortionFileName.append(std::to_string(index));
            requisito2Raw.saveMatrixXML(calibrator.intrinsics, intrinsicsName, intrinsicsFileName);
            requisito2Raw.saveMatrixXML(calibrator.distortions,distortionName, distortionFileName);

            // Start Windows
            cv::namedWindow(RAW_WINDOW_NAME);
            cv::namedWindow(UNDISTORTED_WINDOW_NAME);

            while (true) {
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
                    std::cout << "[MESSAGE] Calibration " << index <<  " Continued by the User!" << std::endl;
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
            }

            // Destroy Windows
            cv::destroyAllWindows();
        }
    } else {
        // Load Matrices
        std::cout << "Loading Camera Parameters" << std::endl;

        // Calculates the mean and standard deviation of a group of Intrinsics and Dist Matrices
        cv::Mat intrinsic, intrinsicStdDev, distortion, distortionStdDev;
        calculateMeanStdDevCameraParametersAndSave(requisito2Raw, &intrinsic, &intrinsicStdDev, &distortion, &distortionStdDev);

        // Calibrator
        CameraCalibrator calibrator(intrinsic, distortion, requisito2Raw.image->size());

        // Start Windows
        cv::namedWindow(RAW_WINDOW_NAME);
        cv::namedWindow(UNDISTORTED_WINDOW_NAME);
        while (true) {
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
                std::cout << "[MESSAGE] Finish Program!" << std::endl;
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
        }
        // Destroy Windows
        cv::destroyAllWindows();
    }

    return 0;
}
