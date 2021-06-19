#include <MultiDimData.hpp>

MultiDimData::MultiDimData()
{
    this->size = {0, 0};
    this->value.clear();
    this->distance.clear();
    this->totalDist = 0.0;
}

MultiDimData::MultiDimData(const MultiDimData* data)
{
    this->size = data->size;
    this->value = data->value;
    this->distance = data->distance;
    this->totalDist = data->totalDist;
}

MultiDimData::~MultiDimData()
{
    return;
}

void MultiDimData::print()
{
    cout << "size: ( " << this->size.first << ", " << this->size.second << " )" << endl;

    cout << endl;

    for(auto y: this->value)
    {
        for(auto x: y)
        {
            printf("%6lf\t", x);
        }
        cout << endl;
    }
}