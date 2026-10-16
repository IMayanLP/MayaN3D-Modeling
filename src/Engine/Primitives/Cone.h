#pragma once

#include "Primitive.h"

class Cone : public Primitive {
    public:
        Cone(float, float);
        Intersection CheckIntersection(const Cell&) const override;
        Cell GetBoundingBox() const override;

    private:
        float radius;
        float height;
};
