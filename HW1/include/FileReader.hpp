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
    string path;
    vector<string> nameList;

public:
    FileReader(string);

    void initNameList();
    FileInfo * readInf(string);
};