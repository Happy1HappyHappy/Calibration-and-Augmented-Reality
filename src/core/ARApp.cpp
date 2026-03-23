/*
Claire Liu, Yu-Jing Wei
ARApp.cpp

Path: src/core/ARApp.cpp
Description: Implementation of the augmented reality application.
*/

#include "ARApp.hpp"
#include <opencv2/opencv.hpp>

/*
Constructor initializes the application
*/
ARApp::ARApp()
{
    // Keep the constructor simple for now, we will initialize the camera in run()
}

/*
Init the camera with the given ID, returns true if successful.
We will call this function in main() before starting the main loop.
*/
bool ARApp::initCamera(int id)
{
    cap.open(id); // only open the camera when this function is called, not in the constructor

    // Try to load existing calibration data from the YAML file
    if (calib.loadParameters(calibFile, cameraMatrix, distCoeffs, currentRMS))
    {
        isCalibrated = true;
        objPoints = Calibrator::getMarkerObjectPoints(1.0f);
        uiStatus = "CALIBRATION PARAMETERS LOADED (RMS: " + std::to_string(currentRMS).substr(0, 4) + ")";
    }
    else
    {
        isCalibrated = false;
        std::cout << "No calibration file found. Please calibrate using 's' key." << std::endl;
    }

    return cap.isOpened();
}

/*
run() is the main loop of the application, it will capture frames from
the camera, detect markers, and handle user input for calibration.
*/
int ARApp::run()
{
    // Check if the camera is opened successfully
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open camera." << std::endl;
        return -1;
    }
    cap >> frame;
    if (frame.empty())
    {
        std::cerr << "Error: Could not read frame from camera." << std::endl;
        return -1;
    }
    // ===== Variable Initialization =====
    // Markers and Collect Calibration Data
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    // Detect markers and draw corners on the frame
    calib.detectAndDraw(frame, ids, corners);

    // Get the key press and current sample count for UI display
    int key = cv::waitKey(10) & 0xFF;
    int currentCount = calib.getSampleCount();

    bool shouldSaveSnapshot = false;   // flag to indicate whether we should save the snapshot at the end of this function
    static std::string currentTS = ""; // to store the timestamp for the current calibration session

    // ===== Mode Handling ======
    // Clear detected corners and reset calibration data when 'c' key is pressed
    if (key == 'c')
    {
        currentFeatureMode = NONE;
        modeStatus = "MODE: NONE";
    }
    // Harris corner detection mode toggle with 'h' key
    else if (key == 'h')
    {
        currentFeatureMode = (currentFeatureMode == HARRIS) ? NONE : HARRIS;
        modeStatus = (currentFeatureMode == HARRIS) ? "MODE: HARRIS CORNERS" : "MODE: NONE";
    }
    // Shi-Tomasi good features detection mode toggle with 'g' key
    else if (key == 'g')
    {
        currentFeatureMode = (currentFeatureMode == GOOD_FEATURES) ? NONE : GOOD_FEATURES;
        modeStatus = (currentFeatureMode == GOOD_FEATURES) ? "MODE: GOOD FEATURES" : "MODE: NONE";
    }
    // SURF feature detection mode toggle with 'f' key
    else if (key == 'f')
    {
        currentFeatureMode = (currentFeatureMode == SURF) ? NONE : SURF;
        modeStatus = (currentFeatureMode == SURF) ? "MODE: SURF FEATURES" : "MODE: NONE";
    }
    // Handle calibration data collection and processing when 's' key is pressed
    else if (key == 's')
    {
        if (ids.empty())
        {
            uiStatus = "FAILURE: NO MARKERS";
            statusColor = cv::Scalar(0, 0, 255); // Red color for failure status
        }
        else
        {
            // Save the detected corners and corresponding 3D points for calibration
            calib.saveCalibrationData(ids, corners);
            // update the sample count after saving data
            currentCount = calib.getSampleCount();

            if (currentCount < calib.getMinSamples())
            {
                // Not enough samples collected yet, prompt user to keep going
                uiStatus = "COLLECTING... #" + std::to_string(currentCount);
                statusColor = cv::Scalar(0, 255, 255); // Yellow color for collecting status
            }
            else
            {
                // Perform camera calibration using the collected data and get the RMS error
                double rms = calib.calibrate(cameraMatrix, distCoeffs);

                if (rms < RMS_THRESHOLD)
                {
                    // Calibration successed, update the status and prepare to save the snapshot
                    isCalibrated = true;
                    // Set the 3D object points for the marker based on its size (assuming marker size is 1.0 unit)
                    objPoints = Calibrator::getMarkerObjectPoints(1.0f);

                    // Update the parameters to current camera_params file
                    // and also save them to a log file for this calibration session
                    currentTS = calib.getCurrentTimestamp();
                    calib.saveParameters(calibFile, cameraMatrix, distCoeffs, rms, currentTS);

                    shouldSaveSnapshot = true; // Set the flag to save the snapshot

                    uiStatus = "CALIBRATED (RMS: " + std::to_string(rms).substr(0, 4) + ")";
                    statusColor = cv::Scalar(0, 255, 0); // Green color for calibrated status
                }
                else
                {
                    // Calibration failed due to high RMS error, prompt user to try again
                    uiStatus = "FAILURE: HIGH RMS (" + std::to_string(rms).substr(0, 4) + ")";
                    statusColor = cv::Scalar(0, 0, 255); // Red color for failure status
                }
            }
        }
    }
    // The drawing modes
    else if (key == '1')
    {
        if (showObject && currentMode == VirtualObjectProjector::ShapeType::PACMAN)
        {
            showObject = false;
            modeStatus = "MODE: NONE";
        }
        else
        {
            currentMode = VirtualObjectProjector::ShapeType::PACMAN;
            projector.setShape(currentMode);
            modeStatus = "MODE: PACMAN";
            showObject = true;
        }
    }
    else if (key == '2')
    {
        if (showObject && currentMode == VirtualObjectProjector::ShapeType::SQUARE)
        {
            showObject = false;
            modeStatus = "MODE: NONE";
        }
        else
        {
            currentMode = VirtualObjectProjector::ShapeType::SQUARE;
            projector.setShape(currentMode);
            modeStatus = "MODE: CUBE";
            showObject = true;
        }
    }
    else if (key == '3')
    {
        if (showObject && currentMode == VirtualObjectProjector::ShapeType::SPACE_NEEDLE)
        {
            showObject = false;
            modeStatus = "MODE: NONE";
        }
        else
        {
            currentMode = VirtualObjectProjector::ShapeType::SPACE_NEEDLE;
            projector.setShape(currentMode);
            modeStatus = "MODE: SPACE NEEDLE";
            showObject = true;
        }
    }

    // ==== Corners / Feature Detection Dispatcher ====
    switch (currentFeatureMode)
    {
    case HARRIS:
        featureDetector.detectAndDrawHarris(frame, 220);
        break;
    case GOOD_FEATURES:
        featureDetector.detectAndDrawGoodFeatures(frame, 150);
        break;
    case SURF:
        featureDetector.detectAndDrawSURF(frame, 400);
        break;
    default:
        break;
    }

    // ==== Draw Detected Markers and UI ====
    // If calibrated and we have detected markers
    if (isCalibrated && !ids.empty())
    {
        // For each detected marker
        for (size_t i = 0; i < ids.size(); i++)
        {
            // estimate its pose and draw the axes on the frame
            cv::Mat rvec, tvec;
            if (cv::solvePnP(objPoints, corners[i], cameraMatrix, distCoeffs, rvec, tvec))
            {
                // Draw coordinate axes
                cv::drawFrameAxes(frame, cameraMatrix, distCoeffs, rvec, tvec, 0.7f);

                // Render the current virtual object
                if (showObject)
                {
                    projector.render(frame, rvec, tvec, cameraMatrix, distCoeffs);
                }

                // Print rotation and translation data in real time for debugging and verification
                // std::cout << "Marker ID: " << ids[i]
                //           << " | rvec: " << rvec.t()
                //           << " | tvec: " << tvec.t() << std::endl;
            }
        }
    }

    // Draw a semi-transparent black rectangle as the background for the status text
    cv::rectangle(frame, cv::Point(10, 10), cv::Point(350, 120), cv::Scalar(0, 0, 0), -1);

    std::string progStr = "SAMPLES: " + std::to_string(currentCount) + "/20";
    cv::putText(frame, progStr, cv::Point(30, 40), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(255, 255, 255), 1);
    cv::putText(frame, modeStatus, cv::Point(30, 70), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(0, 255, 255), 1);

    // Draw the command list on the right side of the frame
    int baseY = frame.rows - 160;
    int xPos = frame.cols - 250;
    cv::putText(frame, "COMMANDS:", cv::Point(xPos, baseY), cv::FONT_HERSHEY_DUPLEX, 0.8, cv::Scalar(50, 50, 50), 1);
    cv::putText(frame, "'s': Save Calibration", cv::Point(xPos, baseY + 25), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(50, 50, 50), 1);
    cv::putText(frame, "'1-3': Toggle Objects", cv::Point(xPos, baseY + 45), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(50, 50, 50), 1);
    cv::putText(frame, "'h': Harris Corners", cv::Point(xPos, baseY + 65), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(50, 50, 50), 1);
    cv::putText(frame, "'g': Good Features", cv::Point(xPos, baseY + 85), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(50, 50, 50), 1);
    cv::putText(frame, "'f': SURF Features", cv::Point(xPos, baseY + 105), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(50, 50, 50), 1);
    cv::putText(frame, "'c': Clear / Reset", cv::Point(xPos, baseY + 125), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(50, 50, 50), 1);
    cv::putText(frame, "'ESC': Exit", cv::Point(xPos, baseY + 145), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(50, 50, 50), 1);

    // Draw the current status message at the bottom center of the frame
    cv::Size textSize = cv::getTextSize(uiStatus, cv::FONT_HERSHEY_DUPLEX, 1.0, 2, 0);
    cv::putText(frame, uiStatus, cv::Point((frame.cols - textSize.width) / 2, frame.rows - 40),
                cv::FONT_HERSHEY_DUPLEX, 1.0, statusColor, 2);

    // Save the snapshot if the flag is set, which means we just successfully calibrated
    if (shouldSaveSnapshot)
    {
        // Draw a white border around the frame to indicate successful calibration
        cv::rectangle(frame, cv::Point(0, 0), cv::Point(frame.cols, frame.rows), {255, 255, 255}, 10);

        // Save the current frame with detected corners and axes drawn on it for documentation
        calib.saveCurrentFrame(frame, "data/snapshots", "snapshot", currentTS);
    }

    // Show the frame with detected markers and axes
    cv::imshow("AR Application", frame);

    return (key == 27) ? -1 : 0; // EXIT on 'ESC' key press, otherwise continue running
}
