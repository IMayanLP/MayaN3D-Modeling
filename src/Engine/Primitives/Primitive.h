#pragma once

#include <glm/glm.hpp>

enum class Intersection {
    Outside,
    Inside,
    Intersecting
};

enum class PrimitiveType {
    Cube,
    Sphere,
    Cylinder,
    Cone
};

struct Cell {
    float x, y, z, size;
};

class Primitive {
    public:
        virtual ~Primitive() = default;
        virtual Intersection CheckIntersection(const Cell&) const = 0;
        virtual Cell GetBoundingBox() const = 0;
        void SetRotation(float, float, float);

    protected:
        glm::vec3 rotation = glm::vec3(0.0f);

        glm::mat4 GetRotationMatrix() const;
};