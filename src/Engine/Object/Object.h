#pragma once

#include "../Octree/Octree.h"
#include "../CSG/CSG.h"
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
    CSG csg;
    std::vector<Triangle> mesh;
    bool visible = true;
    glm::vec3 color;

    Object();
};