#pragma once

#include "Primitive.h"

class Cube : public Primitive {
    public:
        Cube(float, float, float);
        Intersection CheckIntersection(const Cell&) const override;
        Cell GetBoundingBox() const override;

    private:
        float sizeX;
        float sizeY;
        float sizeZ;
};