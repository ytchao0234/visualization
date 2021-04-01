#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Camera.hpp>
#include <Light.hpp>
#include <CubeModel.hpp>
#include <FileReader.hpp>
#include <Isosurface.hpp>

using namespace std;

class WindowManager
{
private:
    int width;
    int height;
    bool leftButtonIsPressing;
    vector<float> clipping;
    bool makeCrossSection;
    GLFWwindow* window;
    Camera *camera;
    Light *light;
    CubeModel *cubeModel;
    FileReader *fr;
    vector<Isosurface *> iso;

public:
    WindowManager(string, int, int);

    void initCallbacks();
    void initObjects(string, float);
    void resizeCallback(GLFWwindow*, int, int);
    void keyCallback(GLFWwindow*, int, int, int, int);
    void checkKeyPress();
    void mousePressCallback(GLFWwindow*, int, int, int);
    void mouseMoveCallback(GLFWwindow*, double, double);
    void scrollCallback(GLFWwindow*, double, double);

    void setClippingNormal(int, bool);
    void setClippingValue(bool);

    int getWidth() { return width; }
    int getHeight() { return height; }
    vector<float> getClipping() { return clipping; }
    bool getMakeCrossSection() { return makeCrossSection; }
    GLFWwindow* getWindow() { return window; }
    Camera* getCamera() { return camera; }
    Light * getLight() { return light; }
    CubeModel* getCube() { return cubeModel; }
    vector<Isosurface *> getIsosurface() { return iso; }
};