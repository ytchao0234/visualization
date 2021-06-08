#include <Texture.hpp>

Texture::Texture(int number)
{
    if(number < 1)
    {
        cout << "ERROR::TEXTURE::NUMBER::INVALID\n" << endl;
        exit(1);
    }

    this->number = number;
    this->tex = new unsigned int[number];
    glGenTextures(number, tex);
}

Texture::~Texture()
{
    if(number == 1) delete tex;
    else if(number > 1) delete[] tex;
}

void Texture::make3DTexture(int order, unsigned char (*data)[4], int width, int height, int depth)
{
    glActiveTexture(GL_TEXTURE0 + order);
    glBindTexture(GL_TEXTURE_3D, this->tex[order]);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage3D(
        GL_TEXTURE_3D,
        0,
        GL_RGBA,
        width,
        height,
        depth,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::make1DTexture(int order, unsigned char (*data)[4], int length)
{
    glActiveTexture(GL_TEXTURE0 + order);
    glBindTexture(GL_TEXTURE_1D, this->tex[order]);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_RGBA,
        length,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );
    
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::active1D(int order)
{
    glActiveTexture(GL_TEXTURE0 + order);
    glBindTexture(GL_TEXTURE_1D, tex[order]);
}

void Texture::active2D(int order)
{
    glActiveTexture(GL_TEXTURE0 + order);
    glBindTexture(GL_TEXTURE_2D, tex[order]);
}

void Texture::active3D(int order)
{
    glActiveTexture(GL_TEXTURE0 + order);
    glBindTexture(GL_TEXTURE_3D, tex[order]);
}