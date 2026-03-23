/*
Claire Liu, Yu-Jing Wei
virtualObjectProjector.hpp

Path: include/virtualObjectProjector.hpp
Description: Declares the virtual object projector functions for the augmented reality application.
*/

#include <opencv2/opencv.hpp>

// Data structure to hold shape information
struct ShapeData
{
    std::vector<cv::Point3f> vertices;
    std::vector<std::vector<int>> lines; // Each inner vector connects indices of vertices
    cv::Scalar color;                    // Color of the shape
};

class VirtualObjectProjector
{
public:
    // Enum for shape types
    enum class ShapeType
    {
        PACMAN,
        SQUARE,
        SPACE_NEEDLE
    };

    VirtualObjectProjector();

    // Set current shape
    void setShape(ShapeType type);
    // Set custom shape data
    void setCustomShape(const ShapeData &shape);

    // Render the virtual object on the frame using the given pose and camera parameters
    void render(cv::Mat &frame, const cv::Mat &rvec, const cv::Mat &tvec, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs);

private:
    // Shape state
    ShapeType currentShapeType;
    // Shape data for current state
    ShapeData currentShape;

    // Default shapes
    void generateSquare();
    void generatePacman();
    void generateSpaceNeedle();
};
