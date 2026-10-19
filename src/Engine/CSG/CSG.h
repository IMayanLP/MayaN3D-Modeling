#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "../Primitives/Primitive.h"

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

struct CSGNode {
    CSGNodeType type;

    std::unique_ptr<CSGNode> left;
    std::unique_ptr<CSGNode> right;

    PrimitiveType primitiveType;
    BooleanOperation booleanOperation;
    TransformType transformType;

    std::unique_ptr<Primitive> primitive;
    glm::vec3 transformValue = glm::vec3(0.0f);
};

class CSG {
    public:
        CSG() = default;
        void SetRoot(std::unique_ptr<CSGNode> node);
        const CSGNode* GetRoot() const;

        static std::unique_ptr<CSGNode> CreatePrimitiveNode(const Primitive& primitive);
        static std::unique_ptr<CSGNode> CreateBooleanNode(BooleanOperation operation, std::unique_ptr<CSGNode> leftChild, std::unique_ptr<CSGNode> rightChild);
        static std::unique_ptr<CSGNode> CreateTransformNode(TransformType transform, glm::vec3 value, std::unique_ptr<CSGNode> child);

    private:
        std::unique_ptr<CSGNode> root;
};
