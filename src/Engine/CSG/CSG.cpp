#include "CSG.h"

void CSG::SetRoot(std::unique_ptr<CSGNode> node) {
    root = std::move(node);
}

const CSGNode* CSG::GetRoot() const {
    return root.get();
}

std::unique_ptr<CSGNode> CSG::CreatePrimitiveNode(const Primitive& primitive) {
    std::unique_ptr<CSGNode> node = std::make_unique<CSGNode>();

    node->type = CSGNodeType::Primitive;
    node->primitiveType = primitive.getType();
    node->primitive = primitive.Clone();

    return node;
}

std::unique_ptr<CSGNode> CSG::CreateBooleanNode(BooleanOperation operation, std::unique_ptr<CSGNode> left, std::unique_ptr<CSGNode> right) {
    std::unique_ptr<CSGNode> node = std::make_unique<CSGNode>();

    node->type = CSGNodeType::Boolean;
    node->booleanOperation = operation;
    node->left = std::move(left);
    node->right = std::move(right);

    return node;
}

std::unique_ptr<CSGNode> CSG::CreateTransformNode(TransformType transform, glm::vec3 value, std::unique_ptr<CSGNode> child) {
    std::unique_ptr<CSGNode> node = std::make_unique<CSGNode>();

    node->type = CSGNodeType::Transform;
    node->transformType = transform;
    node->transformValue = value;
    node->left = std::move(child);

    return node;
}