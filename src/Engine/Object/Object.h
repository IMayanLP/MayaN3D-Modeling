#pragma once

#include "../Octree/Octree.h"
#include "../Primitives/Triangle.h"

#include <vector>

enum class ObjectType {
    Octree,
    CSG,
    BRep
};

class Object {
public:
    ObjectType type = ObjectType::Octree;
    Octree octree;
    std::vector<Triangle> mesh;
    bool visible = true;

    Object();
};