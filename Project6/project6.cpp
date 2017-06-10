#include "project6.h"

Project6::Project6(cv::VideoCapture *videoStream) :
    _videoStream(videoStream),
    _isSurfInit(false),
    _isTemplateInit(false),
    _isHarrisInit(false)
{
    // Set Windows Names
    _windowsNames.push_back(std::string("Original Frame Window"));
    _windowsNames.push_back(std::string("Template Image Window"));
    _windowsNames.push_back(std::string("Surf Matches Image Window"));
    _windowsNames.push_back(std::string("Harris Matches Image Window"));
}

void Project6::setSurfDetector(double surfHessianThreshold)
{
    _surfDetector = cv::SURF(surfHessianThreshold);
    _isSurfInit = true;
}

void Project6::setHarrisDetector(double parameter, double threshold,
                                 double maxStrength, int aperture, int neighboursSize, int nonMaxSize)
{
    _harrisDetector = HarrisDetector(parameter, threshold, maxStrength, aperture, neighboursSize, nonMaxSize);
    _isHarrisInit = true;
}

void Project6::setTemplate()
{
    MouseHandler handler(&_frame);
    cv::namedWindow(_windowsNames.at(0), CV_WINDOW_NORMAL);
    cv::setMouseCallback(_windowsNames.at(0), handler.run, &handler);

    // Loop
    while(hasNextFrame()) {
        // Update Windows
        handler.drawRectangle(_frame);
        cv::imshow(_windowsNames.at(0), _frame);

        cv::waitKey(30);
        if (handler.isReady()) {
            break;
        }
    }
    cv::destroyAllWindows();

    handler.getTemplateImage().copyTo(_template);
    cv::cvtColor(_template, _grayTemplate, CV_BGR2GRAY, CV_8UC1);
    _isTemplateInit = true;
}

bool Project6::isInit()
{
    return _isHarrisInit && _isSurfInit && _isTemplateInit;
}

bool Project6::trackTemplate(int numberOfKeypoints)
{
    bool ret = true;
    if (isInit()) {
        // Keypoints
        std::vector<cv::KeyPoint> surfKeypointsTemplate, harrisKeypointsTemplate;
        cv::Mat surfDescriptionsTemplate, harrisDescriptionstemplate;

        // Surf in Template
        _surfDetector.detect(_template, surfKeypointsTemplate);
        if (surfKeypointsTemplate.size() > numberOfKeypoints) {
            clearKeyPoints(surfKeypointsTemplate, numberOfKeypoints);
        }
        _surfExtractor.compute(_template, surfKeypointsTemplate, surfDescriptionsTemplate);

        //Harris in Template
        _harrisDetector.detect(_grayTemplate, harrisKeypointsTemplate);
        if (harrisKeypointsTemplate.size() > 2*numberOfKeypoints) {
            clearKeyPoints(harrisKeypointsTemplate, 2*numberOfKeypoints);
        }
        _surfExtractor.compute(_grayTemplate, harrisKeypointsTemplate, harrisDescriptionstemplate);

        // Matches Count
        double avgSurfMatches = 0.0, avgHarrisMatches = 0.0, avgSurfGoodMatches = 0.0, avgHarrisGoodMatches = 0.0;
        long long totalLoop = 1;

        // Start Loop
        cv::Mat surfDescriptionsFrame, harrisDescriptionsFrame;
        cv::namedWindow(_windowsNames.at(0), CV_WINDOW_NORMAL);
        cv::namedWindow(_windowsNames.at(1), CV_WINDOW_NORMAL);
        cv::namedWindow(_windowsNames.at(2), CV_WINDOW_NORMAL);
        cv::namedWindow(_windowsNames.at(3), CV_WINDOW_NORMAL);
        while(hasNextFrame()) {
            std::vector<cv::KeyPoint> surfKeypointsFrame, harrisKeypointsFrame;
            // Surf in Frame
            _surfDetector.detect(_frame, surfKeypointsFrame);
            if (surfKeypointsFrame.size() > numberOfKeypoints) {
                clearKeyPoints(surfKeypointsFrame, numberOfKeypoints);
            }
            _surfExtractor.compute(_frame, surfKeypointsFrame, surfDescriptionsFrame);

            //Harris in Frame
            _harrisDetector.detect(_grayFrame, harrisKeypointsFrame);
            if (harrisKeypointsFrame.size() > 2*numberOfKeypoints) {
                clearKeyPoints(harrisKeypointsFrame, 2*numberOfKeypoints);
            }
            _surfExtractor.compute(_grayFrame, harrisKeypointsFrame, harrisDescriptionsFrame);

            // Match Keypoints
            _flannMatcher.match(surfDescriptionsTemplate, surfDescriptionsFrame, _surfMatches);
            _flannMatcher.match(harrisDescriptionstemplate, harrisDescriptionsFrame, _harrisMatches);

            avgSurfMatches += static_cast<double>(_surfMatches.size());
            avgHarrisMatches += static_cast<double>(_harrisMatches.size());

            // Find Good Matches
            findGoodMatches(std::string("Surf"), _surfMatches, surfDescriptionsTemplate);
//            findGoodMatches(std::string("Harris"), _harrisMatches, harrisDescriptionstemplate);

            avgSurfGoodMatches += static_cast<double>(_surfMatches.size());
            avgHarrisGoodMatches += static_cast<double>(_harrisMatches.size());

            // Draw Matches Surf
            if (surfKeypointsTemplate.size() > 2 && surfKeypointsFrame.size() > 2) {
                cv::drawMatches(_template, surfKeypointsTemplate, _frame, surfKeypointsFrame, _surfMatches,
                                _surfMatchesImage);
            }

            // Draw Matches Harris
            if (harrisKeypointsTemplate.size() > 2 && harrisKeypointsFrame.size() > 2) {
                cv::drawMatches(_grayTemplate, harrisKeypointsTemplate, _grayFrame, harrisKeypointsFrame, _harrisMatches,
                            _harrisMatchesImage);
            }

            _harrisMatches.clear();
            _surfMatches.clear();
            // Update Windows
            cv::imshow(_windowsNames.at(0), _frame);
            cv::imshow(_windowsNames.at(1), _template);
            cv::imshow(_windowsNames.at(2), _surfMatchesImage);
            cv::imshow(_windowsNames.at(3), _harrisMatchesImage);
    
            char key = cv::waitKey(30);
            if (key == ASCII_EXIT_CHAR) {
                ret = false;
                break;
            } else if (key == 32) {
                break;
            }

            totalLoop++;
        }
        cv::destroyAllWindows();

        avgSurfMatches /= totalLoop;
        avgHarrisMatches /= totalLoop;
        avgSurfGoodMatches /= totalLoop;
        avgHarrisGoodMatches /= totalLoop;

        std::cout << "Harris-" << numberOfKeypoints << " Avg Matches: " << avgHarrisMatches << " Avg Good Matches: "
                  << avgHarrisGoodMatches << std::endl;
        std::cout << "Surf-" << numberOfKeypoints << " Avg Matches: " << avgSurfMatches << " Avg Good Matches: "
                  << avgSurfGoodMatches << std::endl;
        std::string fileName("surfGoodMatches" + std::to_string(numberOfKeypoints) + ".bmp");
        cv::imwrite(fileName, _surfMatchesImage);
        fileName = std::string("harrisGoodMatches" + std::to_string(numberOfKeypoints) + ".bmp");
        cv::imwrite(fileName, _harrisMatchesImage);


    } else {
        ret = false;
    }
    return ret;
}

bool Project6::hasNextFrame()
{
    // Get next frame
    *_videoStream >> _frame;
    cv::cvtColor(_frame, _grayFrame, CV_BGR2GRAY, CV_8UC1);

    return !_frame.empty();
}

void Project6::clearKeyPoints(std::vector<cv::KeyPoint> &keypoints, int numberOfElements)
{
    if (numberOfElements <= 1 || keypoints.size() < numberOfElements) {
        return;
    }

    // remove all elements after numberOf Elements
    keypoints.erase(keypoints.begin()+numberOfElements, keypoints.end());
}

void Project6::findGoodMatches(const std::string &msg, std::vector<cv::DMatch> &matches, const cv::Mat &descriptors)
{
    std::vector< cv::DMatch > goodMatches;

    double max = 0, min = std::numeric_limits<double>::max();
    for(int i = 0; i < descriptors.rows; ++i){
        double dist = matches[i].distance;
        min = dist < min ? dist : min;
        max = dist > max ? dist : max;
    }

    for(int i = 0; i < descriptors.rows; ++i){
        double dist = matches[i].distance;
        if (dist < 3 * min) {
            goodMatches.push_back(matches[i]);
        }
    }

    matches.clear();
    matches = goodMatches;
}
