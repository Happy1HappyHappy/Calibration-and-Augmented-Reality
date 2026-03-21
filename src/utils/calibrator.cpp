/*
Claire Liu, Yu-Jing Wei
calibrator.cpp

Path: src/utils/calibrator.cpp
Description: Implements the calibrator functions for the augmented reality application.
*/

#include "calibrator.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <iostream>

/*
Constructor initializes the ArUco detector with the specified dictionary
and parameters.
*/
Calibrator::Calibrator()
{
    dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    detectorParams = cv::aruco::DetectorParameters();
    detector.setDictionary(dictionary);
    detector.setDetectorParameters(detectorParams);
}

/*
Detects ArUco markers in the given frame, draws their corners, and returns
the number of detected markers.
The detected marker IDs and their corresponding corners are stored in the
provided vectors.
*/
int Calibrator::detectAndDraw(cv::Mat &frame,
                              std::vector<int> &ids,
                              std::vector<std::vector<cv::Point2f>> &corners)
{
    // Storage for rejected candidates (not used here but can be useful for debugging)
    std::vector<std::vector<cv::Point2f>> rejected;

    // Use the ArUco detector to find markers in the frame. The detected corners
    // and their corresponding IDs will be stored in 'corners' and 'ids'
    detector.detectMarkers(frame, corners, ids, rejected);

    if (!ids.empty())
    {
        // Draw detected markers on the frame for visualization
        drawMarkerCorners(frame, corners);
    }
    imageSize = frame.size(); // update image size for later calibration
    return ids.size();
}

/*
Draws circles at the corners of detected markers on the given frame for
visualization.
*/
void Calibrator::drawMarkerCorners(cv::Mat &frame,
                                   const std::vector<std::vector<cv::Point2f>> &corners)
{
    for (const auto &marker : corners)
    {
        for (const auto &pt : marker)
        {
            cv::circle(frame, pt, 5, cv::Scalar(0, 0, 255), -1); // red
        }
    }
}

/*
Saves the detected corners and corresponding 3D object points for calibration.
If the number of samples exceeds the maximum allowed, it discards the oldest
record to make room for new data.
*/
void Calibrator::saveCalibrationData(const std::vector<int> &ids,
                                     const std::vector<std::vector<cv::Point2f>> &corners)
{
    // corners: [ [corners for id0], [corners for id1], ... ] for each detected marker
    // ids: [id0, id1, ...] corresponding to the detected markers
    // We need to flatten the corners and create corresponding 3D object points for each marker
    // -> flattened_corners:
    //      [ (x1,y1), (x2,y2), (x3,y3), (x4,y4), ... ]
    // -> current_point_set:
    //      [ (0,0,0), (1,0,0), (1,-1,0), (0,-1,0), ... ]

    if (ids.empty())
    {
        std::cout << "No markers detected!" << std::endl;
        return;
    }

    // Too many samples, choose to discard the oldest one to make room for new data
    if (corner_list.size() >= MAX_SAMPLES)
    {
        // Remove the oldest record (the one at the front of the list) to maintain
        // a fixed-size buffer of samples
        corner_list.erase(corner_list.begin());
        point_list.erase(point_list.begin());
        std::cout << "Reached max samples. Removed the oldest record." << std::endl;
    }

    // Store the corners and corresponding 3D points for this marker
    corner_list.push_back(corners[0]);
    point_list.push_back(getMarkerObjectPoints(MARKER_LENGTH));

    std::cout << "Stored sample [" << corner_list.size() << "/" << MAX_SAMPLES << "]" << std::endl;
}

/*
Helper function to generate 3D object points for ARUco marker based on its size
*/
std::vector<cv::Point3f> Calibrator::getMarkerObjectPoints(float marker_size)
{
    return {
        cv::Point3f(0, marker_size, 0),           // lower left
        cv::Point3f(marker_size, marker_size, 0), // lower right
        cv::Point3f(marker_size, 0, 0),           // upper right
        cv::Point3f(0, 0, 0)                      // upper left
    };
}

/*
Calibrates the camera using the collected corner and point data, and returns
the RMS error of the calibration. It also prints the resulting camera matrix
and distortion coefficients in a readable format for verification.
*/
double Calibrator::calibrate(cv::Mat &cameraMatrix, cv::Mat &distCoeffs)
{
    if (corner_list.empty() || point_list.empty())
    {
        std::cout << "No calibration data available!" << std::endl;
        return -1;
    }

    // Initialize camera matrix with some reasonable values
    cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    cameraMatrix.at<double>(0, 2) = imageSize.width / 2.0;  // cx
    cameraMatrix.at<double>(1, 2) = imageSize.height / 2.0; // cy

    // Distortion coefficients (k1, k2, p1, p2, k3)
    distCoeffs = cv::Mat::zeros(5, 1, CV_64F);
    // Rotation and translation vectors
    std::vector<cv::Mat> rvecs, tvecs;

    // Perform camera calibration using the collected corner and point data
    double rms = cv::calibrateCamera(point_list, corner_list, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);

    std::cout << "\n=== Calibration Results ===" << std::endl;
    std::cout << "RMS Error: " << rms << std::endl;

    // print camera matrix in a readable format
    std::cout << "Camera Matrix:\n"
              << cv::format(cameraMatrix, cv::Formatter::FMT_PYTHON) << std::endl;

    // print distortion coefficients in a readable format
    std::cout << "Distortion Coefficients:\n"
              << cv::format(distCoeffs, cv::Formatter::FMT_PYTHON) << std::endl;

    return rms;
}

/*
Save the calibration parameters to a YAML file. This function will overwrite the
main config file with the latest parameters for easy access by other parts of the
application, and also append a new entry to a history log file with a unique key
based on the timestamp for record-keeping.
*/
bool Calibrator::saveParameters(const std::string &filename, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs, double rms, const std::string &ts)
{
    if (cameraMatrix.empty() || distCoeffs.empty())
    {
        std::cerr << "Error: Camera parameters are empty. Cannot save." << std::endl;
        return false;
    }

    // Ensure the directory exists before trying to save the file
    std::filesystem::path p(filename);
    if (p.has_parent_path())
    {
        std::filesystem::create_directories(p.parent_path());
    }

    // --- Save Current Parameters ---
    // We overwrite the main config file with the latest parameters
    // for easy access by other parts of the application
    cv::FileStorage fs_current(filename, cv::FileStorage::WRITE);
    if (fs_current.isOpened())
    {
        fs_current << "camera_matrix" << cameraMatrix;
        fs_current << "distortion_coefficients" << distCoeffs;
        fs_current << "image_width" << imageSize.width;
        fs_current << "image_height" << imageSize.height;
        fs_current << "rms" << rms;
        fs_current.release();
        std::cout << "Current parameters updated in " << filename << std::endl;
    }

    // --- Append to History Log ---
    // We use a different filename for the log to avoid making the main
    // config file too large and hard to read
    std::string log_filename = p.parent_path().string() + "/calibration_log.yaml";
    cv::FileStorage fs_log(log_filename, cv::FileStorage::APPEND);

    if (fs_log.isOpened())
    {
        // Create a new Map for this calibration entry with a unique key based on the timestamp
        fs_log << "log_" + ts << "{";
        fs_log << "timestamp" << ts;
        fs_log << "camera_matrix" << cameraMatrix;
        fs_log << "distortion_coefficients" << distCoeffs;
        fs_log << "rms" << rms;
        fs_log << "}";
        fs_log.release();
        std::cout << "History log appended to " << log_filename << std::endl;
    }

    return true;
}

/*
Helper function to get the current timestamp in a specific format (YYYYMMDD_HHMMSS)
for use in filenames and log entries.
*/
std::string Calibrator::getCurrentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

/*
Saves the current frame with detected corners and axes drawn on it for documentation.
The filename is generated using the provided folder, prefix, and timestamp to ensure
uniqueness and organization of saved snapshots.
*/
bool Calibrator::saveCurrentFrame(const cv::Mat &frame, const std::string &folder, const std::string &prefix, const std::string &ts)
{
    if (frame.empty())
        return false;

    // Ensure the directory exists before trying to save the file
    std::filesystem::path dir(folder);
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }
    std::string filename = folder + "/" + prefix + "_" + ts + ".jpg";

    bool success = cv::imwrite(filename, frame);
    if (success)
    {
        std::cout << "Calibration snapshot saved: " << filename << std::endl;
    }
    return success;
}