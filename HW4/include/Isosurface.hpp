#pragma once
#include <Method.hpp>
using namespace std;

class Isosurface: public Method
{
private:
    VolumeData *data;

    Shader *shader;
    unsigned int *VAO;
    unsigned int *VBO;
    vector<float> vertices;

    Texture *texture;
    unsigned char (*texture3D)[4];
    unsigned char (*texture1D)[4];

    int isovalue;

    static vector<vector<int>> lookUpTable;
    static vector<vector<int>> offsetFromBaseVertex;
    static vector<vector<int>> vertexOfEdges;

public:
    void makeVertices();
    void bindVertices();
    virtual void draw(glm::mat4, glm::mat4, const vector<float>, bool, float, float, float);
    void setShader();
    Shader* getShader() const;

    Isosurface(const VolumeData*, int);
    ~Isosurface();
    
    void marchingCube();
    void marchSingleCube(int, int, int);
    void setVertices(vector<int>, vector<int>);
    void setIsovalue(float);
};