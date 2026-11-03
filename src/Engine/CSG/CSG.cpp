#include "CSG.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "Ray.h"
#include "SMC.h"

void CSG::SetRoot(std::unique_ptr<CSGNode> node) {
    root = std::move(node);
}

const CSGNode* CSG::GetRoot() const {
    return root.get();
}

std::unique_ptr<CSGNode> CSG::GetAndFreeRoot() {
    return std::move(root);
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

SMCClassification CSG::CheckIsInside(const glm::vec3& point) const {
    if (root == nullptr) return SMCClassification::OUT;
    return root->CheckIsInside(point);
}

SMCClassification CSGNode::CheckIsInside(const glm::vec3& point) const {
    if (type == CSGNodeType::Primitive) return primitive->CheckIsInside(point);

    if (type == CSGNodeType::Transform) {
        if (transformType == TransformType::Translation) {
            glm::vec3 localPoint = point - transformValue;
            return left->CheckIsInside(localPoint);
        }

        if (transformType == TransformType::Scale) {
            glm::vec3 scale = transformValue;
            if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) return SMCClassification::OUT;

            glm::vec3 localPoint = point / transformValue;
            return left->CheckIsInside(localPoint);
        }

        if (transformType == TransformType::Rotation) {
            glm::vec3 values = transformValue;

            glm::mat4 rotation(1.0f);
            rotation = glm::rotate(rotation, values.x, glm::vec3(1.0f, 0.0f, 0.0f));
            rotation = glm::rotate(rotation, values.y, glm::vec3(0.0f, 1.0f, 0.0f));
            rotation = glm::rotate(rotation, values.z, glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 inverseRotation = glm::transpose(rotation);
            glm::vec3 localPoint = glm::vec3(inverseRotation * glm::vec4(point, 1.0f));
            return left->CheckIsInside(localPoint);
        }
    }

    if (type == CSGNodeType::Boolean) {
        SMCClassification lClassification = left->CheckIsInside(point);
        SMCClassification rClassification = right->CheckIsInside(point);

        if (booleanOperation == BooleanOperation::Union) {
            if (lClassification == SMCClassification::IN || rClassification == SMCClassification::IN) return SMCClassification::IN;
            if (lClassification == SMCClassification::ON || rClassification == SMCClassification::ON) return SMCClassification::ON;
            return SMCClassification::OUT;
        }

        if (booleanOperation == BooleanOperation::Intersection) {
            if (lClassification == SMCClassification::OUT || rClassification == SMCClassification::OUT) return SMCClassification::OUT;
            if (lClassification == SMCClassification::ON || rClassification == SMCClassification::ON) return SMCClassification::ON;
            return SMCClassification::IN;
        }

        if (booleanOperation == BooleanOperation::Difference) {
            if (lClassification == SMCClassification::OUT || rClassification == SMCClassification::IN) return SMCClassification::OUT;
            if (lClassification == SMCClassification::ON || rClassification == SMCClassification::ON) return SMCClassification::ON;
            return SMCClassification::IN;
        }
    }

    return SMCClassification::OUT;
}

SMCClassification CSG::CheckIsCellInside(const Cell& cell) const {
    if (root == nullptr) return SMCClassification::OUT;
    return root->CheckIsCellInside(cell);
}

SMCClassification CSGNode::CheckIsCellInside(const Cell& cell) const {
    if (type == CSGNodeType::Primitive) return primitive->CheckIsCellInside(cell);

    if (type == CSGNodeType::Transform) {
        glm::mat4 inverseTransform(1.0f);

        if (transformType == TransformType::Translation) {
            inverseTransform = glm::translate(inverseTransform, -transformValue);
        }

        else if (transformType == TransformType::Scale) {
            if (transformValue.x == 0.0f || transformValue.y == 0.0f || transformValue.z == 0.0f) return SMCClassification::OUT;
            inverseTransform = glm::scale(inverseTransform, glm::vec3(1.0f / transformValue.x, 1.0f / transformValue.y, 1.0f / transformValue.z));
        }

        else if (transformType == TransformType::Rotation) {
            glm::vec3 values = transformValue;

            glm::mat4 rotation(1.0f);
            rotation = glm::rotate(rotation, values.x, glm::vec3(1.0f, 0.0f, 0.0f));
            rotation = glm::rotate(rotation, values.y, glm::vec3(0.0f, 1.0f, 0.0f));
            rotation = glm::rotate(rotation, values.z, glm::vec3(0.0f, 0.0f, 1.0f));

            inverseTransform = glm::transpose(rotation);
        }

        glm::vec3 minPoint(cell.x, cell.y, cell.z);
        glm::vec3 maxPoint(cell.x + cell.size, cell.y + cell.size, cell.z + cell.size);

        glm::vec3 corners[8] = {
            {minPoint.x, minPoint.y, minPoint.z},
            {maxPoint.x, minPoint.y, minPoint.z},
            {minPoint.x, maxPoint.y, minPoint.z},
            {maxPoint.x, maxPoint.y, minPoint.z},
            {minPoint.x, minPoint.y, maxPoint.z},
            {maxPoint.x, minPoint.y, maxPoint.z},
            {minPoint.x, maxPoint.y, maxPoint.z},
            {maxPoint.x, maxPoint.y, maxPoint.z}
        };

        glm::vec3 localMin = glm::vec3(inverseTransform * glm::vec4(corners[0], 1.0f));
        glm::vec3 localMax = localMin;

        for (int i = 1; i < 8; i++) {
            glm::vec3 localPoint = glm::vec3(inverseTransform * glm::vec4(corners[i], 1.0f));

            localMin = glm::min(localMin, localPoint);
            localMax = glm::max(localMax, localPoint);
        }

        glm::vec3 localCenter = (localMin + localMax) / 2.0f;
        float localSize = std::max({localMax.x - localMin.x, localMax.y - localMin.y, localMax.z - localMin.z});

        Cell localCell;
        localCell.size = localSize;
        localCell.x = localCenter.x - localSize / 2.0f;
        localCell.y = localCenter.y - localSize / 2.0f;
        localCell.z = localCenter.z - localSize / 2.0f;

        return left->CheckIsCellInside(localCell);
    }

    if (type == CSGNodeType::Boolean) {
        SMCClassification lClassification = left->CheckIsCellInside(cell);
        SMCClassification rClassification = right->CheckIsCellInside(cell);

        if (booleanOperation == BooleanOperation::Union) {
            if (lClassification == SMCClassification::IN || rClassification == SMCClassification::IN) return SMCClassification::IN;
            if (lClassification == SMCClassification::OUT && rClassification == SMCClassification::OUT) return SMCClassification::OUT;
            return SMCClassification::ON;
        }

        if (booleanOperation == BooleanOperation::Intersection) {
            if (lClassification == SMCClassification::OUT || rClassification == SMCClassification::OUT) return SMCClassification::OUT;
            if (lClassification == SMCClassification::IN && rClassification == SMCClassification::IN) return SMCClassification::IN;
            return SMCClassification::ON;
        }

        if (booleanOperation == BooleanOperation::Difference) {
            if (lClassification == SMCClassification::OUT || rClassification == SMCClassification::IN) return SMCClassification::OUT;
            if (lClassification == SMCClassification::IN && rClassification == SMCClassification::OUT) return SMCClassification::IN;
            return SMCClassification::ON;
        }
    }

    return SMCClassification::OUT;
}

Cell CSG::GetBoundingBox() {
    if (root == nullptr) return {0.0f, 0.0f, 0.0f, 0.0f};
    return root->GetBoundingBox();
}

Cell CSGNode::GetBoundingBox() {
    if (type == CSGNodeType::Primitive) return primitive->GetCSGBoundingBox();

    if (type == CSGNodeType::Transform) {
        Cell cell = left->GetBoundingBox();
        if (transformType == TransformType::Translation) {
            cell.x += transformValue.x;
            cell.y += transformValue.y;
            cell.z += transformValue.z;

            return cell;
        }

        if (transformType == TransformType::Scale) {
            glm::vec3 scale = transformValue;

            if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) return {0.0f, 0.0f, 0.0f, 0.0f};

            cell.x *= scale.x;
            cell.y *= scale.y;
            cell.z *= scale.z;
            cell.size *= std::max({scale.x, scale.y, scale.z});

            return cell;
        }

        if (transformType == TransformType::Rotation) {
            glm::vec3 minPoint(cell.x, cell.y, cell.z);
            glm::vec3 maxPoint(cell.x + cell.size, cell.y + cell.size, cell.z + cell.size);

            glm::vec3 vertices[8] = {
                {minPoint.x, minPoint.y, minPoint.z},
                {maxPoint.x, minPoint.y, minPoint.z},
                {minPoint.x, maxPoint.y, minPoint.z},
                {maxPoint.x, maxPoint.y, minPoint.z},
                {minPoint.x, minPoint.y, maxPoint.z},
                {maxPoint.x, minPoint.y, maxPoint.z},
                {minPoint.x, maxPoint.y, maxPoint.z},
                {maxPoint.x, maxPoint.y, maxPoint.z}
            };

            glm::vec3 values = transformValue;

            glm::mat4 rotationMatrix(1.0f);
            rotationMatrix = glm::rotate(rotationMatrix, values.x, glm::vec3(1.0f, 0.0f, 0.0f));
            rotationMatrix = glm::rotate(rotationMatrix, values.y, glm::vec3(0.0f, 1.0f, 0.0f));
            rotationMatrix = glm::rotate(rotationMatrix, values.z, glm::vec3(0.0f, 0.0f, 1.0f));

            glm::vec3 rotatedMin = glm::vec3(rotationMatrix * glm::vec4(vertices[0], 1.0f));
            glm::vec3 rotatedMax = rotatedMin;

            for (int i = 1; i < 8; i++) {
                glm::vec3 point = glm::vec3(rotationMatrix * glm::vec4(vertices[i], 1.0f));

                rotatedMin = glm::min(rotatedMin, point);
                rotatedMax = glm::max(rotatedMax, point);
            }

            float boundingSize = std::max({rotatedMax.x - rotatedMin.x, rotatedMax.y - rotatedMin.y, rotatedMax.z - rotatedMin.z});

            return {
                (rotatedMin.x + rotatedMax.x - boundingSize) / 2.0f,
                (rotatedMin.y + rotatedMax.y - boundingSize) / 2.0f,
                (rotatedMin.z + rotatedMax.z - boundingSize) / 2.0f,
                boundingSize
            };
        }
    }

    if (type == CSGNodeType::Boolean) {
        Cell lBoundingBox = left->GetBoundingBox();
        Cell rBoundingBox = right->GetBoundingBox();

        glm::vec3 lMin(lBoundingBox.x, lBoundingBox.y, lBoundingBox.z);
        glm::vec3 lMax(lBoundingBox.x + lBoundingBox.size, lBoundingBox.y + lBoundingBox.size, lBoundingBox.z + lBoundingBox.size);

        glm::vec3 rMin(rBoundingBox.x, rBoundingBox.y, rBoundingBox.z);
        glm::vec3 rMax(rBoundingBox.x + rBoundingBox.size, rBoundingBox.y + rBoundingBox.size, rBoundingBox.z + rBoundingBox.size);

        glm::vec3 minPoint = glm::max(lMin, rMin);
        glm::vec3 maxPoint = glm::min(lMax, rMax);

        if (booleanOperation == BooleanOperation::Union) {
            float boundingSize = std::max({maxPoint.x - minPoint.x, maxPoint.y - minPoint.y, maxPoint.z - minPoint.z});

            return {(minPoint.x + maxPoint.x - boundingSize) / 2.0f,
                (minPoint.y + maxPoint.y - boundingSize) / 2.0f,
                (minPoint.z + maxPoint.z - boundingSize) / 2.0f,
                boundingSize
            };
        }

        if (booleanOperation == BooleanOperation::Intersection) {
            if (minPoint.x > maxPoint.x || minPoint.y > maxPoint.y || minPoint.z > maxPoint.z) {
                return {0.0f, 0.0f, 0.0f, 0.0f};
            }

            float boundingSize = std::max({
                maxPoint.x - minPoint.x,
                maxPoint.y - minPoint.y,
                maxPoint.z - minPoint.z
            });

            return {
                (minPoint.x + maxPoint.x - boundingSize) / 2.0f,
                (minPoint.y + maxPoint.y - boundingSize) / 2.0f,
                (minPoint.z + maxPoint.z - boundingSize) / 2.0f,
                boundingSize
            };
        }

        if (booleanOperation == BooleanOperation::Difference) {
            return lBoundingBox;
        }
    }

    return {0.0f, 0.0f, 0.0f, 0.0f};
}

Octree CSG::GenerateOctree() {
    Octree octree;

    if (!root) return octree;

    octree.root->cell = GetBoundingBox();

    Aux(octree.root.get(), 0);

    return octree;
}

void CSG::Aux(Node* octreeNode, int depth) {
    SMCClassification CSGClassification = CheckIsCellInside(octreeNode->cell);
    if (CSGClassification == SMCClassification::IN) octreeNode->state = Intersection::Inside;
    else if (CSGClassification == SMCClassification::OUT) octreeNode->state = Intersection::Outside;
    else octreeNode->state = Intersection::Intersecting;

    if (octreeNode->state != Intersection::Intersecting || depth >= 5) return;

    float size = octreeNode->cell.size / 2.0f;

    for (int i = 0; i < 8; i++) {
        octreeNode->children[i] = std::make_unique<Node>();

        octreeNode->children[i]->parent = octreeNode;
        octreeNode->children[i]->cell.size = size;

        Offset offset = generateOffset(i, size);

        octreeNode->children[i]->cell.x = octreeNode->cell.x + offset.x;
        octreeNode->children[i]->cell.y = octreeNode->cell.y + offset.y;
        octreeNode->children[i]->cell.z = octreeNode->cell.z + offset.z;

        Aux(octreeNode->children[i].get(), depth + 1);
    }
}