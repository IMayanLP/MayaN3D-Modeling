#include "CSG.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "SMC.h"
#include <iostream>

#include <fstream>
#include <iomanip>
#include <functional>
#include <algorithm>

void CSG::SetRoot(std::unique_ptr<CSGNode> node) {
    root = std::move(node);
}

const CSGNode* CSG::GetRoot() const {
    return root.get();
}

std::unique_ptr<CSGNode> CSG::GetAndFreeRoot() {
    return std::move(root);
}

void CSG::setLabel(std::string name) {
    label = name;
}
std::string CSG::getLabel() const {
    return label;
}



// Operadores de nós
std::unique_ptr<CSGNode> CSG::CreatePrimitiveNode(Primitive& primitive) {
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



// Conversor para Octree
SMCClassification CSG::CheckIsInside(glm::vec3& point) {
    if (root == nullptr) return SMCClassification::OUT;
    return root->CheckIsInside(point);
}

SMCClassification CSGNode::CheckIsInside(glm::vec3& point) {
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

SMCClassification CSG::CheckIsCellInside(Cell& cell) {
    if (root == nullptr) return SMCClassification::OUT;
    return root->CheckIsCellInside(cell);
}

SMCClassification CSGNode::CheckIsCellInside(Cell& cell) {
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

        if (booleanOperation == BooleanOperation::Union) {
            glm::vec3 minPoint = glm::min(lMin, rMin);
            glm::vec3 maxPoint = glm::max(lMax, rMax);
            float boundingSize = std::max({maxPoint.x - minPoint.x, maxPoint.y - minPoint.y, maxPoint.z - minPoint.z});

            Cell cell = {(minPoint.x + maxPoint.x - boundingSize) / 2.0f,
                (minPoint.y + maxPoint.y - boundingSize) / 2.0f,
                (minPoint.z + maxPoint.z - boundingSize) / 2.0f,
                boundingSize
            };

            return cell;
        }

        if (booleanOperation == BooleanOperation::Intersection) {
            glm::vec3 minPoint = glm::max(lMin, rMin);
            glm::vec3 maxPoint = glm::min(lMax, rMax);
            if (minPoint.x > maxPoint.x || minPoint.y > maxPoint.y || minPoint.z > maxPoint.z) {
                return {0.0f, 0.0f, 0.0f, 0.0f};
            }

            float boundingSize = std::max({maxPoint.x - minPoint.x, maxPoint.y - minPoint.y, maxPoint.z - minPoint.z});

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



// I/O
// auxiliares
char GetPrimitiveCode(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Cube: return 'B';
        case PrimitiveType::Sphere: return 'E';
        case PrimitiveType::Cylinder: return 'C';
        case PrimitiveType::Cone: return 'N';
    }

    return '-';
}

char GetBooleanCode(BooleanOperation operation) {
    switch (operation) {
        case BooleanOperation::Union: return 'U';
        case BooleanOperation::Intersection: return 'I';
        case BooleanOperation::Difference: return 'D';
    }

    return '-';
}

char GetTransformCode(TransformType type) {
    switch (type) {
        case TransformType::Translation: return 'T';
        case TransformType::Rotation: return 'R';
        case TransformType::Scale: return 'S';
    }

    return '-';
}

bool CSG::Export(std::string& fileName) {
    if (!root) return false;

    std::ofstream file("files/output/" + fileName + ".txt");
    if (!file.is_open()) return false;

    int count = 0;
    std::string rootId = ExportNode(root.get(), {}, file, count);

    file.close();
    return !rootId.empty() && file.good();
}

std::string CSG::ExportNode(CSGNode* node, std::vector<CSGNode*> transforms, std::ofstream& file, int& count) {
    if (node->type == CSGNodeType::Transform) {
        if (!node->left) return "";

        transforms.push_back(node);

        return ExportNode(node->left.get(), transforms, file, count);
    }

    if (node->type == CSGNodeType::Primitive) {
        count++;

        std::string nodeId = "No" + std::to_string(count);
        file << nodeId << ";" << GetPrimitiveCode(node->primitiveType);

        glm::vec3 translation = {0.0f, 0.0f, 0.0f};
        glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};

        for (int i = transforms.size() - 1; i >= 0; i--) {
            CSGNode* transform = transforms[i];
            glm::vec3 value = transform->transformValue;

            if (transform->transformType == TransformType::Translation) translation = value;
            else if (transform->transformType == TransformType::Rotation) rotation = value;
            else if (transform->transformType == TransformType::Scale) scale = value;
        }

        file << ";S";
        if (scale.x == scale.y && scale.y == scale.z) file << (scale.x);
        else file << scale.x << "," << scale.y << "," << scale.z;
        file << ";R" << rotation.x << "," << rotation.y << "," << rotation.z;
        file << ";T" << translation.x << "," << translation.y << "," << translation.z;
        file << "\n";

        return nodeId;
    }

    if (node->type == CSGNodeType::Boolean) {
        if (!node->left || !node->right) return "";

        std::string leftId = ExportNode(node->left.get(), {}, file, count);
        std::string rightId = ExportNode(node->right.get(), {}, file, count);

        count++;
        std::string nodeId = "No" + std::to_string(count);
        file << nodeId << ";" << GetBooleanCode(node->booleanOperation) << leftId << "," << rightId << "\n";

        return nodeId;
    }

    return "";
}

bool CSG::ExportLegacy(std::string& fileName) {
    if (!root) return false;

    std::ofstream file("files/output/" + fileName + ".txt");
    if (!file.is_open()) return false;

    int count = 0;
    std::string rootId = ExportLegacyNode(root.get(), file, count);

    file.close();
    return !rootId.empty() && file.good();
}

std::string CSG::ExportLegacyNode(CSGNode* node, std::ofstream& file, int& count) {
    if (node->type == CSGNodeType::Primitive) {
        count++;

        std::string nodeId = "No" + std::to_string(count);
        file << nodeId << ";" << GetPrimitiveCode(node->primitiveType) << "\n";

        return nodeId;
    }

    if (node->type == CSGNodeType::Transform) {
        if (!node->left) return "";

        std::string childId = ExportLegacyNode(node->left.get(), file, count);
        count++;

        std::string nodeId = "No" + std::to_string(count);
        glm::vec3 value = node->transformValue;
        file << nodeId << ";" << GetTransformCode(node->transformType) << ";" << value.x << "," << value.y << "," << value.z << ";" << childId << "\n";

        return nodeId;
    }

    if (node->type == CSGNodeType::Boolean) {
        if (!node->left || !node->right) return "";

        std::string leftId = ExportLegacyNode(node->left.get(), file, count);
        std::string rightId = ExportLegacyNode(node->right.get(), file, count);
        count++;

        std::string nodeId = "No" + std::to_string(count);
        file << nodeId << ";" << GetBooleanCode(node->booleanOperation) << ";" << leftId << "," << rightId << "\n";

        return nodeId;
    }

    return "";
}


// import
static std::vector<std::string> Split(const std::string& text, char delimiter) {
    std::vector<std::string> result;
    std::stringstream stream(text);
    std::string item;

    while (std::getline(stream, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

static std::unique_ptr<CSGNode> GetNode(std::vector<ImportedNode>& nodes, const std::string& label) {
    for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
        if (nodes[i].label == label) {
            std::unique_ptr<CSGNode> node = std::move(nodes[i].node);

            nodes.erase(nodes.begin() + i);

            return node;
        }
    }

    return nullptr;
}

CSG CSG::Import(const std::string& fileName) {
    CSG newCSG;
    newCSG.setLabel("CSG não encontrada");

    std::ifstream file("files/input/" + fileName);
    if (!file.is_open()) return newCSG;

    std::vector<ImportedNode> nodes;
    std::string line;
    int count = 1;
    while (std::getline(file, line)) {
        newCSG.setLabel("Corrompido na linha " + std::to_string(count));
        std::vector<std::string> fields = Split(line, ';');

        if (fields.size() < 2) return newCSG;

        std::string label = fields[0];
        std::string type = fields[1].substr(0, 1);

        if (type == "B" || type == "E" || type == "C" || type == "N") {
            glm::vec3 scale(1.0f);
            glm::vec3 rotation(0.0f);
            glm::vec3 translation(0.0f);

            for (int i = 2; i < fields.size(); i++) {
                std::string transformType = fields[i].substr(0, 1);
                std::string transformValue = fields[i].substr(1);
                std::vector<std::string> values = Split(transformValue, ',');

                if (transformType == "S") {
                    if (values.size() == 1)
                        scale = glm::vec3(std::stof(values[0]));
                    else if (values.size() == 3)
                        scale = glm::vec3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
                    else return newCSG;
                }
                else if (transformType == "R")
                    rotation = glm::vec3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));

                else if (transformType == "T") {
                    if (values.size() != 3) return newCSG;
                    translation = glm::vec3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
                }
            }

            if (type == "B") {
                Cube cube(1.0f, 1.0f, 1.0f);
                std::unique_ptr<CSGNode> cubeNode = CreatePrimitiveNode(cube);

                if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
                    cubeNode = CreateTransformNode(TransformType::Scale, scale, std::move(cubeNode));

                if (rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f)
                    cubeNode = CreateTransformNode(TransformType::Rotation, rotation, std::move(cubeNode));

                if (translation.x != 0.0f || translation.y != 0.0f || translation.z != 0.0f)
                    cubeNode = CreateTransformNode(TransformType::Translation, translation, std::move(cubeNode));

                nodes.push_back({label, std::move(cubeNode)});
            } else if (type == "E") {
                Sphere sphere(0.5f);
                std::unique_ptr<CSGNode> sphereNode = CreatePrimitiveNode(sphere);

                if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
                    sphereNode = CreateTransformNode(TransformType::Scale, scale, std::move(sphereNode));

                if (rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f)
                    sphereNode = CreateTransformNode(TransformType::Rotation, rotation, std::move(sphereNode));

                if (translation.x != 0.0f || translation.y != 0.0f || translation.z != 0.0f)
                    sphereNode = CreateTransformNode(TransformType::Translation, translation, std::move(sphereNode));

                nodes.push_back({label, std::move(sphereNode)});
            } else if (type == "C") {
                Cylinder cylinder(0.5f, 1.0f);
                std::unique_ptr<CSGNode> cylinderNode = CreatePrimitiveNode(cylinder);

                if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
                    cylinderNode = CreateTransformNode(TransformType::Scale, scale, std::move(cylinderNode));

                if (rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f)
                    cylinderNode = CreateTransformNode(TransformType::Rotation, rotation, std::move(cylinderNode));

                if (translation.x != 0.0f || translation.y != 0.0f || translation.z != 0.0f)
                    cylinderNode = CreateTransformNode(TransformType::Translation, translation, std::move(cylinderNode));

                nodes.push_back({label, std::move(cylinderNode)});
            } else {
                Cone cone(0.5f, 1.0f);
                std::unique_ptr<CSGNode> coneNode = CreatePrimitiveNode(cone);

                if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
                    coneNode = CreateTransformNode(TransformType::Scale, scale, std::move(coneNode));

                if (rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f)
                    coneNode = CreateTransformNode(TransformType::Rotation, rotation, std::move(coneNode));

                if (translation.x != 0.0f || translation.y != 0.0f || translation.z != 0.0f)
                    coneNode = CreateTransformNode(TransformType::Translation, translation, std::move(coneNode));

                nodes.push_back({label, std::move(coneNode)});
            }
        }
        else if (type == "U" || type == "I" || type == "D") {
            std::string referencesText = fields[1].substr(1);
            std::vector<std::string> references = Split(referencesText, ',');

            std::unique_ptr<CSGNode> left = GetNode(nodes, references[0]);
            std::unique_ptr<CSGNode> right = GetNode(nodes, references[1]);

            BooleanOperation operation;

            if (type == "U") operation = BooleanOperation::Union;
            else if (type == "I") operation = BooleanOperation::Intersection;
            else operation = BooleanOperation::Difference;

            std::unique_ptr<CSGNode> node = CreateBooleanNode(operation, std::move(left), std::move(right));

            nodes.push_back({label, std::move(node)});
        }

        count++;
    }

    if (nodes.size() != 1) return newCSG;

    newCSG.SetRoot(std::move(nodes[0].node));
    newCSG.setLabel(fileName);

    return newCSG;
}

CSG CSG::ImportLegacy(const std::string& fileName) {
    CSG newCSG;
    newCSG.setLabel("CSG não encontrada");

    std::ifstream file("files/input/" + fileName);
    if (!file.is_open()) return newCSG;

    std::vector<ImportedNode> nodes;
    std::string line;
    int count = 1;
    while (std::getline(file, line)) {
        newCSG.setLabel("Corrompido na linha " + std::to_string(count));
        std::vector<std::string> fields = Split(line, ';');

        if (fields.size() < 2) return newCSG;

        std::string label = fields[0];
        std::string type = fields[1];

        if (type == "B" || type == "E" || type == "C" || type == "N") {
            if (type == "B") {
                Cube cube(1.0f, 1.0f, 1.0f);
                std::unique_ptr<CSGNode> cubeNode = CreatePrimitiveNode(cube);

                nodes.push_back({label, std::move(cubeNode)});
            } else if (type == "E") {
                Sphere sphere(0.5f);
                std::unique_ptr<CSGNode> sphereNode = CreatePrimitiveNode(sphere);

                nodes.push_back({label, std::move(sphereNode)});
            } else if (type == "C") {
                Cylinder cylinder(0.5f, 1.0f);
                std::unique_ptr<CSGNode> cylinderNode = CreatePrimitiveNode(cylinder);

                nodes.push_back({label, std::move(cylinderNode)});
            } else {
                Cone cone(0.5f, 1.0f);
                std::unique_ptr<CSGNode> coneNode = CreatePrimitiveNode(cone);

                nodes.push_back({label, std::move(coneNode)});
            }
        } else if (type == "S" || type == "T" || type == "R") {
            std::vector<std::string> values = Split(fields[2], ',');
            glm::vec3 t = glm::vec3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));

            std::unique_ptr<CSGNode> left = GetNode(nodes, fields[3]);
            std::unique_ptr<CSGNode> transformNode;

            if (type == "S") transformNode = CreateTransformNode(TransformType::Scale, t, std::move(left));
            else if (type == "R") transformNode = CreateTransformNode(TransformType::Rotation, t, std::move(left));
            else if (type == "T") transformNode = CreateTransformNode(TransformType::Translation, t, std::move(left));

            if (!transformNode) return newCSG;

            nodes.push_back({label, std::move(transformNode)});
        } else if (type == "U" || type == "I" || type == "D") {
            std::vector<std::string> references = Split(fields[2], ',');

            std::unique_ptr<CSGNode> left = GetNode(nodes, references[0]);
            std::unique_ptr<CSGNode> right = GetNode(nodes, references[1]);
            BooleanOperation operation;

            if (type == "U") operation = BooleanOperation::Union;
            else if (type == "I") operation = BooleanOperation::Intersection;
            else operation = BooleanOperation::Difference;

            std::unique_ptr<CSGNode> node = CreateBooleanNode(operation, std::move(left), std::move(right));

            nodes.push_back({label, std::move(node)});
        }

        count++;
    }

    if (nodes.size() != 1) return newCSG;

    newCSG.SetRoot(std::move(nodes[0].node));
    newCSG.setLabel(fileName);

    return newCSG;
}