#include <opencv2/opencv.hpp>
#include "common.hpp"
/*
Claire Liu, Yu-Jing Wei
virtualObject.hpp

Path: include/virtualObject.hpp
Description: Declares the virtual object functions for the augmented reality application.
*/


struct ShapeData {
    std::vector<cv::Point3f> vertices;
    std::vector<std::vector<int>> lines; // Each inner vector connects indices of vertices
    cv::Scalar color; // Color of the shape
};

class VirtualObjectProjector
{
public:
    // Enum for shape types
    enum class ShapeType {
        PACMAN,
        SQUARE,
        CUSTOM
    };

    VirtualObjectProjector();
    
    // Set current shape
    void setShape(ShapeType type);
    void setCustomShape(const ShapeData& shape);

    // Update is no longer needed as we pass pose directly to render
    // void update(const CameraPose& pose);
                
    void render(cv::Mat& frame, const CameraPose& pose);

private:
    ShapeType currentShapeType;
    ShapeData currentShape;

    void generateSquare();
    void generatePacman();
};
