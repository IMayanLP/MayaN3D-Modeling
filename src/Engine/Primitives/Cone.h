#pragma once

#include "Primitive.h"

class Cone : public Primitive {
    public:
        Cone(float, float);
        Intersection CheckIntersection(const Cell&) const override;
        Cell GetBoundingBox() const override;

        std::unique_ptr<Primitive> Clone() const override;
        std::vector<RayIntersection> IntersectRay(const Ray& ray) const override;

    private:
        float radius;
        float height;
};
