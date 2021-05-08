#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.hpp>

using namespace std;

class Isosurface
{
private:
    vector<vector<int>> lookUpTable;
    vector<vector<int>> offsetFromBaseVertex;
    vector<vector<int>> vertexOfEdges;
    vector<vector<vector<float>>> data;
    vector<vector<vector<vector<float>>>> gradient;
    vector<float> voxelSize;
    float isovalue;

    Shader *shader;
    unsigned int VAO;
    unsigned int VBO;
    vector<float> vertices;

public:
    Isosurface(vector<vector<vector<float>>>, vector<vector<vector<vector<float>>>>, vector<float>, float);

    void marchingCube();
    void marchSingleCube(float, float, float);
    void bindVertices();
    void draw(glm::mat4, glm::mat4, vector<float>, bool);
    
    void setVoxelSize(vector<float>);
    void setIsovalue(float);
    void setVertices(vector<float>, vector<int>);
    void setShader(string, string);
    void setClippingNormal(int, bool);
    void setClippingValue(bool);

    vector<vector<float>> getGradient(vector<float>);
    Shader * getShader() { return shader; }
};