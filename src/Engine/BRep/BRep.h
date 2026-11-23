#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <iostream>

#include "../Primitives/Triangle.h"

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

        // Adjacencias
        std::vector<Edge*> GetEdgesOfFace(Face*);
        std::vector<Vertex*> GetVerticesOfFace(Face*);
        std::vector<Face*> GetFacesOfVertex(Vertex*);
        std::vector<Edge*> GetEdgesOfVertex(Vertex*);
        std::vector<Face*> GetAdjacentFaces(Face*);

        // EULER
        std::pair<Vertex*, Face*> MVFS(glm::vec3);
        Vertex* MEV(Face*, Vertex*, glm::vec3);
        Face* MEF(Face*, Vertex*, Vertex*);
        void KVFS(Vertex*, Face*);
        void KEV(Edge*, Vertex*);
        void KEF(Edge*);

    private:
        std::vector<std::unique_ptr<Vertex>> vertices;
        std::vector<std::unique_ptr<Edge>> edges;
        std::vector<std::unique_ptr<Face>> faces;

        Edge* FindEdge(Vertex*, Vertex*);
        Edge* FindEdgeLinear(Vertex*, Vertex*);

        Vertex* NewVertex(glm::vec3);
        Edge* NewEdge(Vertex*, Vertex*);
        Face* NewFace();

        void RemoveVertex(Vertex*);
        void RemoveEdge(Edge*);
        void RemoveFace(Face*);

        int nextVertexId = 0;
        int nextEdgeId = 0;
        int nextFaceId = 0;
};