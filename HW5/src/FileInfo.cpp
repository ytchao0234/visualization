#include <FileInfo.hpp>

void FileInfo::init()
{
    resolution = {0,0,0} ;
    valueType = "ub";
    voxelSize = {1.0, 1.0, 1.0};
    endian = "little";
}

void FileInfo::print() const
{
    cout << endl << "FileInfo: " << endl;

    cout << "resolution: ";
    for(auto res: resolution) cout << res << ", ";
    cout << endl;

    cout << "value type: " << valueType.c_str() << endl;

    cout << "voxel size: ";
    for(auto vs: voxelSize) cout << vs << ", ";
    cout << endl;

    cout << "endian: " << endian.c_str() << endl;

    cout << "\n-------------------------------------\n";
}

int FileInfo::getSizeOfValueType() const
{
    if(     valueType == "b" ) return sizeof(char);
    else if(valueType == "ub") return sizeof(unsigned char);
    else if(valueType == "s" ) return sizeof(short);
    else if(valueType == "us") return sizeof(unsigned short);
    else if(valueType == "i" ) return sizeof(int);
    else if(valueType == "ui") return sizeof(unsigned int);
    else if(valueType == "f" ) return sizeof(float);
    else cout << "value type is unknown" << endl;

    return 0;
}