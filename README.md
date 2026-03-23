# Calibration and Augmented Reality

## Authors
- **Claire Liu**
- **Yu-Jing Wei**

## Overview
This project is a real-time Augmented Reality (AR) application built in C++ using OpenCV and ArUco markers. It performs camera calibration, marker detection, and projects virtual objects seamlessly onto physical markers using `solvePnP`.

## Features
- **Camera Calibration**: Computes the camera matrix and distortion coefficients using ArUco markers to correct lens distortion and calibrate the view.
- **Marker Detection**: Detects ArUco dictionary markers in real-time video feeds.
- **Augmented Reality**: Projects dynamic 3D virtual objects (e.g., Pacman, Square, Space Needle) onto detected markers in physical space, with interactive toggles to switch object modes.
- **Real-time Pose Estimation**: Computes and can output 3D poses (rotation and translation vectors) for markers in real-time using `solvePnP`.
- **Save/Load Capabilities**: Saves and automatically loads calibration parameters (`camera_params.yaml`) to avoid recalibration in subsequent runs.
- **Feature Detection**: Detects and draws robust features (e.g., Harris corners, Shi-Tomasi, SURF) in video frames.

## System Architecture
The application is structured using Object-Oriented design principles:

- **`main.cpp`**: The entry point. It instantiates the `ARApp`, initializes the camera (ID 0), and executes the main application loop.
- **`ARApp`**: The core controller class. It orchestrates the video capture, calibration phases, and delegates rendering AR content.
- **`Calibrator`**: Handles the underlying ArUco marker detection, collects calibration samples, computes camera matrices/distortion parameters, and manages saving/loading calibration data.
- **`FeatureDetector`**: Provides robust feature detection algorithms (e.g., Harris corners, Shi-Tomasi, SURF) to identify and track keypoints in the video feed.
- **`VirtualObjectProjector`**: Unifies the representation and rendering of 3D virtual objects. It handles the definition of various 3D shapes (e.g., Pacman, Square, Space Needle) and projects them onto the 2D image plane using the marker's pose and camera matrix.

## Directory Structure
```
.
├── Makefile                # Build configuration
├── bin/                    # Compiled executable output directory
├── data/                   # Directory for output data (e.g., camera_params.yaml)
├── include/                # Header files (.hpp)
│   ├── ARApp.hpp
│   ├── calibrator.hpp
│   ├── featureDetector.hpp
│   └── virtualObjectProjector.hpp
├── obj/                    # Compiled object files (.o)
└── src/                    # Source files (.cpp)
    ├── core/               # Core application logic (main.cpp, ARApp.cpp)
    └── utils/              # Component implementations (calibrator, featureDetector, virtualObjectProjector)
```

## Prerequisites
- **macOS** (Minimum Target Version 26.2 as configured)
- **C++17** compatible compiler (e.g., Apple Clang/g++)
- **OpenCV 4**: Installed via Homebrew (expected at `/opt/homebrew/include/opencv4`)
- **Make**: Build automation tool

## Build Instructions
A `Makefile` is provided to easily compile the system.
1. Open your terminal and navigate to the project directory.
2. Build the project using `make`:
   ```bash
   make
   ```

## Running the Application
After compiling, the executable `ar` will be generated inside the `bin/` folder. Ensure your camera is connected and run:
```bash
./bin/ar
```

## Acknowledgements
- **CS5330 Pattern Recognition & Computer Vision** (Northeastern University)
- [**OpenCV**](https://opencv.org/) for offering the robust open-source computer vision & machine learning software library.