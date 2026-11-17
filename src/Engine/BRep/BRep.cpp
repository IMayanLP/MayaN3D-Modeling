#include "BRep.h"

void BRep::Print() const {
    std::cout << "\n========== VERTICES ==========\n";

    for (const std::unique_ptr<Vertex>& vertex : vertices) {
        std::cout << "Vertex " << vertex->id
                  << " | Position: (" << vertex->coords.x << ", "
                  << vertex->coords.y << ", " << vertex->coords.z << ")"
                  << " | Edge: " << (vertex->e ? vertex->e->id : -1)
                  << '\n';
    }

    std::cout << "\n========== EDGES ==========\n";

    for (const std::unique_ptr<Edge>& edge : edges) {
        std::cout << "Edge " << edge->id
                  << " | Vertices: " << (edge->vs ? edge->vs->id : -1)
                  << " -> " << (edge->ve ? edge->ve->id : -1)
                  << " | FCW: " << (edge->fcw ? edge->fcw->id : -1)
                  << " | FCCW: " << (edge->fccw ? edge->fccw->id : -1)
                  << " | NCW: " << (edge->ncw ? edge->ncw->id : -1)
                  << " | PCW: " << (edge->pcw ? edge->pcw->id : -1)
                  << " | NCCW: " << (edge->nccw ? edge->nccw->id : -1)
                  << " | PCCW: " << (edge->pccw ? edge->pccw->id : -1)
                  << '\n';
    }

    std::cout << "\n========== FACES ==========\n";

    for (const std::unique_ptr<Face>& face : faces) {
        std::cout << "Face " << face->id
                  << " | Edge: " << (face->e ? face->e->id : -1)
                  << '\n';
    }

    std::cout << "============================\n";
}

void BRep::CreateDebugCube() {
    vertices.clear();
    edges.clear();
    faces.clear();

    const glm::vec3 positions[8] = {
        {-1.0f, -1.0f, -1.0f},
        { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f}
    };

    for (int i = 0; i < 8; i++) {
        std::unique_ptr<Vertex> vertex = std::make_unique<Vertex>();
        vertex->id = i;
        vertex->coords = positions[i];

        vertices.push_back(std::move(vertex));
    }

    const int faceVertices[6][4] = {
        {0, 3, 2, 1}, // Inferior
        {4, 5, 6, 7}, // Superior
        {0, 1, 5, 4}, // Frontal
        {3, 7, 6, 2}, // Traseira
        {0, 4, 7, 3}, // Esquerda
        {1, 2, 6, 5}  // Direita
    };

    for (int i = 0; i < 6; i++) {
        std::unique_ptr<Face> face = std::make_unique<Face>();
        face->id = i;
        faces.push_back(std::move(face));
    }

    for (int i = 0; i < 6; i++) {
        Face* face = faces[i].get();
        Edge* faceFirstEdge = nullptr;
        Edge* previousEdge = nullptr;

        for (int j = 0; j < 4; j++) {
            Vertex* vs = vertices[faceVertices[i][j]].get();
            Vertex* ve = vertices[faceVertices[i][(j + 1) % 4]].get();

            Edge* edge = FindEdgeLinear(vs, ve);

            if (edge == nullptr) {
                std::unique_ptr<Edge> newEdge = std::make_unique<Edge>();
                newEdge->id = static_cast<int>(edges.size());
                newEdge->vs = vs;
                newEdge->ve = ve;

                edge = newEdge.get();
                edges.push_back(std::move(newEdge));

                edge->fccw = face;
            }
            else if (edge->vs == vs && edge->ve == ve) {
                edge->fccw = face;
            }
            else {
                edge->fcw = face;
            }

            if (vs->e == nullptr) vs->e = edge;

            if (faceFirstEdge == nullptr) faceFirstEdge = edge;

            if (previousEdge != nullptr) {
                if (previousEdge->fccw == face) previousEdge->nccw = edge;
                else previousEdge->ncw = edge;

                if (edge->fccw == face) edge->pccw = previousEdge;
                else edge->pcw = previousEdge;
            }

            previousEdge = edge;
        }

        if (previousEdge->fccw == face) previousEdge->nccw = faceFirstEdge;
        else previousEdge->ncw = faceFirstEdge;

        if (faceFirstEdge->fccw == face) faceFirstEdge->pccw = previousEdge;
        else faceFirstEdge->pcw = previousEdge;

        face->e = faceFirstEdge;
    }

    DebugWingedEdge();
}

void BRep::DebugWingedEdge() {
    int errors = 0;

    std::cout << "\n========== WINGED-EDGE DEBUG ==========\n";

    // FindEdge
    for (const std::unique_ptr<Vertex>& vs : vertices) {
        for (const std::unique_ptr<Vertex>& ve : vertices) {
            if (vs.get() == ve.get()) continue;

            Edge* linear = FindEdgeLinear(vs.get(), ve.get());
            Edge* winged = FindEdge(vs.get(), ve.get());

            if (linear != winged) {
                std::cout << "[ERRO] FindEdge: " << vs->id << " -> " << ve->id << '\n';
                errors++;
            }
        }
    }

    // GetEdgesOfFace
    for (const std::unique_ptr<Face>& face : faces) {
        std::vector<Edge*> result = GetEdgesOfFace(face.get());
        int expected = 0;

        for (const std::unique_ptr<Edge>& edge : edges) {
            if (edge->fcw == face.get() || edge->fccw == face.get()) expected++;
        }

        if (static_cast<int>(result.size()) != expected) {
            std::cout << "[ERRO] GetEdgesOfFace: Face " << face->id
                      << " | Esperado: " << expected
                      << " | Obtido: " << result.size() << '\n';
            errors++;
        }

        for (Edge* edge : result) {
            if (edge == nullptr || (edge->fcw != face.get() && edge->fccw != face.get())) {
                std::cout << "[ERRO] GetEdgesOfFace: aresta invalida na Face " << face->id << '\n';
                errors++;
                continue;
            }

            if (std::count(result.begin(), result.end(), edge) > 1) {
                std::cout << "[ERRO] GetEdgesOfFace: aresta duplicada na Face " << face->id << '\n';
                errors++;
            }
        }
    }

    // GetVerticesOfFace
    for (const std::unique_ptr<Face>& face : faces) {
        std::vector<Vertex*> result = GetVerticesOfFace(face.get());
        std::vector<Edge*> faceEdges = GetEdgesOfFace(face.get());

        if (result.size() != faceEdges.size()) {
            std::cout << "[ERRO] GetVerticesOfFace: quantidade incorreta na Face " << face->id << '\n';
            errors++;
            continue;
        }

        for (int i = 0; i < static_cast<int>(faceEdges.size()); i++) {
            if (result[i] != faceEdges[i]->vs) {
                std::cout << "[ERRO] GetVerticesOfFace: ordem incorreta na Face " << face->id << '\n';
                errors++;
                break;
            }
        }

        for (Vertex* vertex : result) {
            if (vertex == nullptr) {
                std::cout << "[ERRO] GetVerticesOfFace: vertice nulo na Face " << face->id << '\n';
                errors++;
            }
        }
    }

    // GetFacesOfVertex
    for (const std::unique_ptr<Vertex>& vertex : vertices) {
        std::vector<Face*> result = GetFacesOfVertex(vertex.get());
        std::vector<Face*> expected;

        for (const std::unique_ptr<Edge>& edge : edges) {
            if (edge->vs != vertex.get() && edge->ve != vertex.get()) continue;

            if (edge->fcw != nullptr &&
                std::find(expected.begin(), expected.end(), edge->fcw) == expected.end()) {
                expected.push_back(edge->fcw);
            }

            if (edge->fccw != nullptr &&
                std::find(expected.begin(), expected.end(), edge->fccw) == expected.end()) {
                expected.push_back(edge->fccw);
            }
        }

        if (result.size() != expected.size()) {
            std::cout << "[ERRO] GetFacesOfVertex: Vertex " << vertex->id
                      << " | Esperado: " << expected.size()
                      << " | Obtido: " << result.size() << '\n';
            errors++;
        }

        for (Face* face : expected) {
            if (std::find(result.begin(), result.end(), face) == result.end()) {
                std::cout << "[ERRO] GetFacesOfVertex: Face ausente no Vertex " << vertex->id << '\n';
                errors++;
            }
        }

        if (std::adjacent_find(result.begin(), result.end()) != result.end()) {
            std::cout << "[ERRO] GetFacesOfVertex: faces duplicadas no Vertex " << vertex->id << '\n';
            errors++;
        }

        for (int i = 0; i < static_cast<int>(result.size()); i++) {
            if (std::count(result.begin(), result.end(), result[i]) > 1) {
                std::cout << "[ERRO] GetFacesOfVertex: face duplicada no Vertex " << vertex->id << '\n';
                errors++;
                break;
            }
        }
    }

    // GetEdgesOfVertex
    for (const std::unique_ptr<Vertex>& vertex : vertices) {
        std::vector<Edge*> result = GetEdgesOfVertex(vertex.get());
        std::vector<Edge*> expected;

        for (const std::unique_ptr<Edge>& edge : edges) {
            if (edge->vs == vertex.get() || edge->ve == vertex.get()) {
                expected.push_back(edge.get());
            }
        }

        if (result.size() != expected.size()) {
            std::cout << "[ERRO] GetEdgesOfVertex: Vertex " << vertex->id
                      << " | Esperado: " << expected.size()
                      << " | Obtido: " << result.size() << '\n';
            errors++;
        }

        for (Edge* edge : expected) {
            if (std::find(result.begin(), result.end(), edge) == result.end()) {
                std::cout << "[ERRO] GetEdgesOfVertex: aresta ausente no Vertex " << vertex->id << '\n';
                errors++;
            }
        }

        for (Edge* edge : result) {
            if (edge == nullptr || (edge->vs != vertex.get() && edge->ve != vertex.get())) {
                std::cout << "[ERRO] GetEdgesOfVertex: aresta invalida no Vertex " << vertex->id << '\n';
                errors++;
            }

            if (std::count(result.begin(), result.end(), edge) > 1) {
                std::cout << "[ERRO] GetEdgesOfVertex: aresta duplicada no Vertex " << vertex->id << '\n';
                errors++;
            }
        }
    }

    // GetAdjacentFaces
    for (const std::unique_ptr<Face>& face : faces) {
        std::vector<Face*> result = GetAdjacentFaces(face.get());
        std::vector<Face*> expected;

        for (const std::unique_ptr<Edge>& edge : edges) {
            Face* adjacent = nullptr;

            if (edge->fcw == face.get()) adjacent = edge->fccw;
            else if (edge->fccw == face.get()) adjacent = edge->fcw;

            if (adjacent != nullptr &&
                std::find(expected.begin(), expected.end(), adjacent) == expected.end()) {
                expected.push_back(adjacent);
            }
        }

        if (result.size() != expected.size()) {
            std::cout << "[ERRO] GetAdjacentFaces: Face " << face->id
                      << " | Esperado: " << expected.size()
                      << " | Obtido: " << result.size() << '\n';
            errors++;
        }

        for (Face* adjacent : expected) {
            if (std::find(result.begin(), result.end(), adjacent) == result.end()) {
                std::cout << "[ERRO] GetAdjacentFaces: Face adjacente ausente na Face " << face->id << '\n';
                errors++;
            }
        }

        for (Face* adjacent : result) {
            if (adjacent == face.get() || adjacent == nullptr) {
                std::cout << "[ERRO] GetAdjacentFaces: resultado invalido na Face " << face->id << '\n';
                errors++;
            }

            if (std::count(result.begin(), result.end(), adjacent) > 1) {
                std::cout << "[ERRO] GetAdjacentFaces: face duplicada na Face " << face->id << '\n';
                errors++;
            }
        }
    }

    std::cout << "\n========== RESULTADO ==========\n";

    if (errors == 0) std::cout << "[OK] Todos os testes passaram!\n";
    else std::cout << "[FALHA] Total de erros encontrados: " << errors << '\n';

    std::cout << "================================\n";
}

Edge* BRep::FindEdgeLinear(Vertex* vs, Vertex* ve) {
    if (vs == nullptr || ve == nullptr) return nullptr;

    for (const std::unique_ptr<Edge>& edge : edges) {
        if ((edge->vs == vs && edge->ve == ve) || (edge->vs == ve && edge->ve == vs)) return edge.get();
    }

    return nullptr;
}

//
Edge* BRep::FindEdge(Vertex* vs, Vertex* ve) {
    if (vs == nullptr || ve == nullptr || vs == ve || vs->e == nullptr) return nullptr;

    Edge* start = vs->e;
    Edge* current = start;
    int iterations = 0;

    do {
        if ((current->vs == vs && current->ve == ve) || (current->vs == ve && current->ve == vs)) return current;
        if (current->vs == vs) current = current->pccw;
        else if (current->ve == vs) current = current->pcw;
        else return nullptr;

        if (current == nullptr) return nullptr;

        iterations++;
        if (iterations > edges.size()) return nullptr;
    } while (current != start);

    return nullptr;
}

std::vector<Edge*> BRep::GetEdgesOfFace(Face* face) {
    std::vector<Edge*> result;

    if (face == nullptr || face->e == nullptr) return result;

    Edge* start = face->e;
    Edge* current = start;
    int iterations = 0;

    do {
        result.push_back(current);

        if (current->fcw == face) current = current->ncw;
        else if (current->fccw == face) current = current->nccw;
        else return {};

        if (current == nullptr) return {};

        iterations++;
        if (iterations > edges.size()) return {};
    } while (current != start);

    return result;
}

std::vector<Vertex*> BRep::GetVerticesOfFace(Face* face) {
    std::vector<Vertex*> result;

    std::vector<Edge*> faceEdges = GetEdgesOfFace(face);
    if (faceEdges.empty()) return result;

    for (Edge* edge : faceEdges) {
        result.push_back(edge->vs);
    }

    return result;
}

std::vector<Face*> BRep::GetFacesOfVertex(Vertex* vertex) {
    std::vector<Face*> result;

    if (vertex == nullptr || vertex->e == nullptr) return result;

    Edge* start = vertex->e;
    Edge* current = start;
    int iterations = 0;

    do {
        if (current->fcw != nullptr && std::find(result.begin(), result.end(), current->fcw) == result.end()) {
            result.push_back(current->fcw);
        }

        if (current->fccw != nullptr && std::find(result.begin(), result.end(), current->fccw) == result.end()) {
            result.push_back(current->fccw);
        }

        if (current->vs == vertex) current = current->pccw;
        else if (current->ve == vertex) current = current->pcw;
        else return {};

        if (current == nullptr) return {};

        iterations++;
        if (iterations > edges.size()) return {};
    } while (current != start);

    return result;
}

std::vector<Edge*> BRep::GetEdgesOfVertex(Vertex* vertex) {
    std::vector<Edge*> result;

    if (vertex == nullptr || vertex->e == nullptr) return result;

    Edge* start = vertex->e;
    Edge* current = start;
    int iterations = 0;

    do {
        if (current->vs != vertex && current->ve != vertex) return {};

        result.push_back(current);

        if (current->vs == vertex) current = current->pccw;
        else current = current->pcw;

        if (current == nullptr) return {};

        iterations++;
        if (iterations > edges.size()) return {};
    } while (current != start);

    return result;
}

std::vector<Face*> BRep::GetAdjacentFaces(Face* face) {
    std::vector<Face*> result;

    if (face == nullptr) return result;

    std::vector<Edge*> faceEdges = GetEdgesOfFace(face);
    if (faceEdges.empty()) return result;

    for (Edge* edge : faceEdges) {
        Face* adjacentFace = nullptr;

        if (edge->fcw == face) adjacentFace = edge->fccw;
        else if (edge->fccw == face) adjacentFace = edge->fcw;

        if (adjacentFace != nullptr &&
            std::find(result.begin(), result.end(), adjacentFace) == result.end()) {
            result.push_back(adjacentFace);
            }
    }

    return result;
}