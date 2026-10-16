#include "Cylinder.h"
#include <algorithm>
#include <cmath>

Cylinder::Cylinder(float radius, float height) : radius(radius), height(height) {}

Intersection Cylinder::CheckIntersection(const Cell& cell) const {
    glm::mat4 inverseRotation = glm::inverse(GetRotationMatrix());

    glm::vec3 minPoint(cell.x, cell.y, cell.z);
    glm::vec3 maxPoint(cell.x + cell.size, cell.y + cell.size, cell.z + cell.size);

    glm::vec3 corners[8] = {
        {minPoint.x, minPoint.y, minPoint.z},
        {maxPoint.x, minPoint.y, minPoint.z},
        {minPoint.x, maxPoint.y, minPoint.z},
        {maxPoint.x, maxPoint.y, minPoint.z},
        {minPoint.x, minPoint.y, maxPoint.z},
        {maxPoint.x, minPoint.y, maxPoint.z},
        {minPoint.x, maxPoint.y, maxPoint.z},
        {maxPoint.x, maxPoint.y, maxPoint.z}
    };

    glm::vec3 localMin = glm::vec3(inverseRotation * glm::vec4(corners[0], 1.0f));
    glm::vec3 localMax = localMin;

    for (int i = 1; i < 8; i++) {
        glm::vec3 localPoint = glm::vec3(inverseRotation * glm::vec4(corners[i], 1.0f));

        localMin = glm::min(localMin, localPoint);
        localMax = glm::max(localMax, localPoint);
    }

    float halfHeight = height / 2.0f;

    float cylinderMinY = -halfHeight;
    float cylinderMaxY = halfHeight;

    float minX = localMin.x;
    float maxX = localMax.x;
    float minY = localMin.y;
    float maxY = localMax.y;
    float minZ = localMin.z;
    float maxZ = localMax.z;

    if (maxY <= cylinderMinY || minY >= cylinderMaxY) return Intersection::Outside;

    float closestX = 0.0f;

    if (0.0f < minX) closestX = minX;
    else if (0.0f > maxX) closestX = maxX;

    float closestZ = 0.0f;

    if (0.0f < minZ) closestZ = minZ;
    else if (0.0f > maxZ) closestZ = maxZ;

    float d = closestX * closestX + closestZ * closestZ;

    if (d >= radius * radius) return Intersection::Outside;

    float farthestX = std::max(std::abs(minX), std::abs(maxX));
    float farthestZ = std::max(std::abs(minZ), std::abs(maxZ));

    float maxD = farthestX * farthestX + farthestZ * farthestZ;

    if ((minY >= cylinderMinY && maxY <= cylinderMaxY) && maxD <= radius * radius) return Intersection::Inside;

    return Intersection::Intersecting;
}

Cell Cylinder::GetBoundingBox() const {
    float halfHeight = height / 2.0f;

    glm::vec3 corners[8] = {
        {-radius, -halfHeight, -radius},
        { radius, -halfHeight, -radius},
        {-radius,  halfHeight, -radius},
        { radius,  halfHeight, -radius},
        {-radius, -halfHeight,  radius},
        { radius, -halfHeight,  radius},
        {-radius,  halfHeight,  radius},
        { radius,  halfHeight,  radius}
    };

    glm::mat4 rotationMatrix = GetRotationMatrix();

    glm::vec3 minPoint = glm::vec3(rotationMatrix * glm::vec4(corners[0], 1.0f));
    glm::vec3 maxPoint = minPoint;

    for (int i = 1; i < 8; i++) {
        glm::vec3 point = glm::vec3(rotationMatrix * glm::vec4(corners[i], 1.0f));

        minPoint = glm::min(minPoint, point);
        maxPoint = glm::max(maxPoint, point);
    }

    float boundingSize = std::max({
        maxPoint.x - minPoint.x,
        maxPoint.y - minPoint.y,
        maxPoint.z - minPoint.z
    });

    return {
        -boundingSize / 2.0f,
        -boundingSize / 2.0f,
        -boundingSize / 2.0f,
        boundingSize
    };
}