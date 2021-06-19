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
#include <VolumeData.hpp>
#include <VectorData.hpp>
#include <MultiDimData.hpp>

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
    vector<string> vecNameList;
    vector<string> datNameList;
    FileInfo *info;
    VolumeData *data1;
    VectorData *data2;
    MultiDimData *data3;
    string endian;

public:
    FileReader(string);
    ~FileReader();

    void initNameList();
    bool isBigEndian();
    
    void readFile(string, string);
    void readFile(string, bool = true);
    void readInf(string);
    void readRawData(string);
    template<typename T>
    void readRawData(string);

    vector<string> getInfNameList() const;
    vector<string> getRawNameList() const;
    vector<string> getVecNameList() const;
    vector<string> getDatNameList() const;
    FileInfo* getInfo() const;
    VolumeData* getVolumeData() const;
    VectorData* getVectorData() const;
    MultiDimData* getMultiDimData() const;
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
    
    int sizeOfValue = info->getSizeOfValueType();
    long long int byteCount = sizeOfValue * data1->resolution.x * data1->resolution.y * data1->resolution.z;
    char *buffer = new char[byteCount];
    
    fs.read(buffer, byteCount);
    fs.close();

    vector<vector<vector<float>>> vertexValues(data1->resolution.x, vector<vector<float>>(data1->resolution.y, vector<float>(data1->resolution.z)));
    long long int pointer = 0;
    bool toReverse = (endian != info->getEndian()) ? true : false;

    for(int x = 0; x < data1->resolution.x; x++)
    for(int y = 0; y < data1->resolution.y; y++)
    for(int z = 0; z < data1->resolution.z; z++)
    {
        if(toReverse)
            reverse(buffer + pointer, buffer + pointer + sizeOfValue);

        T rawData;
        memcpy(&rawData, buffer + pointer, sizeOfValue);
        pointer += sizeOfValue;

        vertexValues[x][y][z] = (float) rawData;
    }

    data1->setValue(vertexValues);
    data1->setGradient();
}