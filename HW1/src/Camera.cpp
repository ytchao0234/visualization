#include <Camera.hpp>

Camera::Camera(glm::vec3 position, float yaw, float pitch): zoom(1)
{
    this->position = position;
    this->yaw = yaw;
    this->pitch = pitch;
    
    glm::vec3 dTemp;
    dTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dTemp.y = sin(glm::radians(pitch));
    dTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(dTemp);

    target = position + direction;
}

glm::mat4 Camera::getProjection()
{
    glm::mat4 projection = glm::ortho(-400.0f*zoom, 400.0f*zoom, -300.0f*zoom, 300.0f*zoom, -400.0f, 400.0f);
    return projection;
}

glm::mat4 Camera::getView()
{
    glm::vec3 right = glm::normalize(glm::cross(direction, viewUp));
    glm::vec3 up = glm::cross(right, direction);
    
    glm::mat4 view = glm::lookAt(position, target, up);
    return view;
}