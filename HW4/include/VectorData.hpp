#pragma once
#include <iostream>
#include <vector>
using namespace std;

class VectorData
{
public:
    pair<int, int> size;
    vector<pair<double, double>> value;

    VectorData();
    VectorData(const VectorData*);
    ~VectorData();
    void print();
};