/*
Claire Liu, Yu-Jing Wei
featureDetector.hpp

Path: include/featureDetector.hpp
Description: Declares the feature detector functions for the augmented reality application.
*/

#pragma once // Include guard

#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>

class FeatureDetector
{
public:
    FeatureDetector();
    void detectAndDrawHarris(cv::Mat &frame, int threshold = 150);
    void detectAndDrawGoodFeatures(cv::Mat &frame, int maxCorners = 100);
    void detectAndDrawSURF(cv::Mat &frame, int minHessian);

private:
    cv::Mat gray, dst, dst_norm;
};