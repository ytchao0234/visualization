#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <VolumeData.hpp>
using namespace std;

class Histogram
{
private:
    vector<float> data_origin;
    int rangeMin_origin;
    int rangeMax_origin;
    int rangeOffset_origin;
    float valueMin_origin;
    float valueMax_origin;
    
    vector<float> data_log;
    int rangeMin_log;
    int rangeMax_log;
    int rangeOffset_log;
    float valueMin_log;
    float valueMax_log;
    
    vector<float> data_equal;
    int rangeMin_equal;
    int rangeMax_equal;
    int rangeOffset_equal;
    float valueMin_equal;
    float valueMax_equal;
    
public:
    Histogram(const VolumeData*);
    ~Histogram();

    vector<float> getData(string) const;
    int getRangeMin(string) const;
    int getRangeMax(string) const;
    int getRangeOffset(string) const;
    float getValueMin(string) const;
    float getValueMax(string) const;
};