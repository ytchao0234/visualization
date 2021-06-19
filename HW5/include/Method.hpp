#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VolumeData.hpp>
#include <VectorData.hpp>
#include <MultiDimData.hpp>
#include <Shader.hpp>
#include <Texture.hpp>
using namespace std;

class Method
{
private:
    VolumeData *data1;
    VectorData *data2;
    MultiDimData *data3;

    Shader *shader;
    unsigned int *VAO;
    unsigned int *VBO;
    vector<float> vertices;

    Texture *texture;
    unsigned char (*texture3D)[4];
    unsigned char (*texture2D)[4];
    unsigned char (*texture1D)[4];

public:
    virtual void makeVertices() = 0;
    virtual void bindVertices() = 0;
    virtual void draw(glm::mat4, glm::mat4, const vector<float>, bool, float, float, float, float, bool) = 0;
    virtual void setShader() = 0;
    virtual Shader* getShader() const = 0;
};