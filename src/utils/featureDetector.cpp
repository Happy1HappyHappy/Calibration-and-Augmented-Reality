/*
Claire Liu, Yu-Jing Wei
featureDetector.cpp

Path: src/utils/featureDetector.cpp
Description: Declares the feature detector functions for the augmented reality application.
*/

#include "featureDetector.hpp"
#include <opencv2/opencv.hpp>

FeatureDetector::FeatureDetector()
{
    // Constructor can be used to initialize any parameters if needed
}

/*
Detect Harris corners in the frame and draw them.
*/
void FeatureDetector::detectAndDrawHarris(cv::Mat &frame, int threshold)
{
    if (frame.empty())
        return;

    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);
    dst = cv::Mat::zeros(frame.size(), CV_32FC1);

    // Harris parameters: blockSize=2, ksize=3, k=0.04
    cv::cornerHarris(gray, dst, 2, 3, 0.04);

    // Normalize the corner strength map to [0, 255] for visualization
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

    // Draw circles at locations where the corner strength exceeds the threshold
    for (int j = 0; j < dst_norm.rows; j++)
    {
        for (int i = 0; i < dst_norm.cols; i++)
        {
            if ((int)dst_norm.at<float>(j, i) > threshold)
            {
                // Red circles for Harris corners
                cv::circle(frame, cv::Point(i, j), 4, cv::Scalar(0, 0, 255), 1);
            }
        }
    }
}

/*
Detect good features to track (Shi-Tomasi) in the frame and draw them.
*/
void FeatureDetector::detectAndDrawGoodFeatures(cv::Mat &frame, int maxCorners)
{
    if (frame.empty())
        return;

    // Gray and blur the image to reduce noise before feature detection
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);

    // Save the detected corners in this vector
    std::vector<cv::Point2f> corners;

    // Extract good features to track using Shi-Tomasi method
    // The detected corners will be stored in 'corners'
    cv::goodFeaturesToTrack(gray,
                            corners,
                            maxCorners,
                            0.1,
                            20.0);

    // Draw circles at the detected corners for visualization
    for (const auto &pt : corners)
    {
        // Green circles for good features
        cv::circle(frame, pt, 5, cv::Scalar(0, 255, 0), -1); // 實心圓
        cv::circle(frame, pt, 8, cv::Scalar(0, 255, 0), 1);  // 外圈
    }
}

/*
Detect SURF features in the frame and draw them.
*/
void FeatureDetector::detectAndDrawSURF(cv::Mat &frame, int minHessian)
{
    if (frame.empty())
        return;

    // Gray the image before feature detection
    cv::Mat gray_surf;
    cv::cvtColor(frame, gray_surf, cv::COLOR_BGR2GRAY);

    // Initialize the SURF detector with the specified minimum Hessian threshold
    // minHessian usually ranges from 400 to 800 for typical images, but can be adjusted based on the scene
    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);

    // Detect keypoints in the image using the SURF detector
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(gray_surf, keypoints);

    // Draw the detected keypoints on the original frame
    // Using DRAW_RICH_KEYPOINTS will draw circles with size and orientation
    cv::drawKeypoints(frame, keypoints, frame, cv::Scalar(255, 0, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
}
