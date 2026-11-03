#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "../Primitives/Primitive.h"
#include "../Primitives/Triangle.h"
#include "../Octree/Octree.h"

enum class CSGNodeType {
    Primitive,
    Boolean,
    Transform
};

enum class BooleanOperation {
    Union,
    Difference,
    Intersection
};

enum class TransformType {
    Scale,
    Rotation,
    Translation
};

class CSGNode {
    public:
        CSGNodeType type;

        std::unique_ptr<CSGNode> left;
        std::unique_ptr<CSGNode> right;

        PrimitiveType primitiveType;
        BooleanOperation booleanOperation;
        TransformType transformType;

        std::unique_ptr<Primitive> primitive;
        glm::vec3 transformValue = glm::vec3(0.0f);

        SMCClassification CheckIsInside(const glm::vec3&) const;
        SMCClassification CheckIsCellInside(const Cell&) const;
        Cell GetBoundingBox();
};

class CSG {
    public:
        CSG() = default;
        void SetRoot(std::unique_ptr<CSGNode>);
        std::unique_ptr<CSGNode> GetAndFreeRoot();
        const CSGNode* GetRoot() const;
        SMCClassification CheckIsInside(const glm::vec3&) const;
        SMCClassification CheckIsCellInside(const Cell&) const;
        Cell GetBoundingBox();

        Octree GenerateOctree();

        static std::unique_ptr<CSGNode> CreatePrimitiveNode(const Primitive&);
        static std::unique_ptr<CSGNode> CreateBooleanNode(BooleanOperation, std::unique_ptr<CSGNode>, std::unique_ptr<CSGNode>);
        static std::unique_ptr<CSGNode> CreateTransformNode(TransformType, glm::vec3, std::unique_ptr<CSGNode>);

    private:
        void Aux(Node*, int);

        std::unique_ptr<CSGNode> root;
};
