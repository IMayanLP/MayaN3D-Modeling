#include "Octree.h"

#include <iostream>

int DefaultMaxLevel = 6;

Octree::Octree() : root(std::make_unique<Node>()) {}

Offset generateOffset(int index, float size) {
    Offset offset{};

    if (index == 0) {
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = 0.0f;
    } else if (index == 1) {
        offset.x = size;
        offset.y = 0.0f;
        offset.z = 0.0f;
    } else if (index == 2) {
        offset.x = 0.0f;
        offset.y = size;
        offset.z = 0.0f;
    } else if (index == 3) {
        offset.x = size;
        offset.y = size;
        offset.z = 0.0f;
    } else if (index == 4) {
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = size;
    } else if (index == 5) {
        offset.x = size;
        offset.y = 0.0f;
        offset.z = size;
    } else if (index == 6) {
        offset.x = 0.0f;
        offset.y = size;
        offset.z = size;
    } else {
        offset.x = size;
        offset.y = size;
        offset.z = size;
    }

    return offset;
}

const Node* Octree::GetRoot() const {
    return root.get();
}

void Octree::setMaxLevel(int level) {
    maxLevel = level;
}

int Octree::GetMaxLevel() const {
    return maxLevel;
}

void Octree::setLabel(std::string name) {
    label = name;
}
std::string Octree::getLabel() const {
    return label;
}

void Octree::Print() {
    PrintNode(root.get());
}

void Octree::PrintNode(const Node* node) const {
    if (node == nullptr) {
        return;
    }

    if (node->state == Intersection::Intersecting) {
        bool hasChildren = false;
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr) {
                hasChildren = true;
                break;
            }
        }

        if (hasChildren) {
            std::cout << '(';
            for (int i = 0; i < 8; i++) {
                PrintNode(node->children[i].get());
            }
        } else {
            std::cout << 'B';
        }
    } else {
        char c = (node->state == Intersection::Outside) ? 'W' : 'B';
        std::cout << c;
    }
}

std::string Octree::ToString() const {
    std::string result;
    ToStringNode(root.get(), result);
    return result;
}

void Octree::ToStringNode(const Node* node, std::string& out) const {
    if (node == nullptr) {
        return;
    }

    if (node->state == Intersection::Intersecting) {
        bool hasChildren = false;
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr) {
                hasChildren = true;
                break;
            }
        }

        if (hasChildren) {
            out += '(';
            for (int i = 0; i < 8; i++) {
                ToStringNode(node->children[i].get(), out);
            }
        } else {
            out += 'B';
        }
    } else {
        out += (node->state == Intersection::Outside) ? 'W' : 'B';
    }
}



Octree Octree::ImportFromString(const std::string& data) {
    Octree octree;

    int pos = 0;
    int maxDepthFound = 0;

    const Cell rootCell = {-1, -1, -1, 2};

    octree.root = ImportNode(data, pos, rootCell, nullptr, 0, maxDepthFound);
    octree.setMaxLevel(maxDepthFound);

    return octree;
}

std::unique_ptr<Node> Octree::ImportNode(const std::string& data, int& pos, const Cell& cell, Node* parent, int depth, int& maxDepthOut) {
    if (pos >= data.size()) {
        return nullptr;
    }

    auto node = std::make_unique<Node>();
    node->cell = cell;
    node->parent = parent;

    maxDepthOut = std::max(maxDepthOut, depth);

    char c = data[pos];

    if (c == '(') {
        pos++; // consome '('
        node->state = Intersection::Intersecting;

        float childSize = cell.size / 2.0f;

        for (int i = 0; i < 8; i++) {
            Offset offset = generateOffset(i, childSize);

            Cell childCell;
            childCell.size = childSize;
            childCell.x = cell.x + offset.x;
            childCell.y = cell.y + offset.y;
            childCell.z = cell.z + offset.z;

            node->children[i] = ImportNode(data, pos, childCell, node.get(), depth + 1, maxDepthOut);
        }
    } else if (c == 'B') {
        pos++;
        node->state = Intersection::Inside;
    } else if (c == 'W') {
        pos++;
        node->state = Intersection::Outside;
    } else {
        pos++; // caractere inesperado — ignora e segue (defensivo)
    }

    return node;
}



// GERADOR DA ARVORE
void Octree::Generate(Primitive& primitive) {
    root->cell = primitive.GetBoundingBox();

    Aux(primitive, root.get(), 0);
}

void Octree::Aux(Primitive& primitive, Node* node, int depth) {
    node->state = primitive.CheckIntersection(node->cell);

    if (node->state != Intersection::Intersecting || depth >= maxLevel) {
        return;
    }

    float size = node->cell.size / 2.0f;

    for (int i = 0; i < 8; i++) {
        node->children[i] = std::make_unique<Node>();

        node->children[i]->parent = node;
        node->children[i]->cell.size = size;

        Offset offset = generateOffset(i, size);

        node->children[i]->cell.x = node->cell.x + offset.x;
        node->children[i]->cell.y = node->cell.y + offset.y;
        node->children[i]->cell.z = node->cell.z + offset.z;

        Aux(primitive, node->children[i].get(), depth + 1);
    }
}



// OPERADORES GEOMÉTRICOS
void Octree::SetScale(float newScale) {
    float factor = newScale / scale;
    scale = newScale;

    ScaleNode(root.get(), factor, translation);
}

void Octree::ScaleNode(Node* node, float factor, const glm::vec3& center) {
    if (node == nullptr) return;

    node->cell.x = center.x + (node->cell.x - center.x) * factor;
    node->cell.y = center.y + (node->cell.y - center.y) * factor;
    node->cell.z = center.z + (node->cell.z - center.z) * factor;
    node->cell.size *= factor;

    for (int i = 0; i < 8; i++) {
        ScaleNode(node->children[i].get(), factor, center);
    }
}

float Octree::GetScale() const {
    return scale;
}

void Octree::SetTranslation(float x, float y, float z) {
    glm::vec3 newTranslation(x, y, z);
    glm::vec3 offset = newTranslation - translation;

    translation = newTranslation;

    TranslateNode(root.get(), offset);
}

void Octree::TranslateNode(Node* node, const glm::vec3& offset) {
    if (node == nullptr) return;

    node->cell.x += offset.x;
    node->cell.y += offset.y;
    node->cell.z += offset.z;

    for (int i = 0; i < 8; i++) {
        TranslateNode(node->children[i].get(), offset);
    }
}

glm::vec3 Octree::GetTranslation() const {
    return translation;
}



// OPERACOES BOOLEANAS (PREPARAÇÃO)
Cell Octree::GenerateOctreeBoundingBox(const Octree& octree1, const Octree& octree2) {
    const Cell& cell1 = octree1.GetRoot()->cell;
    const Cell& cell2 = octree2.GetRoot()->cell;

    float minX = std::min(cell1.x, cell2.x);
    float minY = std::min(cell1.y, cell2.y);
    float minZ = std::min(cell1.z, cell2.z);

    float maxX = std::max(cell1.x + cell1.size, cell2.x + cell2.size);
    float maxY = std::max(cell1.y + cell1.size, cell2.y + cell2.size);
    float maxZ = std::max(cell1.z + cell1.size, cell2.z + cell2.size);

    float size = std::max({
        maxX - minX,
        maxY - minY,
        maxZ - minZ
    });

    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float centerZ = (minZ + maxZ) / 2.0f;

    return {
        centerX - size / 2.0f,
        centerY - size / 2.0f,
        centerZ - size / 2.0f,
        size
    };
}

int Octree::CalculateMaxLevel(const Cell& boundingBox, const Octree& octree1, const Octree& octree2) {
    float cellSize1 = octree1.GetRoot()->cell.size / std::pow(2.0f, octree1.GetMaxLevel());
    float cellSize2 = octree2.GetRoot()->cell.size / std::pow(2.0f, octree2.GetMaxLevel());

    float smallestCellSize = std::min(cellSize1, cellSize2);

    float ratio = boundingBox.size / smallestCellSize;

    return static_cast<int>(std::ceil(std::log2(ratio)));
}

Octree Octree::GenerateBoundedOctree(const Cell& boundingBox, const Octree& octree, int maxLevel) {
    Octree result;

    result.root->cell = boundingBox;
    result.maxLevel = maxLevel;

    result.AuxBoundedOctree(result.root.get(), 0, maxLevel);

    result.ClassifyNode(octree, result.root.get(), 0);

    return result;
}

void Octree::AuxBoundedOctree(Node* node, int depth, int maxLevel) {
    node->state = (depth == maxLevel) ? Intersection::Outside : Intersection::Intersecting;

    if (depth >= maxLevel) return;

    float size = node->cell.size / 2.0f;

    for (int i = 0; i < 8; i++) {
        node->children[i] = std::make_unique<Node>();

        node->children[i]->parent = node;
        node->children[i]->cell.size = size;

        Offset offset = generateOffset(i, size);

        node->children[i]->cell.x = node->cell.x + offset.x;
        node->children[i]->cell.y = node->cell.y + offset.y;
        node->children[i]->cell.z = node->cell.z + offset.z;

        AuxBoundedOctree(node->children[i].get(), depth + 1, maxLevel);
    }
}

void Octree::ClassifyNode(const Octree& octree, Node* node, int depth) {
    if (depth < maxLevel) {
        bool allInside = true;
        bool allOutside = true;

        for (int i = 0; i < 8; i++) {
            ClassifyNode(octree, node->children[i].get(), depth + 1);

            if (node->children[i]->state != Intersection::Inside) allInside = false;
            if (node->children[i]->state != Intersection::Outside) allOutside = false;
        }

        if (allInside) {
            node->state = Intersection::Inside;

            for (int i = 0; i < 8; i++) node->children[i].reset();

            return;
        }

        if (allOutside) {
            node->state = Intersection::Outside;

            for (int i = 0; i < 8; i++) node->children[i].reset();

            return;
        }

        node->state = Intersection::Intersecting;
        return;
    }

    node->state = GetNodeIntersection(octree.root.get(), node->cell);
}

Intersection Octree::GetNodeIntersection(const Node* node, const Cell& cell) {
    bool outside = cell.x + cell.size <= node->cell.x || cell.x >= node->cell.x + node->cell.size ||
        cell.y + cell.size <= node->cell.y || cell.y >= node->cell.y + node->cell.size ||
        cell.z + cell.size <= node->cell.z || cell.z >= node->cell.z + node->cell.size;

    if (outside) return Intersection::Outside;

    bool inside = cell.x >= node->cell.x && cell.x + cell.size <= node->cell.x + node->cell.size &&
        cell.y >= node->cell.y && cell.y + cell.size <= node->cell.y + node->cell.size &&
        cell.z >= node->cell.z && cell.z + cell.size <= node->cell.z + node->cell.size;

    if (node->state == Intersection::Inside && inside) return Intersection::Inside;
    if (node->state == Intersection::Outside) return Intersection::Outside;
    if (node->children[0] == nullptr) return Intersection::Intersecting;

    bool hasInside = false;
    bool hasOutside = false;
    bool hasIntersecting = false;
    bool hasOverlap = false;

    for (int i = 0; i < 8; i++) {
        const Node* child = node->children[i].get();
        const Cell& childCell = child->cell;

        bool childOutside = cell.x + cell.size <= childCell.x || cell.x >= childCell.x + childCell.size ||
            cell.y + cell.size <= childCell.y || cell.y >= childCell.y + childCell.size ||
            cell.z + cell.size <= childCell.z || cell.z >= childCell.z + childCell.size;

        if (childOutside) continue;

        hasOverlap = true;

        Intersection result = GetNodeIntersection(child, cell);

        if (result == Intersection::Inside) hasInside = true;
        else if (result == Intersection::Outside) hasOutside = true;
        else hasIntersecting = true;
    }

    if (!hasOverlap) return Intersection::Outside;
    if (hasIntersecting || (hasInside && hasOutside)) return Intersection::Intersecting;
    if (hasInside) return Intersection::Inside;

    return Intersection::Outside;
}



// OPERAÇÕES BOOLEANAS (DE VERDADE AGORA)
Octree Octree::ApplyUnion(const Octree& A, const Octree& B, int level, bool useDynamicLevel) {
    Cell boundingBox = GenerateOctreeBoundingBox(A, B);

    if (level <= 0) level = DefaultMaxLevel;

    int maxLevel = useDynamicLevel ? CalculateMaxLevel(boundingBox, A, B) : level;

    Octree treeA = GenerateBoundedOctree(boundingBox, A, maxLevel);
    Octree treeB = GenerateBoundedOctree(boundingBox, B, maxLevel);

    Octree result;

    result.root->cell = boundingBox;
    result.maxLevel = maxLevel;

    UnionNode(treeA.root.get(), treeB.root.get(), result.root.get());

    return result;
}

Octree Octree::ApplyIntersection(const Octree& A, const Octree& B, int level, bool useDynamicLevel) {
    Cell boundingBox = GenerateOctreeBoundingBox(A, B);

    if (level <= 0) level = DefaultMaxLevel;

    int maxLevel = useDynamicLevel ? CalculateMaxLevel(boundingBox, A, B) : level;

    Octree treeA = GenerateBoundedOctree(boundingBox, A, maxLevel);
    Octree treeB = GenerateBoundedOctree(boundingBox, B, maxLevel);

    Octree result;

    result.root->cell = boundingBox;
    result.maxLevel = maxLevel;

    IntersectionNode(treeA.root.get(), treeB.root.get(), result.root.get());

    return result;
}

void Octree::UnionNode(const Node* nodeA, const Node* nodeB, Node* result) {
    if (nodeA->state == Intersection::Inside || nodeB->state == Intersection::Inside) {
        result->state = Intersection::Inside;
        return;
    }

    if (nodeA->state == Intersection::Outside && nodeB->state == Intersection::Outside) {
        result->state = Intersection::Outside;
        return;
    }

    bool leafA = nodeA->children[0] == nullptr;
    bool leafB = nodeB->children[0] == nullptr;

    if ((nodeA->state == Intersection::Intersecting && leafA) ||
        (nodeB->state == Intersection::Intersecting && leafB)) {
        result->state = Intersection::Intersecting;
        return;
    }

    result->state = Intersection::Intersecting;

    bool hasChildrenA = !leafA;
    bool hasChildrenB = !leafB;

    for (int i = 0; i < 8; i++) {
        result->children[i] = std::make_unique<Node>();

        result->children[i]->parent = result;

        float size = result->cell.size / 2.0f;

        result->children[i]->cell.size = size;

        Offset offset = generateOffset(i, size);

        result->children[i]->cell.x = result->cell.x + offset.x;
        result->children[i]->cell.y = result->cell.y + offset.y;
        result->children[i]->cell.z = result->cell.z + offset.z;

        const Node* childA = hasChildrenA ? nodeA->children[i].get() : nodeA;
        const Node* childB = hasChildrenB ? nodeB->children[i].get() : nodeB;

        UnionNode(childA, childB, result->children[i].get());
    }

    bool allInside = true;
    bool allOutside = true;

    for (int i = 0; i < 8; i++) {
        if (result->children[i]->state != Intersection::Inside) allInside = false;
        if (result->children[i]->state != Intersection::Outside) allOutside = false;
    }

    if (allInside) {
        result->state = Intersection::Inside;

        for (int i = 0; i < 8; i++) result->children[i].reset();
    } else if (allOutside) {
        result->state = Intersection::Outside;

        for (int i = 0; i < 8; i++) result->children[i].reset();
    }
}

void Octree::IntersectionNode(const Node* nodeA, const Node* nodeB, Node* result) {
    if (nodeA->state == Intersection::Outside || nodeB->state == Intersection::Outside) {
        result->state = Intersection::Outside;
        return;
    }

    if (nodeA->state == Intersection::Inside && nodeB->state == Intersection::Inside) {
        result->state = Intersection::Inside;
        return;
    }

    bool leafA = nodeA->children[0] == nullptr;
    bool leafB = nodeB->children[0] == nullptr;

    if ((nodeA->state == Intersection::Intersecting && leafA) ||
        (nodeB->state == Intersection::Intersecting && leafB)) {
        result->state = Intersection::Intersecting;
        return;
    }

    result->state = Intersection::Intersecting;

    bool hasChildrenA = !leafA;
    bool hasChildrenB = !leafB;

    for (int i = 0; i < 8; i++) {
        result->children[i] = std::make_unique<Node>();

        result->children[i]->parent = result;

        float size = result->cell.size / 2.0f;

        result->children[i]->cell.size = size;

        Offset offset = generateOffset(i, size);

        result->children[i]->cell.x = result->cell.x + offset.x;
        result->children[i]->cell.y = result->cell.y + offset.y;
        result->children[i]->cell.z = result->cell.z + offset.z;

        const Node* childA = hasChildrenA ? nodeA->children[i].get() : nodeA;
        const Node* childB = hasChildrenB ? nodeB->children[i].get() : nodeB;

        IntersectionNode(childA, childB, result->children[i].get());
    }

    bool allInside = true;
    bool allOutside = true;

    for (int i = 0; i < 8; i++) {
        if (result->children[i]->state != Intersection::Inside) {
            allInside = false;
        }

        if (result->children[i]->state != Intersection::Outside) {
            allOutside = false;
        }
    }

    if (allInside) {
        result->state = Intersection::Inside;

        for (int i = 0; i < 8; i++) {
            result->children[i].reset();
        }
    } else if (allOutside) {
        result->state = Intersection::Outside;

        for (int i = 0; i < 8; i++) {
            result->children[i].reset();
        }
    }
}



// Procedimentos de análise
float Octree::getVolume() const {
    float volume = auxVolume(root.get());

    return volume;
}

float Octree::auxVolume(Node* node) const {
    if (node == nullptr) {
        return 0.0f;
    }

    float volume = node->cell.size * node->cell.size * node->cell.size;

    if (node->state == Intersection::Outside) {
        return 0.0f;
    }

    if (node->state == Intersection::Inside) {
        return volume;
    }

    float totalVolume = 0.0f;

    if (node->state == Intersection::Intersecting) {
        bool hasChildren = false;
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr) {
                hasChildren = true;
                break;
            }
        }

        if (hasChildren) {
            for (int i = 0; i < 8; i++) {
                totalVolume += auxVolume(node->children[i].get());
            }
        } else {
            return volume;
        }
    }

    return totalVolume;
}




// area
int Octree::GetChildIndex(const Node* node) const {
    if (node == nullptr || node->parent == nullptr) {
        return -1;
    }

    for (int i = 0; i < 8; i++) {
        if (node->parent->children[i].get() == node) {
            return i;
        }
    }

    return -1;
}

std::vector<const Node*> Octree::GetAdjacentNodes(const Node* node) const {
    std::vector<const Node*> neighbors;

    if (node == nullptr) return neighbors;

    FindAdjacentNodes(node, Direction::NegativeX, neighbors);
    FindAdjacentNodes(node, Direction::PositiveX, neighbors);
    FindAdjacentNodes(node, Direction::NegativeY, neighbors);
    FindAdjacentNodes(node, Direction::PositiveY, neighbors);
    FindAdjacentNodes(node, Direction::NegativeZ, neighbors);
    FindAdjacentNodes(node, Direction::PositiveZ, neighbors);

    return neighbors;
}

void Octree::FindAdjacentNodes(const Node* node, Direction direction, std::vector<const Node*>& neighbors) const {
    const Node* current = node;

    while (current->parent != nullptr) {
        int childIndex = GetChildIndex(current);

        bool onBoundary;
        int neighborIndex;

        switch (direction) {
            case Direction::PositiveX:
                onBoundary = (childIndex == 1 || childIndex == 3 || childIndex == 5 || childIndex == 7);
                neighborIndex = childIndex + 1;
                break;

            case Direction::NegativeX:
                onBoundary = (childIndex == 0 || childIndex == 2 || childIndex == 4 || childIndex == 6);
                neighborIndex = childIndex - 1;
                break;

            case Direction::PositiveY:
                onBoundary = (childIndex == 2 || childIndex == 3 || childIndex == 6 || childIndex == 7);
                neighborIndex = childIndex + 2;
                break;

            case Direction::NegativeY:
                onBoundary = (childIndex == 0 || childIndex == 1 || childIndex == 4 || childIndex == 5);
                neighborIndex = childIndex - 2;
                break;

            case Direction::PositiveZ:
                onBoundary = (childIndex == 4 || childIndex == 5 || childIndex == 6 || childIndex == 7);
                neighborIndex = childIndex + 4;
                break;

            case Direction::NegativeZ:
                onBoundary = (childIndex == 0 || childIndex == 1 || childIndex == 2 || childIndex == 3);
                neighborIndex = childIndex - 4;
                break;
        }

        if (!onBoundary) {
            const Node* neighbor = current->parent->children[neighborIndex].get();
            CollectFaceNodes(neighbor, direction, neighbors);
            return;
        }

        current = current->parent;
    }
}

void Octree::CollectFaceNodes(const Node* node, Direction direction, std::vector<const Node*>& neighbors) const {
    if (node == nullptr) return;

    if (node->children[0] == nullptr) {
        neighbors.push_back(node);
        return;
    }

    for (int i = 0; i < 8; i++) {
        bool onFace;

        switch (direction) {
            case Direction::PositiveX:
                onFace = (i == 0 || i == 2 || i == 4 || i == 6);
                break;

            case Direction::NegativeX:
                onFace = (i == 1 || i == 3 || i == 5 || i == 7);
                break;

            case Direction::PositiveY:
                onFace = (i == 0 || i == 1 || i == 4 || i == 5);
                break;

            case Direction::NegativeY:
                onFace = (i == 2 || i == 3 || i == 6 || i == 7);
                break;

            case Direction::PositiveZ:
                onFace = (i == 0 || i == 1 || i == 2 || i == 3);
                break;

            case Direction::NegativeZ:
                onFace = (i == 4 || i == 5 || i == 6 || i == 7);
                break;
        }

        if (onFace) CollectFaceNodes(node->children[i].get(), direction, neighbors);
    }
}

void Octree::GetLeaves(const Node* node, std::vector<const Node*>& cells) const {
    if (node == nullptr || node->state == Intersection::Outside) {
        return;
    }

    if (node->children[0] == nullptr) {
        cells.push_back(node);
        return;
    }

    for (int i = 0; i < 8; i++) {
        GetLeaves(node->children[i].get(), cells);
    }
}

float Octree::getSurfaceArea() {
    std::vector<const Node*> cells;

    GetLeaves(root.get(), cells);

    float surfaceArea = 0.0f;

    for (const Node* node : cells) {
        const Cell& cell = node->cell;

        float cellMin[3] = {cell.x, cell.y, cell.z};
        float cellMax[3] = {cell.x + cell.size, cell.y + cell.size, cell.z + cell.size};

        float cellFaceArea = cell.size * cell.size;

        std::vector<const Node*> neighbors = GetAdjacentNodes(node);

        float coveredArea[6] = {
            0.0f, // -X
            0.0f, // +X
            0.0f, // -Y
            0.0f, // +Y
            0.0f, // -Z
            0.0f  // +Z
        };

        for (const Node* neighbor : neighbors) {
            if (neighbor->state == Intersection::Outside) {
                continue;
            }

            const Cell& other = neighbor->cell;

            float otherMin[3] = {other.x, other.y, other.z};
            float otherMax[3] = {other.x + other.size, other.y + other.size, other.z + other.size};

            // +X
            if (otherMin[0] == cellMax[0]) {
                float overlapY = std::min(cellMax[1], otherMax[1]) - std::max(cellMin[1], otherMin[1]);

                float overlapZ = std::min(cellMax[2], otherMax[2]) - std::max(cellMin[2], otherMin[2]);

                if (overlapY > 0.0f && overlapZ > 0.0f) {
                    coveredArea[1] += overlapY * overlapZ;
                }
            }

            // -X
            if (otherMax[0] == cellMin[0]) {
                float overlapY = std::min(cellMax[1], otherMax[1]) - std::max(cellMin[1], otherMin[1]);

                float overlapZ = std::min(cellMax[2], otherMax[2]) - std::max(cellMin[2], otherMin[2]);

                if (overlapY > 0.0f && overlapZ > 0.0f) {
                    coveredArea[0] += overlapY * overlapZ;
                }
            }

            // +Y
            if (otherMin[1] == cellMax[1]) {
                float overlapX = std::min(cellMax[0], otherMax[0]) - std::max(cellMin[0], otherMin[0]);

                float overlapZ = std::min(cellMax[2], otherMax[2]) - std::max(cellMin[2], otherMin[2]);

                if (overlapX > 0.0f && overlapZ > 0.0f) {
                    coveredArea[3] += overlapX * overlapZ;
                }
            }

            // -Y
            if (otherMax[1] == cellMin[1]) {
                float overlapX = std::min(cellMax[0], otherMax[0]) - std::max(cellMin[0], otherMin[0]);

                float overlapZ = std::min(cellMax[2], otherMax[2]) - std::max(cellMin[2], otherMin[2]);

                if (overlapX > 0.0f && overlapZ > 0.0f) {
                    coveredArea[2] += overlapX * overlapZ;
                }
            }

            // +Z
            if (otherMin[2] == cellMax[2]) {
                float overlapX = std::min(cellMax[0], otherMax[0]) - std::max(cellMin[0], otherMin[0]);

                float overlapY = std::min(cellMax[1], otherMax[1]) - std::max(cellMin[1], otherMin[1]);

                if (overlapX > 0.0f && overlapY > 0.0f) {
                    coveredArea[5] += overlapX * overlapY;
                }
            }

            // -Z
            if (otherMax[2] == cellMin[2]) {
                float overlapX = std::min(cellMax[0], otherMax[0]) - std::max(cellMin[0], otherMin[0]);

                float overlapY = std::min(cellMax[1], otherMax[1]) - std::max(cellMin[1], otherMin[1]);

                if (overlapX > 0.0f && overlapY > 0.0f) {
                    coveredArea[4] += overlapX * overlapY;
                }
            }
        }

        for (int i = 0; i < 6; i++) {
            surfaceArea += cellFaceArea - coveredArea[i];
        }
    }

    return surfaceArea;
}



// Mesh
std::vector<Triangle> Octree::GenerateMesh() const {
    std::vector<Triangle> mesh;
    std::vector<const Node*> cells;

    GetLeaves(root.get(), cells);

    auto addFace = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
        mesh.emplace_back(a, b, c);
        mesh.emplace_back(a, c, d);
    };

    for (const Node* node : cells) {
        const Cell& cell = node->cell;

        float cellMin[3] = {cell.x, cell.y, cell.z};
        float cellMax[3] = {cell.x + cell.size, cell.y + cell.size, cell.z + cell.size};
        float cellFaceArea = cell.size * cell.size;

        std::vector<const Node*> neighbors = GetAdjacentNodes(node);
        float coveredArea[6] = {};

        for (const Node* neighbor : neighbors) {
            if (neighbor->state == Intersection::Outside) continue;

            const Cell& other = neighbor->cell;

            float otherMin[3] = {other.x, other.y, other.z};
            float otherMax[3] = {
                other.x + other.size,
                other.y + other.size,
                other.z + other.size
            };

            for (int axis = 0; axis < 3; axis++) {
                int positiveFace = axis * 2 + 1;
                int negativeFace = axis * 2;

                int axisA = (axis + 1) % 3;
                int axisB = (axis + 2) % 3;

                float overlapA = std::min(cellMax[axisA], otherMax[axisA])
                               - std::max(cellMin[axisA], otherMin[axisA]);

                float overlapB = std::min(cellMax[axisB], otherMax[axisB])
                               - std::max(cellMin[axisB], otherMin[axisB]);

                if (overlapA <= 0.0f || overlapB <= 0.0f) continue;

                if (otherMin[axis] == cellMax[axis]) {
                    coveredArea[positiveFace] += overlapA * overlapB;
                }

                if (otherMax[axis] == cellMin[axis]) {
                    coveredArea[negativeFace] += overlapA * overlapB;
                }
            }
        }

        float x0 = cellMin[0], y0 = cellMin[1], z0 = cellMin[2];
        float x1 = cellMax[0], y1 = cellMax[1], z1 = cellMax[2];

        if (coveredArea[0] < cellFaceArea)
            addFace({x0,y0,z0}, {x0,y0,z1}, {x0,y1,z1}, {x0,y1,z0});

        if (coveredArea[1] < cellFaceArea)
            addFace({x1,y0,z0}, {x1,y1,z0}, {x1,y1,z1}, {x1,y0,z1});

        if (coveredArea[2] < cellFaceArea)
            addFace({x0,y0,z0}, {x1,y0,z0}, {x1,y0,z1}, {x0,y0,z1});

        if (coveredArea[3] < cellFaceArea)
            addFace({x0,y1,z0}, {x0,y1,z1}, {x1,y1,z1}, {x1,y1,z0});

        if (coveredArea[4] < cellFaceArea)
            addFace({x0,y0,z0}, {x0,y1,z0}, {x1,y1,z0}, {x1,y0,z0});

        if (coveredArea[5] < cellFaceArea)
            addFace({x0,y0,z1}, {x1,y0,z1}, {x1,y1,z1}, {x0,y1,z1});
    }

    return mesh;
}
