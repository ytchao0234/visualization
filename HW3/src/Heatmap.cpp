#include <Heatmap.hpp>

Heatmap::Heatmap(const VolumeData* source, float maxLimit)
{
    static const glm::vec3 resolution = source->getResolution();
    static const vector<vector<vector<float>>> sourceData = source->getData();
    static const float dataMin = source->getDataMin();
    static const float dataMax = source->getDataMax();
    static const vector<vector<vector<float>>> gradMag = source->getGradMag();
    static const float gradMin = source->getGradMin();
    static const float gradMax = source->getGradMax();

    rangeMin_intensity = dataMin;
    rangeMax_intensity = dataMax;
    rangeOffset_intensity = 0 - dataMin;

    if(maxLimit > gradMax)
        maxLimit_gradMag = gradMax;
    else if(maxLimit < gradMin)
        maxLimit_gradMag = gradMin;
    else
        maxLimit_gradMag = maxLimit;

    if(gradMin < 1.0f) rangeMin_gradMag = 0.0f;
    else rangeMin_gradMag = 20*log2f(gradMin);

    if(maxLimit_gradMag < 1.0f) rangeMax_gradMag = 0.0f;
    else rangeMax_gradMag = 20*log2f(maxLimit_gradMag);

    this->data.assign((rangeMax_gradMag - rangeMin_gradMag + 1) * 
                      (rangeMax_intensity - rangeMin_intensity + 1) , 0.0f);

    vector<vector<float>> temp(rangeMax_gradMag - rangeMin_gradMag + 1,
                               vector<float>(rangeMax_intensity - rangeMin_intensity + 1, 0.0f));

    valueMin = 0.0f;
    valueMax = 0.0f;

    int index_gradMag = 0;
    int index_intensity = 0;

    for(int x = 0; x < resolution.x; x++)
    for(int y = 0; y < resolution.y; y++)
    for(int z = 0; z < resolution.z; z++)
    {
        if(gradMag[x][y][z] < 1.0f)
            index_gradMag = 0;
        else if(gradMag[x][y][z] > maxLimit_gradMag)
            index_gradMag = 20*log2f(maxLimit_gradMag);
        else
            index_gradMag = 20*log2f(gradMag[x][y][z]);

        index_intensity = sourceData[x][y][z] + rangeOffset_intensity;

        temp[index_gradMag][index_intensity] ++;
    }

    for(int i = 0; i < (int)(this->data.size()); i++)
    {
        this->data[i] = temp[(temp.size() - 1) - (i / temp[0].size())][i % temp[0].size()];

        if( this->data[i] > 0.0f ) this->data[i] = logf(this->data[i]);
        
        valueMax = max(valueMax, this->data[i]);
    }
}

Heatmap::~Heatmap()
{
    return;
}

vector<float> Heatmap::getData() const
{
    return this->data;
}

int Heatmap::getRangeMin(string option = "INTENSITY") const
{
    if(option == "INTENSITY")
        return this->rangeMin_intensity;
    else if(option == "GRADMAG")
        return this->rangeMin_gradMag;
    else cout << "ERROR::Heatmap member is not found." << endl;
    return 0;
}

int Heatmap::getRangeMax(string option = "INTENSITY") const
{
    if(option == "INTENSITY")
        return this->rangeMax_intensity;
    else if(option == "GRADMAG")
        return this->rangeMax_gradMag;
    else cout << "ERROR::Heatmap member is not found." << endl;
    return 0;
}

int Heatmap::getRangeOffset() const
{
    return this->rangeOffset_intensity;
}

float Heatmap::getMaxLimit() const
{
    return this->maxLimit_gradMag;
}

float Heatmap::getValueMin() const
{
    return this->valueMin;
}

float Heatmap::getValueMax() const
{
    return this->valueMax;
}