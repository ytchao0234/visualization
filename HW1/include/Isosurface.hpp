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
    vector<float> voxelSize;
    float isovalue;
    unsigned int VAO;
    unsigned int VBO;
    vector<float> vertices;

public:
    Isosurface(vector<vector<vector<float>>>, vector<float>, float);

    void marchingCube();
    void marchSingleCube(float, float, float);
    void bindVertices();
    void draw(glm::mat4, glm::mat4);
    
    void setIsovalue();
    void setVertices(vector<float>, vector<int>);
};