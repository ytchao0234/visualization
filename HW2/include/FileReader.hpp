#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <vector>
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
    vector<vector<vector<float>>> data;
    vector<float> histogram;
    float minValue;
    float maxValue;
    int offset;
    int maxNum;
    string endian;

public:
    FileReader(string);

    void initNameList();
    bool isBigEndian();
    
    void readFile(string, string);
    void readInf(string);
    void readRawData(string);
    template<typename T>
    void readRawData(string);
    void printRawData() const;

    vector<string> getInfNameList() const { return infNameList; }
    vector<string> getRawNameList() const { return rawNameList; }
    FileInfo * getInfo() const { return info; }
    vector<float> getHistogram() const { return histogram; }
    float getMinValue() const { return minValue; }
    float getMaxValue() const { return maxValue; }
    float getMaxNum() const { return maxNum; }
    int getOffset() const { return offset; }
    vector<vector<vector<float>>> getData() const { return data; }
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

    minValue = numeric_limits<float>::max();
    maxValue = numeric_limits<float>::min();

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

        minValue = min(minValue, data[x][y][z]);
        maxValue = max(maxValue, data[x][y][z]);
    }

    offset = 0 - (int)minValue;
    maxNum = 0;
    histogram.assign((int)(maxValue - minValue) + 1, 0);

    for(int x = 0; x < res[2]; x++)
    for(int y = 0; y < res[1]; y++)
    for(int z = 0; z < res[0]; z++)
    {
        histogram[(int)data[x][y][z] + offset] ++;
    }

    for( int i = 0; i < histogram.size(); i++ )
    {
        histogram[i] = log10(histogram[i]);
        maxNum = max(maxNum, histogram[i]);
    }
}