#pragma once
#include <Method.hpp>
#include <set>
#include <algorithm>
using namespace std;

class StreamLine: public Method
{
private:
    VolumeData *data1;
    VectorData *data2;
    MultiDimData *data3;

    Shader *shader;
    unsigned int *VAO;
    unsigned int *VBO;
    vector<double> vertices;

    Texture *texture;
    unsigned char (*texture3D)[4];
    unsigned char (*texture2D)[4];
    unsigned char (*texture1D)[4];

    double h;
    int iteration;
    int minLength;
    double gridSize;
    double distanceLimit;
    vector<vector<bool>> collisionTable;
    double valueMax;
    bool useDefault_U;

public:
    void makeVertices();
    void bindVertices();
    void draw(glm::mat4, glm::mat4, const vector<float>, bool, float, float, float, float, bool);
    void setShader();
    Shader* getShader() const;

    StreamLine(const VectorData*, double, int, int, double, double, bool = false);
    ~StreamLine();

    glm::dvec2 interpolate(double, double);
    vector<double> forward(double, double, set<pair<int, int>>&);
    vector<double> backward(double, double, set<pair<int, int>>&);
    void makeSingleLine(double, double);

    glm::dvec2 default_U(double, double);
    vector<double> forward_U(double, double, set<pair<int, int>>&);
    vector<double> backward_U(double, double, set<pair<int, int>>&);
    void makeSingleLine_U(double, double);
    void make1DTexture();
};