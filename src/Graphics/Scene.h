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

    void setSelectedOctree(int);
    void setSecondSelectedOctree(int);
    void setSceneState(SceneState);

    Octree& getSelectedOctree();
    Octree& getSecondSelectedOctree();
    void addOctree(Octree&&);
    std::size_t getOctreesSize() const;

    void removeSelectedOctrees();
    void removeSelectedOctree();
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
    int selectedOctree = -1;
    int secondSelectedOctree = -1;
};