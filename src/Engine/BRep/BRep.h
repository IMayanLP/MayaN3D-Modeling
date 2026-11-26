#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <iostream>

#include "../Primitives/Triangle.h"
#include "../Ray/Ray.h"

struct Vertex;
struct Edge;
struct Face;

struct Vertex {
    int id = -1;
    glm::vec3 coords;
    Edge* e = nullptr;
};

struct Edge {
    int id = -1;

    Vertex* vs = nullptr;
    Vertex* ve = nullptr;

    Face* fcw = nullptr;
    Face* fccw = nullptr;

    Edge* ncw = nullptr;
    Edge* pcw = nullptr;

    Edge* nccw = nullptr;
    Edge* pccw = nullptr;
};

struct Face {
    int id = -1;
    Edge* e = nullptr;
};

class BRep {
    public:
        void CreateDebugCube();
        void Print() const;
        void DebugWingedEdge();
        void TestEuler();
        std::vector<Triangle> TriangulateMesh();

        // Primitivas
        void CreateEulerCube();
        void CreateEulerSphere(int);
        void CreateEulerCylinder(int);
        void CreateEulerCone(int);

        // Adjacencias
        Edge* FindEdge(Vertex*, Vertex*);
        Edge* FindEdgeLinear(Vertex*, Vertex*);

        std::vector<Edge*> GetEdgesOfFace(Face*);
        std::vector<Vertex*> GetVerticesOfFace(Face*);
        std::vector<Face*> GetFacesOfVertex(Vertex*);
        std::vector<Edge*> GetEdgesOfVertex(Vertex*);
        std::vector<Face*> GetAdjacentFaces(Face*);
        std::vector<Edge*> GetAdjacentEdges(Edge*);
        std::vector<Vertex*> GetAdjacentVertices(Vertex*);

        // EULER
        std::pair<Vertex*, Face*> MVFS(glm::vec3);
        Vertex* MEV(Face*, Vertex*, glm::vec3);
        Face* MEF(Face*, Vertex*, Vertex*);
        void KVFS(Vertex*, Face*);
        void KEV(Edge*, Vertex*);
        void KEF(Edge*);

        // Transformações
        void Translate(glm::vec3);
        void Scale(glm::vec3);
        void Rotate(glm::vec3);

        // Análises
        static float CalculateSurfaceArea(std::vector<Triangle>&);
        static float CalculateVolume(std::vector<Triangle>&);

        // I/O
        bool Import(std::vector<glm::vec3>&, std::vector<std::vector<int>>&);

        // Demais
        const std::vector<std::unique_ptr<Edge>>& GetEdges() const;
        const std::vector<std::unique_ptr<Vertex>>& GetVertices() const;
        const std::vector<std::unique_ptr<Face>>& GetFaces() const;

        const size_t GetEdgesSize() const;
        const size_t GetVerticesSize() const;
        const size_t GetFacesSize() const;

        Vertex* SelectVertex(Ray);
        Face* AutoFaceMEV(Vertex*, glm::vec3);
        Face* AutoFaceMEF(Vertex*, Vertex*);

        void setLabel(std::string);
        std::string getLabel() const;

        float PI = 3.14159265f;

    private:
        std::string label = "";

        std::vector<std::unique_ptr<Vertex>> vertices;
        std::vector<std::unique_ptr<Edge>> edges;
        std::vector<std::unique_ptr<Face>> faces;

        glm::vec3 getSpherePoint(int, int, int, int, float);
        glm::vec3 getCylinderPoint(int, int, float, float);

        Vertex* NewVertex(glm::vec3);
        Edge* NewEdge(Vertex*, Vertex*);
        Face* NewFace();

        void RemoveVertex(Vertex*);
        void RemoveEdge(Edge*);
        void RemoveFace(Face*);

        int nextVertexId = 0;
        int nextEdgeId = 0;
        int nextFaceId = 0;

        glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
};