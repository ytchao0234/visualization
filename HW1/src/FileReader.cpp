#include <FileReader.hpp>

FileReader::FileReader(string path)
{
    this->path = path;
    nameList.clear();
}

void FileReader::initNameList()
{
    DIR * dirp;
    struct dirent * entry;

    string fullname = "";

    dirp = opendir(path.c_str());

    if (dirp == NULL) 
    {
        cout << "Error(" << errno << ") opening " << path.c_str() << endl;
        perror("opendir");

        return;
    }

    while ((entry = readdir(dirp)) != NULL) {
        if(strstr(entry->d_name, ".inf") != NULL)
        {
            fullname = entry->d_name;
            nameList.push_back(fullname.substr(0, fullname.length() - 4));
        }
    }

    closedir(dirp);
}

FileInfo * FileReader::readInf(string filename)
{
    FileInfo *info = new FileInfo();

    filename += ".inf";

    ifstream fs;
    fs.open(filename.c_str(), ios::in | ios::binary );

    if (!fs.is_open())
    {
        cout << "ERROR::FILEREADER::OPENFILE::" << filename.c_str() << "_FAILED"<< endl;
        exit(1);
    }

    string line;
    regex reg("");
    smatch matches;

    while(getline(fs, line))
    {
        transform(line.begin(), line.end(), line.begin(), ::tolower);

        reg.assign("^resolution(.*?)(\\d+).(\\d+).(\\d+)");

        if(regex_search(line, matches, reg))
        {
            info->setResolution({stoi(matches[2]), stoi(matches[3]), stoi(matches[4])});
        }

        reg.assign("^(sample-?type|value-?type)(.*?)(\\w+)");

        if(regex_search(line, matches, reg))
        {
            string origin = matches[3];
            string vtype = "";

            if(strstr(origin.c_str(), "unsigned") != NULL)
            {
                vtype = "u";
            }

            if(strstr(origin.c_str(), "char") != NULL)
            {
                vtype += "b";
            }
            else if(strstr(origin.c_str(), "short"))
            {
                vtype += "s";
            }
            else if(strstr(origin.c_str(), "int"))
            {
                vtype += "i";
            }
            else if(strstr(origin.c_str(), "float"))
            {
                vtype += "f";
            }
            else
            {
                vtype = origin;
            }
            
            info->setValueType(vtype);
        }

        reg.assign("^(voxel-?size|ratio)(.*?)(\\d+\\.\\d+|\\d+).(\\d+\\.\\d+|\\d+).(\\d+\\.\\d+|\\d+)");

        if(regex_search(line, matches, reg))
        {
            info->setVoxelSize({stof(matches[3]), stof(matches[4]), stof(matches[5])});
        }

        reg.assign("^(endian)(.*?)(\\w+)");

        if(regex_search(line, matches, reg))
        {
            string endian = matches[3];

            if(endian[0] == 'b')
            {
                endian = "big";
            }
            if(endian[0] == 'l')
            {
                endian = "little";
            }

            info->setEndian(endian);
        }
    }

    return info;
}