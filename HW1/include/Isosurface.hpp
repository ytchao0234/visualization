#pragma once
#include <vector>

using namespace std;

class Isosurface
{
private:
    vector<vector<int>> lookUpTable;
    vector<vector<vector<float>>> data;
    float isovalue;
    vector<float> vertices;

public:
    Isosurface(vector<vector<vector<float>>>, float);

    void marchingCube();
    void draw();
    
    void setIsovalue();
};