#include <Texture.hpp>

Texture::Texture(int number)
{
    // if(number < 1)
    // {
    //     cout << "ERROR::TEXTURE::NUMBER::INVALID\n" << endl;
    //     exit(1);
    // }

    // this->number = number;
    // tex = new unsigned int[number];
    // glGenTextures(number, tex);
}

Texture::~Texture()
{
    // if(number == 1) delete tex;
    // else if(number > 1) delete[] tex;
}

void Texture::make3DTexture(unsigned char (*data)[4], int width, int height, int depth)
{
    glGenTextures(1, &tex3D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tex3D);

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

void Texture::make1DTexture(unsigned char (*data)[4], int length)
{
    glGenTextures(1, &tex1D);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, tex1D);

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
    glBindTexture(GL_TEXTURE_1D, tex1D);
}

void Texture::active2D(int order)
{
    glActiveTexture(GL_TEXTURE0 + order);
    glBindTexture(GL_TEXTURE_2D, tex2D);
}

void Texture::active3D(int order)
{
    glActiveTexture(GL_TEXTURE0 + order);
    glBindTexture(GL_TEXTURE_3D, tex3D);
}