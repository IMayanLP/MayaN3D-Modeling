#pragma once

#include <glm/glm.hpp>

class Camera {
    public:
        Camera();

        glm::mat4 GetViewMatrix() const;
        void Rotate(float, float);
        void Zoom(float);

        void MoveForward(float);
        void MoveRight(float);
        void MoveUp(float);

        void ResetPosition();
        void ResetRotation();
        void ResetZoom();

    private:
        void UpdatePosition();

        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up;

        float distance;
        float rotationY;
        float rotationX;
};