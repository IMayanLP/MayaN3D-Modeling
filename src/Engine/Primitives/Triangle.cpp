#include "Triangle.h"

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) : v0(v0), v1(v1), v2(v2) {}

void Triangle::OrientTriangleCCW() {
    glm::vec3 normal = glm::cross(v1 - v0, v2 - v0);

    if (normal.z < 0.0f) {
        std::swap(v1, v2);
    }
}