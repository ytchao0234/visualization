#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 direction;
    glm::vec3 viewUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw;
    float pitch;
    float zoomValue;

public:
    Camera(glm::vec3, float, float);

    void zoom(float);
    void move(float, float);
    
    glm::mat4 getProjection(int, int);
    glm::mat4 getView();

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getTarget() const { return target; }
    glm::vec3 getDirection() const { return direction; }
    float getYaw() { return yaw; }
    float getPitch() { return pitch; }
    float getZoom() { return zoomValue; }
};