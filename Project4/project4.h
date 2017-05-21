#ifndef PROJECT4_H
#define PROJECT4_H

#include <opencv2/opencv.hpp>

class Project4
{
    // Windows Names
    const std::string _sobelName;
    const std::string _laplaceName;
    const std::string _cannyName;
    const std::string _gtName;
    const std::string _originalName;

    // Default folder names
    const std::string _defaultImagesSrcsDir;
    const std::string _defaultImagesDstsDir;
    const std::string _defaultImagesGtDir;

    // Default Images Names
    const std::string _defaultImage1Name;
    const std::string _defaultImage2Name;
    const std::string _defaultImage3Name;
    const std::string _defaultImage4Name;
    const std::string _defaultImage5Name;
    const std::string _defaultImage6Name;

    // Image Formats
    const std::string _GifFormatName;
    const std::string _PngFormatName;
    const std::string _JpgFormatName;
    const std::string _JpegFormatName;

    // Lists of Source Image
    std::vector<cv::Mat> _sourceImages;

    // Lists of Result Images
    std::vector<cv::Mat> _sobelImages, _laplaceImages, _cannyImages, _groundTruthImages;

    // Binary Threshold value
    int _thresholdValue;

    // Sobel Parameters
    int _sobelWindow; // Size of Sobel Window

    // Canny Parameters
    int _cannyWindow; // Size of Canny Window
    int _cannyLowThreshold; // Canny Low Threshold
    int _cannyHighThreshold; // Canny High Threshold

    // Laplace Parameters
    int _laplaceWindow; // Size of Laplace Window

public:
    // List of Scores
    cv::Mat _scores;

    // Constructor
    Project4(int thresholdValue);

    // Window Name Enumerate
    enum Image {
        Sobel = 0,
        Laplace,
        Canny,
        GroundTruth,
        Original
    };

    // Images Formats Enumerate
    enum ImageFormat {
        GIF = 0,
        PNG,
        JPG,
        JPEG
    };

    // Return window name
    const std::string &getImageName(Image window);

    // Return default image name
    const std::string &getDefaultImageName(unsigned imageNumber);

    // Return image format name
    const std::string &getImageFormatName(ImageFormat imageFormat);

    // Return default images directory name
    const std::string &getDefaultImagesSrcsDirName();

    // Return default results directory name
    const std::string &getDefaultImagesDstsDirName();

    // Return default ground truth images directory name
    const std::string &getDefaultImagesGtDirName();

    // Calculate borders and apply binarization
    void calculateBorders(unsigned sourceNumber);

    // Return original image
    cv::Mat& getOriginal(unsigned sourceNumber);

    // Return sobel image grayscale binary
    cv::Mat& getSobel(unsigned sourceNumber);

    // Return canny image binary grayscale binary
    cv::Mat& getCanny(unsigned sourceNumber);

    // Return laplace image grayscale binary
    cv::Mat& getLaplace(unsigned sourceNumber);

    // Return ground truth image grayscale binary
    cv::Mat& getGroundTruth(unsigned sourceNumber);

    // Sets the source image
    void setSourceImage(cv::Mat &srcImage);

    // Sets the Ground Truth image
    void setGroundTruthImage(cv::Mat &groundTruthImage);

    // Sets the Sobel parameters
    void setSobelParameters(int sobelWindowSize);

    // Sets the Canny parameters
    void setCannyParameters(int cannyWindowSize, int cannyLowThreshold, int cannyHighThreshold);

    // Sets the Laplace parameters
    void setLaplaceParameters(int laplaceWindowSize);

    // Calculates Score
    void calculateScore(unsigned sourceNumber);

    // Return the scores
    cv::Mat &getScores();

    // Return the type of Matrix
    std::string matType2String(int type);

    // Save a matrix in a XML File
    void saveMatrixXML(const cv::Mat &matrix, const std::string &matrixName, const std::string &fileName,
                       bool appendFlag);
};

const std::string &Project4::getImageName(Project4::Image window)
{
    switch (window) {
    case Sobel:
        return _sobelName;
        break;
    case Laplace:
        return _laplaceName;
        break;
    case Canny:
        return _cannyName;
        break;
    case GroundTruth:
        return _gtName;
        break;
    case Original:
    default:
        return _originalName;
        break;
    }
}

const std::string &Project4::getDefaultImageName(unsigned imageNumber)
{
    switch (imageNumber) {
    case 0:
        return _defaultImage1Name;
        break;
    case 1:
        return _defaultImage2Name;
        break;
    case 2:
        return _defaultImage3Name;
        break;
    case 3:
        return _defaultImage4Name;
        break;
    case 4:
        return _defaultImage5Name;
        break;
    case 5:
    default:
        return _defaultImage6Name;
        break;
    }
}

const std::string &Project4::getImageFormatName(Project4::ImageFormat imageFormat)
{
    switch (imageFormat) {
    case GIF:
        return _GifFormatName;
        break;
    case JPG:
        return _JpgFormatName;
        break;
    case JPEG:
        return _JpegFormatName;
        break;
    case PNG:
        return _PngFormatName;
        break;
    default:
        return _PngFormatName;
        break;
    }
}

const std::string &Project4::getDefaultImagesDstsDirName()
{
    return _defaultImagesDstsDir;
}

const std::string &Project4::getDefaultImagesGtDirName()
{
    return _defaultImagesGtDir;
}

const std::string &Project4::getDefaultImagesSrcsDirName()
{
    return _defaultImagesSrcsDir;
}

Project4::Project4(int thresholdValue) :
    _sobelName("Sobel"),
    _laplaceName("Laplace"),
    _cannyName("Canny"),
    _gtName("gt"),
    _originalName("Original"),
    _defaultImagesSrcsDir("./edges-images/srcs/"),
    _defaultImagesDstsDir("./edges-images/dsts/"),
    _defaultImagesGtDir("./edges-images/gt/"),
    _defaultImage1Name("46"),
    _defaultImage2Name("140"),
    _defaultImage3Name("208"),
    _defaultImage4Name("212"),
    _defaultImage5Name("217"),
    _defaultImage6Name("221"),
    _GifFormatName(".gif"),
    _PngFormatName(".png"),
    _JpgFormatName(".jpg"),
    _JpegFormatName(".jpeg")
{
    // Binary Threshold
    _thresholdValue = thresholdValue;

    // Sobel Default Parameters
    _sobelWindow = 3; // 3x3 Window

    // Canny Default Parameters
    _cannyWindow = 3; // 3x3 Window
    _cannyLowThreshold = 100; // Low Threshold
    _cannyHighThreshold = 200; // High Threshold

    // Laplace Default Parameters
    _laplaceWindow = 3; // 3x3 Window

    // Fill Scores
    _scores = cv::Mat::zeros(3, 6, CV_32F);
}

void Project4::calculateBorders(unsigned sourceNumber)
{
    cv::Mat gradX, gradY, absGradX, absGradY, absSobel;
    cv::Mat canny;
    cv::Mat laplace, absLaplace;

    // Calculate Sobel Borders
    cv::Sobel(_sourceImages.at(sourceNumber), gradX, CV_16S, 1, 0, _sobelWindow); // Sobel in X with Window 3x3
    cv::convertScaleAbs(gradX, absGradX); // Convert Grad X Values to Absolute
    cv::Sobel(_sourceImages.at(sourceNumber), gradY, CV_16S, 0, 1, _sobelWindow); // Sobel in X with Window 3x3
    cv::convertScaleAbs(gradY, absGradY); // Convert Grad X Values to Absolute
    cv::addWeighted( absGradX, 0.5, absGradY, 0.5, 0, absSobel); // Sum up both gradients
    cv::threshold(absSobel, absSobel, _thresholdValue, 0xFF,CV_THRESH_BINARY_INV);
    _sobelImages.push_back(absSobel);

    // Calculate Canny Borders
    cv::Canny(_sourceImages.at(sourceNumber), canny, _cannyLowThreshold, _cannyHighThreshold, _cannyWindow);
    cv::threshold(canny, canny, _thresholdValue, 0xFF,CV_THRESH_BINARY_INV);
    _cannyImages.push_back(canny);

    // Calculate Laplace Borders
    cv::Laplacian(_sourceImages.at(sourceNumber), laplace, CV_16S, _laplaceWindow);
    convertScaleAbs(laplace, absLaplace);
    cv::threshold(absLaplace, absLaplace, _thresholdValue, 0xFF,CV_THRESH_BINARY_INV);
    _laplaceImages.push_back(absLaplace);
}

cv::Mat &Project4::getOriginal(unsigned sourceNumber)
{
    return _sourceImages.at(sourceNumber);
}

cv::Mat &Project4::getSobel(unsigned sourceNumber)
{
    return _sobelImages.at(sourceNumber);
}

cv::Mat &Project4::getCanny(unsigned sourceNumber)
{
    return _cannyImages.at(sourceNumber);
}

cv::Mat &Project4::getLaplace(unsigned sourceNumber)
{
    return _laplaceImages.at(sourceNumber);
}

cv::Mat &Project4::getGroundTruth(unsigned sourceNumber)
{
    return _groundTruthImages.at(sourceNumber);
}

void Project4::setSourceImage(cv::Mat &srcImage)
{
    if (srcImage.empty()) {
        std::cout << "[Critical Error] Source Image is Empty!" << std::endl;
        throw;
    }

    cv::Mat source;
    cv::cvtColor(srcImage, source, CV_BGR2GRAY);

    _sourceImages.push_back(source);
}

void Project4::setGroundTruthImage(cv::Mat &groundTruthImage)
{
    if (groundTruthImage.empty()) {
        std::cout << "[Critical Error] Ground Truth Image is Empty!" << std::endl;
        throw;
    }

    cv::Mat source;
    cv::cvtColor(groundTruthImage, source, CV_BGR2GRAY);
    _groundTruthImages.push_back(source);
}

void Project4::setSobelParameters(int sobelWindowSize)
{
    _sobelWindow = sobelWindowSize;
}

void Project4::setCannyParameters(int cannyWindowSize, int cannyLowThreshold, int cannyHighThreshold)
{
    _cannyWindow = cannyWindowSize;
    _cannyLowThreshold = cannyLowThreshold;
    _cannyHighThreshold = cannyHighThreshold;
}

void Project4::setLaplaceParameters(int laplaceWindowSize)
{
    _laplaceWindow = laplaceWindowSize;
}

void Project4::calculateScore(unsigned sourceNumber)
{
    cv::Mat andRes, xorRes;
    float hitCounter, missCounter, score;

    // Sobel
    cv::bitwise_and(_sobelImages.at(sourceNumber), _groundTruthImages.at(sourceNumber), andRes);
    cv::bitwise_xor(_sobelImages.at(sourceNumber), _groundTruthImages.at(sourceNumber), xorRes);
    hitCounter = static_cast<float>(cv::sum(andRes)[0]) / 255; // Max Pixel value
    missCounter = static_cast<float>(cv::sum(xorRes)[0]) / 255; // Max Pixel value
    score = hitCounter / (hitCounter + missCounter);
    _scores.at<float>(0, static_cast<int>(sourceNumber)) = score * 100.0;

    // Canny
    cv::bitwise_and(_cannyImages.at(sourceNumber), _groundTruthImages.at(sourceNumber), andRes);
    cv::bitwise_xor(_cannyImages.at(sourceNumber), _groundTruthImages.at(sourceNumber), xorRes);
    hitCounter = static_cast<float>(cv::sum(andRes)[0]) / 255; // Max Pixel value
    missCounter = static_cast<float>(cv::sum(xorRes)[0]) / 255; // Max Pixel value
    score = hitCounter / (hitCounter + missCounter);
    _scores.at<float>(1, static_cast<int>(sourceNumber)) = score * 100.0;

    // Laplace
    cv::bitwise_and(_laplaceImages.at(sourceNumber), _groundTruthImages.at(sourceNumber), andRes);
    cv::bitwise_xor(_laplaceImages.at(sourceNumber), _groundTruthImages.at(sourceNumber), xorRes);
    hitCounter = static_cast<float>(cv::sum(andRes)[0]) / 255; // Max Pixel value
    missCounter = static_cast<float>(cv::sum(xorRes)[0]) / 255; // Max Pixel value
    score = hitCounter / (hitCounter + missCounter);
    _scores.at<float>(2, static_cast<int>(sourceNumber)) = score * 100.0;
}

cv::Mat &Project4::getScores()
{
    return _scores;
}

std::string Project4::matType2String(int type)
{
    std::string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
    }

    r += "C";
    r += (chans+'0');

    return r;
}

void Project4::saveMatrixXML(const cv::Mat &matrix, const std::string &matrixName, const std::string &fileName, bool appendFlag)
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

#endif // PROJECT4_H
