#include <Histogram.hpp>

Histogram::Histogram(const VolumeData* source)
{
    static const vector<vector<vector<float>>> data = source->getData();
    static const float dataMin = source->getDataMin();
    static const float dataMax = source->getDataMax();

    data_origin.assign((int)(dataMax - dataMin) + 1, 0);
    rangeMin_origin = dataMin;
    rangeMax_origin = dataMax;
    rangeOffset_origin = 0 - dataMin;
    valueMin_origin = 0.0f;
    valueMax_origin = 0.0f;

    data_log.assign((int)(dataMax - dataMin) + 1, 0);
    rangeMin_log = dataMin;
    rangeMax_log = dataMax;
    rangeOffset_log = 0 - dataMin;
    valueMin_log = 0.0f;
    valueMax_log = 0.0f;

    data_equal.assign(256, 0);
    rangeMin_equal = 0;
    rangeMax_equal = 255;
    rangeOffset_equal = 0;
    valueMin_equal = 0.0f;
    valueMax_equal = 0.0f;

    int index_origin = 0;
    int index_equal = 0;

    for(auto x: data)
    for(auto y:    x)
    for(auto z:    y)
    {
        index_origin = z + rangeOffset_origin;
        index_equal = (z - dataMin) / (dataMax - dataMin) * 255;

        data_origin[index_origin] ++;
        data_equal[index_equal] ++;

        valueMax_origin = max(valueMax_origin, data_origin[index_origin]);
        valueMax_equal = max(valueMax_equal, data_equal[index_equal]);
    }

    for(int i = 0; i < (int)data_origin.size(); i++)
    {
        data_log[i] = logf(data_origin[i]);
        valueMax_log = max(valueMax_log, data_log[i]);
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