#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
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

void BRep::TestEuler() {
    auto contar = [&](const char* passo) {
        int V = vertices.size(), E = edges.size(), F = faces.size();
        std::cout << passo << "  V=" << V << " E=" << E << " F=" << F
                  << "  V-E+F=" << (V - E + F) << "\n";
    };

    // ---- construção
    auto [v0, f] = MVFS({0, 0, 0});
    Vertex* v1 = MEV(f, v0, {1, 0,  0});
    Vertex* v2 = MEV(f, v1, {1, 0, -1});
    Vertex* v3 = MEV(f, v2, {0, 0, -1});
    Face* fundo = MEF(f, v3, v0);

    Vertex* v4 = MEV(f, v0, {0, 1,  0});
    Vertex* v5 = MEV(f, v1, {1, 1,  0});
    Vertex* v6 = MEV(f, v2, {1, 1, -1});
    Vertex* v7 = MEV(f, v3, {0, 1, -1});

    Face* s1 = MEF(f, v4, v5);
    Face* s2 = MEF(f, v5, v6);
    Face* s3 = MEF(f, v6, v7);
    Face* s4 = MEF(f, v7, v4);
    contar("cubo   ");                 // esperado: V=8 E=12 F=6, V-E+F=2
    DebugWingedEdge();

    // ---- desmontagem (ordem inversa)
    KEF(s4->e); KEF(s3->e); KEF(s2->e); KEF(s1->e);
    contar("sem lat");                 // V=8 E=8 F=2
    KEV(v7->e, v7); KEV(v6->e, v6); KEV(v5->e, v5); KEV(v4->e, v4);
    contar("base   ");                 // V=4 E=4 F=2
    KEF(fundo->e);
    KEV(v3->e, v3); KEV(v2->e, v2); KEV(v1->e, v1);
    contar("1 vert ");                 // V=1 E=0 F=1
    KVFS(v0, f);
    contar("vazio  ");                 // V=0 E=0 F=0
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

    std::cout << "\n========== resultADO ==========\n";

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





// Triangular BRep
float ProdVetorial2D(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool isInside(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c) {
    return ProdVetorial2D(a, b, p) >= 0 && ProdVetorial2D(b, c, p) >= 0 && ProdVetorial2D(c, a, p) >= 0;
}

void newTriangle(std::vector<Triangle>& mesh, const std::vector<Vertex*>& vs, bool isInverted, int i, int j, int k) {
    if (isInverted) std::swap(j, k);
    mesh.emplace_back(vs[i]->coords, vs[j]->coords, vs[k]->coords);
}

std::vector<Triangle> BRep::TriangulateMesh() {
    std::vector<Triangle> mesh;

    for (std::unique_ptr<Face>& face : faces) {
        std::vector<Vertex*> vertices = GetVerticesOfFace(face.get());
        int n = vertices.size();
        if (n < 3) continue;

        glm::vec3 min = vertices[0]->coords;
        glm::vec3 max = vertices[0]->coords;
        for (Vertex* v : vertices) {
            min = glm::min(min, v->coords);
            max = glm::max(max, v->coords);
        }
        glm::vec3 boundingBox = max - min;

        int a = 0, b = 1;
        if (boundingBox.x <= boundingBox.y && boundingBox.x <= boundingBox.z) {
            a = 1;
            b = 2;
        }
        else if (boundingBox.y <= boundingBox.x && boundingBox.y <= boundingBox.z) {
            a = 0; 
            b = 2;
        }

        // adicional Ear Cliping (se nao as faces ficam tudo bugadas)
        std::vector<glm::vec2> projectedVertices(n);
        for (int i = 0; i < n; i++) projectedVertices[i] = { vertices[i]->coords[a], vertices[i]->coords[b] };

        std::vector<int> idx(n);
        for (int i = 0; i < n; i++) idx[i] = i;

        float area = 0.0f;
        for (int i = 0; i < n; i++) {
            glm::vec2 p0 = projectedVertices[i];
            glm::vec2 p1 = projectedVertices[(i + 1) % n];
            area += p0.x * p1.y - p1.x * p0.y;
        }

        bool isInverted = (area < 0);
        if (isInverted) std::reverse(idx.begin(), idx.end());

        while (idx.size() > 3) {
            int m = idx.size();
            bool cortou = false;

            for (int i = 0; i < m; i++) {
                int ia = idx[(i + m - 1) % m];
                int ib = idx[i];
                int ic = idx[(i + 1) % m];

                if (ProdVetorial2D(projectedVertices[ia], projectedVertices[ib], projectedVertices[ic]) <= 0) continue;

                bool vazia = true;
                for (int j : idx) {
                    if (j == ia || j == ib || j == ic) continue;
                    if (isInside(projectedVertices[j], projectedVertices[ia], projectedVertices[ib], projectedVertices[ic])) {
                        vazia = false;
                        break;
                    }
                }
                if (!vazia) continue;

                newTriangle(mesh, vertices, isInverted, ia, ib, ic);
                idx.erase(idx.begin() + i);
                cortou = true;
                break;
            }

            if (!cortou) {
                newTriangle(mesh, vertices, isInverted, idx[m - 1], idx[0], idx[1]);
                idx.erase(idx.begin());
            }
        }
        
        newTriangle(mesh, vertices, isInverted, idx[0], idx[1], idx[2]);
    }
    return mesh;
}





// Primitivas
void BRep::CreateEulerCube() {
    auto [v0, f] = MVFS({-0.5f, -0.5f,  0.5f});
    Vertex* v1 = MEV(f, v0, { 0.5f, -0.5f,  0.5f});
    Vertex* v2 = MEV(f, v1, { 0.5f, -0.5f, -0.5f});
    Vertex* v3 = MEV(f, v2, {-0.5f, -0.5f, -0.5f});
    Face* fundo = MEF(f, v3, v0);

    Vertex* v4 = MEV(f, v0, {-0.5f,  0.5f,  0.5f});
    Vertex* v5 = MEV(f, v1, { 0.5f,  0.5f,  0.5f});
    Vertex* v6 = MEV(f, v2, { 0.5f,  0.5f, -0.5f});
    Vertex* v7 = MEV(f, v3, {-0.5f,  0.5f, -0.5f});

    Face* s1 = MEF(f, v4, v5);
    Face* s2 = MEF(f, v5, v6);
    Face* s3 = MEF(f, v6, v7);
    Face* s4 = MEF(f, v7, v4);
}

glm::vec3 BRep::getSpherePoint(int j, int i, int camadas, int res, float radius) {
    float phi = PI * j / camadas;
    float theta = 2.0f * PI * i / res;

    float r = radius * std::sin(phi);
    return {r * std::cos(theta), -radius * std::cos(phi), -r * std::sin(theta)};
}

void BRep::CreateEulerSphere(int res) {
    int camadas = res / 2;
    float radius = 0.5f;

    std::vector<Vertex*> midRing(res);

    auto [v0, f] = MVFS(getSpherePoint(1, 0, camadas, res, radius));
    midRing[0] = v0;

    for (int i = 1; i < res; i++) midRing[i] = MEV(f, midRing[i - 1], getSpherePoint(1, i, camadas, res, radius));
    Face* bottomFace = MEF(f, midRing[res - 1], midRing[0]);
    std::vector<Vertex*> base = midRing;

    for (int j = 2; j < camadas; j++) {
        std::vector<Vertex*> midRingBkp = midRing;
        for (int i = 0; i < res; i++) midRing[i] = MEV(f, midRingBkp[i], getSpherePoint(j, i, camadas, res, radius));
        for (int i = 0; i < res; i++) MEF(f, midRing[i], midRing[(i + 1) % res]);
    }

    Vertex* top = MEV(f, midRing[0], {0.0f, radius, 0.0f});
    for (int i = 1; i < res; i++) MEF(f, top, midRing[i]);

    Vertex* bottom = MEV(bottomFace, base[0], {0.0f, -radius, 0.0f});
    for (int i = res - 1; i >= 1; i--) MEF(bottomFace, bottom, base[i]);
}

glm::vec3 BRep::getCylinderPoint(int i, int res, float radius, float y) {
    float theta = 2.0f * PI * i / res;
    return {radius * std::cos(theta), y, -radius * std::sin(theta)};
}

void BRep::CreateEulerCylinder(int res) {
    float radius = 0.5f;
    float height = 1.0f;
    float bot = -height / 2.0f;
    float top =  height / 2.0f;

    std::vector<Vertex*> base(res);
    std::vector<Vertex*> topo(res);

    auto [v0, f] = MVFS(getCylinderPoint(0, res, radius, bot));
    base[0] = v0;
    for (int i = 1; i < res; i++) base[i] = MEV(f, base[i - 1], getCylinderPoint(i, res, radius, bot));
    Face* bottomFace = MEF(f, base[res - 1], base[0]);

    for (int i = 0; i < res; i++) topo[i] = MEV(f, base[i], getCylinderPoint(i, res, radius, top));
    for (int i = 0; i < res; i++) MEF(f, topo[i], topo[(i + 1) % res]);
}

void BRep::CreateEulerCone(int res) {
    float radius = 0.5f;
    float height = 1.0f;
    float bot = -height / 2.0f;
    float top =  height / 2.0f;

    std::vector<Vertex*> base(res);

    auto [v0, f] = MVFS(getCylinderPoint(0, res, radius, bot));
    base[0] = v0;
    for (int i = 1; i < res; i++) base[i] = MEV(f, base[i - 1], getCylinderPoint(i, res, radius, bot));
    Face* bottomFace = MEF(f, base[res - 1], base[0]);

    Vertex* topVertex = MEV(f, base[0], {0.0f, top, 0.0f});
    for (int i = 1; i < res; i++) MEF(f, topVertex, base[i]);
}





// ADJACENCIAS
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

std::vector<Edge*> BRep::GetEdgesOfFace(Face* f) {
    std::vector<Edge*> result;
    if (f == nullptr || f->e == nullptr) return result;

    Edge* e = f->e;
    Edge* start = e;

    bool isCCW = (e->fccw == f);
    bool startIsCCW = isCCW;
    int iterations = 0;

    do {
        result.push_back(e);
        Vertex* currentEnd = isCCW ? e->ve : e->vs;
        e = isCCW ? e->nccw : e->ncw;
        if (e == nullptr) return {};
        isCCW = (e->vs == currentEnd);

        iterations++;
        if (iterations > 2 * edges.size()) return {};
    } while (e != start || isCCW != startIsCCW);

    return result;
}

std::vector<Vertex*> BRep::GetVerticesOfFace(Face* f) {
    std::vector<Vertex*> result;
    if (f == nullptr || f->e == nullptr) return result;

    Edge* e = f->e;
    Edge* start = e;

    bool isCCW = (e->fccw == f);
    bool startIsCCW = isCCW;
    int iterations = 0;

    do {
        result.push_back(isCCW ? e->vs : e->ve);
        Vertex* fim = isCCW ? e->ve : e->vs;
        e = isCCW ? e->nccw : e->ncw;
        isCCW = (e->vs == fim);

        iterations++;
        if (iterations > 2 * edges.size()) return {};
    } while (e != start || isCCW != startIsCCW);
    return result;
}

std::vector<Face*> BRep::GetFacesOfVertex(Vertex* v) {
    std::vector<Face*> result;
    if (v == nullptr || v->e == nullptr) return result;

    Edge* start = v->e;
    Edge* current = start;
    int iterations = 0;

    do {
        if (current->fcw != nullptr && std::find(result.begin(), result.end(), current->fcw) == result.end()) result.push_back(current->fcw);
        if (current->fccw != nullptr && std::find(result.begin(), result.end(), current->fccw) == result.end()) result.push_back(current->fccw);

        if (current->vs == v) current = current->pccw;
        else if (current->ve == v) current = current->pcw;
        else return {};

        if (current == nullptr) return {};

        iterations++;
        if (iterations > edges.size()) return {};
    } while (current != start);

    return result;
}

std::vector<Edge*> BRep::GetEdgesOfVertex(Vertex* v) {
    std::vector<Edge*> result;
    if (v == nullptr || v->e == nullptr) return result;

    Edge* start = v->e;
    Edge* current = start;
    int iterations = 0;

    do {
        if (current->vs != v && current->ve != v) return {};
        result.push_back(current);

        if (current->vs == v) current = current->pccw;
        else current = current->pcw;

        if (current == nullptr) return {};

        iterations++;
        if (iterations > edges.size()) return {};
    } while (current != start);

    return result;
}

std::vector<Face*> BRep::GetAdjacentFaces(Face* f) {
    std::vector<Face*> result;
    if (f == nullptr) return result;

    std::vector<Edge*> edges = GetEdgesOfFace(f);
    if (edges.empty()) return result;

    for (Edge* e : edges) {
        Face* adjacentFace = nullptr;

        if (e->fcw == f) adjacentFace = e->fccw;
        else if (e->fccw == f) adjacentFace = e->fcw;

        if (adjacentFace != nullptr && adjacentFace != f && std::find(result.begin(), result.end(), adjacentFace) == result.end()) {
            result.push_back(adjacentFace);
        }
    }

    return result;
}

std::vector<Edge*> BRep::GetAdjacentEdges(Edge* e) {
    std::vector<Edge*> result;
    if (e == nullptr) return result;

    for (Vertex* v : { e->vs, e->ve }) {
        for (Edge* x : GetEdgesOfVertex(v)) {
            if (x != e && std::find(result.begin(), result.end(), x) == result.end()) result.push_back(x);
        }
    }
    return result;
}

std::vector<Vertex*> BRep::GetAdjacentVertices(Vertex* v) {
    std::vector<Vertex*> result;
    if (v == nullptr) return result;

    for (Edge* e : GetEdgesOfVertex(v)) {
        Vertex* v2 = (e->vs == v) ? e->ve : e->vs;
        if (v2 != nullptr && std::find(result.begin(), result.end(), v2) == result.end()) result.push_back(v2);
    }
    return result;
}





// EULER AUX
Vertex* BRep::NewVertex(glm::vec3 p) {
    vertices.push_back(std::make_unique<Vertex>());
    Vertex* v = vertices.back().get();
    v->id = nextVertexId++;
    v->coords = p;
    return v;
}

Edge* BRep::NewEdge(Vertex* vs, Vertex* ve) {
    edges.push_back(std::make_unique<Edge>());
    Edge* e = edges.back().get();
    e->id = nextEdgeId++;
    e->vs = vs;
    e->ve = ve;
    return e;
}

Face* BRep::NewFace() {
    faces.push_back(std::make_unique<Face>());
    Face* f = faces.back().get();
    f->id = nextFaceId++;
    return f;
}

void BRep::RemoveVertex(Vertex* v) {
    for (size_t i = 0; i < vertices.size(); i++) {
        if (vertices[i].get() == v) {
            vertices.erase(vertices.begin() + i);
            return;
        }
    }
}

void BRep::RemoveEdge(Edge* e) {
    for (size_t i = 0; i < edges.size(); i++) {
        if (edges[i].get() == e) {
            edges.erase(edges.begin() + i);
            return;
        }
    }
}

void BRep::RemoveFace(Face* f) {
    for (size_t i = 0; i < faces.size(); i++) {
        if (faces[i].get() == f) {
            faces.erase(faces.begin() + i);
            return;
        }
    }
}





// EULER
std::pair<Vertex*, Face*> BRep::MVFS(glm::vec3 p) {
    return { NewVertex(p), NewFace() };
}

Vertex* BRep::MEV(Face* f, Vertex* v1, glm::vec3 p) {
    Vertex* v2 = NewVertex(p);
    Edge* e = NewEdge(v1, v2);
    e->fccw = e->fcw = f;

    if (!v1->e) {
        e->nccw = e->pccw = e->ncw = e->pcw = e;
        v1->e = e;
        f->e = e;
    } else {
        Edge* b = f->e;
        bool bccw = (b->fccw == f);
        while ((bccw ? b->vs : b->ve) != v1) {
            Vertex* fim = bccw ? b->ve : b->vs;
            b = bccw ? b->nccw : b->ncw;
            bccw = (b->vs == fim);
        }
        Edge* a = bccw ? b->pccw : b->pcw;
        bool accw = (a->ve == v1);

        e->pccw = a; e->nccw = e;
        e->pcw  = e; e->ncw  = b;
        if (accw) a->nccw = e; else a->ncw = e;
        if (bccw) b->pccw = e; else b->pcw = e;
    }
    v2->e = e;
    return v2;
}

Face* BRep::MEF(Face* f, Vertex* v1, Vertex* v2) {
    Edge* b1 = f->e;
    bool b1ccw = (b1->fccw == f);
    while ((b1ccw ? b1->vs : b1->ve) != v1) {
        Vertex* fim = b1ccw ? b1->ve : b1->vs;
        b1 = b1ccw ? b1->nccw : b1->ncw;
        b1ccw = (b1->vs == fim);
    }
    Edge* a1 = b1ccw ? b1->pccw : b1->pcw;
    bool a1ccw = (a1->ve == v1);

    Edge* b2 = f->e;
    bool b2ccw = (b2->fccw == f);
    while ((b2ccw ? b2->vs : b2->ve) != v2) {
        Vertex* fim = b2ccw ? b2->ve : b2->vs;
        b2 = b2ccw ? b2->nccw : b2->ncw;
        b2ccw = (b2->vs == fim);
    }
    Edge* a2 = b2ccw ? b2->pccw : b2->pcw;
    bool a2ccw = (a2->ve == v2);

    Face* f2 = NewFace();
    Edge* e = NewEdge(v1, v2);

    Edge* x = b1;
    bool isCCW = b1ccw;
    while (true) {
        if (isCCW) x->fccw = f2; else x->fcw = f2;
        if (x == a2 && isCCW == a2ccw) break;
        Vertex* fim = isCCW ? x->ve : x->vs;
        x = isCCW ? x->nccw : x->ncw;
        isCCW = (x->vs == fim);
    }

    e->fccw = f;  e->pccw = a1; e->nccw = b2;
    e->fcw  = f2; e->pcw  = a2; e->ncw  = b1;
    if (a1ccw) a1->nccw = e; else a1->ncw = e;
    if (b2ccw) b2->pccw = e; else b2->pcw = e;
    if (a2ccw) a2->nccw = e; else a2->ncw = e;
    if (b1ccw) b1->pccw = e; else b1->pcw = e;

    f->e = e;
    f2->e = e;
    return f2;
}

void BRep::KVFS(Vertex* v, Face* f) {
    RemoveVertex(v);
    RemoveFace(f);
}

void BRep::KEV(Edge* e, Vertex* v) {
    Face* f = e->fccw;
    Vertex* v1 = (e->vs == v) ? e->ve : e->vs;

    if (e->pccw == e && e->nccw == e) {
        v1->e = nullptr;
        f->e = nullptr;
    } else {
        bool idaCcw = (e->ve == v);
        Edge* a = idaCcw ? e->pccw : e->pcw;
        bool accw = (a->ve == v1);
        Edge* b = idaCcw ? e->ncw : e->nccw;
        bool bccw = (b->vs == v1);

        if (accw) a->nccw = b; else a->ncw = b;
        if (bccw) b->pccw = a; else b->pcw = a;
        if (v1->e == e) v1->e = b;
        if (f->e  == e) f->e  = a;
    }

    RemoveVertex(v);
    RemoveEdge(e);
}

void BRep::KEF(Edge* e) {
    Face* f  = e->fccw;
    Face* f2 = e->fcw;
    Vertex* v1 = e->vs;
    Vertex* v2 = e->ve;

    Edge* a1 = e->pccw; bool a1ccw = (a1->ve == v1);
    Edge* b2 = e->nccw; bool b2ccw = (b2->vs == v2);
    Edge* a2 = e->pcw;  bool a2ccw = (a2->ve == v2);
    Edge* b1 = e->ncw;  bool b1ccw = (b1->vs == v1);

    Edge* x = b1;
    bool isCCW = b1ccw;
    while (x != e) {
        if (isCCW) x->fccw = f; else x->fcw = f;
        Vertex* fim = isCCW ? x->ve : x->vs;
        x = isCCW ? x->nccw : x->ncw;
        isCCW = (x->vs == fim);
    }

    if (a1ccw) a1->nccw = b1; else a1->ncw = b1;
    if (b1ccw) b1->pccw = a1; else b1->pcw = a1;
    if (a2ccw) a2->nccw = b2; else a2->ncw = b2;
    if (b2ccw) b2->pccw = a2; else b2->pcw = a2;

    if (v1->e == e) v1->e = b1;
    if (v2->e == e) v2->e = b2;
    f->e = a1;

    RemoveFace(f2);
    RemoveEdge(e);
}





// Transformações
void BRep::Translate(glm::vec3 value) {
    for (std::unique_ptr<Vertex>& v : vertices) v->coords += value;
    translation += value;
}

void BRep::Scale(glm::vec3 value) {
    for (std::unique_ptr<Vertex>& v : vertices) v->coords = translation + (v->coords - translation) * value;
}

void BRep::Rotate(glm::vec3 value) {
    glm::mat4 rotation(1.0f);
    rotation = glm::rotate(rotation, glm::radians(value.z), glm::vec3(0, 0, 1));
    rotation = glm::rotate(rotation, glm::radians(value.y), glm::vec3(0, 1, 0));
    rotation = glm::rotate(rotation, glm::radians(value.x), glm::vec3(1, 0, 0));
    glm::mat3 rotation3x3(rotation);

    for (std::unique_ptr<Vertex>& v : vertices) v->coords = translation + rotation3x3 * (v->coords - translation);
}





// Analises
float BRep::CalculateSurfaceArea(std::vector<Triangle>& mesh) {
    float area = 0.0f;

    for (const Triangle& t : mesh) {
        glm::vec3 u = t.v1 - t.v0;
        glm::vec3 v = t.v2 - t.v0;

        area += glm::length(glm::cross(u, v)) * 0.5f;
    }

    return area;
}

// lembrar: https://algorithms.alexli.ca/polyhedron-volume
float BRep::CalculateVolume(std::vector<Triangle>& mesh) {
    float volume = 0.0f;

    for (const Triangle& t : mesh) {
        volume += glm::dot(t.v0, glm::cross(t.v1, t.v2)) / 6.0f;
    }

    return std::abs(volume);
}