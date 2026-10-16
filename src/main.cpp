#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Interface/Interface.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include "Graphics/Scene.h"
#include "Graphics/Renderer.h"

#include "Engine/Primitives/Sphere.h"
#include "Engine/Primitives/Cylinder.h"
#include "Engine/Primitives/Cone.h"
#include "Engine/Primitives/Cube.h"
#include "Engine/Octree/Octree.h"

namespace fs = std::filesystem;

int main() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Modelador", nullptr, nullptr);

    if (!window) {
        std::cerr << "Erro ao criar janela GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar GLAD!" << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    Shader globalShader("shaders/basic.vert", "shaders/basic.frag");
    Renderer renderer;
    Camera camera;
    Scene scene;
    Interface UI;
    float navBarSize = 320.0f;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDepthMask(GL_TRUE);

    // Callbacks
    UI.OnSelectObject = [&scene](int index) {
        scene.setSelectedOctree(index);
    };

    UI.OnSelectSecondObject = [&scene](int index) {
        scene.setSecondSelectedOctree(index);
    };

    UI.OnCreateSphere = [&scene](std::string label, float radius, int maxLevel) {
        Octree newOctree;
        Sphere sphere(radius);
        if (label.empty()) label = "(Esfera) Object " + std::to_string(scene.getOctreesSize());
        newOctree.setLabel(label);
        newOctree.setMaxLevel(maxLevel);
        newOctree.Generate(sphere);

        scene.addOctree(std::move(newOctree));
    };

    UI.OnCreateCube = [&scene](std::string label, float sizeX, float sizeY, float sizeZ, int maxLevel, float rotationX, float rotationY, float rotationZ) {
        Octree newOctree;
        Cube cube(sizeX, sizeY, sizeZ);
        cube.SetRotation(rotationX, rotationY, rotationZ);
        if (label.empty()) label = "(Bloco) Object " + std::to_string(scene.getOctreesSize());
        newOctree.setLabel(label);
        newOctree.setMaxLevel(maxLevel);
        newOctree.Generate(cube);

        scene.addOctree(std::move(newOctree));
    };

    UI.OnCreateCylinder = [&scene](std::string label, float radius, float height, int maxLevel, float rotationX, float rotationY, float rotationZ) {
        Octree newOctree;
        Cylinder cylinder(radius, height);
        cylinder.SetRotation(rotationX, rotationY, rotationZ);
        if (label.empty()) label = "(Cilindro) Object " + std::to_string(scene.getOctreesSize());
        newOctree.setLabel(label);
        newOctree.setMaxLevel(maxLevel);
        newOctree.Generate(cylinder);

        scene.addOctree(std::move(newOctree));
    };

    UI.OnCreateCone = [&scene](std::string label, float radius, float height, int maxLevel, float rotationX, float rotationY, float rotationZ) {
        Octree newOctree;
        Cone cone(radius, height);
        cone.SetRotation(rotationX, rotationY, rotationZ);
        if (label.empty()) label = "(Cone) Object " + std::to_string(scene.getOctreesSize());
        newOctree.setLabel(label);
        newOctree.setMaxLevel(maxLevel);
        newOctree.Generate(cone);

        scene.addOctree(std::move(newOctree));
    };

    UI.OnMoveObject = [&scene](Axys axis, float value, bool useGlobalTransform) {
        glm::vec3 t = scene.getSelectedOctree().GetTranslation();

        switch (axis) {
            case Axys::X: t.x = useGlobalTransform ? value : t.x + value; break;
            case Axys::Y: t.y = useGlobalTransform ? value : t.y + value; break;
            case Axys::Z: t.z = useGlobalTransform ? value : t.z + value; break;
        }

        scene.getSelectedOctree().SetTranslation(t.x, t.y, t.z);
    };

    UI.OnScaleObject = [&scene](float scale, bool useGlobalTransform) {
        if (!useGlobalTransform) scale = scene.getSelectedOctree().GetScale() * scale;
        scene.getSelectedOctree().SetScale(scale);
    };

    UI.OnCalculateVolume = [&scene]() -> float {
        return scene.getSelectedOctree().getVolume();
    };

    UI.OnCalculateArea = [&scene]() -> float {
        return scene.getSelectedOctree().getSurfaceArea();
    };

    UI.toggleVisibility = [&scene]() {
        scene.toggleSelectedVisibility();
    };

    UI.toggleWireframe = [&scene]() {
        scene.toggleWireFrame();
    };

    UI.toggleCell = [&scene]() {
        scene.toggleCell();
    };

    UI.OnUnion = [&scene, &UI](std::string label, int maxLevel, bool useDynamicLevel) {
        Octree newOctree = Octree::ApplyUnion(scene.getSelectedOctree(), scene.getSecondSelectedOctree(), maxLevel, useDynamicLevel);
        newOctree.setLabel(label);

        scene.removeSelectedOctrees();
        scene.addOctree(std::move(newOctree));
        scene.resetSelectedIndex();
        scene.resetSecondSelectedIndex();

        UI.ResetSelection();
    };

    UI.OnIntersection = [&scene, &UI](std::string label, int maxLevel, bool useDynamicLevel) {
        Octree newOctree = Octree::ApplyIntersection(scene.getSelectedOctree(), scene.getSecondSelectedOctree(), maxLevel, useDynamicLevel);
        newOctree.setLabel(label);

        scene.removeSelectedOctrees();
        scene.addOctree(std::move(newOctree));
        scene.resetSelectedIndex();
        scene.resetSecondSelectedIndex();

        UI.ResetSelection();
    };

    UI.OnDeleteObject = [&scene, &UI]() {
        scene.removeSelectedOctree();

        scene.resetSelectedIndex();
        UI.ResetSelection();
    };

    UI.ResetPosition = [&camera]() {
        camera.ResetPosition();
    };

    UI.ResetRotation = [&camera]() {
        camera.ResetRotation();
    };

    UI.ResetZoom = [&camera]() {
        camera.ResetZoom();
    };

    UI.OnExportObject = [&scene](std::string fileName) {
        fs::create_directories("files/output");

        std::ofstream file("files/output/" + fileName + ".txt");
        file << scene.getSelectedOctree().ToString();
    };

    UI.OnImportObject = [&scene](std::string fileName) {
        std::ifstream file("files/input/" + fileName);
        if (!file.is_open()) return;

        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string data = buffer.str();

        if (data.empty()) return;

        Octree newOctree = Octree::ImportFromString(data);
        newOctree.setLabel(fs::path(fileName).stem().string());

        scene.addOctree(std::move(newOctree));
    };

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // config
        int windowWidth;
        int windowHeight;

        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        int viewportWidth = windowWidth - navBarSize;
        int viewportHeight = windowHeight;

        glViewport(navBarSize, 0, viewportWidth, viewportHeight);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);

        // UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool isMoving = false;

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            camera.Rotate(mouseDelta.x * 0.5f, mouseDelta.y * 0.5f);
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            isMoving = true;
        }

        float speed = 5.0f * deltaTime;

        if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
            camera.MoveForward(speed);
            isMoving = true;
        }
        if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
            camera.MoveForward(-speed);
            isMoving = true;
        }

        if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
            camera.MoveRight(speed);
            isMoving = true;
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
            camera.MoveRight(-speed);
            isMoving = true;
        }

        if (ImGui::IsKeyDown(ImGuiKey_PageUp)) {
            camera.MoveUp(speed);
            isMoving = true;
        }
        if (ImGui::IsKeyDown(ImGuiKey_PageDown)) {
            camera.MoveUp(-speed);
            isMoving = true;
        }

        float mouseWheel = ImGui::GetIO().MouseWheel;
        if (mouseWheel != 0.0f) {
            camera.Zoom(mouseWheel);
            isMoving = true;
        }

        scene.setSceneState(isMoving ? SceneState::onMoving : SceneState::Static);

        UI.Render(navBarSize, scene.getObjectNames());
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // draw objects
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            aspectRatio,
            0.1f,
            300.0f
        );

        globalShader.Use();

        globalShader.SetMat4("view", camera.GetViewMatrix());
        globalShader.SetMat4("projection", projection);

        scene.RenderScene(renderer, globalShader);

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}