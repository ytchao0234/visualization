#pragma once
#include <Method.hpp>
#include <set>
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
    int gridSize;
    double distanceLimit;
    vector<vector<bool>> collisionTable;

public:
    void makeVertices();
    void bindVertices();
    void draw(glm::mat4, glm::mat4, const vector<float>, bool, float, float, float);
    void setShader();
    Shader* getShader() const;

    StreamLine(const VectorData*, double, int, double);
    ~StreamLine();

    void makeSingleLine(double, double);
    bool hasCollision(double, double);
    void make1DTexture();
    void make1DTexture(glm::vec2, vector<glm::vec2>, vector<glm::vec2>, vector<glm::vec2>, vector<glm::vec2>);
    void setColorValue(glm::vec2, vector<glm::vec2>, int);
    double default_U(double, double);
    void generateTestData(int, int, double (*)(double, double) = NULL);
};