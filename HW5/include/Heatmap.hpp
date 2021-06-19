#pragma once
#include <iostream>
#include <vector>
#include <VolumeData.hpp>
using namespace std;

class Heatmap
{
private:
    vector<float> data;
    int rangeMin_intensity;
    int rangeMax_intensity;
    int rangeOffset_intensity;
    int rangeMin_gradMag;
    int rangeMax_gradMag;
    float maxLimit_gradMag;
    float valueMin;
    float valueMax;
    
public:
    Heatmap(const VolumeData*, float);
    ~Heatmap();

    vector<float> getData() const;
    int getRangeMin(string) const;
    int getRangeMax(string) const;
    int getRangeOffset() const;
    float getMaxLimit() const;
    float getValueMin() const;
    float getValueMax() const;
};