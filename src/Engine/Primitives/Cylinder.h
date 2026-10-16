#pragma once

#include "Primitive.h"

class Cylinder : public Primitive {
    public:
        Cylinder(float, float);
        Intersection CheckIntersection(const Cell&) const override;
        Cell GetBoundingBox() const override;

    private:
        float radius;
        float height;
};