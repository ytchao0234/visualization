#pragma once

#include <glad/glad.h> 
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;
  
class Shader
{
public:
    unsigned int programID;
  
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
    void use();

    void setBool(const string &, bool) const;
    void setInt(const string &, int) const;
    void setUInt(const string &, unsigned int) const;
    void setFloat(const string &, float) const;

    void setBoolVec(const string &, vector<bool>, int) const;
    void setIntVec(const string &, vector<int>, int) const;
    void setUIntVec(const string &, vector<unsigned int>, int) const;
    void setFloatVec(const string &, vector<float>, int) const;
    void setMatrix4(const string &, vector<vector<float>>) const;
    void setMatrix4(const string &, float *) const;
};