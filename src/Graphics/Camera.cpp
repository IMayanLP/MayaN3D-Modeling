#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : position(5.0f, 5.0f, 5.0f),
      target(0.0f, 0.0f, 0.0f),
      up(0.0f, 1.0f, 0.0f),
      distance(8.660f),
      rotationY(45.0f),
      rotationX(35.264f) {
}

void Camera::ResetRotation() {
    rotationY = 45.0f;
    rotationX = 35.264f;

    UpdatePosition();
}

void Camera::ResetZoom() {
    distance = 8.660f;

    UpdatePosition();
}

void Camera::ResetPosition() {
    target = {0.0f, 0.0f, 0.0f};
    distance = 8.660f;
    rotationY = 45.0f;
    rotationX = 35.264f;

    UpdatePosition();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position, target, up);
}

void Camera::UpdatePosition() {
    float rotationYRadians = glm::radians(rotationY);
    float rotationXRadians = glm::radians(rotationX);

    position.x = target.x + distance * cos(rotationXRadians) * cos(rotationYRadians);
    position.y = target.y + distance * sin(rotationXRadians);
    position.z = target.z + distance * cos(rotationXRadians) * sin(rotationYRadians);
}

void Camera::Rotate(float x, float y) {
    rotationY += x;
    rotationX += y;
    rotationX = glm::clamp(rotationX, -89.0f, 89.0f);

    UpdatePosition();
}

void Camera::Zoom(float zoom) {
    distance -= zoom;
    distance = glm::clamp(distance, 1.0f, 100.0f);

    UpdatePosition();
}

void Camera::MoveForward(float amount) {
    glm::vec3 forward = glm::normalize(target - position);
    forward.y = 0.0f;
    forward = glm::normalize(forward);

    position += forward * amount;
    target += forward * amount;
}

void Camera::MoveRight(float amount) {
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));

    position += right * amount;
    target += right * amount;
}

void Camera::MoveUp(float amount) {
    glm::vec3 offset(0.0f, amount, 0.0f);

    position += offset;
    target += offset;
}