#include <Texture.hpp>

Texture::Texture(int number)
{
    if(number < 1)
    {
        cout << "ERROR::TEXTURE::NUMBER::INVALID\n" << endl;
        exit(1);
    }

    this->number = number;
    tex = new unsigned int[number];
    glGenTextures(number, tex);
}

Texture::~Texture()
{
    if(number == 1) delete tex;
    else if(number > 1) delete[] tex;
}

void Texture::make3DTexture(vector<unsigned int> data, int width, int height, int depth)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glBindTexture(GL_TEXTURE_3D, tex[0]);
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
        data.data()
    );
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_MIPMAP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_MIPMAP);
}

void Texture::make1DTexture(vector<unsigned int> data, int length)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glBindTexture(GL_TEXTURE_1D, tex[1]);
    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        GL_RGBA,
        length,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data.data()
    );
    
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::active(int order)
{
    glActiveTexture(GL_TEXTURE0 + order);
    glBindTexture(GL_TEXTURE_2D, tex[order]);
}