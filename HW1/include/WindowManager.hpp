#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Camera.hpp>
#include <CubeModel.hpp>

using namespace std;

class WindowManager
{
private:
    int width;
    int height;
    GLFWwindow* window;
    Camera *camera;
    CubeModel *cubeModel;

public:
    WindowManager(string, int, int);

    void initCallbacks();
    void resizeCallback(GLFWwindow*, int, int);
    void keyCallback(GLFWwindow*, int, int, int, int);
    void mouseCallback(GLFWwindow*, double, double);
    void scrollCallback(GLFWwindow*, double, double);

    void initObjects();

    GLFWwindow* getWindow() { return window; }
    Camera* getCamera() { return camera; }
    CubeModel* getCube() { return cubeModel; }
};