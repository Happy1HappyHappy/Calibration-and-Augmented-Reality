/*
Claire Liu, Yu-Jing Wei
ARApp.hpp

Path: include/ARApp.hpp
Description: Declares the ARApp class for the augmented reality application.
*/

#pragma once // Include guard

#include "calibrator.hpp"
#include "featureDetector.hpp"
#include "virtualObjectProjector.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>

class ARApp
{
private:
    Calibrator calib;                               // Calibrator object for handling marker detection and camera calibration
    VirtualObjectProjector projector;               // Virtual object projector for rendering virtual objects in the scene
    cv::VideoWriter writer;                         // Video writer for saving output video (if processing video stream)
    cv::Mat frame;                                  // current frame captured from the camera
    cv::VideoCapture cap;                           // video capture object for accessing the camera
    std::string uiStatus = "READY";                 // Status message for UI display
    cv::Scalar statusColor = cv::Scalar(0, 255, 0); // Green color for status text

    void loadCalibration(); // Load calibration parameters from file, sets isCalibrated flag accordingly

    // Virtual object rendering control
    std::string modeStatus = "MODE: NONE";
    bool showObject = false;

    // feature-based tracking control
    FeatureDetector featureDetector;
    enum FeatureMode
    {
        NONE,
        HARRIS,
        GOOD_FEATURES,
        SURF
    };
    FeatureMode currentFeatureMode = NONE;

    // Calibrator data storage
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    double currentRMS;
    float RMS_THRESHOLD = 2.0f; // threshold for acceptable RMS error during calibration

    bool isCalibrated = false;                                                                 // flag to indicate whether the camera has been calibrated
    VirtualObjectProjector::ShapeType currentMode = VirtualObjectProjector::ShapeType::PACMAN; // state variable for current shape mode

    // 3D object points for solvePnP (assuming a single marker for simplicity)
    std::vector<cv::Point3f> objPoints;

    // file path for saving calibration parameters
    const std::string calibFile = "data/camera_params.yaml";

public:
    ARApp();                                           // constructor to initialize the application
    bool initSource(const std::string &path);          // init source (camera or video file), returns true if successful
    bool initCamera(int id);                           // init camera with given ID, returns true if successful
    void setInitialMode(const std::string &name);      // set the initial mode of the virtual object projector
    void saveFinalResult(const std::string &filename); // save the final result for static image output
    int run();                                         // process the main loop of the application
    bool isProcessingImage = false;                    // flag to indicate whether we are processing a static image
    bool isProcessingVideo = false;                    // flag to indicate whether we are processing a video stream
};