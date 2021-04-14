#include <FileReader.hpp>

FileReader::FileReader(string rootPath)
{
    this->rootPath = rootPath;
    infNameList.clear();
    rawNameList.clear();
    info = new FileInfo();
    data.clear();
    dataGradient.clear();
    endian = isBigEndian() ? "big" : "little";
    iHistogram.clear();
    logIHistogram.clear();
    iMin = 0;
    iMax = 0;
    offset = 0;
    iMaxNum = 0;
    logIMaxNum = 0;
    gMin = 0;
    gMax = 0;
    gMaxLimit = 0;
}

bool FileReader::isBigEndian()
{
    union testValue
    {
        uint32_t value;
        unsigned char eachByte[4];
    };

    union testValue test;
    test.value = 0x12345678;

    if(test.eachByte[0] == 0x12 && test.eachByte[1] == 0x34
    && test.eachByte[2] == 0x56 && test.eachByte[3] == 0x78)
    {
        return true;
    }
    else return false;
}

void FileReader::initNameList()
{
    DIR * dirp;
    struct dirent * entry;

    string fullname = "";

    dirp = opendir(rootPath.c_str());

    if (dirp == NULL) 
    {
        cout << "Error(" << errno << ") opening " << rootPath.c_str() << endl;
        perror("opendir");

        return;
    }

    while ((entry = readdir(dirp)) != NULL) {
        if(strstr(entry->d_name, ".inf") != NULL)
        {
            fullname = entry->d_name;
            infNameList.push_back(fullname.substr(0, fullname.length() - 4));
        }
        else if(strstr(entry->d_name, ".raw") != NULL)
        {
            fullname = entry->d_name;
            rawNameList.push_back(fullname.substr(0, fullname.length() - 4));
        }
    }

    closedir(dirp);
}

void FileReader::readFile(string infFile, string rawFile)
{
    readInf(infFile);
    readRawData(rawFile);
}

void FileReader::readInf(string filename)
{
    info->init();

    filename = rootPath + filename;
    filename += ".inf";

    ifstream fs;
    fs.open(filename.c_str(), ios::in | ios::binary );

    if (!fs.is_open())
    {
        cout << "ERROR::FILEREADER::OPENFILE::" << filename.c_str() << "_FAILED"<< endl;
        exit(1);
    }

    string line;
    smatch matches;
    regex reg_resolution("^resolution.*?(\\d+).*?(\\d+).*?(\\d+)");
    regex reg_valueType("^(sample-?type|value-?type).*?((\\w+).?(\\w+)?)");
    regex reg_voxelSize("^(voxel-?size|ratio).*?(\\d+\\.\\d+|\\d+).(\\d+\\.\\d+|\\d+).(\\d+\\.\\d+|\\d+)");
    regex reg_endian("^endian.*?(\\w+)");

    while(getline(fs, line))
    {
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        
        if(regex_search(line, matches, reg_resolution))
        {
            info->setResolution({stoi(matches[1]), stoi(matches[2]), stoi(matches[3])});
        }

        else if(regex_search(line, matches, reg_valueType))
        {
            string origin = matches[2];
            string vtype = "";

            if(strstr(origin.c_str(), "unsigned") != NULL)
            {
                if(     strstr(origin.c_str(), "char"    ) != NULL) vtype = "ub";
                else if(strstr(origin.c_str(), "short"   ) != NULL) vtype = "us";
                else if(strstr(origin.c_str(), "int"     ) != NULL) vtype = "ui";
            }
            else
            {
                if(     strstr(origin.c_str(), "char"    ) != NULL) vtype = "b";
                else if(strstr(origin.c_str(), "short"   ) != NULL) vtype = "s";
                else if(strstr(origin.c_str(), "int"     ) != NULL) vtype = "i";
                else if(strstr(origin.c_str(), "float"   ) != NULL) vtype = "f";
                else vtype = origin;
            }
            
            info->setValueType(vtype);
        }

        else if(regex_search(line, matches, reg_voxelSize))
        {
            info->setVoxelSize({stof(matches[2]), stof(matches[3]), stof(matches[4])});
        }

        else if(regex_search(line, matches, reg_endian))
        {
            string endian = matches[1];

            if(endian[0] == 'b') endian = "big";
            else if(endian[0] == 'l') endian = "little";

            info->setEndian(endian);
        }
    }

    fs.close();

    info->print();
}

void FileReader::readRawData(string filename)
{
    if(info->getValueType() == "ub")
        readRawData<unsigned char>(filename);
    else if(info->getValueType() == "us")
        readRawData<unsigned short>(filename);
    else if(info->getValueType() == "ui")
        readRawData<unsigned int>(filename);
    else if(info->getValueType() == "b")
        readRawData<char>(filename);
    else if(info->getValueType() == "s")
        readRawData<short>(filename);
    else if(info->getValueType() == "i")
        readRawData<int>(filename);
    else if(info->getValueType() == "f")
        readRawData<float>(filename);
}

void FileReader::calcuGradient()
{
    dataGradient.assign(data.size(),
                        vector<vector<vector<float>>>(data[0].size(),
                        vector<vector<float>>(data[0][0].size(),
                        vector<float>(3, 0))));

    vector<float> voxelSize = info->getVoxelSize();

    for( int x = 0; x < (int)data.size() - 1; x++ )
    for( int y = 0; y < (int)data[0].size() - 1; y++ )
    for( int z = 0; z < (int)data[0][0].size() - 1; z++ )
    {
        if( x-1 > 0 && x+1 < data.size() )
            dataGradient[x][y][z][0] = (data[x+1][y][z] - data[x-1][y][z]) / voxelSize[2] / 2.0f;
        else if( x+1 < data.size())
            dataGradient[x][y][z][0] = (data[x+1][y][z] - data[x][y][z]) / voxelSize[2];
        else if( x-1 > 0 )
            dataGradient[x][y][z][0] = (data[x][y][z] - data[x-1][y][z]) / voxelSize[2];
        else
            cout << "dataGradient not found: x = " << x << endl;

        if( y-1 > 0 && y+1 < data[0].size() )
            dataGradient[x][y][z][1] = (data[x][y+1][z] - data[x][y-1][z]) / voxelSize[1] / 2.0f;
        else if( y+1 < data[0].size())
            dataGradient[x][y][z][1] = (data[x][y+1][z] - data[x][y][z]) / voxelSize[1];
        else if( y-1 > 0 )
            dataGradient[x][y][z][1] = (data[x][y][z] - data[x][y-1][z]) / voxelSize[1];
        else
            cout << "dataGradient not found: y = " << y << endl;

        if( z-1 > 0 && z+1 < data[0][0].size() )
            dataGradient[x][y][z][2] = (data[x][y][z+1] - data[x][y][z-1]) / voxelSize[0] / 2.0f;
        else if( z+1 < data[0][0].size())
            dataGradient[x][y][z][2] = (data[x][y][z+1] - data[x][y][z]) / voxelSize[0];
        else if( z-1 > 0 )
            dataGradient[x][y][z][2] = (data[x][y][z] - data[x][y][z-1]) / voxelSize[0];
        else
            cout << "dataGradient not found: z = " << z << endl;
    }
}

void FileReader::printRawData() const
{
    for(auto x: data)
    {
        for(auto y: x)
        {
            for(auto z: y)
            {
                cout << z << " ";
            }
        }
    }
}

void FileReader::calcuGraph()
{
    iMin = numeric_limits<float>::max();
    iMax = numeric_limits<float>::min();

    glm::vec3 grad;
    float magnitude;
    gMin = numeric_limits<float>::max();
    gMax = numeric_limits<float>::min();

    for(int x = 0; x < (int)data.size(); x++)
    for(int y = 0; y < (int)data[0].size(); y++)
    for(int z = 0; z < (int)data[0][0].size(); z++)
    {
        iMin = min(iMin, data[x][y][z]);
        iMax = max(iMax, data[x][y][z]);

        grad = glm::vec3(dataGradient[x][y][z][0],
                         dataGradient[x][y][z][1],
                         dataGradient[x][y][z][2]);

        magnitude = glm::distance(grad, glm::vec3(0.0f));

        gMin = min(gMin, magnitude);
        gMax = max(gMax, magnitude);
    }

    if(gMin < 1.0f) gMin = 1.0f;
    
    if(gMaxLimit - 0.0f < 0.001f) gMaxLimit = gMax;
    else if(gMaxLimit > gMax) gMaxLimit = gMax;
    else if(gMaxLimit < gMin) gMaxLimit = gMin;

    offset = 0 - (int)iMin;
    iHistogram.assign((int)(iMax - iMin) + 1, 0);

    for(int x = 0; x < (int)data.size(); x++)
    for(int y = 0; y < (int)data[0].size(); y++)
    for(int z = 0; z < (int)data[0][0].size(); z++)
    {
        iHistogram[(int)data[x][y][z] + offset] ++;
    }

    logIHistogram.assign(iHistogram.begin(), iHistogram.end());

    iMaxNum = 0;
    logIMaxNum = 0;

    for(int i = 0; i < (int)logIHistogram.size(); i++)
    {
        if(logIHistogram[i] > 0.0f) 
            logIHistogram[i] = logf(logIHistogram[i]);
        
        iMaxNum = max(iMaxNum, iHistogram[i]);
        logIMaxNum = max(logIMaxNum, logIHistogram[i]);
    }

    heatMap.assign((int)(20*log2f(gMaxLimit) - 20*log2f(gMin)) + 1, vector<float>(iHistogram.size(), 0));

    hMaxNum = 0;

    for(int x = 0; x < (int)data.size(); x++)
    for(int y = 0; y < (int)data[0].size(); y++)
    for(int z = 0; z < (int)data[0][0].size(); z++)
    {
        grad = glm::vec3(dataGradient[x][y][z][0],
                         dataGradient[x][y][z][1],
                         dataGradient[x][y][z][2]);

        magnitude = glm::distance(grad, glm::vec3(0.0f));

        if(magnitude < 1.0f) magnitude = 1.0f;
        else if(magnitude > gMaxLimit) magnitude = gMaxLimit;
        magnitude = 20*log2f(magnitude);

        heatMap[(int)magnitude][(int)data[x][y][z] + offset] ++;
    }

    static float* temp = NULL;
    
    if(temp) delete[] temp;
    temp = new float[heatMap.size() * heatMap[0].size()];
            
    for(int i = 0; i < (int)(heatMap.size() * heatMap[0].size()); i++)
    {
        temp[i] = heatMap[(heatMap.size() - 1) - (i / heatMap[0].size())][i % heatMap[0].size()];

        if( temp[i] > 0.0f ) temp[i] = logf(temp[i]);
        
        hMaxNum = max(hMaxNum, temp[i]);
    }

    heatMap_array = temp;
}