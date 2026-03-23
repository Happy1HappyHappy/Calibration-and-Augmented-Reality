/*
Claire Liu, Yu-Jing Wei
virtualObjectProjector.cpp

Path: src/utils/virtualObjectProjector.cpp
Description: Implements the virtual object projector functions for the augmented reality application.
*/

#include "virtualObjectProjector.hpp"
#include <cmath>

// Constructor initializes the default shape -> square
VirtualObjectProjector::VirtualObjectProjector() : currentShapeType(ShapeType::SQUARE)
{
    generateSquare(); // Default shape
}

// Set the current shape type
void VirtualObjectProjector::setShape(ShapeType type)
{
    currentShapeType = type;
    if (type == ShapeType::PACMAN)
    {
        generatePacman();
    }
    else if (type == ShapeType::SQUARE)
    {
        generateSquare();
    }
    else if (type == ShapeType::SPACE_NEEDLE)
    {
        generateSpaceNeedle();
    }
}

// Set current shape data to a square
void VirtualObjectProjector::generateSquare()
{
    currentShape.vertices.clear();
    currentShape.lines.clear();

    // Create a 3D square (cube) on top of the marker
    // Let's make the cube size 1.0 (to match default marker size)
    float size = 1.0f;
    currentShape.vertices = {
        {0, 0, 0}, {size, 0, 0}, {size, size, 0}, {0, size, 0}, // Bottom face
        {0, 0, size},
        {size, 0, size},
        {size, size, size},
        {0, size, size} // Top face (Z negative is "above" marker)
    };

    currentShape.lines = {
        {0, 1, 2, 3, 0}, // Bottom
        {4, 5, 6, 7, 4}, // Top
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7} // Vertical edges
    };
    currentShape.color = cv::Scalar(255, 0, 0); // Blue lines
}

// Set current shape data to a pacman
void VirtualObjectProjector::generatePacman()
{
    currentShape.vertices.clear();
    currentShape.lines.clear();

    // Generate a 3D wireframe Pacman (sphere missing a wedge)
    float radius = 0.5f;
    cv::Point3f center(0.5f, 0.5f, radius); // Centered above marker
    int slices = 15;
    int stacks = 15;

    float mouthAngle = CV_PI / 4.0; // 45 degrees

    for (int i = 0; i <= stacks; ++i)
    {
        float phi = CV_PI * float(i) / float(stacks);
        for (int j = 0; j <= slices; ++j)
        {
            float theta = 2.0f * CV_PI * float(j) / float(slices);

            float t = theta;
            if (theta > (2.0f * CV_PI - mouthAngle / 2.0f) || theta < mouthAngle / 2.0f)
            {
                t = (theta > CV_PI) ? (2.0f * CV_PI - mouthAngle / 2.0f) : (mouthAngle / 2.0f);
            }

            float x = center.x + radius * std::sin(phi) * std::cos(t);
            float y = center.y - radius * std::cos(phi);
            float z = center.z + radius * std::sin(phi) * std::sin(t);
            currentShape.vertices.push_back({x, y, z});
        }
    }

    // Add lines for wireframe
    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            int current = i * (slices + 1) + j;
            int next = current + 1;
            int bottom = (i + 1) * (slices + 1) + j;

            currentShape.lines.push_back({current, next});
            currentShape.lines.push_back({current, bottom});
        }
    }

    currentShape.color = cv::Scalar(0, 255, 255); // Yellow colored pacman
}

// Set current shape data to the space needle
void VirtualObjectProjector::generateSpaceNeedle()
{
    currentShape.vertices.clear();
    currentShape.lines.clear();

    // Overall size normalized to the marker footprint (1.0 x 1.0)
    // Space Needle centered at x=0.5, y=0.5 and extending upward
    float cx = 0.5f, cy = 0.5f;

    // ─── Z heights for each structural level ─────────────────
    float zBase = 0.0f;
    float zTripodTop = 0.25f;
    float zColBottom = 0.28f;
    float zColMid = 0.68f;
    float zDiscBot = 0.73f;
    float zDiscTop = 0.83f;
    float zSpireTop = 1.15f;

    // ─── Helper: add a regular polygon ring at a given height ──
    // Returns the index of the first vertex
    auto addRing = [&](float x, float y, float z, float r, int n) -> int
    {
        int startIdx = currentShape.vertices.size();
        for (int i = 0; i < n; ++i)
        {
            float angle = 2.0f * CV_PI * i / n;
            currentShape.vertices.push_back({x + r * std::cos(angle),
                                             y + r * std::sin(angle),
                                             z});
        }
        return startIdx;
    };

    // ─── Helper: connect two rings with the same segment count ─
    auto connectRings = [&](int a, int b, int n)
    {
        for (int i = 0; i < n; ++i)
        {
            int next = (i + 1) % n;
            currentShape.lines.push_back({a + i, a + next}); // Horizontal edge on the source ring
            currentShape.lines.push_back({a + i, b + i});    // Vertical connection between rings
        }
    };

    // ─── Helper: draw the polygon edges within one ring ────────
    auto ringLines = [&](int start, int n)
    {
        for (int i = 0; i < n; ++i)
            currentShape.lines.push_back({start + i, start + (i + 1) % n});
    };

    const int SEG = 12; // Number of segments used for circular parts

    // ══════════════════════════════════════════
    // 1. Base platform
    // ══════════════════════════════════════════
    int rBase0 = addRing(cx, cy, zBase, 0.34f, SEG);
    int rBase1 = addRing(cx, cy, zBase + 0.02f, 0.34f, SEG);
    connectRings(rBase0, rBase1, SEG);
    ringLines(rBase1, SEG);

    // ══════════════════════════════════════════
    // 2. Tripod supports from the outer base to the main column
    // ══════════════════════════════════════════
    // Each leg is wider at the bottom and narrows toward the column
    float legAngles[3] = {0.0f, 2.0f * CV_PI / 3.0f, 4.0f * CV_PI / 3.0f};
    float legWidthAngle = 0.15f;
    float colRadius = 0.07f;

    for (int k = 0; k < 3; ++k)
    {
        float a = legAngles[k];
        // Two bottom corners of the leg at base height
        float x0L = cx + 0.31f * std::cos(a - legWidthAngle);
        float y0L = cy + 0.31f * std::sin(a - legWidthAngle);
        float x0R = cx + 0.31f * std::cos(a + legWidthAngle);
        float y0R = cy + 0.31f * std::sin(a + legWidthAngle);
        // Two top corners of the leg, narrowed toward the column
        float x1L = cx + colRadius * std::cos(a - legWidthAngle * 0.3f);
        float y1L = cy + colRadius * std::sin(a - legWidthAngle * 0.3f);
        float x1R = cx + colRadius * std::cos(a + legWidthAngle * 0.3f);
        float y1R = cy + colRadius * std::sin(a + legWidthAngle * 0.3f);

        int i0 = currentShape.vertices.size();
        currentShape.vertices.push_back({x0L, y0L, zBase + 0.02f}); // i0
        currentShape.vertices.push_back({x0R, y0R, zBase + 0.02f}); // i0+1
        currentShape.vertices.push_back({x1L, y1L, zTripodTop});    // i0+2
        currentShape.vertices.push_back({x1R, y1R, zTripodTop});    // i0+3

        // Four boundary edges of the leg
        currentShape.lines.push_back({i0, i0 + 1});     // Bottom edge
        currentShape.lines.push_back({i0, i0 + 2});     // Left edge
        currentShape.lines.push_back({i0 + 1, i0 + 3}); // Right edge
        currentShape.lines.push_back({i0 + 2, i0 + 3}); // Top edge
        // Diagonal brace across the leg
        currentShape.lines.push_back({i0, i0 + 3});
    }

    // ══════════════════════════════════════════
    // 3. Main column, slightly tapered upward
    // ══════════════════════════════════════════
    int rCol0 = addRing(cx, cy, zColBottom, colRadius, SEG);     // Column base
    int rCol1 = addRing(cx, cy, zColMid, colRadius * 0.6f, SEG); // Narrowed mid column
    connectRings(rCol0, rCol1, SEG);

    // ══════════════════════════════════════════
    // 4. Observation deck disc
    // ══════════════════════════════════════════
    int rD0 = addRing(cx, cy, zDiscBot, 0.28f, SEG); // Outer rim at disc bottom
    int rD1 = addRing(cx, cy, zDiscBot, 0.12f, SEG); // Inner rim at disc bottom
    int rD2 = addRing(cx, cy, zDiscTop, 0.25f, SEG); // Outer rim at disc top
    int rD3 = addRing(cx, cy, zDiscTop, 0.08f, SEG); // Inner rim at disc top

    // Connect column top to the lower inner disc rim
    connectRings(rCol1, rD1, SEG);
    ringLines(rD1, SEG);

    // Bottom outer disc rim
    ringLines(rD0, SEG);
    // Radial lines across the bottom face of the disc
    for (int i = 0; i < SEG; ++i)
        currentShape.lines.push_back({rD1 + i, rD0 + i});

    // Disc side surface
    connectRings(rD0, rD2, SEG);

    // Top face of the disc
    ringLines(rD2, SEG);
    ringLines(rD3, SEG);
    for (int i = 0; i < SEG; ++i)
        currentShape.lines.push_back({rD3 + i, rD2 + i});

    // ══════════════════════════════════════════
    // 5. Antenna spire tapering upward in simple sections
    // ══════════════════════════════════════════
    float spireR[4] = {0.06f, 0.03f, 0.015f, 0.0f};
    float spireZ[4] = {zDiscTop, zDiscTop + 0.10f, zDiscTop + 0.20f, zSpireTop};

    int prevSpire = addRing(cx, cy, spireZ[0], spireR[0], SEG);
    connectRings(rD3, prevSpire, SEG); // Connect the disc top inner rim to the spire base

    for (int s = 1; s < 3; ++s)
    {
        int curSpire = addRing(cx, cy, spireZ[s], spireR[s], SEG);
        connectRings(prevSpire, curSpire, SEG);
        prevSpire = curSpire;
    }
    // Tip of the spire as a single vertex
    int tipIdx = currentShape.vertices.size();
    currentShape.vertices.push_back({cx, cy, zSpireTop});
    for (int i = 0; i < SEG; ++i)
        currentShape.lines.push_back({prevSpire + i, tipIdx});

    currentShape.color = cv::Scalar(255, 200, 0); // Blue-white tone
}

// Rendering the virtual object on the frame using the given pose and camera parameters
void VirtualObjectProjector::render(cv::Mat &frame, const cv::Mat &rvec, const cv::Mat &tvec, const cv::Mat &cameraMatrix, const cv::Mat &distCoeffs)
{
    if (currentShape.vertices.empty() || rvec.empty() || tvec.empty())
        return;

    std::vector<cv::Point2f> imagePoints;
    cv::projectPoints(currentShape.vertices, rvec, tvec, cameraMatrix, distCoeffs, imagePoints);

    // Draw lines
    for (const auto &lineIndices : currentShape.lines)
    {
        for (size_t i = 1; i < lineIndices.size(); ++i)
        {
            int pt1_idx = lineIndices[i - 1];
            int pt2_idx = lineIndices[i];

            // Simple bound check in case projectPoints returns invalid points
            if (pt1_idx < imagePoints.size() && pt2_idx < imagePoints.size())
            {
                cv::line(frame, imagePoints[pt1_idx], imagePoints[pt2_idx], currentShape.color, 2);
            }
        }
    }
}
