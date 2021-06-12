#include <FileReader.hpp>
#include <FileReader.hpp>

FileReader::FileReader(string rootPath)
{
    this->rootPath = rootPath;
    infNameList.clear();
    rawNameList.clear();
    vecNameList.clear();
    info = new FileInfo();
    data1 = new VolumeData();
    data2 = new VectorData();
    endian = isBigEndian() ? "big" : "little";
}

FileReader::~FileReader()
{
    delete info;
    delete data1;
    delete data2;
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

    dirp = opendir((rootPath + "Volume/").c_str());

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

    dirp = opendir((rootPath + "Vector/").c_str());

    if (dirp == NULL) 
    {
        cout << "Error(" << errno << ") opening " << rootPath.c_str() << endl;
        perror("opendir");

        return;
    }

    while ((entry = readdir(dirp)) != NULL) {
        if(strstr(entry->d_name, ".vec") != NULL)
        {
            fullname = entry->d_name;
            vecNameList.push_back(fullname.substr(0, fullname.length() - 4));
        }
    }

    closedir(dirp);
}

void FileReader::readFile(string filename)
{
    filename = rootPath + "Vector/" + filename;
    filename += ".vec";

    ifstream fs;
    fs.open(filename.c_str(), ios::in | ios::binary );

    if (!fs.is_open())
    {
        cout << "ERROR::FILEREADER::OPENFILE::" << filename.c_str() << "_FAILED"<< endl;
        exit(1);
    }

    string content;
    getline(fs, content);
    fs.close();

    istringstream ss(content);
    double value1, value2;
    int i = 0;

    ss >> value1 >> value2;

    this->data2->size = {value1, value2};
    this->data2->value.assign(this->data2->size.first,
                              vector<pair<double, double>>(this->data2->size.second));
    
    for(int x = 0; x < this->data2->size.first; x++)
    for(int y = 0; y < this->data2->size.second; y++)
    {
        ss >> value1 >> value2;

        this->data2->value[x][y] = {value1, value2};

        i++;
    }
    
    data2->print();
}

void FileReader::readFile(string infFile, string rawFile)
{
    readInf(infFile);
    readRawData(rawFile);
}

void FileReader::readInf(string filename)
{
    info->init();

    filename = rootPath + "Volume/" + filename;
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
            info->setResolution({stoi(matches[3]), stoi(matches[2]), stoi(matches[1])});
            data1->setResolution(stoi(matches[3]), stoi(matches[2]), stoi(matches[1]));
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
            info->setVoxelSize({stof(matches[4]), stof(matches[3]), stof(matches[2])});
            data1->setVoxelSize(stof(matches[4]), stof(matches[3]), stof(matches[2]));
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

vector<string> FileReader::getInfNameList() const
{
    return infNameList;
}

vector<string> FileReader::getRawNameList() const
{
    return rawNameList;
}

vector<string> FileReader::getVecNameList() const
{
    return vecNameList;
}

FileInfo* FileReader::getInfo() const
{
    return info;
}

VolumeData* FileReader::getVolumeData() const
{
    return data1;
}

VectorData* FileReader::getVectorData() const
{
    return data2;
}
