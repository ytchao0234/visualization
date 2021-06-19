#pragma once
#include <iostream>
#include <algorithm>
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
#include <StreamLine.hpp>

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
    float gap;
    float adjust;
    float threshold;
    double alpha;
    GLFWwindow* window;
    Camera *camera;
    Light *light;
    CubeModel *cubeModel;
    FileReader *fr;
    Histogram* histogram;
    Heatmap* heatmap;
    vector<Method *> volumeList;
    vector<string> methods;
    vector<string> importList;

public:
    WindowManager(string, int, int, string);
    ~WindowManager();

    void initGUI();
    void renderGUI();
    void makeMainMenu(bool&, bool&, string&, float&);
    void makeGraph(float&);
    void makeCanvas(string);
    void setLt(float&, float);
    void setGt(float&, float);
    vector<pair<glm::vec2, int>>::iterator getIntersectedPoint(vector<pair<glm::vec2, int>>&, glm::vec2);
    vector<glm::vec2>::iterator getIntersectedPoint(vector<glm::vec2>&, glm::vec2);
    void drawPoints(ImDrawList*, vector<glm::vec2>, glm::vec2, ImU32, glm::vec2);
    void drawLine(ImDrawList*, vector<glm::vec2>, glm::vec2, ImU32);

    void initCallbacks();
    void initObjects();
    void initImportList();

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
    float getGap() { return gap; }
    float getAdjust() { return adjust; }
    float getThreshold() { return threshold; }
    double getAlpha() { return alpha; }
    GLFWwindow* getWindow() { return window; }
    Camera* getCamera() { return camera; }
    Light * getLight() { return light; }
    CubeModel* getCube() { return cubeModel; }
    FileReader* getFileReader() { return fr; }
    vector<Method *> getVolumeList() { return volumeList; }
};