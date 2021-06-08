#pragma once
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
using namespace std;

class VolumeData
{
public:
    glm::ivec3 resolution;
    glm::vec3 voxelSize;

    vector<vector<vector<float>>> data;
    float dataMin;
    float dataMax;

    vector<vector<vector<vector<float>>>> gradient;
    vector<vector<vector<float>>> gradMag;
    float gradMin;
    float gradMax;

    VolumeData();
    VolumeData(const VolumeData*);
    ~VolumeData();
    void setResolution(int, int, int);
    void setVoxelSize(float, float, float);
    void setData(vector<vector<vector<float>>>);
    void setGradient();
};