/*
Claire Liu, Yu-Jing Wei
calibrator.hpp

Path: include/calibrator.hpp
Description: Declares the calibrator functions for the augmented reality application.
*/

#pragma once // Include guard
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>

class Calibrator
{
private:
    // Constants for calibration
    const int MAX_SAMPLES = 20;
    const int MIN_SAMPLES = 5;
    const float MARKER_LENGTH = 1.0f; // Assuming marker size is 1.0 unit for object points

    // For ArUco marker detection
    cv::aruco::Dictionary dictionary;
    cv::aruco::DetectorParameters detectorParams;
    cv::aruco::ArucoDetector detector;

    // Storage for calibrator data
    std::vector<std::vector<cv::Point2f>> corner_list;
    std::vector<std::vector<cv::Point3f>> point_list;

    cv::Size imageSize; // to store the size of the images for calibration

    void drawMarkerCorners(cv::Mat &frame,
                           const std::vector<std::vector<cv::Point2f>> &corners);

public:
    Calibrator(); // Initializes the ArUco detector with the specified dictionary and parameters

    int detectAndDraw(cv::Mat &frame,
                      std::vector<int> &ids,
                      std::vector<std::vector<cv::Point2f>> &corners);
    int getSampleCount() const { return corner_list.size(); }
    void saveCalibrationData(const std::vector<int> &ids,
                             const std::vector<std::vector<cv::Point2f>> &corners);
    static std::vector<cv::Point3f> getMarkerObjectPoints(float marker_size);
    double calibrate(cv::Mat &cameraMatrix,
                     cv::Mat &distCoeffs);
    bool saveParameters(const std::string &filename,
                        const cv::Mat &cameraMatrix,
                        const cv::Mat &distCoeffs,
                        double rms,
                        const std::string &ts);
    std::string getCurrentTimestamp();
    bool saveCurrentFrame(const cv::Mat &frame,
                          const std::string &folder,
                          const std::string &prefix,
                          const std::string &ts);
};