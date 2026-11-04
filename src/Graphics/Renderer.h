#pragma once

#include <glad/glad.h>

#include "../Engine/Primitives/Primitive.h"
#include "../Engine/Octree/Octree.h"
#include "../Engine/CSG/CSG.h"
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

    void InitializeRayCastTexture();
    void DrawRayCastImage(const CSG& csg, const Camera& camera, int viewportWidth, int viewportHeight);

private:
    GLuint VAO{};
    GLuint VBO{};
    GLuint wireframeVAO{};
    GLuint wireframeVBO{};
    GLuint meshVAO{};
    GLuint meshVBO{};
};