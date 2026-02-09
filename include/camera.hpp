#pragma once

#include <glm/glm.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {

    public:

    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;

    float yaw;
    float pitch;
    float fov;

    float lastX;
    float lastY;

    bool firstMouse;

    const float CAMERA_SPEED = 2.5f;

    Camera(glm::vec3 posIn, glm::vec3 upIn, int screenWidth, int screenHeight);
    void ProcessKeyboard(Camera_Movement DIRECTION, float deltaTime);
    void ProcessMouse(double xpos, double ypos);
    void ProcessScroll(double yoffset);
    glm::mat4 GetViewMatrix();
};
