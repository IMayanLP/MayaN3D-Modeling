#include "Cube.h"
#include <algorithm>

Cube::Cube(float width, float height, float depth) : sizeX(width), sizeY(height), sizeZ(depth) {}

Intersection Cube::CheckIntersection(const Cell& cell) const {
    glm::mat4 inverseRotation = glm::inverse(GetRotationMatrix());

    glm::vec3 minPoint(cell.x, cell.y, cell.z);
    glm::vec3 maxPoint(cell.x + cell.size, cell.y + cell.size, cell.z + cell.size);

    glm::vec3 vertices[8] = {
        {minPoint.x, minPoint.y, minPoint.z},
        {maxPoint.x, minPoint.y, minPoint.z},
        {minPoint.x, maxPoint.y, minPoint.z},
        {maxPoint.x, maxPoint.y, minPoint.z},
        {minPoint.x, minPoint.y, maxPoint.z},
        {maxPoint.x, minPoint.y, maxPoint.z},
        {minPoint.x, maxPoint.y, maxPoint.z},
        {maxPoint.x, maxPoint.y, maxPoint.z}
    };

    glm::vec3 localMin = glm::vec3(inverseRotation * glm::vec4(vertices[0], 1.0f));
    glm::vec3 localMax = localMin;

    for (int i = 1; i < 8; i++) {
        glm::vec3 localPoint = glm::vec3(inverseRotation * glm::vec4(vertices[i], 1.0f));

        localMin = glm::min(localMin, localPoint);
        localMax = glm::max(localMax, localPoint);
    }

    float halfWidth = sizeX / 2.0f;
    float halfHeight = sizeY / 2.0f;
    float halfDepth = sizeZ / 2.0f;

    if (localMax.x <= -halfWidth || localMin.x >= halfWidth ||
        localMax.y <= -halfHeight || localMin.y >= halfHeight ||
        localMax.z <= -halfDepth || localMin.z >= halfDepth) {
        return Intersection::Outside;
        }

    if (localMin.x >= -halfWidth && localMax.x <= halfWidth &&
        localMin.y >= -halfHeight && localMax.y <= halfHeight &&
        localMin.z >= -halfDepth && localMax.z <= halfDepth) {
        return Intersection::Inside;
        }

    return Intersection::Intersecting;
}

Cell Cube::GetBoundingBox() const {
    float halfX = sizeX / 2.0f;
    float halfY = sizeY / 2.0f;
    float halfZ = sizeZ / 2.0f;

    glm::vec3 vertices[8] = {
        {-halfX, -halfY, -halfZ},
        { halfX, -halfY, -halfZ},
        {-halfX,  halfY, -halfZ},
        { halfX,  halfY, -halfZ},
        {-halfX, -halfY,  halfZ},
        { halfX, -halfY,  halfZ},
        {-halfX,  halfY,  halfZ},
        { halfX,  halfY,  halfZ}
    };

    glm::mat4 rotationMatrix = GetRotationMatrix();

    glm::vec3 minPoint = glm::vec3(rotationMatrix * glm::vec4(vertices[0], 1.0f));
    glm::vec3 maxPoint = minPoint;

    for (int i = 1; i < 8; i++) {
        glm::vec3 point = glm::vec3(rotationMatrix * glm::vec4(vertices[i], 1.0f));

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