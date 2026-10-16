#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() {
    float defaultCell[] = {
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,

         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,

         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,

         0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(defaultCell), defaultCell, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    
    float wireframeCell[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,

         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,

         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,

         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,

         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,

         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f
    };

    glGenVertexArrays(1, &wireframeVAO);
    glGenBuffers(1, &wireframeVBO);

    glBindVertexArray(wireframeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, wireframeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wireframeCell), wireframeCell, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);

    glBindVertexArray(meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteVertexArrays(1, &wireframeVAO);
    glDeleteBuffers(1, &wireframeVBO);

    glDeleteVertexArrays(1, &meshVAO);
    glDeleteBuffers(1, &meshVBO);
}

void Renderer::DrawCell(const Cell& cell, Shader& shader) {
 glm::mat4 model = glm::translate(
     glm::mat4(1.0f),
     glm::vec3(
         cell.x + cell.size / 2.0f,
         cell.y + cell.size / 2.0f,
         cell.z + cell.size / 2.0f
     )
 );

 model = glm::scale(model, glm::vec3(cell.size));

 shader.SetMat4("model", model);

 glBindVertexArray(VAO);
 glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::DrawCellWireFrame(const Cell& cell, Shader& shader) {
    glm::mat4 model = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(
            cell.x + cell.size / 2.0f,
            cell.y + cell.size / 2.0f,
            cell.z + cell.size / 2.0f
        )
    );

    model = glm::scale(model, glm::vec3(cell.size));

    shader.SetMat4("model", model);

    glBindVertexArray(wireframeVAO);
    glDrawArrays(GL_LINES, 0, 24);
}

void Renderer::DrawOctree(const Node* node, Shader& shader, bool showWireframe, bool showCell, int level, int maxRenderLevel) {
    if (node == nullptr || level > maxRenderLevel) {
        return;
    }

    bool hasChildren = node->children[0] != nullptr;
    bool isRenderLimit = level == maxRenderLevel;

    if (node->state != Intersection::Outside && (!hasChildren || isRenderLimit)) {
        if (showCell) DrawCell(node->cell, shader);
        if (showWireframe) DrawCellWireFrame(node->cell, shader);
    } else if (showWireframe) DrawCellWireFrame(node->cell, shader);

    for (int i = 0; i < 8; i++) {
        if (node->children[i] != nullptr) {
            DrawOctree(node->children[i].get(), shader, showWireframe, showCell, level + 1, maxRenderLevel);
        }
    }
}

void Renderer::DrawMesh(const std::vector<Triangle>& mesh, Shader& shader, float scale, const glm::vec3& translation) {
    if (mesh.empty()) return;

    std::vector<float> vertices;
    vertices.reserve(mesh.size() * 18);

    for (const Triangle& triangle : mesh) {
        glm::vec3 normal = glm::normalize(glm::cross(triangle.v1 - triangle.v0, triangle.v2 - triangle.v0));

        for (const glm::vec3& v : {triangle.v0, triangle.v1, triangle.v2}) {
            vertices.insert(vertices.end(), {v.x, v.y, v.z, normal.x, normal.y, normal.z});
        }
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    model = glm::scale(model, glm::vec3(scale));
    shader.SetMat4("model", model);

    glBindVertexArray(meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.size() * 3));

    glBindVertexArray(0);
}