#pragma once

#include <imgui.h>
#include <vector>
#include <string>
#include <functional>

#include <ImGuizmo.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "../Graphics/Renderer.h"

#include "Octree.h"
#include "../BRep/BRep.h"
#include "../Graphics/Camera.h"

extern ImGuiWindowFlags flags;

enum class ToolType {
    None,
    Sphere,
    Cube,
    Cone,
    Cylinder,
    Union,
    Intersection,
    Difference,
    Volume,
    Area,
    Move,
    Scale,
    Rotate,
    Camera,
    Import,
    Export,
    Render,
    MVFS,
    MEV,
    MEF,
    KVFS,
    KEV,
    KEF,
    SelectVertex
};

enum class Axys {
    X,
    Y,
    Z
};

enum class ObjectToolCollection {
    Octree,
    CSG,
    BREP
};

class Interface {
    public:
        void Render(float, const std::vector<std::string>&, std::vector<Vertex*>);

        // callbacks
        // octree
        std::function<void(std::string, float, int)> OnCreateSphere;
        std::function<void(std::string, float, float, float, int, float, float, float)> OnCreateCube;
        std::function<void(std::string, float, float, int, float, float, float)> OnCreateCylinder;
        std::function<void(std::string, float, float, int, float, float, float)> OnCreateCone;

        std::function<void(Axys ,float, bool)> OnMoveObject;
        std::function<void(float, bool)> OnScaleObject;

        std::function<void(std::string, int, bool)> OnUnion;
        std::function<void(std::string, int, bool)> OnIntersection;

        // CSG
        std::function<void(std::string)> OnCreateCSGSphere;
        std::function<void(std::string)> OnCreateCSGCube;
        std::function<void(std::string)> OnCreateCSGCylinder;
        std::function<void(std::string)> OnCreateCSGCone;

        std::function<void(std::string)> OnCSGUnion;
        std::function<void(std::string)> OnCSGIntersection;
        std::function<void(std::string)> OnCSGDifference;

        std::function<void(glm::vec3)> OnMoveCSGObject;
        std::function<void(glm::vec3)> OnScaleCSGObject;
        std::function<void(glm::vec3)> OnRotateCSGObject;

        // BREP
        std::function<void(std::string)> OnCreateBREPCube;
        std::function<void(std::string, int)> OnCreateBREPSphere;
        std::function<void(std::string, int)> OnCreateBREPCylinder;
        std::function<void(std::string, int)> OnCreateBREPCone;

        std::function<void(glm::vec3)> OnMoveBREPObject;
        std::function<void(glm::vec3)> OnScaleBREPObject;
        std::function<void(glm::vec3)> OnRotateBREPObject;

        std::function<void(std::string, glm::vec3)> OnMVFS;
        std::function<void(glm::vec3)> OnMEV;
        std::function<void()> OnMEF;
        std::function<void()> OnKVFS;
        std::function<void()> OnKEV;
        std::function<void()> OnKEF;

        std::function<void(std::string fileName)> OnImportBREPObject;
        std::function<void()> PrintWingedEdge;

        // Gerais
        std::function<void(int)> OnSelectObject;
        std::function<void(int)> OnSelectSecondObject;

        std::function<void()> OnDeleteObject;

        std::function<void()> toggleVisibility;
        std::function<void()> toggleWireframe;
        std::function<void()> toggleCell;
        std::function<void()> ResetPosition;
        std::function<void()> ResetRotation;
        std::function<void()> ResetZoom;

        std::function<float()> OnCalculateVolume;
        std::function<float()> OnCalculateArea;

        std::function<void(std::string fileName, int)> OnExportObject;
        std::function<void(std::string fileName)> OnExportLegacyObject;
        std::function<void(std::string fileName)> OnImportObject;
        std::function<void(std::string fileName)> OnImportCSGObject;
        std::function<void(std::string fileName)> OnImportLegacyCSGObject;
        std::function<void(std::string fileName, int, int, bool, Ray)> OnRenderObject;

        std::function<void()> OnToggleRenderType;

        void ResetSelection();
        void ResetInputs();
        void ResetTransformValues();

        void DrawCameraGizmo(const Camera&, int, int);

        ToolType getSelectedTool();
        void resetSelectedTool();

        void LoadTextures(Renderer&);
    private:
        void RenderCollectionToolsSection();

        void RenderToolsSection();
        void RenderObjectListSection(const std::vector<std::string>&);
        void RenderDetailsSection(const std::vector<std::string>&, std::vector<Vertex*>);

        ObjectToolCollection selectCollection = ObjectToolCollection::BREP;
        ToolType selectedTool = ToolType::None;

        int selectedObjectIndex = -1;
        int secondSelectedObjectIndex = -1;

        float lastResult = 0.0f;
        bool hasResult  = false;
        // defaults
        float defaultLastResult = 0.0f;
        bool defaultHasResult  = false;

        float scale = 1.0f;
        float translation = 0.0f;
        Axys selectedAxys = Axys::X;
        bool useGlobalTransform = false;
        glm::vec3 csgTransformValues = {0.0f, 0.0f, 0.0f};
        glm::vec3 brepTransformValues = {0.0f, 0.0f, 0.0f};
        // defaults
        float defaultScale = 1.0f;
        float defaultTranslation = 0.0f;

        // primitivas
        int maxLevel = DefaultMaxLevel;
        bool useDynamicLevel = false;
        std::string label = "";
        float rotationX = 0.0f;
        float rotationY = 0.0f;
        float rotationZ = 0.0f;
        // defaults
        int defaultMaxLevel = DefaultMaxLevel;
        bool defaultUseDynamicLevel = false;
        std::string defaultLabel = "";
        float defaultRotationX = 0.0f;
        float defaultRotationY = 0.0f;
        float defaultRotationZ = 0.0f;

        // Resolucao das primitivas BRep
        int brepResolution = 32;
        // default
        int defaultBrepResolution = 32;

        // esfera
        float sphereRadius = 1.0f;
        // defaults
        float defaultSphereRadius = 1.0f;

        // cubo
        float cubeSizeX = 1.0f;
        float cubeSizeY = 1.0f;
        float cubeSizeZ = 1.0f;
        // defaults
        float defaultCubeSizeX = 1.0f;
        float defaultCubeSizeY = 1.0f;
        float defaultCubeSizeZ = 1.0f;

        // cilindro
        float cylinderRadius = 1.0f;
        float cylinderHeight = 1.0f;
        // defaults
        float defaultCylinderRadius = 1.0f;
        float defaultCylinderHeight = 1.0f;

        // cone
        float coneRadius = 1.0f;
        float coneHeight = 1.0f;
        // defaults
        float defaultConeRadius = 1.0f;
        float defaultConeHeight = 1.0f;

        // IN/OUT
        std::string exportFileName = "";
        std::string defaultExportFileName = "";
        std::string selectedImportFile = "";
        std::string defaultSelectedImportFile = "";
        int renderResolutionX = 800;
        int renderResolutionY = 600;
        glm::vec3 debugRayOrigin = {0.0f, 0.0f, 0.0f};
        glm::vec3 debugRayDirection = {0.0f, 0.0f, 0.0f};
        bool useDebugRay = false;

        // Texturas de botoes
        GLuint areaBtn;
        GLuint cameraBtn;
        GLuint celulasBtn;
        GLuint cilindroBtn;
        GLuint coneBtn;
        GLuint cuboBtn;
        GLuint deletarBtn;
        GLuint diferencaBtn;
        GLuint escalarBtn;
        GLuint esferaBtn;
        GLuint exportarBtn;
        GLuint importarBtn;
        GLuint intersecaoBtn;
        GLuint kefBtn;
        GLuint kevBtn;
        GLuint kvfsBtn;
        GLuint mevBtn;
        GLuint mefBtn;
        GLuint moverBtn;
        GLuint mvfsBtn;
        GLuint ocultarBtn;
        GLuint printBtn;
        GLuint raycastBtn;
        GLuint renderBtn;
        GLuint rotacionarBtn;
        GLuint selectBtn;
        GLuint uniaoBtn;
        GLuint volumeBtn;
        GLuint wireframeBtn;
};