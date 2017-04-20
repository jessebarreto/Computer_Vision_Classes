#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <utility>


#ifndef PROJETO2_H
#define PROJETO2_H

class Projeto2
{
public:
    // Image Raw Pointer
    cv::Mat *image;

    // Grid Size
    int gridSize;

    // Init Flag
    bool initFlag;

    // Lines
    std::vector<std::pair<cv::Point*, cv::Point*> > lines;

    // Lines Color
    cv ::Scalar lineColor;

    // Window Name
    std::string windowName;

    Projeto2(cv::Mat *imagePtr,
             const std::string &_windowName,
             const cv::Scalar& cor = cv::Scalar(0, 0, 255));

    ~Projeto2();

    // Update this Window
    void updateWindow();

    // Draw lines on image
    void drawLines();

    // Draw grid on image
    void drawGrid();

    // Save a Matrix to XML
    void saveMatrixXML(const cv::Mat &matrix, const std::string &matrixName, const std::string &fileName,
                       bool appendFlag = false);

    // Load a Matrix from XML
    void loadMatrixXML(cv::Mat &matrix, const std::string &matrixName, const std::string &fileName);

    // Save Lines to XML
    void saveLinesXML(const std::string &fileName, const int &distance = 15, bool flagAppend = false);

    // Clear Lines
    void clearLines();
};

class CameraCalibrator
{
public:
    // Video Camera
    cv::VideoCapture* videoCam;

    // Board Dimensions
    cv::Size boardSize;

    // Number of Frames to be Added at Calibration
    int boardNumber;

    // Number of Frames to Skip
    int frameStep;

    // Size of Board's Squares in mm
    double squareSize;

    //Note:
    //Intrinsic Matrix - 3x3			   Lens Distorstion Matrix - 4x1
    //	[fx 0 cx]							[k1 k2 p1 p2   k3(optional)]
    //	[0 fy cy]
    //	[0  0  1]
    cv::Mat intrinsics;
    cv::Mat distortions;

    // Rectify transformations
    cv::Mat mapx;
    cv::Mat mapy;

    // Video Resolution
    cv::Size cameraResolution;

    // Corners from board
    std::vector<cv::Point2f> imageCornersPoints;
    std::vector<cv::Point3f> objectCornerPoints;

    // Vector of Corners from Different Frames
    std::vector<std::vector<cv::Point2f>> imagesCornersPoints;
    std::vector<std::vector<cv::Point3f>> objectsCornersPoints;


    CameraCalibrator(cv::VideoCapture *video, const int &boardNumber = 25, const int &_frameStep = 25,
                     const cv::Size &boardDimensions = cv::Size(8,6), const cv::Size &camRes = cv::Size(640, 480));

    CameraCalibrator(const cv::Mat& intrinsicsMatrix, const cv::Mat& distortionMatrix,
                     const cv::Size &camRes = cv::Size(640,480));

    ~CameraCalibrator();

    // Calibrate the Camera
    int calibrateCamera(double &rms);

    // Converts the image points to object points in 3D
    void calculateBoardCornerPositions(std::vector<cv::Point3f> &objectCorners);

    // Undistorts A Image From the Camera
    void undistortImageFromCamera(const cv::Mat &rawImage, cv::Mat &undistortedImage);

    // Calculates the Rectify Transformations
    void calculateUndistortedMap();
};

Projeto2::Projeto2(cv::Mat *imagePtr, const std::string &_windowName, const cv::Scalar& cor)
{
    if (!imagePtr || _windowName.empty()) {
        std::cout << "\033[1;31m [ERROR] EXITING PROGRAM... \033[0m" << __LINE__ << " " << __FILE__ << std::endl;
        throw std::exception();
    }
    image = imagePtr;
    windowName = _windowName;
    lineColor = cor;
    initFlag = true;
    gridSize = 3;
}

Projeto2::~Projeto2()
{
    clearLines();

    if (image != NULL) {
        delete image;
    }
}

void Projeto2::clearLines()
{
    initFlag = true;
    for (std::pair<cv::Point*, cv::Point*> ptPair : this->lines) {
        if (ptPair.first)
            delete ptPair.first;

        if (ptPair.second)
            delete ptPair.second;
    }
    this->lines.clear();
}

void Projeto2::updateWindow()
{
    cv::imshow(windowName, *image);
}

void Projeto2::drawLines()
{
    for (std::pair<cv::Point*, cv::Point*> ptPair : lines) {
        if (ptPair.first != NULL && ptPair.second != NULL)
            cv::line(*image, *ptPair.first, *ptPair.second, lineColor, 2);
    }
}

void Projeto2::drawGrid()
{
    for (int i = 0; i <= gridSize; i++) {
        int x = i * image->cols / gridSize;
        int y = i * image->rows / gridSize;
        cv::line(*image, cv::Point(x, 0),
               cv::Point(x, image->rows),
               cv::Scalar(255,255,255), 2);
        cv::line(*image, cv::Point(0, y),
               cv::Point(image->cols, y),
               cv::Scalar(255,255,255), 2);
    }

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int y = i * image->rows / gridSize + image->rows / (2 * gridSize);
            int x = j * image->cols / gridSize + image->cols / (2 * gridSize);
            cv::circle(*image, cv::Point(x,y), 5, cv::Scalar(0,0,255));
        }
    }
}

void Projeto2::saveMatrixXML(const cv::Mat &matrix, const std::string &matrixName, const std::string &fileName, bool appendFlag)
{
    std::string fileNameExtension = fileName;
    fileNameExtension.append(".xml");
    cv::FileStorage *file;
    if (!appendFlag)
        file = new cv::FileStorage(fileNameExtension, cv::FileStorage::WRITE);
    else
        file = new cv::FileStorage(fileNameExtension, cv::FileStorage::APPEND);

    if (!file->isOpened())
        return;

    *file << matrixName << matrix;
    file->release();

    std::cout << "Matrix: " << matrixName << " saved!" << std::endl;
    delete file;
}

void Projeto2::loadMatrixXML(cv::Mat &matrix, const std::string &matrixName, const std::string &fileName)
{
    std::string fileNameExtension = fileName;
    fileNameExtension.append(".xml");
    cv::FileStorage file(fileNameExtension, cv::FileStorage::READ);
    if (!file.isOpened())
        return;

    file[matrixName.c_str()] >> matrix;

    file.release();

    std::cout << "Matrix: " << matrixName << " loaded!" << std::endl;
}

void Projeto2::saveLinesXML(const std::string &fileName, const int &distance, bool flagAppend)
{
    std::string fileNameExtension = fileName;
    fileNameExtension.append(std::to_string(distance));
    fileNameExtension.append(".xml");
    cv::FileStorage file(fileNameExtension, flagAppend ? cv::FileStorage::APPEND : cv::FileStorage::WRITE);
    if (!file.isOpened())
        return;

    file << "Distance" << distance;

    cv::Mat pointsInitAsMat(2, lines.size(), CV_32F);
    cv::Mat pointsEndAsMat(2, lines.size(), CV_32F);
    cv::Mat lenghtsAsMat(1, lines.size(), CV_32F);
    for (unsigned int i = 0; i < lines.size(); i++) {
        std::pair<cv::Point*, cv::Point*> &line = lines.at(i);
        if (line.first == NULL || line.second == NULL)
            continue;

        pointsInitAsMat.at<float>(0, static_cast<int>(i)) = line.first->x;
        pointsInitAsMat.at<float>(1, static_cast<int>(i)) = line.first->y;
        pointsEndAsMat.at<float>(0, static_cast<int>(i)) = line.second->x;
        pointsEndAsMat.at<float>(1, static_cast<int>(i)) = line.second->y;
        lenghtsAsMat.at<float>(0, static_cast<int>(i)) = cv::norm(*line.first - *line.second);
    }

    file << "PointsInitAsMatrix" << pointsInitAsMat;
    file << "PointsEndAsMatrix" << pointsEndAsMat;
    file << "LengthAsMatrix" << lenghtsAsMat;

    file.release();

    std::cout << "Lines: " << fileName  << " saved!" << std::endl;
}

CameraCalibrator::CameraCalibrator(cv::VideoCapture* video, const int &_boardNumber, const int &_frameStep,
                                   const cv::Size& boardDimensions, const cv::Size &camRes) :
    videoCam(video),
    boardSize(boardDimensions),
    boardNumber(_boardNumber),
    frameStep(_frameStep),
    squareSize(2.8),
    intrinsics(cv::Mat::eye(3, 3, CV_64FC1)),
    distortions(cv::Mat::zeros(4, 1, CV_64FC1)),
    mapx(cv::Mat::eye(camRes, CV_64FC1)),
    mapy(cv::Mat::eye(camRes, CV_64FC1)),
    cameraResolution(camRes)
{

}

CameraCalibrator::CameraCalibrator(const cv::Mat &intrinsicsMatrix, const cv::Mat &distortionMatrix,
                                   const cv::Size &camRes) :
    videoCam(NULL),
    intrinsics(intrinsicsMatrix),
    distortions(distortionMatrix),
    cameraResolution(camRes)
{
    calculateUndistortedMap();
}

CameraCalibrator::~CameraCalibrator()
{
    videoCam = NULL;

}

int CameraCalibrator::calibrateCamera(double &rms)
{
    cv::Mat frame, frameGray;

    int sucesses = 0;
    int frameCounter = 0;

    std::string snapWindowName("Snapshots Video");
    std::string rawWindowName("Raw Video");

    cv::namedWindow(snapWindowName);
    cv::namedWindow(rawWindowName);

    std::cout << "Start Capturing Images to Calibrate..." << std::endl;
    std::string statusMessageBegin("Capturing Images to Calibrate \nStatus: ");

    while (sucesses < boardNumber) {
        if (!videoCam->isOpened()) {
            std::cout << "[ERROR] Could not open camera." << std::endl;
            return -1;
        }

        *videoCam >> frame;
        if (frame.empty()) {
            std::cout << "[ERROR] Camera Disconnected!";
            cv::destroyAllWindows();
            return -1;
        }

        if (cv::waitKey(30) >= 0) {
            std::cout << "[WARNING] Camera Calibration Stopped by User!" << std::endl;
            cv::destroyAllWindows();
            return -2;
        }

        if ((frameCounter++ % frameStep) == 0) {
            bool found = cv::   findChessboardCorners( frame, boardSize, imageCornersPoints,
                                                CV_CALIB_CB_ADAPTIVE_THRESH |
                                                CV_CALIB_CB_FAST_CHECK |
                                                CV_CALIB_CB_NORMALIZE_IMAGE);
            cv::drawChessboardCorners(frame, boardSize, cv::Mat(imageCornersPoints), found);
            std::cout << "Finding Chessboard " << found << std::endl;
            cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
            if (found) {
                cv::imshow(snapWindowName, frame);
                cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
                cv::cornerSubPix(frameGray, imageCornersPoints, cv::Size(11,11), cv::Size(-1,-1),
                                 cv::TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
                if (static_cast<int>(imageCornersPoints.size()) == (boardSize.width * boardSize.height))
                {
                    calculateBoardCornerPositions(objectCornerPoints);
                    imagesCornersPoints.push_back(imageCornersPoints);
                    objectsCornersPoints.push_back(objectCornerPoints);
                    sucesses++;
                    cv::imshow(snapWindowName, frame);
                }
            } else {
                cv::imshow(snapWindowName, frameGray);
            }
            std::cout << statusMessageBegin << sucesses << " / " << boardNumber << std::endl;
        }

        cv::imshow(rawWindowName, frame);
    }
    cv::destroyAllWindows();

    std::cout << "Start Calculating Camera Parameters...\n" << std::endl;

    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;
    cameraResolution = frame.size();
    rms = cv::calibrateCamera(objectsCornersPoints, imagesCornersPoints, cameraResolution, intrinsics,
                              distortions, rvecs, tvecs, CV_CALIB_FIX_K5 );

    std::cout << "Calibration Finished! Re-projection error: " << rms << std::endl;
    calculateUndistortedMap();
    return 0;
}

void CameraCalibrator::calculateBoardCornerPositions(std::vector<cv::Point3f>& objectCorners)
{
    objectCorners.clear();
    for( int i = 0; i < boardSize.height; ++i )
        for( int j = 0; j < boardSize.width; ++j )
            objectCorners.push_back(cv::Point3f(float( j*squareSize ), float( i*squareSize ), 0));
}

void CameraCalibrator::calculateUndistortedMap()
{
    cv::initUndistortRectifyMap(intrinsics, distortions, cv::Mat(),
                                cv::getOptimalNewCameraMatrix(intrinsics, distortions, cameraResolution, 1, cameraResolution, 0),
                                cameraResolution, CV_16SC2, mapx, mapy);
}

void CameraCalibrator::undistortImageFromCamera(const cv::Mat &rawImage, cv::Mat &undistortedImage)
{
    cv::remap(rawImage, undistortedImage, mapx, mapy, cv::INTER_LINEAR);
}


#endif // PROJETO2_H
