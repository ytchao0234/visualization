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

    int i = 0;
    for(auto val: this->value)
    {
        printf("(%12lf, %12lf)", val.first, val.second);

        if(++i % this->size.first == 0 )
            cout << endl;
    }
}