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
        static std::vector<SMCInterval> Classify(const CSG&, const Ray&);

    private:
        static std::vector<SMCInterval> ClassifyNode(const CSGNode*, const Ray&);
        static std::vector<SMCInterval> ClassifyPrimitive(const CSGNode*, const Ray&);
        static std::vector<SMCInterval> ClassifyTransform(const CSGNode*, const Ray&);
        static std::vector<SMCInterval> ClassifyBoolean(const CSGNode*, const Ray&);

        static std::vector<SMCInterval> NormalizeIntervals(const std::vector<SMCInterval>&, const std::vector<SMCInterval>&);
        static std::vector<SMCInterval> MergeIntervals(const std::vector<SMCInterval>&);
};