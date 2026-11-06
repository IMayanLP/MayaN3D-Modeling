#include "Ray.h"
#include <glm/geometric.hpp>

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(glm::normalize(direction)) {}

glm::vec3 Ray::GetOrigin() const {
    return origin;
}

glm::vec3 Ray::GetDirection() const {
    return direction;
}

bool Ray::RaysIntersect(const Ray& ray1, const Ray& ray2, float e, float& insertectionT) {
    glm::vec3 origem1 = ray1.GetOrigin();
    glm::vec3 origem2 = ray2.GetOrigin();

    glm::vec3 direcao1 = glm::normalize(ray1.GetDirection());
    glm::vec3 direcao2 = glm::normalize(ray2.GetDirection());

    glm::vec3 diff = origem2 - origem1;
    glm::vec3 produtoVetorial = glm::cross(direcao1, direcao2);

    float produtoEscalar = glm::dot(produtoVetorial, produtoVetorial);

    if (produtoEscalar < 1e-6f) return false;

    float t1 = glm::dot(glm::cross(diff, direcao2), produtoVetorial) / produtoEscalar;
    float t2 = glm::dot(glm::cross(diff, direcao1), produtoVetorial) / produtoEscalar;

    if (t1 < 0.0f || t2 < 0.0f) return false;

    glm::vec3 ponto1 = origem1 + direcao1 * t1;
    glm::vec3 ponto2 = origem2 + direcao2 * t2;

    insertectionT = t2;

    glm::vec3 diffPontosMin = ponto1 - ponto2;

    float distancia = std::sqrt(diffPontosMin.x * diffPontosMin.x + diffPontosMin.y * diffPontosMin.y + diffPontosMin.z * diffPontosMin.z);

    return distancia <= e;
}