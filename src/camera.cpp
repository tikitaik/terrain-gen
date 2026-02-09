#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "camera.hpp"

Camera::Camera(glm::vec3 posIn, glm::vec3 upIn, int screenWidth, int screenHeight) {

    pos = posIn;
    up = upIn;

    yaw = -90.0f;
    pitch = 0.0f;
    fov = 45.0f;

    lastX = float(screenWidth) / 2;
    lastY = float(screenHeight) / 2;

    glm::vec3 direction(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
            sin(glm::radians(pitch)),
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    );

    front = glm::normalize(direction);

    firstMouse = true;
}

void Camera::ProcessKeyboard(Camera_Movement DIRECTION, float deltaTime) {

    if (DIRECTION == FORWARD) {
        pos += front * CAMERA_SPEED * deltaTime;
    }
    if (DIRECTION == BACKWARD) {
        pos -= front * CAMERA_SPEED * deltaTime;
    }
    if (DIRECTION == LEFT) {
        pos -= glm::normalize(glm::cross(front, up)) * CAMERA_SPEED * deltaTime;
    }
    if (DIRECTION == RIGHT) {
        pos += glm::normalize(glm::cross(front, up)) * CAMERA_SPEED * deltaTime;
    }
    if (DIRECTION == UP) {
        pos += up * CAMERA_SPEED * deltaTime;
    }
    if (DIRECTION == DOWN) {
        pos -= up * CAMERA_SPEED * deltaTime;
    }
}

void Camera::ProcessMouse(double xpos, double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = xpos;
    lastY = ypos;

    const float sens = 0.1f;
    xoffset *= sens;
    yoffset *= sens;

    yaw   += xoffset;
    pitch += yoffset;
    
    pitch = pitch > 89.0f ? 89.0 : pitch;
    pitch = pitch < -89.0f ? -89.0 : pitch;

    //std::cout << yaw << ' ' << pitch << '\n';

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

void Camera::ProcessScroll(double yoffset) {

    fov -= (float)yoffset;
    if (fov < 1.0f) {
        fov = 1.0f;
    } else if (fov > 45.0f) {
        fov = 45.0f;
    }    
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(pos, pos + front, up);
}
