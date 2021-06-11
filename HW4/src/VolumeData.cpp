#include <VolumeData.hpp>

VolumeData::VolumeData()
{
    this->resolution = glm::ivec3(0.0f, 0.0f, 0.0f);
    this->voxelSize = glm::vec3(0.0f, 0.0f, 0.0f);

    this->value.clear();
    this->valueMin = 0.0f;
    this->valueMax = 0.0f;

    this->gradient.clear();
    this->gradMag.clear();
    this->gradMin = 0.0f;
    this->gradMax = 0.0f;
}

VolumeData::VolumeData(const VolumeData* that)
{
    this->resolution = that->resolution;
    this->voxelSize = that->voxelSize;

    this->value = that->value;
    this->valueMin = that->valueMin;
    this->valueMax = that->valueMax;

    this->gradient = that->gradient;
    this->gradMag = that->gradMag;
    this->gradMin = that->gradMin;
    this->gradMax = that->gradMax;
}

VolumeData::~VolumeData()
{
    return;
}

void VolumeData::setResolution(int x, int y, int z)
{
    this->resolution = glm::ivec3(x, y, z);

    value.assign(x, vector<vector<float>>(y, vector<float>(z, 0)));
    gradient.assign(x, vector<vector<vector<float>>>(y, vector<vector<float>>(z, vector<float>())));
    gradMag.assign(x, vector<vector<float>>(y, vector<float>(z, 0)));
}

void VolumeData::setVoxelSize(float x, float y, float z)
{
    this->voxelSize = glm::vec3(x, y, z);
}

void VolumeData::setValue(vector<vector<vector<float>>> thatData)
{
    this->value.assign(thatData.begin(), thatData.end());

    valueMin = valueMax = value[0][0][0];

    for(auto x: this->value)
    for(auto y:          x)
    for(auto z:          y)
    {
        valueMin = min(valueMin, z);
        valueMax = max(valueMax, z);
    }
}

void VolumeData::setGradient()
{
    glm::vec3 g = glm::vec3(0.0f, 0.0f, 0.0f);

    for( int x = 0; x < resolution.x; x++ )
    for( int y = 0; y < resolution.y; y++ )
    for( int z = 0; z < resolution.z; z++ )
    {
        if( x-1 > 0 && x+1 < resolution.x)
            g.x = (value[x+1][y][z] - value[x-1][y][z]) / voxelSize[2] / 2.0f;
        else if( x+1 < resolution.x)
            g.x = (value[x+1][y][z] - value[x][y][z]) / voxelSize[2];
        else if( x-1 > 0 )
            g.x = (value[x][y][z] - value[x-1][y][z]) / voxelSize[2];

        if( y-1 > 0 && y+1 < resolution.y)
            g.y = (value[x][y+1][z] - value[x][y-1][z]) / voxelSize[1] / 2.0f;
        else if( y+1 < resolution.y)
            g.y = (value[x][y+1][z] - value[x][y][z]) / voxelSize[1];
        else if( y-1 > 0 )
            g.y = (value[x][y][z] - value[x][y-1][z]) / voxelSize[1];

        if( z-1 > 0 && z+1 < resolution.z )
            g.z = (value[x][y][z+1] - value[x][y][z-1]) / voxelSize[0] / 2.0f;
        else if( z+1 < resolution.z)
            g.z = (value[x][y][z+1] - value[x][y][z]) / voxelSize[0];
        else if( z-1 > 0 )
            g.z = (value[x][y][z] - value[x][y][z-1]) / voxelSize[0];

        this->gradMag[x][y][z] = glm::length(g);

        g = glm::normalize(g);
        this->gradient[x][y][z] = { g.x, g.y, g.z };
    }

    gradMin = gradMax = gradMag[0][0][0];

    for(auto x: gradMag)
    for(auto y:    x)
    for(auto z:    y)
    {
        gradMin = min(gradMin, z);
        gradMax = max(gradMax, z);
    }
}
