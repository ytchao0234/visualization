#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <Shader.hpp>
#include <Texture.hpp>

using namespace std;

class CubeModel
{
private:
    unsigned int VAO;
    unsigned int VBO;
    vector<float> vertices;
public:
    CubeModel();
    void draw(glm::mat4, glm::mat4);
};