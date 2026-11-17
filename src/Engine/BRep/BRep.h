#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <iostream>

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

        std::vector<Edge*> GetEdgesOfFace(Face* face);
        std::vector<Vertex*> GetVerticesOfFace(Face* face);
        std::vector<Face*> GetFacesOfVertex(Vertex* vertex);
        std::vector<Edge*> GetEdgesOfVertex(Vertex* vertex);
        std::vector<Face*> GetAdjacentFaces(Face* face);
    private:
        std::vector<std::unique_ptr<Vertex>> vertices;
        std::vector<std::unique_ptr<Edge>> edges;
        std::vector<std::unique_ptr<Face>> faces;

        Edge* FindEdge(Vertex* vs, Vertex* ve);
        Edge* FindEdgeLinear(Vertex* vs, Vertex* ve);
        void AddFace(std::vector<int>& vertexIndices);
};