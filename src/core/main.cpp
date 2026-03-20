/*
Claire Liu, Yu-Jing Wei
main.cpp

Path: src/core/main.cpp
Description: Entry point for the augmented reality application.
*/
#include "ARApp.hpp"

/*
main function that initializes and runs the ARApp.
It also catches any exceptions thrown during execution and prints an
error message before exiting with a non-zero status code.
*/
int main()
{
    try
    {
        ARApp app;

        if (!app.initCamera(0))
            return -1;

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
}