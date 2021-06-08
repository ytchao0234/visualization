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
    VolumeData *data;
    string endian;

public:
    FileReader(string);
    ~FileReader();

    void initNameList();
    bool isBigEndian();
    
    void readFile(string, string);
    void readInf(string);
    void readRawData(string);
    template<typename T>
    void readRawData(string);

    vector<string> getInfNameList() const;
    vector<string> getRawNameList() const;
    FileInfo* getInfo() const;
    VolumeData* getVolumeData() const;
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
    long long int byteCount = sizeOfValue * data->resolution.x * data->resolution.y * data->resolution.z;
    char *buffer = new char[byteCount];
    
    fs.read(buffer, byteCount);
    fs.close();

    vector<vector<vector<float>>> vertexValues(data->resolution.x, vector<vector<float>>(data->resolution.y, vector<float>(data->resolution.z)));
    long long int pointer = 0;
    bool toReverse = (endian != info->getEndian()) ? true : false;

    for(int x = 0; x < data->resolution.x; x++)
    for(int y = 0; y < data->resolution.y; y++)
    for(int z = 0; z < data->resolution.z; z++)
    {
        if(toReverse)
            reverse(buffer + pointer, buffer + pointer + sizeOfValue);

        T rawData;
        memcpy(&rawData, buffer + pointer, sizeOfValue);
        pointer += sizeOfValue;

        vertexValues[x][y][z] = (float) rawData;
    }

    data->setData(vertexValues);
    data->setGradient();
}