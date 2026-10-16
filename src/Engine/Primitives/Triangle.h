#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

class Triangle {
public:
    Triangle(glm::vec3, glm::vec3, glm::vec3);
    void OrientTriangleCCW();

    glm::vec3 v0, v1, v2;
};