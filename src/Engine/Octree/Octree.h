#pragma once

#include "../Primitives/Primitive.h"
#include "../Primitives/Triangle.h"
#include <memory>

extern int DefaultMaxLevel;

struct Node {
    Cell cell;
    Intersection state;

    Node* parent = nullptr;
    std::unique_ptr<Node> children[8] = {};
};

struct Offset {
    float x, y, z;
};

enum class Direction {
    NegativeX,
    PositiveX,
    NegativeY,
    PositiveY,
    NegativeZ,
    PositiveZ
};

Offset generateOffset(int, float);

class Octree {
public:
    Octree();
    void Generate(Primitive&);
    const Node* GetRoot() const;
    void Print();
    std::string ToString() const;
    static Octree ImportFromString(const std::string& data);

    void setMaxLevel(int);
    int GetMaxLevel() const;
    void setLabel(std::string);
    std::string getLabel() const;

    // procedimentos de analise
    float getVolume() const;
    float getSurfaceArea();

    // operadores geometricos
    void SetScale(float);
    float GetScale() const;
    void SetTranslation(float, float, float);
    glm::vec3 GetTranslation() const;

    // operadores booleanos
    static Octree ApplyUnion(const Octree&, const Octree&, int, bool);
    static Octree ApplyIntersection(const Octree&, const Octree&, int, bool);

    // otimização
    std::vector<Triangle> GenerateMesh() const;

private:
    std::string label = "";

    std::unique_ptr<Node> root;
    int maxLevel = DefaultMaxLevel;
    float scale = 1.0f;
    glm::vec3 translation = glm::vec3(0.0f);

    void Aux(Primitive&, Node*, int);
    void PrintNode(const Node*) const;
    void ToStringNode(const Node*, std::string&) const;
    static std::unique_ptr<Node> ImportNode(const std::string& data, int& pos, const Cell& cell, Node* parent, int depth, int& maxDepthOut);

    // procedimentos de analise
    float auxVolume(Node*) const;

    // operadores geometricos
    void ScaleNode(Node*, float, const glm::vec3&);
    void TranslateNode(Node*, const glm::vec3&);

    // operadores booleanos
    void ClassifyNode(const Octree&, Node*, int);
    static Intersection GetNodeIntersection(const Node*, const Cell&);

    static Octree GenerateBoundedOctree(const Cell&, const Octree&, int);
    static void AuxBoundedOctree(Node*, int, int);
    static Cell GenerateOctreeBoundingBox(const Octree&, const Octree&);
    static int CalculateMaxLevel(const Cell&, const Octree&, const Octree&);

    static void UnionNode(const Node*, const Node*, Node*);
    static void IntersectionNode(const Node*, const Node*, Node*);

    std::vector<const Node*> GetAdjacentNodes(const Node*) const;
    void FindAdjacentNodes(const Node*, Direction, std::vector<const Node*>&) const;
    void CollectFaceNodes(const Node*, Direction, std::vector<const Node*>&) const;
    int GetChildIndex(const Node*) const;

    void GetLeaves(const Node*, std::vector<const Node*>&) const;
};