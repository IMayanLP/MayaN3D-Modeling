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
    selectedVertices = {nullptr, nullptr};
}

void Scene::setSecondSelectedObject(int index) {
    secondSelectedObject = index;
}

void Scene::resetSelectedIndex() {
    secondSelectedObject = -1;
    selectedVertices = {nullptr, nullptr};
}

void Scene::resetSecondSelectedIndex() {
    secondSelectedObject = -1;
    selectedVertices = {nullptr, nullptr};
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

BRep& Scene::getSelectedBRep() {
    return objects.at(selectedObject).brep;
}

BRep& Scene::getSecondSelectedBRep() {
    return objects.at(secondSelectedObject).brep;
}

Object& Scene::getSelectedObject() {
    return objects.at(selectedObject);
}

Object& Scene::getSecondSelectedObject() {
    return objects.at(secondSelectedObject);
}

std::vector<Object>& Scene::getObjects() {
    return objects;
}

bool Scene::hasAnObjectSelected() {
    return selectedObject < 0 ? false : true;
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

void Scene::addBRep(BRep&& brep) {
    Object object;
    object.type = ObjectType::BRep;
    object.brep = std::move(brep);

    std::vector<Triangle> mesh = object.brep.TriangulateMesh();
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

void Scene::RenderScene(Renderer& renderer, Shader& shader, Camera& camera, RayCastTexture& rayCastTexture, RayCast& rayCast) {
    if (renderType == RenderType::RayCast) {
        int renderWidth;
        int renderHeight;

        if (state == SceneState::Static) {
            renderWidth = 320;
            renderHeight = 225;
        } else {
            renderWidth = 200;
            renderHeight = 150;
        }

        rayCastTexture.Resize(renderWidth, renderHeight);

        Ray debugRay({0, 0, 0}, {0, 0, 1});

        std::vector<unsigned char> pixels = rayCast.Render(objects, camera, renderWidth, renderHeight, {0.0f, 0.8f, 1.0f}, false, debugRay);

        rayCastTexture.Update(pixels);
    } else {
        for (Object& o : objects) {
            if (!o.visible) continue;
            shader.SetVec3("objectColor", o.color);

            if (o.type == ObjectType::Octree || o.type == ObjectType::CSG) {
                if (state == SceneState::Static) {
                    if (!showWireframe) renderer.DrawMesh(o.mesh, shader, o.octree.GetScale(), o.octree.GetTranslation());
                    else renderer.DrawOctree(o.octree.GetRoot(), shader, showWireframe, showCell, 0, o.octree.GetMaxLevel());
                } else renderer.DrawOctree(o.octree.GetRoot(), shader, showWireframe, showCell, 0, maxLevelOnMoving);
            } else if (o.type == ObjectType::BRep) {
                if (showCell) renderer.DrawMesh(o.mesh, shader, 1, {0.0f, 0.0f, 0.0f});
                shader.SetVec3("objectColor", {1.0f, 0.0f, 0.0f});
                for (Vertex* v: selectedVertices) {
                    if (v != nullptr) {
                        renderer.DrawVertex(v, shader);
                    }
                }
                if (showWireframe) {
                    shader.SetVec3("objectColor", {0.0f, 0.0f, 0.0f});
                    if (o.brep.GetEdgesSize() < 1) renderer.DrawBrepVertices(o.brep.GetVertices(), shader);
                    else renderer.DrawBrepWireFrame(o.brep.GetEdges(), shader);
                }
            }
        }
    }
}

std::vector<std::string> Scene::getObjectNames() const {
    std::vector<std::string> objectNames;

    for (const Object& o : objects) {
        std::string objType;
        if (o.type == ObjectType::Octree) objType = "[Octree] ";
        else if (o.type == ObjectType::CSG) objType = "[CSG] ";
        else if (o.type == ObjectType::BRep) objType = "[BRep] ";
        std::string objVisibility = !o.visible ? " <Oculto>" : "";
        std::string label;
        if (o.type == ObjectType::Octree) label = o.octree.getLabel();
        else if (o.type == ObjectType::CSG) label = o.csg.getLabel();
        else if (o.type == ObjectType::BRep) label = o.brep.getLabel();
        if (label.empty()) label = "unnamed";
        objectNames.push_back(objType + label + objVisibility);
    }

    return objectNames;
}

SceneState Scene::getSceneState() const {
    return state;
}

RenderType Scene::getRenderType() const {
    return renderType;
}

void Scene::toggleRenderType() {
    if (renderType == RenderType::Default) renderType = RenderType::RayCast;
    else renderType = RenderType::Default;
}

void Scene::setSelectedVertex(Vertex* v, int index) {
    if (index == 0) selectedVertices[0] = v;
    else if (index == 1) selectedVertices[1] = v;
    else {
        if (selectedVertices[0] == nullptr) selectedVertices[0] = v;
        else selectedVertices[1] = v;
    }
}

void Scene::resetSelectedVertex() {
    selectedVertices = {nullptr, nullptr};
}

std::vector<Vertex*> Scene::getSelectedVertices() {
    return selectedVertices;
}