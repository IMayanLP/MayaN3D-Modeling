#include "CSG.h"
#include "../Ray/Ray.h"
#include "../Primitives/Primitive.h"

struct SMCIntersection {
    RayIntersection intersection;
    SMCClassification classification;
};

struct SMCInterval {
    SMCIntersection start;
    SMCIntersection end;
    SMCClassification classification;
};

class SMC {
    public:
        static std::vector<SMCInterval> Classify(const CSG& csg, const Ray& ray);

    private:
        static std::vector<SMCInterval> ClassifyNode(const CSGNode* node, const Ray& ray);
        static std::vector<SMCInterval> ClassifyPrimitive(const CSGNode* node, const Ray& ray);
        static std::vector<SMCInterval> ClassifyTransform(const CSGNode* node, const Ray& ray);
};