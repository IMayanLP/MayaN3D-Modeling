#pragma once

#include <glad/glad.h>

#include "../Engine/Octree/Octree.h"
#include "../Engine/Object/Object.h"
#include "../Ray/RayCast.h"
#include "../Ray/RayCastTexture.h"
#include "Renderer.h"
#include <iostream>

enum class SceneState {
    Static,
    onMoving
};

enum class RenderType {
    Default,
    RayCast
};

class Scene {
public:
    Scene();

    void toggleWireFrame();
    bool getShowWireframe() const;
    void toggleCell();
    bool getShowCell() const;

    void setSelectedObject(int);
    void setSecondSelectedObject(int);
    void setSceneState(SceneState);

    ObjectType getSelectedObjectType() const;
    ObjectType getSecondSelectedObjectType() const;

    Octree& getSelectedOctree();
    Octree& getSecondSelectedOctree();
    void addOctree(Octree&&);

    CSG& getSelectedCSG();
    CSG& getSecondSelectedCSG();
    void addCSG(CSG&&);

    Object& getSelectedObject();
    Object& getSecondSelectedObject();

    std::vector<Object>& getObjects();

    void updateOctreeFromSelectedCSG();

    std::size_t getObjectsSize() const;

    void removeSelectedObjects();
    void removeSelectedObject();
    void removeSecondSelectedObject();
    void resetSelectedIndex();
    void resetSecondSelectedIndex();

    void toggleSelectedVisibility();

    void RenderScene(Renderer&, Shader&, Camera&, RayCastTexture&, RayCast&);
    std::vector<std::string> getObjectNames() const;

    SceneState getSceneState() const;
    RenderType getRenderType() const;
    void toggleRenderType();

private:
    SceneState state = SceneState::Static;
    RenderType renderType = RenderType::Default;
    int maxLevelOnMoving = 4;

    std::vector<Object> objects = {};
    
    bool showWireframe = false;
    bool showCell = true;
    int selectedObject = -1;
    int secondSelectedObject = -1;
};