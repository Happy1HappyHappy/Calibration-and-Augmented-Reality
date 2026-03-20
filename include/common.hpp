#pragma once
#include <opencv2/opencv.hpp>

struct CameraPose {
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Mat rvec;
    cv::Mat tvec;
};
