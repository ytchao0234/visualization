#pragma once
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
using namespace std;

class VolumeData
{
private:
    glm::vec3 resolution;
    glm::vec3 voxelSize;

    vector<vector<vector<float>>> data;
    float dataMin;
    float dataMax;

    vector<vector<vector<vector<float>>>> gradient;
    vector<vector<vector<float>>> gradMag;
    float gradMin;
    float gradMax;

public:
    VolumeData();
    VolumeData(const VolumeData*);
    ~VolumeData();
    void setResolution(int, int, int);
    void setVoxelSize(float, float, float);
    void setData(vector<vector<vector<float>>>);
    void setGradient();
    
    glm::vec3 getResolution() const;
    glm::vec3 getVoxelSize() const;
    vector<vector<vector<float>>> getData() const;
    float getDataMin() const;
    float getDataMax() const;
    vector<vector<vector<vector<float>>>> getGradient() const;
    vector<vector<vector<float>>> getGradMag() const;
    float getGradMin() const;
    float getGradMax() const;
};