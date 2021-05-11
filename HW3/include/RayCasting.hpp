#pragma once
#include <Method.hpp>
using namespace std;

class RayCasting: public Method
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

public:
    void makeVertices();
    void bindVertices();
    void draw(glm::mat4, glm::mat4, const vector<float>, bool);
    void setShader();
    Shader* getShader() const;

    RayCasting(const VolumeData*);
    ~RayCasting();

    void make3DTexture();
    void make1DTexture();
};