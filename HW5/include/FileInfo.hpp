#pragma once
#include <iostream>
#include <vector>

using namespace std;

class FileInfo
{
private:
    vector<int> resolution;
    string valueType;
    vector<float> voxelSize;
    string endian;

public:
    FileInfo() :resolution({0,0,0}),
                valueType("ub"),
                voxelSize({1.0, 1.0, 1.0}),
                endian("little") {}

    void init();
    void print() const;

    void setResolution(vector<int> value) { resolution.assign(value.begin(), value.end()); }
    void setValueType(string value) { valueType = value; }
    void setVoxelSize(vector<float> value) { voxelSize.assign(value.begin(), value.end()); }
    void setEndian(string value) { endian = value; }

    vector<int> getResolution() const { return resolution; }
    string getValueType() const { return valueType; }
    int getSizeOfValueType() const;
    vector<float> getVoxelSize() const { return voxelSize; }
    string getEndian() const { return endian; }
};