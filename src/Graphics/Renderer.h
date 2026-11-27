#pragma once

#include <glad/glad.h>

#include "../Engine/Primitives/Primitive.h"
#include "../Engine/Octree/Octree.h"
#include "../Engine/CSG/CSG.h"
#include "../Engine/BRep/BRep.h"
#include "Camera.h"
#include "Shader.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    void DrawCell(const Cell&, Shader&);
    void DrawCellWireFrame(const Cell&, Shader&);
    void DrawOctree(const Node*, Shader&, bool, bool, int, int);

    void DrawMesh(const std::vector<Triangle>&, Shader&, float, const glm::vec3&);
    void DrawBrepWireFrame(const std::vector<std::unique_ptr<Edge>>&, Shader&);
    void DrawBrepVertices(const std::vector<std::unique_ptr<Vertex>>&, Shader&);
    void DrawVertex(const Vertex*, Shader&);

    GLuint LoadTexture(const char* path);

private:
    GLuint VAO{};
    GLuint VBO{};
    GLuint wireframeVAO{};
    GLuint wireframeVBO{};
    GLuint meshVAO{};
    GLuint meshVBO{};
};