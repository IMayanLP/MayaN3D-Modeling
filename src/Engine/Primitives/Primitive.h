#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "../Ray/Ray.h"

enum class Intersection {
    Outside,
    Inside,
    Intersecting
};

enum class SMCClassification {
    IN,
    ON,
    OUT
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
        Primitive(PrimitiveType type) : type(type) {}

        // Octree
        virtual Intersection CheckIntersection(const Cell&) const = 0;
        virtual Cell GetBoundingBox() const = 0;
        void SetRotation(float, float, float);
        PrimitiveType getType() const { return type; }

        // CSG
        virtual std::unique_ptr<Primitive> Clone() const = 0;
        virtual std::vector<RayIntersection> IntersectRay(const Ray&) const = 0;
        virtual SMCClassification CheckIsInside(const glm::vec3&) const = 0;

    protected:
        glm::vec3 rotation = glm::vec3(0.0f);
        PrimitiveType type;

        glm::mat4 GetRotationMatrix() const;
};