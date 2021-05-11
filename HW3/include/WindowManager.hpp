#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Camera.hpp>
#include <Light.hpp>
#include <CubeModel.hpp>
#include <FileReader.hpp>
#include <Histogram.hpp>
#include <Heatmap.hpp>
#include <Method.hpp>
#include <Isosurface.hpp>
#include <RayCasting.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <implot/implot.h>

using namespace std;

class WindowManager
{
private:
    string title;
    int width;
    int height;
    string glslVersion;
    bool leftButtonIsPressing;
    vector<float> clipping;
    bool makeCrossSection;
    GLFWwindow* window;
    Camera *camera;
    Light *light;
    CubeModel *cubeModel;
    FileReader *fr;
    Histogram* histogram;
    Heatmap* heatmap;
    vector<Method *> volumeList;
    vector<string> methods;

public:
    WindowManager(string, int, int, string);
    ~WindowManager();

    void initGUI();
    void renderGUI(vector<string>, vector<string>);

    void initCallbacks();
    void initObjects();

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
    FileReader* getFileReader() { return fr; }
    vector<Method *> getVolumeList() { return volumeList; }
};