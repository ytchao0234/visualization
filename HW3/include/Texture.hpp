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
    // int number;
    // unsigned int* tex;
    unsigned int tex3D;
    unsigned int tex2D;
    unsigned int tex1D;

public:
    Texture(int);
    ~Texture();
    void make3DTexture(unsigned char (*)[4], int, int, int);
    void make1DTexture(unsigned char (*)[4], int);
    void active1D(int);
    void active2D(int);
    void active3D(int);
};