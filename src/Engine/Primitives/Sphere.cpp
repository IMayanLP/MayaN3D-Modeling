#include "Sphere.h"
#include <algorithm>
#include <cmath>

Sphere::Sphere(float radius) : radius(radius) {}

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