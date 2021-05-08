#pragma once
#include <glm/glm.hpp>
#include <vector>

using namespace std;

class SpotLight
{
private:
    vector<float> position;
    vector<float> direction;
    vector<float> intensity;
    float cutoff;
    float exponent;

public:
    SpotLight(vector<float> pos, vector<float> direct, vector<float> intens, float coff, float exp): 
        position(pos), direction(direct), intensity(intens), cutoff(coff), exponent(exp) {}

    void setPosition(glm::vec3 pos) { position = { pos.x, pos.y, pos.z }; }
    void setDirection(glm::vec3 direct) { direction = { direct.x, direct.y, direct.z }; }
    void setIntensity(vector<float> intens) { intensity = intens; }
    void setCutoff(float coff) { cutoff = coff; }
    void setExponent(float exp) { exponent = exp; }

    vector<float> getPosition() const { return position; }
    vector<float> getDirection() const { return direction; }
    vector<float> getIntensity() const { return intensity; }
    float getCutoff() const { return cutoff; }
    float getExponent(float exp) const { return exponent; }
};