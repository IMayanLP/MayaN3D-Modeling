#include "Sphere.h"
#include <algorithm>
#include <cmath>

Sphere::Sphere(float radius) : Primitive(PrimitiveType::Sphere), radius(radius) {}

Intersection Sphere::CheckIntersection(const Cell& cell) const {
    float minX = cell.x;
    float maxX = cell.x + cell.size;
    float minY = cell.y;
    float maxY = cell.y + cell.size;
    float minZ = cell.z;
    float maxZ = cell.z + cell.size;

    float closestX = std::clamp(0.0f, minX, maxX);
    float closestY = std::clamp(0.0f, minY, maxY);
    float closestZ = std::clamp(0.0f, minZ, maxZ);

    float d = closestX * closestX + closestY * closestY + closestZ * closestZ;

    if (d > radius * radius) return Intersection::Outside;

    float farthestX = std::max(std::abs(minX), std::abs(maxX));
    float farthestY = std::max(std::abs(minY), std::abs(maxY));
    float farthestZ = std::max(std::abs(minZ), std::abs(maxZ));

    float farthestDistance = farthestX * farthestX + farthestY * farthestY + farthestZ * farthestZ;

    if (farthestDistance <= radius * radius) return Intersection::Inside;
    return Intersection::Intersecting;
}

Cell Sphere::GetBoundingBox() const {
    float size = radius * 2.0f;

    return {
        -radius,
        -radius,
        -radius,
        size
    };
}

std::unique_ptr<Primitive> Sphere::Clone() const {
    return std::make_unique<Sphere>(*this);
}

std::vector<RayIntersection> Sphere::IntersectRay(const Ray& ray) const {
    std::vector<RayIntersection> intersections;

    glm::vec3 origin = ray.GetOrigin();
    glm::vec3 direction = ray.GetDirection();

    float a = glm::dot(direction, direction);
    float b = 2.0f * glm::dot(origin, direction);
    float c = glm::dot(origin, origin) - radius * radius;

    float delta = b * b - 4.0f * a * c;

    if (delta < 0.0f) return intersections;

    float deltaSqrt = std::sqrt(delta);
    float x1 = (-b - deltaSqrt) / (2.0f * a);
    float x2 = (-b + deltaSqrt) / (2.0f * a);

    if (x1 >= 0.0f) {
        glm::vec3 intersecPoint = origin + x1 * direction;
        glm::vec3 normal = glm::normalize(intersecPoint);
        intersections.push_back({x1, intersecPoint, normal});
    }

    if (delta > 0.0f && x2 >= 0.0f) {
        glm::vec3 intersecPoint = origin + x2 * direction;
        glm::vec3 normal = glm::normalize(intersecPoint);
        intersections.push_back({x2, intersecPoint, normal});
    }

    return intersections;
}

SMCClassification Sphere::CheckIsInside(const glm::vec3& point) const {
    float d = point.x * point.x + point.y * point.y + point.z * point.z;

    if (std::abs(d - radius * radius) < 1e-6f) return SMCClassification::ON;
    if (d < radius * radius) return SMCClassification::IN;

    return SMCClassification::OUT;
}