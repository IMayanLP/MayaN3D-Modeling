#pragma once

struct Vertex;
struct Edge;
struct Face;

struct Vertex {
    int id;
    glm::vec3 coords;
    Edge* e;
};

struct Edge {
    int id;

    Vertex* vs;
    Vertex* ve;

    Face* fcw;
    Face* fccw;

    Edge* ncw;
    Edge* pcw;

    Edge* nccw;
    Edge* pccw;
};

struct Face {
    int id;
    Edge* e;
};

class BRep {
    public:
    private:
        std::vector<std::unique_ptr<Vertex>> vertices;
        std::vector<std::unique_ptr<Edge>> edges;
        std::vector<std::unique_ptr<Face>> faces;
};