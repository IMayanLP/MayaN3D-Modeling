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

void Scene::setSelectedObject(int index) {
    selectedObject = index;
}

void Scene::setSecondSelectedObject(int index) {
    secondSelectedObject = index;
}

void Scene::resetSelectedIndex() {
    secondSelectedObject = -1;
}

void Scene::resetSecondSelectedIndex() {
    secondSelectedObject = -1;
}

void Scene::setSceneState(SceneState newState) {
    state = newState;
}

ObjectType Scene::getSelectedObjectType() const {
    return objects.at(selectedObject).type;
}

ObjectType Scene::getSecondSelectedObjectType() const {
    return objects.at(secondSelectedObject).type;
}

Octree& Scene::getSelectedOctree() {
    return objects.at(selectedObject).octree;
}

Octree& Scene::getSecondSelectedOctree() {
    return objects.at(secondSelectedObject).octree;
}

CSG& Scene::getSelectedCSG() {
    return objects.at(selectedObject).csg;
}

CSG& Scene::getSecondSelectedCSG() {
    return objects.at(secondSelectedObject).csg;
}

void Scene::addOctree(Octree&& octree) {
    std::vector<Triangle> mesh = octree.GenerateMesh();

    std::cout << "Triangulos gerados: " << mesh.size() << '\n';
    std::cout << "Nivel maximo: " << octree.GetMaxLevel() << '\n';

    Object object;
    object.type = ObjectType::Octree;
    object.mesh = std::move(mesh);
    object.octree = std::move(octree);
    objects.push_back(std::move(object));
}

void Scene::addCSG(CSG&& csg) {
    Object object;
    object.type = ObjectType::CSG;
    object.csg = std::move(csg);

    object.octree = object.csg.GenerateOctree();
    std::vector<Triangle> mesh = object.octree.GenerateMesh();
    object.mesh = std::move(mesh);

    objects.push_back(std::move(object));
}

void Scene::updateOctreeFromSelectedCSG() {
    if (objects[selectedObject].type == ObjectType::CSG) {
        objects[selectedObject].octree = objects[selectedObject].csg.GenerateOctree();
        std::vector<Triangle> mesh = objects[selectedObject].octree.GenerateMesh();
        objects[selectedObject].mesh = std::move(mesh);

    }
}

std::size_t Scene::getObjectsSize() const {
    return objects.size();
}

void Scene::removeSelectedObjects() {
    if (selectedObject > secondSelectedObject) std::swap(selectedObject, secondSelectedObject);

    objects.erase(objects.begin() + secondSelectedObject);
    objects.erase(objects.begin() + selectedObject);

    selectedObject = -1;
    secondSelectedObject = -1;
}

void Scene::removeSelectedObject() {
    objects.erase(objects.begin() + selectedObject);

    selectedObject = -1;
}

void Scene::removeSecondSelectedObject() {
    objects.erase(objects.begin() + secondSelectedObject);

    secondSelectedObject = -1;
}

void Scene::toggleSelectedVisibility() {
    if (selectedObject > -1) objects[selectedObject].visible = !objects[selectedObject].visible;
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
        std::string objType = o.type == ObjectType::Octree ? "[Octree]" : "[CSG]";
        std::string label;
        if (o.type == ObjectType::Octree) label = o.octree.getLabel();
        else if (o.type == ObjectType::CSG) label = o.csg.getLabel();
        if (label.empty()) label = "unnamed";
        objectNames.push_back(objType + " " + label);
    }

    return objectNames;
}