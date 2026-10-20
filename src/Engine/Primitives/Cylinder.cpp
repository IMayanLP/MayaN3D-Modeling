#include "Cylinder.h"
#include <algorithm>
#include <cmath>

Cylinder::Cylinder(float radius, float height) : Primitive(PrimitiveType::Cylinder), radius(radius), height(height) {}

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

std::unique_ptr<Primitive> Cylinder::Clone() const {
    return std::make_unique<Cylinder>(*this);
}

std::vector<RayIntersection> Cylinder::IntersectRay(const Ray& ray) const {
    std::vector<RayIntersection> intersections;

    glm::vec3 origin = ray.GetOrigin();
    glm::vec3 direction = ray.GetDirection();

    float halfHeight = height / 2.0f;

    float a = direction.x * direction.x + direction.z * direction.z;
    float b = 2.0f * (origin.x * direction.x + origin.z * direction.z);
    float c = origin.x * origin.x + origin.z * origin.z - radius * radius;

    if (a > 1e-6f) {
        float delta = b * b - 4.0f * a * c;

        if (delta >= 0.0f) {
            float deltaSqrt = std::sqrt(delta);
            float x1 = (-b - deltaSqrt) / (2.0f * a);
            float x2 = (-b + deltaSqrt) / (2.0f * a);

            if (x1 >= 0.0f) {
                glm::vec3 intersecPoint = origin + x1 * direction;

                if (intersecPoint.y >= -halfHeight && intersecPoint.y <= halfHeight) {
                    glm::vec3 normal = glm::normalize(glm::vec3(intersecPoint.x, 0.0f, intersecPoint.z));
                    intersections.push_back({x1, intersecPoint, normal});
                }
            }

            if (delta > 0.0f && x2 >= 0.0f) {
                glm::vec3 intersecPoint = origin + x2 * direction;

                if (intersecPoint.y >= -halfHeight && intersecPoint.y <= halfHeight) {
                    glm::vec3 normal = glm::normalize(glm::vec3(intersecPoint.x, 0.0f, intersecPoint.z));
                    intersections.push_back({x2, intersecPoint, normal});
                }
            }
        }
    }

    if (std::abs(direction.y) > 1e-6f) {
        float bot = (-halfHeight - origin.y) / direction.y;

        if (bot >= 0.0f) {
            glm::vec3 point = origin + bot * direction;

            if (point.x * point.x + point.z * point.z <= radius * radius) {
                intersections.push_back({bot, point, glm::vec3(0.0f, -1.0f, 0.0f)});
            }
        }

        float top = (halfHeight - origin.y) / direction.y;

        if (top >= 0.0f) {
            glm::vec3 point = origin + top * direction;

            if (point.x * point.x + point.z * point.z <= radius * radius) {
                intersections.push_back({top, point, glm::vec3(0.0f, 1.0f, 0.0f)});
            }
        }
    }

    std::sort(intersections.begin(), intersections.end(), [](const RayIntersection& a, const RayIntersection& b) {
            return a.t < b.t;
        });

    return intersections;
}

SMCClassification Cylinder::CheckIsInside(const glm::vec3& point) const {
    float halfHeight = height / 2.0f;
    float radialDist2 = point.x * point.x + point.z * point.z;
    float radius2 = radius * radius;
    float e = 1e-6f;

    bool insideHeight = std::abs(point.y) < halfHeight;
    bool insideRadius = radialDist2 < radius2;

    if (std::abs(std::abs(point.y) - halfHeight) <= e && radialDist2 <= radius2 + e) return SMCClassification::ON;
    if (std::abs(radialDist2 - radius2) <= e && std::abs(point.y) <= halfHeight + e) return SMCClassification::ON;
    if (insideHeight && insideRadius) return SMCClassification::IN;

    return SMCClassification::OUT;
}