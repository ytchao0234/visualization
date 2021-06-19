#pragma once
#include <iostream>
#include <vector>
using namespace std;

class MultiDimData
{
public:
    pair<int, int> size;
    vector<vector<double>> value;
    
    vector<vector<double>> distance;
    double totalDist;

    MultiDimData();
    MultiDimData(const MultiDimData*);
    ~MultiDimData();
    void print();
};