#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <dirent/dirent.h>
#include <regex>
#include <vector>
#include <FileInfo.hpp>

using namespace std;

class FileReader
{
private:
    string rootPath;
    vector<string> nameList;
    FileInfo *info;
    vector<vector<vector<float>>> data;
    string endian;

public:
    FileReader(string);

    void initNameList();

    bool isBigEndian();
    void readInf(string);

    void readRawData(string);
    template<typename T>
    void readRawData(string);
    void printRawData() const;

    FileInfo * getInfo() const { return info; }
    vector<vector<vector<float>>> getData() const { return data; }
};

template<typename T>
void FileReader::readRawData(string filename)
{
    readInf(filename);
    this->info->print();

    cout << "\n--------------------------------------------\n";

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
    {
        for(int y = 0; y < res[1]; y++)
        {
            for(int z = 0; z < res[0]; z++)
            {
                if(toReverse)
                    reverse(buffer + pointer, buffer + pointer + sizeOfValue);

                T rawData;
                memcpy(&rawData, buffer + pointer, sizeOfValue);
                pointer += sizeOfValue;

                data[x][y][z] = (float) rawData;
            }
        }
    }
}