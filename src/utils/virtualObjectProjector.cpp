/*
Claire Liu, Yu-Jing Wei
virtualObject.cpp

Path: src/utils/virtualObject.cpp
Description: Implements the virtual object functions for the augmented reality application.
*/

#include "virtualObjectProjector.hpp"
#include <cmath>

VirtualObjectProjector::VirtualObjectProjector() : currentShapeType(ShapeType::SQUARE) {
    // generateSquare(); // Default shape
    generatePacman();
}

void VirtualObjectProjector::setShape(ShapeType type) {
    currentShapeType = type;
    if (type == ShapeType::PACMAN) {
        generatePacman();
    } else if (type == ShapeType::SQUARE) {
        generateSquare();
    }
}

void VirtualObjectProjector::setCustomShape(const ShapeData& customShape) {
    currentShapeType = ShapeType::CUSTOM;
    currentShape = customShape;
}

void VirtualObjectProjector::generateSquare() {
    currentShape.vertices.clear();
    currentShape.lines.clear();
    
    // Create a 3D square (cube) on top of the marker
    // Let's make the cube size 1.0 (to match default marker size)
    float size = 1.0f;
    currentShape.vertices = {
        {0, 0, 0}, {size, 0, 0}, {size, size, 0}, {0, size, 0},       // Bottom face
        {0, 0, size}, {size, 0, size}, {size, size, size}, {0, size, size} // Top face (Z negative is "above" marker)
    };
    
    currentShape.lines = {
        {0, 1, 2, 3, 0}, // Bottom
        {4, 5, 6, 7, 4}, // Top
        {0, 4}, {1, 5}, {2, 6}, {3, 7} // Vertical edges
    };
    currentShape.color = cv::Scalar(255, 0, 0); // Blue lines
}

void VirtualObjectProjector::generatePacman() {
    currentShape.vertices.clear();
    currentShape.lines.clear();
    
    // Generate a 3D wireframe Pacman (sphere missing a wedge)
    float radius = 0.5f;
    cv::Point3f center(0.5f, 0.5f, -radius); // Centered above marker
    int slices = 15;
    int stacks = 15;
    
    float mouthAngle = CV_PI / 4.0; // 45 degrees
    
    for (int i = 0; i <= stacks; ++i) {
        float phi = CV_PI * float(i) / float(stacks);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * CV_PI * float(j) / float(slices);
            
            // Limit theta to create the mouth "wedge" opening
            if (theta > (2.0f * CV_PI - mouthAngle/2.0f) || theta < mouthAngle/2.0f) {
                float clampedTheta = (theta > CV_PI) ? (2.0f * CV_PI - mouthAngle/2.0f) : (mouthAngle/2.0f);
                float x = center.x + radius * std::sin(phi) * std::cos(clampedTheta);
                float y = center.y + radius * std::sin(phi) * std::sin(clampedTheta);
                float z = center.z + radius * std::cos(phi);
                currentShape.vertices.push_back({x, y, z});
            } else {
                float x = center.x + radius * std::sin(phi) * std::cos(theta);
                float y = center.y + radius * std::sin(phi) * std::sin(theta);
                float z = center.z + radius * std::cos(phi);
                currentShape.vertices.push_back({x, y, z});
            }
        }
    }
    
    // Add lines for wireframe
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int current = i * (slices + 1) + j;
            int next = current + 1;
            int bottom = (i + 1) * (slices + 1) + j;
            
            currentShape.lines.push_back({current, next});
            currentShape.lines.push_back({current, bottom});
        }
    }
    
    currentShape.color = cv::Scalar(0, 255, 255); // Yellow colored pacman
}

void VirtualObjectProjector::render(cv::Mat& frame, const CameraPose& pose)
{
    if (currentShape.vertices.empty() || pose.rvec.empty() || pose.tvec.empty()) return;

    std::vector<cv::Point2f> imagePoints;
    cv::projectPoints(currentShape.vertices, pose.rvec, pose.tvec, pose.cameraMatrix, pose.distCoeffs, imagePoints);

    // Draw lines
    for (const auto& lineIndices : currentShape.lines) {
        for (size_t i = 1; i < lineIndices.size(); ++i) {
            int pt1_idx = lineIndices[i - 1];
            int pt2_idx = lineIndices[i];
            
            // Simple bound check in case projectPoints returns invalid points
            if (pt1_idx < imagePoints.size() && pt2_idx < imagePoints.size()) {
                cv::line(frame, imagePoints[pt1_idx], imagePoints[pt2_idx], currentShape.color, 2);
            }
        }
    }
}