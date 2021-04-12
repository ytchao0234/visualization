#include <FileReader.hpp>

FileReader::FileReader(string rootPath)
{
    this->rootPath = rootPath;
    infNameList.clear();
    rawNameList.clear();
    info = new FileInfo();
    data.clear();
    endian = isBigEndian() ? "big" : "little";
    histogram.clear();
    minValue = 0;
    maxValue = 0;
    offset = 0;
    maxNum = 0;
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