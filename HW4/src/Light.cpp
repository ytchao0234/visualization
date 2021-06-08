#include <Light.hpp>

Light::Light(glm::vec3 eyePos, glm::vec3 eyeDir)
{
    spotLights.push_back(new SpotLight({eyePos.x, eyePos.y, eyePos.z},
                                       {eyeDir.x, eyeDir.y, eyeDir.z}, 
                                       {1.0f, 1.0f, 1.0f}, 45.0f, 0.0f));
}

void Light::lighting(Shader* shader, glm::vec3 eyePos, glm::vec3 eyeDir)
{
    shader->use();

    shader->setFloatVec("eyePos", {eyePos.x, eyePos.y, eyePos.z}, 3);
    shader->setFloatVec("eyeDir", {eyeDir.x, eyeDir.y, eyeDir.z}, 3);
    
    shader->setFloatVec("LightPosition0", spotLights[0]->getPosition(), 3);
    shader->setFloatVec("LightDirection0", spotLights[0]->getDirection(), 3);
    shader->setFloatVec("LightIntensity0", spotLights[0]->getIntensity(), 3);
    shader->setFloat("LightCutoff0", glm::cos(glm::radians(spotLights[0]->getCutoff())));
}