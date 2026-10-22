#pragma once

#include <glm/vec3.hpp>

struct RayIntersection {
    float t;
    glm::vec3 point;
    glm::vec3 normal;
};

class Ray {
public:
    Ray(const glm::vec3&, const glm::vec3&);

    glm::vec3 GetOrigin() const;
    glm::vec3 GetDirection() const;

private:
    glm::vec3 origin;
    glm::vec3 direction;
};