#include <Camera.hpp>

Camera::Camera(glm::vec3 position, float yaw, float pitch): zoomValue(50)
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

void Camera::zoom( float value )
{
    zoomValue -= (float)value;

    if (zoomValue < 1.0f)
        zoomValue = 1.0f;
    else if (zoomValue > 100.0f)
        zoomValue = 100.0f;
}

void Camera::move(float xoffset, float yoffset)
{
    yaw   += xoffset;
    pitch += yoffset;
    
    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 dTemp;
    dTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dTemp.y = sin(glm::radians(pitch));
    dTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(dTemp);

    position = target - direction;
}

glm::mat4 Camera::getProjection(int width, int height)
{
    glm::mat4 projection;

    // projection = glm::ortho(-4.0f*zoomValue, 4.0f*zoomValue,
    //                         -3.0f*zoomValue, 3.0f*zoomValue,
    //                         -400.0f, 400.0f);

    if( width > height )
    {
        projection = glm::ortho(-4.0f*zoomValue, 4.0f*zoomValue,
                                -3.0f*zoomValue * ((float)height/width), 3.0f*zoomValue * ((float)height/width),
                                -400.0f, 400.0f);
    }
    else
        projection = glm::ortho(-4.0f*zoomValue * ((float)width/height), 4.0f*zoomValue * ((float)width/height),
                                -3.0f*zoomValue, 3.0f*zoomValue,
                                -400.0f, 400.0f);

    return projection;
}

glm::mat4 Camera::getView()
{
    glm::vec3 right = glm::normalize(glm::cross(direction, viewUp));
    glm::vec3 up = glm::cross(right, direction);
    
    glm::mat4 view = glm::lookAt(position, target, up);
    return view;
}