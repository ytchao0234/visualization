#pragma once
#include <glm/glm.hpp>
#include <SpotLight.hpp>
#include <Shader.hpp>

class Light
{
private:
    vector<SpotLight*> spotLights;

public:
    Light(glm::vec3, glm::vec3);
    void lighting(Shader*, glm::vec3, glm::vec3);

    SpotLight* getSpotlight(int i) { return spotLights[i]; }
};