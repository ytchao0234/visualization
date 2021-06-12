#include <VectorData.hpp>

VectorData::VectorData()
{
    this->size = {0, 0};
    this->value.clear();
}

VectorData::VectorData(const VectorData* data)
{
    this->size = data->size;
    this->value = data->value;
}

VectorData::~VectorData()
{
    return;
}

void VectorData::print()
{
    cout << "size: ( " << this->size.first << ", " << this->size.second << " )" << endl;

    cout << endl;

    for(auto y: this->value)
    {
        for(auto x: y)
        {
            printf("(%12lf, %12lf)", x.first, x.second);
        }
        cout << endl;
    }
}