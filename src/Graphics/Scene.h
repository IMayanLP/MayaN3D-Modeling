#pragma once

#include <glad/glad.h>

#include "../Engine/Octree/Octree.h"
#include "../Engine/Object/Object.h"
#include "Renderer.h"
#include <iostream>

enum class SceneState {
    Static,
    onMoving
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

    void updateOctreeFromSelectedCSG();

    std::size_t getObjectsSize() const;

    void removeSelectedObjects();
    void removeSelectedObject();
    void removeSecondSelectedObject();
    void resetSelectedIndex();
    void resetSecondSelectedIndex();


    void toggleSelectedVisibility();

    void RenderScene(Renderer&, Shader&);
    std::vector<std::string> getObjectNames() const;

private:
    SceneState state = SceneState::Static;
    int maxLevelOnMoving = 4;

    std::vector<Object> objects = {};
    
    bool showWireframe = false;
    bool showCell = true;
    int selectedObject = -1;
    int secondSelectedObject = -1;
};