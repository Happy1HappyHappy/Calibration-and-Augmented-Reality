/*
Claire Liu, Yu-Jing Wei
main.cpp

Path: src/core/main.cpp
Description: Entry point for the augmented reality application.
*/
#include "ARApp.hpp"
#include <iostream>

/*
Main function initializes the AR application, sets up the camera or video
source, and starts the main loop.
*/
int main(int argc, char **argv)
{
    try
    {
        ARApp app;
        std::string source;
        std::string objName;

        // Check if a source path is provided as a command-line argument,
        // if not, default to open camera
        if (argc > 1)
        {
            source = argv[1];
            std::cout << "Attempting to open source: " << source << std::endl;

            if (!app.initSource(source))
            {
                std::cerr << "Failed to open source file. Exiting." << std::endl;
                return -1;
            }
            // Second argument can be used to specify the initial virtual object mode
            if (argc > 2)
            {
                objName = argv[2];
                app.setInitialMode(objName); // set the initial mode
            }
        }
        else
        {
            std::cout << "No source provided. Opening default camera..." << std::endl;
            if (!app.initCamera(0))
            {
                std::cerr << "Failed to open camera. Exiting." << std::endl;
                return -1;
            }
        }

        // static image or video stream provided
        app.isProcessingImage = (argc > 1);

        while (true)
        {
            if (app.run() == -1)
                break;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}