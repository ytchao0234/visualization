#include <Histogram.hpp>

Histogram::Histogram(const VolumeData* source)
{
    data_origin.assign((int)(source->valueMax - source->valueMin) + 1, 0);
    rangeMin_origin = source->valueMin;
    rangeMax_origin = source->valueMax;
    rangeOffset_origin = 0 - source->valueMin;
    valueMin_origin = 0.0f;
    valueMax_origin = 0.0f;

    data_log.assign((int)(source->valueMax - source->valueMin) + 1, 0);
    rangeMin_log = source->valueMin;
    rangeMax_log = source->valueMax;
    rangeOffset_log = 0 - source->valueMin;
    valueMin_log = 0.0f;
    valueMax_log = 0.0f;

    data_equal.assign((int)(source->valueMax - source->valueMin) + 1, 0);
    rangeMin_equal = source->valueMin;
    rangeMax_equal = source->valueMax;
    rangeOffset_equal = 0 - source->valueMin;
    valueMin_equal = 0.0f;
    valueMax_equal = 0.0f;

    int index_origin = 0;

    for(auto x: source->value)
    for(auto y:    x)
    for(auto z:    y)
    {
        index_origin = z + rangeOffset_origin;
        data_origin[index_origin] ++;
        valueMax_origin = max(valueMax_origin, data_origin[index_origin]);
    }
    valueMax_equal = valueMax_origin;

    for(int i = 0; i < (int)data_origin.size(); i++)
    {
        data_log[i] = logf(data_origin[i]);
        valueMax_log = max(valueMax_log, data_log[i]);
    }

    vector<float> cdf((int)(source->valueMax - source->valueMin) + 1, 0);

    cdf[0] = data_origin[0];
    float cdfMin = cdf[0];
    float cdfMax = cdf[0];

    for(int i = 1; i < (int)data_origin.size(); i++)
    {
        cdf[i] = cdf[i - 1] + data_origin[i];

        cdfMin = min(cdfMin, cdf[i]);
        cdfMax = max(cdfMax, cdf[i]);
    }

    int index_equal = 0;

    for(int i = 0; i < (int)data_equal.size(); i++)
    {
        index_equal = (cdf[i] - cdfMin) / (cdfMax - cdfMin) * data_equal.size();
        data_equal[index_equal] = data_origin[i];
    }
}

Histogram::~Histogram()
{
    return;
}

vector<float> Histogram::getData(string option = "ORIGIN") const
{
    if(option == "ORIGIN") return data_origin;
    else if(option == "LOG") return data_log;
    else if(option == "EQUAL") return data_equal;
    else cout << "ERROR::Histogram type is not supported." << endl;

    vector<float> temp;
    return temp;
}

int Histogram::getRangeMin(string option = "ORIGIN") const
{
    if(option == "ORIGIN") return rangeMin_origin;
    else if(option == "LOG") return rangeMin_log;
    else if(option == "EQUAL") return rangeMin_equal;
    else cout << "ERROR::Histogram type is not supported." << endl;
    return 0;
}

int Histogram::getRangeMax(string option = "ORIGIN") const
{
    if(option == "ORIGIN") return rangeMax_origin;
    else if(option == "LOG") return rangeMax_log;
    else if(option == "EQUAL") return rangeMax_equal;
    else cout << "ERROR::Histogram type is not supported." << endl;
    return 0;
}

int Histogram::getRangeOffset(string option = "ORIGIN") const
{
    if(option == "ORIGIN") return rangeOffset_origin;
    else if(option == "LOG") return rangeOffset_log;
    else if(option == "EQUAL") return rangeOffset_equal;
    else cout << "ERROR::Histogram type is not supported." << endl;
    return 0;
}

float Histogram::getValueMin(string option = "ORIGIN") const
{
    if(option == "ORIGIN") return valueMin_origin;
    else if(option == "LOG") return valueMin_log;
    else if(option == "EQUAL") return valueMin_equal;
    else cout << "ERROR::Histogram type is not supported." << endl;
    return 0;
}

float Histogram::getValueMax(string option = "ORIGIN") const
{
    if(option == "ORIGIN") return valueMax_origin;
    else if(option == "LOG") return valueMax_log;
    else if(option == "EQUAL") return valueMax_equal;
    else cout << "ERROR::Histogram type is not supported." << endl;
    return 0;
}