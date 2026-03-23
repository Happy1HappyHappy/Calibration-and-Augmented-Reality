# Calibration and Augmented Reality

## Authors
- **Claire Liu**
- **Yu-Jing Wei**

## Overview
This project is a real-time Augmented Reality (AR) application built in C++ using OpenCV that allows users to select and project different virtual 3D objects onto detected ArUco markers in a live camera view. The system first identifies markers from the video stream, then performs camera calibration to estimate the intrinsic parameters and distortion coefficients needed for accurate alignment between the virtual and physical worlds. After calibration, it employs `solvePnP` to estimate the real-time 3D pose of each marker, using that information to overlay virtual objects so they appear seamlessly attached to the physical markers in the scene. The application correctly identifies and processes multiple ArUco targets simultaneously.

The system is highly interactive, enabling users to switch between multiple virtual object designs during runtime, including a cube, a Pac-Man model, and a Space Needle wireframe structure. The project also supports saving calibration parameters in YAML files to avoid recalibration, and taking calibration snapshots. In addition to live AR projection, the tool includes capabilities for processing static images and pre-recorded video sequences, performing robust feature detection (Harris corners, Shi-Tomasi, SURF), and comparing calibration performance across multiple cameras.

## Features
- **Camera Calibration**: Computes the camera matrix and distortion coefficients using ArUco markers to correct lens distortion and calibrate the view.
- **Targets Detection**: Detects ArUco dictionary markers in real-time video feeds. The system correctly identifies and processes multiple ArUco targets simultaneously within the same scene.
- **Augmented Reality**: Projects dynamic 3D virtual objects (e.g., Pacman, Square, Space Needle) onto detected markers in physical space, with interactive toggles to switch object modes.
- **Real-time Pose Estimation**: Computes and can output 3D poses (rotation and translation vectors) for markers in real-time using `solvePnP`.
- **Save/Load Capabilities**: Saves and automatically loads calibration parameters (`camera_params.yaml`) to avoid recalibration in subsequent runs.
- **Feature Detection**: Detects and draws robust features (e.g., Harris corners, Shi-Tomasi, SURF) in video frames.
- **Multi-Camera Calibration & Comparison**: Calibrated and compared the performance of different cameras (e.g., built-in Webcam vs. iPhone 17), demonstrating variations in the intrinsic camera matrix and RMS re-projection errors.
- **Static Image & Video Sequence Support**: The application can process static images and pre-captured video sequences instead of live camera feeds, allowing virtual objects to be inserted into pre-recorded scenes.

## System Architecture
The application is structured using Object-Oriented design principles:

- **`main.cpp`**: The entry point. It instantiates the `ARApp`, initializes the camera (ID 0), and executes the main application loop.
- **`ARApp`**: The core controller class. It orchestrates the video capture, calibration phases, and delegates rendering AR content.
- **`Calibrator`**: Handles the underlying ArUco marker detection, collects calibration samples, computes camera matrices/distortion parameters, and manages saving/loading calibration data.
- **`FeatureDetector`**: Provides robust feature detection algorithms (e.g., Harris corners, Shi-Tomasi, SURF) to identify and track keypoints in the video feed.
- **`VirtualObjectProjector`**: Unifies the representation and rendering of 3D virtual objects. It handles the definition of various 3D shapes (e.g., Pacman, Square, Space Needle) and projects them onto the 2D image plane using the marker's pose and camera matrix.

### Core Class Methods

#### `ARApp`
- `initSource(const std::string &path)` / `initCamera(int id)`: Initialize the video feed from a file or webcam.
- `setInitialMode(const std::string &name)`: Set the starting 3D object to project.
- `run()`: Executes the main loop, handling rendering, calibration logic, and user input.
- `saveFinalResult(const std::string &filename)`: Saves the rendered output containing AR elements.

#### `Calibrator`
- `detectAndDraw(...)`: Detects ArUco dictionary markers in the frame and visually draws boundaries.
- `saveCalibrationData(...)`: Accumulates valid frames for later camera calibration.
- `calibrate(...)`: Returns camera matrix and distortion coefficients using the saved sample frames.
- `saveParameters(...)` / `loadParameters(...)`: Manages the serialization of configuration data (via YAML) so calibrations can be warm-started.

#### `FeatureDetector`
- `detectAndDrawHarris(...)`: Identifies and draws Harris Corners for feature tracking.
- `detectAndDrawGoodFeatures(...)`: Detects Shi-Tomasi good features to track.
- `detectAndDrawSURF(...)`: Employs the SURF algorithm to identify robust scale-invariant keypoints.

#### `VirtualObjectProjector`
- `setShape(ShapeType type)`: Toggles the system to render predefined objects (Pacman, Square, Space Needle).
- `setCustomShape(const ShapeData& shape)`: Configures the projector with custom user-defined 3D shapes.
- `render(...)`: Projects 3D points to the 2D image frame and calculates shading/lines according to the target's rotation (`rvec`) and translation (`tvec`).

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

## Interactive Controls
While the application is running, use the following keys to interact:
- **`s`**: Save calibration parameters (after collecting enough samples).
- **`1`** - **`3`**: Toggle 3D Virtual Objects (Pacman, Square, Space Needle).
- **`h`**: Toggle Harris Corners detection.
- **`g`**: Toggle Shi-Tomasi Good Features detection.
- **`f`**: Toggle SURF Features detection.
- **`c`**: Clear / Reset current feature detection mode.
- **`ESC`**: Exit the application.

## Acknowledgements
- **CS5330 Pattern Recognition & Computer Vision** (Northeastern University)
- [**OpenCV**](https://opencv.org/) for offering the robust open-source computer vision & machine learning software library.