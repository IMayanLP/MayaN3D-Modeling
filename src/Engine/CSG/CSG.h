#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "../Primitives/Primitive.h"
#include "../Primitives/Sphere.h"
#include "../Primitives/Cube.h"
#include "../Primitives/Cone.h"
#include "../Primitives/Cylinder.h"
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

        SMCClassification CheckIsInside(glm::vec3&);
        SMCClassification CheckIsCellInside(Cell&);
        Cell GetBoundingBox();
};

struct ImportedNode {
    std::string label;
    std::unique_ptr<CSGNode> node;
};

class CSG {
    public:
        CSG() = default;
        void SetRoot(std::unique_ptr<CSGNode>);
        std::unique_ptr<CSGNode> GetAndFreeRoot();
        const CSGNode* GetRoot() const;

        // Conversao Octree
        SMCClassification CheckIsInside(glm::vec3&);
        SMCClassification CheckIsCellInside(Cell&);
        Cell GetBoundingBox();
        Octree GenerateOctree();

        // Operadores de nós
        static std::unique_ptr<CSGNode> CreatePrimitiveNode(Primitive&);
        static std::unique_ptr<CSGNode> CreateBooleanNode(BooleanOperation, std::unique_ptr<CSGNode>, std::unique_ptr<CSGNode>);
        static std::unique_ptr<CSGNode> CreateTransformNode(TransformType, glm::vec3, std::unique_ptr<CSGNode>);

        // I/O
        bool Export(std::string&);
        bool ExportLegacy(std::string&);
        static CSG Import(const std::string&);
        static CSG ImportLegacy(const std::string&);

        void setLabel(std::string);
        std::string getLabel() const;

    private:
        void Aux(Node*, int);

        static std::string ExportNode(CSGNode*, std::vector<CSGNode*>, std::ofstream&, int&);
        std::string ExportLegacyNode(CSGNode*,std::ofstream&, int&);

        std::unique_ptr<CSGNode> root;
        std::string label;
};
