/*
Claire Liu, Yu-Jing Wei
virtualObject.hpp

Path: include/virtualObject.hpp
Description: Declares the virtual object functions for the augmented reality application.
*/

#pragma once // Include guard

class VirtualObject
{
public:
    VirtualObject();
    void render();

private:
    void update();
};