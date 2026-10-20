#pragma once

#include "Primitive.h"

class Cube : public Primitive {
    public:
        Cube(float, float, float);
        Intersection CheckIntersection(const Cell&) const override;
        Cell GetBoundingBox() const override;

        std::unique_ptr<Primitive> Clone() const override;
        std::vector<RayIntersection> IntersectRay(const Ray& ray) const override;
        SMCClassification CheckIsInside(const glm::vec3& point) const override;

    private:
        float sizeX;
        float sizeY;
        float sizeZ;
};