#include "Ray.h"
#include <glm/geometric.hpp>

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(glm::normalize(direction)) {}

glm::vec3 Ray::GetOrigin() const {
    return origin;
}

glm::vec3 Ray::GetDirection() const {
    return direction;
}