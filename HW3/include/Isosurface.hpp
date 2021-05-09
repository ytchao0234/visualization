#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.hpp>
#include <Texture.hpp>

using namespace std;

class Isosurface
{
private:
    string method;
    unsigned int methodNum;
    vector<vector<int>> lookUpTable;
    vector<vector<int>> offsetFromBaseVertex;
    vector<vector<int>> vertexOfEdges;
    vector<vector<vector<float>>> data;
    int dataMin;
    int dataMax;
    vector<vector<vector<vector<float>>>> gradient;
    vector<float> voxelSize;
    float isovalue;
    unsigned char (*texture3D)[4];
    unsigned char (*texture1D)[4];
    Texture *textures;

    Shader *shader;
    unsigned int *VAO;
    unsigned int *VBO;
    vector<float> vertices;

public:
    Isosurface(vector<vector<vector<float>>>, int, int, vector<vector<vector<vector<float>>>>, vector<float>, string, float);
    ~Isosurface();

    void makeVertices();
    void marchingCube();
    void marchSingleCube(float, float, float);
    void bindVertices();
    void make3DTexture();
    void make1DTexture();
    void rayCasting();
    void draw(glm::mat4, glm::mat4, vector<float>, bool);
    void drawMarchingCube(vector<float>, bool);
    void drawRayCasting();
    
    void setMethod(string);
    void setVoxelSize(vector<float>);
    void setIsovalue(float);
    void setVertices(vector<float>, vector<int>);
    void setShader();
    void setClippingNormal(int, bool);
    void setClippingValue(bool);

    vector<vector<float>> getGradient(vector<float>);
    Shader * getShader() { return shader; }
};