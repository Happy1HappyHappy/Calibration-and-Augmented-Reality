/*
Claire Liu, Yu-Jing Wei
ARApp.hpp

Path: include/ARApp.hpp
Description: Declares the ARApp class for the augmented reality application.
*/

#pragma once // Include guard

#include "calibrator.hpp"
#include "virtualObjectProjector.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <opencv2/opencv.hpp>

class ARApp
{
private:
    Calibrator calib;                 // Calibrator object for handling marker detection and camera calibration
    VirtualObjectProjector projector; // Virtual object projector for rendering virtual objects in the scene
    cv::Mat frame;                    // current frame captured from the camera
    cv::VideoCapture cap;             // video capture object for accessing the camera

    // Calibrator data storage
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    float RMS_THRESHOLD = 2.0f; // threshold for acceptable RMS error during calibration

    bool isCalibrated = false;                                                                 // flag to indicate whether the camera has been calibrated
    VirtualObjectProjector::ShapeType currentMode = VirtualObjectProjector::ShapeType::PACMAN; // state variable for current shape mode

    // 3D object points for solvePnP (assuming a single marker for simplicity)
    std::vector<cv::Point3f> objPoints;

    // file path for saving calibration parameters
    const std::string calibFile = "data/camera_params.yaml";

public:
    ARApp();                 // constructor to initialize the application
    bool initCamera(int id); // init camera with given ID, returns true if successful
    int run();               // process the main loop of the application
};