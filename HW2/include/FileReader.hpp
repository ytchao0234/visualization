#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <FileInfo.hpp>

#define WINDOWS_SYSTEM
#ifdef WINDOWS_SYSTEM
    #include <dirent/dirent.h>
#else
    #include <dirent.h>
#endif

using namespace std;

class FileReader
{
private:
    string rootPath;
    vector<string> infNameList;
    vector<string> rawNameList;
    FileInfo *info;
    string endian;
    vector<vector<vector<float>>> data;
    vector<vector<vector<vector<float>>>> dataGradient;

    vector<float> iHistogram;
    vector<float> logIHistogram;
    float iMin;
    float iMax;
    int offset;
    float iMaxNum;
    float logIMaxNum;

    vector<vector<float>> heatMap;
    float* heatMap_array;
    float gMin;
    float gMax;
    float hMaxNum;

public:
    float gMaxLimit;

    FileReader(string);

    void initNameList();
    bool isBigEndian();
    
    void readFile(string, string);
    void readInf(string);
    void readRawData(string);
    template<typename T>
    void readRawData(string);
    void calcuGradient();
    void calcuGraph();
    void printRawData() const;

    void setGMaxLimit(float limit) { gMaxLimit = limit; }

    vector<string> getInfNameList() const { return infNameList; }
    vector<string> getRawNameList() const { return rawNameList; }
    FileInfo * getInfo() const { return info; }
    vector<vector<vector<float>>> getData() const { return data; }
    vector<vector<vector<vector<float>>>> getDataGradient() const { return dataGradient; }
    vector<float> getIHistogram() const { return iHistogram; }
    vector<float> getLogIHistogram() const { return logIHistogram; }
    float getIMin() const { return iMin; }
    float getIMax() const { return iMax; }
    float getIMaxNum() const { return iMaxNum; }
    float getLogIMaxNum() const { return logIMaxNum; }
    int getOffset() const { return offset; }
    float* getHeatMap() const { return heatMap_array; }
    int getHeatMapRows() const { return heatMap.size(); }
    int getHeatMapCols() const { return heatMap[0].size(); }
    float getGMin() const { return gMin; }
    float getGMax() const { return gMax; }
    float getDecibelMin() const { return (gMin < 1.0f) ? 0.0f: 20 * log(gMin); }
    float getDecibelMax() const { return 20 * log(gMaxLimit); }
    float getHMaxNum() const { return hMaxNum; }
};

template<typename T>
void FileReader::readRawData(string filename)
{
    filename = rootPath + filename;
    filename += ".raw";

    ifstream fs;
    fs.open(filename.c_str(), ios::in | ios::binary );

    if (!fs.is_open())
    {
        cout << "ERROR::FILEREADER::OPENFILE::" << filename.c_str() << "_FAILED"<< endl;
        exit(1);
    }

    vector<int> res = info->getResolution();
    int sizeOfValue = info->getSizeOfValueType();
    long long int byteCount = sizeOfValue * res[0] * res[1] * res[2];
    char *buffer = new char[byteCount];
    
    fs.read(buffer, byteCount);
    fs.close();

    data.assign(res[2], vector<vector<float>>(res[1], vector<float>(res[0])));
    long long int pointer = 0;
    bool toReverse = (endian != info->getEndian()) ? true : false;

    for(int x = 0; x < res[2]; x++)
    for(int y = 0; y < res[1]; y++)
    for(int z = 0; z < res[0]; z++)
    {
        if(toReverse)
            reverse(buffer + pointer, buffer + pointer + sizeOfValue);

        T rawData;
        memcpy(&rawData, buffer + pointer, sizeOfValue);
        pointer += sizeOfValue;

        data[x][y][z] = (float) rawData;
    }

    calcuGradient();
    calcuGraph();
}