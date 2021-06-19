#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

class Texture
{
private:
    int number;
    unsigned int *tex;

public:
    Texture(int);
    ~Texture();
    void make3DTexture(int, unsigned char (*)[4], int, int, int);
    void make2DTexture(int, unsigned char (*)[4], int, int);
    void make1DTexture(int, unsigned char (*)[4], int);
    void active1D(int);
    void active2D(int);
    void active3D(int);
};