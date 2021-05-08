// #include <Texture.hpp>

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb/stb_image.h"

// Texture::Texture(int number)
// {
//     if(number < 1)
//     {
//         cout << "ERROR::TEXTURE::NUMBER::INVALID\n" << endl;
//         exit(1);
//     }

//     this->number = number;
//     unit = new unsigned int[number];
//     glGenTextures(number, unit);

//     stbi_set_flip_vertically_on_load(true);
// }

// Texture::~Texture()
// {
//     if(number == 1) delete unit;
//     else if(number > 1) delete[] unit;
// }

// void Texture::loadImage(string imagePath, int order)
// {
//     glBindTexture(GL_TEXTURE_2D, unit[order]);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

//     static int width, height, nrChannels;
//     static unsigned char *data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         cout << "Failed to load texture: " << imagePath.c_str() << endl;
//     }
//     stbi_image_free(data);
// }

// void Texture::active(int order)
// {
//     glActiveTexture(GL_TEXTURE0 + order);
//     glBindTexture(GL_TEXTURE_2D, unit[order]);
// }