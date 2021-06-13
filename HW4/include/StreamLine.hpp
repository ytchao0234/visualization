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
    double gridSize;
    double distanceLimit;
    vector<vector<bool>> collisionTable;
    double valueMax;
    bool useDefault_U;

public:
    void makeVertices();
    void bindVertices();
    void draw(glm::mat4, glm::mat4, const vector<float>, bool, float, float, float);
    void setShader();
    Shader* getShader() const;

    StreamLine(const VectorData*, double, int, double, double, bool);
    ~StreamLine();

    void makeSingleLine(double, double);
    bool hasCollision(double, double);
    void make1DTexture();
    pair<double, double> default_U(double, double);
};