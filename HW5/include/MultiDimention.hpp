#pragma once
#include <Method.hpp>
#include <random>
#include <set>
#include <eigen-3.3.9/Eigen/Dense>
using namespace std;

class MultiDimention: public Method
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

    double x_offset;
    double y_offset;
    double lambda;
    double alpha;
    vector<double> cluster;
    vector<double> parallel;
    string method;

public:
    void makeVertices();
    void bindVertices();
    void draw(glm::mat4, glm::mat4, const vector<float>, bool, float, float, float, float, bool);
    void setShader();
    Shader* getShader() const;

    MultiDimention(const MultiDimData*, double, double, string);
    ~MultiDimention();

    void unionCluser(int, int);
    bool findCluser(int, int);
    void normalize();
    void preprocessing();
    void makeParallel();
    void SammonMapping2D();
    void PCA();
    void make1DTexture();
};