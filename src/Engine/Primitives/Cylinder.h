#pragma once

#include "Primitive.h"

class Cylinder : public Primitive {
    public:
        Cylinder(float, float);
        Intersection CheckIntersection(const Cell&) const override;
        Cell GetBoundingBox() const override;

        std::unique_ptr<Primitive> Clone() const override;
        std::vector<RayIntersection> IntersectRay(const Ray&) const override;
        SMCClassification CheckIsInside(const glm::vec3&) const override;

    private:
        float radius;
        float height;
};