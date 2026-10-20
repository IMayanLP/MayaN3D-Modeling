#include "Cube.h"
#include <algorithm>

Cube::Cube(float width, float height, float depth) : Primitive(PrimitiveType::Cube), sizeX(width), sizeY(height), sizeZ(depth) {}

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

std::unique_ptr<Primitive> Cube::Clone() const {
    return std::make_unique<Cube>(*this);
}

std::vector<RayIntersection> Cube::IntersectRay(const Ray& ray) const {
    std::vector<RayIntersection> intersections;

    glm::vec3 origin = ray.GetOrigin();
    glm::vec3 direction = ray.GetDirection();

    glm::vec3 halfSize(sizeX / 2.0f, sizeY / 2.0f, sizeZ / 2.0f);
    float xMin = -std::numeric_limits<float>::infinity();
    float xMax = std::numeric_limits<float>::infinity();
    glm::vec3 normalMin(0.0f);
    glm::vec3 normalMax(0.0f);

    for (int axis = 0; axis < 3; axis++) {
        if (std::abs(direction[axis]) < 1e-6f) {
            if (origin[axis] < -halfSize[axis] || origin[axis] > halfSize[axis]) return intersections;
            continue;
        }

        float x1 = (-halfSize[axis] - origin[axis]) / direction[axis];
        float x2 = (halfSize[axis] - origin[axis]) / direction[axis];

        glm::vec3 normal1(0.0f);
        glm::vec3 normal2(0.0f);
        normal1[axis] = -1.0f;
        normal2[axis] = 1.0f;

        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(normal1, normal2);
        }

        if (x1 > xMin) {
            xMin = x1;
            normalMin = normal1;
        }

        if (x2 < xMax) {
            xMax = x2;
            normalMax = normal2;
        }

        if (xMin > xMax) {
            return intersections;
        }
    }

    if (xMax < 0.0f) return intersections;

    if (xMin >= 0.0f) {
        glm::vec3 intersecPoint = origin + xMin * direction;
        intersections.push_back({xMin, intersecPoint, normalMin});
    }

    if (xMax >= 0.0f && xMax > xMin) {
        glm::vec3 intersecPoint = origin + xMax * direction;
        intersections.push_back({xMax, intersecPoint, normalMax});
    }

    return intersections;
}

SMCClassification Cube::CheckIsInside(const glm::vec3& point) const {
    float halfWidth = sizeX / 2.0f;
    float halfHeight = sizeY / 2.0f;
    float halfDepth = sizeZ / 2.0f;
    float e = 1e-6f;

    if (std::abs(std::abs(point.x) - halfWidth) <= e &&
        std::abs(point.y) <= halfHeight + e &&
        std::abs(point.z) <= halfDepth + e) {
        return SMCClassification::ON;
        }

    if (std::abs(std::abs(point.y) - halfHeight) <= e &&
        std::abs(point.x) <= halfWidth + e &&
        std::abs(point.z) <= halfDepth + e) {
        return SMCClassification::ON;
        }

    if (std::abs(std::abs(point.z) - halfDepth) <= e &&
        std::abs(point.x) <= halfWidth + e &&
        std::abs(point.y) <= halfHeight + e) {
        return SMCClassification::ON;
        }

    if (std::abs(point.x) < halfWidth &&
        std::abs(point.y) < halfHeight &&
        std::abs(point.z) < halfDepth) {
        return SMCClassification::IN;
        }

    return SMCClassification::OUT;
}