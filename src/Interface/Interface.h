#pragma once

#include <imgui.h>
#include <vector>
#include <string>
#include <functional>

#include "Octree.h"

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
    Camera,
    Import,
    Export
};

enum class Axys {
    X,
    Y,
    Z
};

class Interface {
    public:
        void Render(float, const std::vector<std::string>&);

        // callbacks
        std::function<void(std::string, float, int)> OnCreateSphere;
        std::function<void(std::string, float, float, float, int, float, float, float)> OnCreateCube;
        std::function<void(std::string, float, float, int, float, float, float)> OnCreateCylinder;
        std::function<void(std::string, float, float, int, float, float, float)> OnCreateCone;

        std::function<void(int)> OnSelectObject;
        std::function<void(int)> OnSelectSecondObject;

        std::function<void()> OnDeleteObject;

        std::function<void()> toggleVisibility;
        std::function<void()> toggleWireframe;
        std::function<void()> toggleCell;
        std::function<void()> ResetPosition;
        std::function<void()> ResetRotation;
        std::function<void()> ResetZoom;

        std::function<void(Axys ,float, bool)> OnMoveObject;
        std::function<void(float, bool)> OnScaleObject;
        std::function<float()> OnCalculateVolume;
        std::function<float()> OnCalculateArea;

        std::function<void(std::string, int, bool)> OnUnion;
        std::function<void(std::string, int, bool)> OnIntersection;

        std::function<void(std::string fileName)> OnExportObject;
        std::function<void(std::string fileName)> OnImportObject;

        void ResetSelection();
        void ResetInputs();

    private:
        void RenderToolsSection(float);
        void RenderObjectListSection(const std::vector<std::string>&);
        void RenderDetailsSection(const std::vector<std::string>&);

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
};