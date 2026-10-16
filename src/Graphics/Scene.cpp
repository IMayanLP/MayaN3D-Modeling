#include "Scene.h"

Scene::Scene() {}

void Scene::toggleWireFrame() {
    showWireframe = !showWireframe;
}

bool Scene::getShowWireframe() const {
    return showWireframe;
}

void Scene::toggleCell() {
    showCell = !showCell;
}

bool Scene::getShowCell() const {
    return showCell;
}

void Scene::setSelectedOctree(int index) {
    selectedOctree = index;
}

void Scene::setSecondSelectedOctree(int index) {
    secondSelectedOctree = index;
}

void Scene::resetSelectedIndex() {
    secondSelectedOctree = -1;
}

void Scene::resetSecondSelectedIndex() {
    secondSelectedOctree = -1;
}

void Scene::setSceneState(SceneState newState) {
    state = newState;
}

Octree& Scene::getSelectedOctree() {
    return objects.at(selectedOctree).octree;
}

Octree& Scene::getSecondSelectedOctree() {
    return objects.at(secondSelectedOctree).octree;
}

void Scene::addOctree(Octree&& octree) {
    std::vector<Triangle> mesh = octree.GenerateMesh();

    std::cout << "Triangulos gerados: " << mesh.size() << '\n';
    std::cout << "Nivel maximo: " << octree.GetMaxLevel() << '\n';

    Object object;
    object.mesh = std::move(mesh);
    object.octree = std::move(octree);
    objects.push_back(std::move(object));
}

std::size_t Scene::getOctreesSize() const {
    return objects.size();
}

void Scene::removeSelectedOctrees() {
    if (selectedOctree > secondSelectedOctree) std::swap(selectedOctree, secondSelectedOctree);

    objects.erase(objects.begin() + secondSelectedOctree);
    objects.erase(objects.begin() + selectedOctree);

    selectedOctree = -1;
    secondSelectedOctree = -1;
}

void Scene::removeSelectedOctree() {
    objects.erase(objects.begin() + selectedOctree);

    selectedOctree = -1;
}

void Scene::toggleSelectedVisibility() {
    if (selectedOctree > -1) objects[selectedOctree].visible = !objects[selectedOctree].visible;
}

void Scene::RenderScene(Renderer& renderer, Shader& shader) {
    for (Object& o : objects) {
        if (!o.visible) continue;

        if (state == SceneState::Static) {
            if (!showWireframe) renderer.DrawMesh(o.mesh, shader, o.octree.GetScale(), o.octree.GetTranslation());
            else renderer.DrawOctree(o.octree.GetRoot(), shader, showWireframe, showCell, 0, o.octree.GetMaxLevel());
        } else renderer.DrawOctree(o.octree.GetRoot(), shader, showWireframe, showCell, 0, maxLevelOnMoving);
    }
}

std::vector<std::string> Scene::getObjectNames() const {
    std::vector<std::string> objectNames;

    for (const Object& o : objects) {
        std::string label = o.octree.getLabel();
        if (label.empty()) label = "unnamed";
        objectNames.push_back(label);
    }

    return objectNames;
}