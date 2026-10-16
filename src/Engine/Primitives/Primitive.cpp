#include "Primitive.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Primitive::GetRotationMatrix() const {
    glm::mat4 rotationMatrix(1.0f);

    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    return rotationMatrix;
}

void Primitive::SetRotation(float x, float y, float z) {
    rotation = glm::vec3(x, y, z);
}