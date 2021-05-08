#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

class Texture
{
private:
    int number;
    unsigned int* tex;

public:
    Texture(int);
    ~Texture();
    void make3DTexture(vector<unsigned int>, int, int, int);
    void make1DTexture(vector<unsigned int>, int);
    void active(int);
};