#include <iostream>
#include <filesystem>
#include <fstream>

#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Interface/Interface.h"

#include "Engine/Primitives/Sphere.h"
#include "Engine/Primitives/Cylinder.h"
#include "Engine/Primitives/Cone.h"
#include "Engine/Primitives/Cube.h"
#include "Engine/CSG/CSG.h"
#include "Engine/Ray/Ray.h"
#include "Engine/CSG/SMC.h"

namespace fs = std::filesystem;

void TestSMC(const std::string& description, const Primitive& primitive,
             const glm::vec3& origin, const glm::vec3& direction) {
    std::cout << "\n========================================\n";
    std::cout << "Teste: " << description << '\n';
    std::cout << "Origem: (" << origin.x << ", " << origin.y << ", " << origin.z << ")\n";
    std::cout << "Direcao: (" << direction.x << ", " << direction.y << ", " << direction.z << ")\n";

    CSG csg;
    csg.SetRoot(CSG::CreatePrimitiveNode(primitive));

    Ray ray(origin, direction);
    std::vector<RayIntersection> intersections = primitive.IntersectRay(ray);

    std::cout << "\nIntersecoes encontradas: " << intersections.size() << '\n';

    for (size_t i = 0; i < intersections.size(); i++) {
        const RayIntersection& intersection = intersections[i];

        std::cout << "  [" << i << "] t = " << intersection.t
                  << " | P = (" << intersection.point.x << ", "
                  << intersection.point.y << ", "
                  << intersection.point.z << ")\n";
    }

    SMC smc;
    std::vector<SMCInterval> intervals = smc.Classify(csg, ray);

    std::cout << "\nIntervalos SMC: " << intervals.size() << '\n';

    for (const SMCInterval& interval : intervals) {
        std::cout << "  [" << interval.start.intersection.t << ", ";

        if (std::isinf(interval.end.intersection.t)) {
            std::cout << "infinito";
        } else {
            std::cout << interval.end.intersection.t;
        }

        std::cout << "] ";

        switch (interval.classification) {
            case SMCClassification::IN:
                std::cout << "IN";
                break;
            case SMCClassification::ON:
                std::cout << "ON";
                break;
            case SMCClassification::OUT:
                std::cout << "OUT";
                break;
        }

        std::cout << '\n';
    }
}

void TestSMCTransform(const std::string& description, const CSG& csg, const glm::vec3& origin, const glm::vec3& direction) {
    Ray ray(origin, direction);
    std::vector<SMCInterval> intervals = SMC::Classify(csg, ray);

    std::cout << "\n=== " << description << " ===\n";
    std::cout << "Origin: (" << origin.x << ", " << origin.y << ", " << origin.z << ")\n";

    for (const SMCInterval& interval : intervals) {
        std::cout << "t: " << interval.start.intersection.t << " -> "
                  << interval.end.intersection.t << " | Classification: "
                  << (interval.classification == SMCClassification::IN ? "IN" : "OUT") << '\n';
    }
}

int main() {
    Sphere sphere(1.0f);
    Cube cube(2.0f, 2.0f, 2.0f);
    Cylinder cylinder(1.0f, 2.0f);
    Cone cone(1.0f, 2.0f);

    // ==================== SPHERE ====================

    TestSMC("Sphere - dentro apontando para fora", sphere, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    TestSMC("Sphere - fora apontando para dentro", sphere, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMC("Sphere - borda apontando para fora", sphere, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    TestSMC("Sphere - borda apontando para dentro", sphere, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f));

    // ==================== CUBE ====================

    TestSMC("Cube - dentro apontando para fora", cube, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    TestSMC("Cube - fora apontando para dentro", cube, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMC("Cube - borda apontando para fora", cube, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    TestSMC("Cube - borda apontando para dentro", cube, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMC("Cube - atravessando face plana", cube, glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

    // ==================== CYLINDER ====================

    TestSMC("Cylinder - dentro apontando para fora", cylinder, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    TestSMC("Cylinder - fora apontando para dentro", cylinder, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMC("Cylinder - borda apontando para fora", cylinder, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    TestSMC("Cylinder - borda apontando para dentro", cylinder, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMC("Cylinder - atravessando tampa plana", cylinder, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));

    // ==================== CONE ====================

    TestSMC("Cone - dentro apontando para fora", cone, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    TestSMC("Cone - fora apontando para dentro", cone, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMC("Cone - borda apontando para fora", cone, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    TestSMC("Cone - borda apontando para dentro", cone, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    TestSMC("Cone - atravessando base plana", cone, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));



    // ================= TRANSFORM ======================
    std::unique_ptr<CSGNode> sphereNode = CSG::CreateTransformNode(TransformType::Translation, glm::vec3(2.0f, 0.0f, 0.0f), CSG::CreatePrimitiveNode(sphere));
    std::unique_ptr<CSGNode> cubeNode = CSG::CreateTransformNode(TransformType::Translation, glm::vec3(2.0f, 0.0f, 0.0f), CSG::CreatePrimitiveNode(cube));
    std::unique_ptr<CSGNode> cylinderNode = CSG::CreateTransformNode(TransformType::Translation, glm::vec3(2.0f, 0.0f, 0.0f), CSG::CreatePrimitiveNode(cylinder));
    std::unique_ptr<CSGNode> coneNode = CSG::CreateTransformNode(TransformType::Translation, glm::vec3(2.0f, 0.0f, 0.0f), CSG::CreatePrimitiveNode(cone));

    CSG sphereCSG;
    sphereCSG.SetRoot(std::move(sphereNode));

    CSG cubeCSG;
    cubeCSG.SetRoot(std::move(cubeNode));

    CSG cylinderCSG;
    cylinderCSG.SetRoot(std::move(cylinderNode));

    CSG coneCSG;
    coneCSG.SetRoot(std::move(coneNode));

    Ray ray(glm::vec3(2.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));

    TestSMCTransform("Sphere Translation", sphereCSG, glm::vec3(2.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMCTransform("Cube Translation", cubeCSG, glm::vec3(2.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMCTransform("Cylinder Translation", cylinderCSG, glm::vec3(2.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    TestSMCTransform("Cone Translation", coneCSG, glm::vec3(2.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));

    return 0;
}
