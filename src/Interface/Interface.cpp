#include "Interface.h"
#include "misc/cpp/imgui_stdlib.h"
#include <filesystem>
#include <fstream>

ImGuiWindowFlags flags =
    ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoBackground |
    ImGuiWindowFlags_NoNavInputs |
    ImGuiWindowFlags_NoNavFocus;

void Interface::ResetSelection() {
    selectedObjectIndex = -1;
    secondSelectedObjectIndex = -1;
}

void Interface::ResetInputs() {
    lastResult = defaultLastResult;
    hasResult = defaultHasResult;

    scale = defaultScale;
    translation = defaultTranslation;

    maxLevel = defaultMaxLevel;
    useDynamicLevel = defaultUseDynamicLevel;
    label = defaultLabel;

    rotationX = defaultRotationX;
    rotationY = defaultRotationY;
    rotationZ = defaultRotationZ;

    sphereRadius = defaultSphereRadius;

    cubeSizeX = defaultCubeSizeX;
    cubeSizeY = defaultCubeSizeY;
    cubeSizeZ = defaultCubeSizeZ;

    cylinderRadius = defaultCylinderRadius;
    cylinderHeight = defaultCylinderHeight;

    coneRadius = defaultConeRadius;
    coneHeight = defaultConeHeight;

    exportFileName = defaultExportFileName;
    selectedImportFile = defaultSelectedImportFile;

    renderResolutionX = 800;
    renderResolutionY = 600;

    debugRayOrigin = {0.0f, 0.0f, 0.0f};
    debugRayDirection = {0.0f, 0.0f, 0.0f};
    useDebugRay = false;

    brepResolution = defaultBrepResolution;

    ResetTransformValues();
}

void Interface::ResetTransformValues() {
    if (selectedTool == ToolType::Scale) {
        csgTransformValues = {1.0f, 1.0f, 1.0f};
        brepTransformValues = {1.0f, 1.0f, 1.0f};
    }
    else {
        csgTransformValues = {0.0f, 0.0f, 0.0f};
        brepTransformValues = {0.0f, 0.0f, 0.0f};
    }
}

void Interface::Render(float navBarSize, const std::vector<std::string>& objectNames, std::vector<Vertex*> selectedVertices) {
    ImGuiViewport* mainViewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(mainViewport->WorkPos);
    ImGui::SetNextWindowSize(mainViewport->WorkSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("ToolsWindow", nullptr, flags);
    ImGui::PopStyleVar();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.114f, 0.114f, 0.114f, 1.0f));
    ImGui::BeginChild("ToolsPanel", ImVec2(navBarSize, 0.0f), true);

    ImGui::Text("COLEÇÃO DE FERRAMENTAS");
    ImGui::Separator();

    RenderCollectionToolsSection();
    ImGui::Separator();

    ImGui::Text("FERRAMENTAS");
    ImGui::Separator();

    RenderToolsSection();
    ImGui::Separator();
    RenderObjectListSection(objectNames);
    ImGui::Separator();
    RenderDetailsSection(objectNames, selectedVertices);

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::End();
}

void Interface::RenderCollectionToolsSection() {
    const float spacing    = ImGui::GetStyle().ItemSpacing.x;
    const float availWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 buttonSize((availWidth - spacing * 3.0f) / 4.0f, 40.0f);

    if (ImGui::Button("Octree", buttonSize)) selectCollection = ObjectToolCollection::Octree;
    ImGui::SameLine();
    if (ImGui::Button("CSG", buttonSize)) selectCollection = ObjectToolCollection::CSG;
    ImGui::SameLine();
    if (ImGui::Button("BRep", buttonSize)) selectCollection = ObjectToolCollection::BREP;

}


void Interface::RenderToolsSection() {
    if (selectCollection == ObjectToolCollection::Octree) {
        // primitivas
        if (ImGui::ImageButton("Cubo", (ImTextureID)(intptr_t)cuboBtn, ImVec2(24, 24))) selectedTool = ToolType::Cube;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cubo");

        ImGui::SameLine();
        if (ImGui::ImageButton("Esfera", (ImTextureID)(intptr_t)esferaBtn, ImVec2(24, 24))) selectedTool = ToolType::Sphere;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Esfera");

        ImGui::SameLine();
        if (ImGui::ImageButton("Cone", (ImTextureID)(intptr_t)coneBtn, ImVec2(24, 24))) selectedTool = ToolType::Cone;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cone");

        ImGui::SameLine();
        if (ImGui::ImageButton("Cilindro", (ImTextureID)(intptr_t)cilindroBtn, ImVec2(24, 24))) selectedTool = ToolType::Cylinder;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cilindro");

        // operações
        if (ImGui::ImageButton("Uniao", (ImTextureID)(intptr_t)uniaoBtn, ImVec2(24, 24))) selectedTool = ToolType::Union;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("União");

        ImGui::SameLine();
        if (ImGui::ImageButton("Intersec", (ImTextureID)(intptr_t)intersecaoBtn, ImVec2(24, 24))) selectedTool = ToolType::Intersection;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Interseção");

        // ferramentas
        ImGui::SameLine();
        if (ImGui::ImageButton("Mover", (ImTextureID)(intptr_t)moverBtn, ImVec2(24, 24))) {selectedTool = ToolType::Move;}
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Mover");

        ImGui::SameLine();
        if (ImGui::ImageButton("Escalar", (ImTextureID)(intptr_t)escalarBtn, ImVec2(24, 24))) {selectedTool = ToolType::Scale;}
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Escalar");

        ImGui::SameLine();
        if (ImGui::ImageButton("Volume", (ImTextureID)(intptr_t)volumeBtn, ImVec2(24, 24))) selectedTool = ToolType::Volume;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Volume");

        ImGui::SameLine();
        if (ImGui::ImageButton("Area", (ImTextureID)(intptr_t)areaBtn, ImVec2(24, 24))) selectedTool = ToolType::Area;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Área");

        // visualização
        if (ImGui::ImageButton("Wireframe", (ImTextureID)(intptr_t)wireframeBtn, ImVec2(24, 24)) && toggleWireframe) toggleWireframe();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Wireframe");

        ImGui::SameLine();
        if (ImGui::ImageButton("Celulas", (ImTextureID)(intptr_t)celulasBtn, ImVec2(24, 24)) && toggleCell) toggleCell();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Células");

        ImGui::SameLine();
        if (ImGui::ImageButton("RayCast", (ImTextureID)(intptr_t)raycastBtn, ImVec2(24, 24)) && OnToggleRenderType) OnToggleRenderType();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("RayCast");

        ImGui::SameLine();
        if (ImGui::ImageButton("Exportar", (ImTextureID)(intptr_t)exportarBtn, ImVec2(24, 24))) selectedTool = ToolType::Export;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Exportar");

        ImGui::SameLine();
        if (ImGui::ImageButton("Importar", (ImTextureID)(intptr_t)importarBtn, ImVec2(24, 24))) selectedTool = ToolType::Import;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Importar");
    } else if (selectCollection == ObjectToolCollection::CSG) {
        // primitivas
        if (ImGui::ImageButton("Cubo", (ImTextureID)(intptr_t)cuboBtn, ImVec2(24, 24))) selectedTool = ToolType::Cube;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cubo");

        ImGui::SameLine();
        if (ImGui::ImageButton("Esfera", (ImTextureID)(intptr_t)esferaBtn, ImVec2(24, 24))) selectedTool = ToolType::Sphere;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Esfera");

        ImGui::SameLine();
        if (ImGui::ImageButton("Cone", (ImTextureID)(intptr_t)coneBtn, ImVec2(24, 24))) selectedTool = ToolType::Cone;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cone");

        ImGui::SameLine();
        if (ImGui::ImageButton("Cilindro", (ImTextureID)(intptr_t)cilindroBtn, ImVec2(24, 24))) selectedTool = ToolType::Cylinder;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cilindro");

        // operações
        if (ImGui::ImageButton("Uniao", (ImTextureID)(intptr_t)uniaoBtn, ImVec2(24, 24))) selectedTool = ToolType::Union;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("União");

        ImGui::SameLine();
        if (ImGui::ImageButton("Intersec", (ImTextureID)(intptr_t)intersecaoBtn, ImVec2(24, 24))) selectedTool = ToolType::Intersection;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Interseção");

        ImGui::SameLine();
        if (ImGui::ImageButton("Dif.", (ImTextureID)(intptr_t)diferencaBtn, ImVec2(24, 24))) selectedTool = ToolType::Difference;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Diferença");

        // ferramentas
        ImGui::SameLine();
        if (ImGui::ImageButton("Mover", (ImTextureID)(intptr_t)moverBtn, ImVec2(24, 24))) {selectedTool = ToolType::Move; ResetTransformValues();}
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Mover");

        ImGui::SameLine();
        if (ImGui::ImageButton("Escalar", (ImTextureID)(intptr_t)escalarBtn, ImVec2(24, 24))) {selectedTool = ToolType::Scale; ResetTransformValues();}
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Escalar");

        ImGui::SameLine();
        if (ImGui::ImageButton("Rotacionar", (ImTextureID)(intptr_t)rotacionarBtn, ImVec2(24, 24))) {selectedTool = ToolType::Rotate; ResetTransformValues();}
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rotacionar");

        ImGui::SameLine();
        if (ImGui::ImageButton("Volume", (ImTextureID)(intptr_t)volumeBtn, ImVec2(24, 24))) selectedTool = ToolType::Volume;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Volume");

        ImGui::SameLine();
        if (ImGui::ImageButton("Area", (ImTextureID)(intptr_t)areaBtn, ImVec2(24, 24))) selectedTool = ToolType::Area;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Área");

        if (ImGui::ImageButton("RayCast", (ImTextureID)(intptr_t)raycastBtn, ImVec2(24, 24)) && OnToggleRenderType) OnToggleRenderType();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("RayCast");

        ImGui::SameLine();
        if (ImGui::ImageButton("Render", (ImTextureID)(intptr_t)renderBtn, ImVec2(24, 24))) selectedTool = ToolType::Render;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Render");

        ImGui::SameLine();
        if (ImGui::ImageButton("Exportar", (ImTextureID)(intptr_t)exportarBtn, ImVec2(24, 24))) selectedTool = ToolType::Export;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Exportar");

        ImGui::SameLine();
        if (ImGui::ImageButton("Importar", (ImTextureID)(intptr_t)importarBtn, ImVec2(24, 24))) selectedTool = ToolType::Import;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Importar");
    } else if (selectCollection == ObjectToolCollection::BREP) {
        if (ImGui::ImageButton("Vertice", (ImTextureID)(intptr_t)selectBtn, ImVec2(24, 24))) selectedTool = ToolType::SelectVertex;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Selecionar vértice");

        ImGui::SameLine();
        if (ImGui::ImageButton("MVFS", (ImTextureID)(intptr_t)mvfsBtn, ImVec2(24, 24))) selectedTool = ToolType::MVFS;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("MVFS");

        ImGui::SameLine();
        if (ImGui::ImageButton("KVFS", (ImTextureID)(intptr_t)kvfsBtn, ImVec2(24, 24))) selectedTool = ToolType::KVFS;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("KVFS");

        ImGui::SameLine();
        if (ImGui::ImageButton("MEV", (ImTextureID)(intptr_t)mevBtn, ImVec2(24, 24))) selectedTool = ToolType::MEV;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("MEV");

        ImGui::SameLine();
        if (ImGui::ImageButton("KEV", (ImTextureID)(intptr_t)kevBtn, ImVec2(24, 24))) selectedTool = ToolType::KEV;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("KEV");

        ImGui::SameLine();
        if (ImGui::ImageButton("MEF", (ImTextureID)(intptr_t)mefBtn, ImVec2(24, 24))) selectedTool = ToolType::MEF;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("MEF");

        ImGui::SameLine();
        if (ImGui::ImageButton("KEF", (ImTextureID)(intptr_t)kefBtn, ImVec2(24, 24))) selectedTool = ToolType::KEF;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("KEF");

        // primitivas
        if (ImGui::ImageButton("Cubo", (ImTextureID)(intptr_t)cuboBtn, ImVec2(24, 24))) selectedTool = ToolType::Cube;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cubo");

        ImGui::SameLine();
        if (ImGui::ImageButton("Esfera", (ImTextureID)(intptr_t)esferaBtn, ImVec2(24, 24))) selectedTool = ToolType::Sphere;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Esfera");

        ImGui::SameLine();
        if (ImGui::ImageButton("Cone", (ImTextureID)(intptr_t)coneBtn, ImVec2(24, 24))) selectedTool = ToolType::Cone;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cone");

        ImGui::SameLine();
        if (ImGui::ImageButton("Cilindro", (ImTextureID)(intptr_t)cilindroBtn, ImVec2(24, 24))) selectedTool = ToolType::Cylinder;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cilindro");

        // ferramentas
        if (ImGui::ImageButton("Mover", (ImTextureID)(intptr_t)moverBtn, ImVec2(24, 24))) {selectedTool = ToolType::Move; ResetTransformValues();}
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Mover");

        ImGui::SameLine();
        if (ImGui::ImageButton("Escalar", (ImTextureID)(intptr_t)escalarBtn, ImVec2(24, 24))) {selectedTool = ToolType::Scale; ResetTransformValues();}
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Escalar");

        ImGui::SameLine();
        if (ImGui::ImageButton("Rotacionar", (ImTextureID)(intptr_t)rotacionarBtn, ImVec2(24, 24))) {selectedTool = ToolType::Rotate; ResetTransformValues();}
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rotacionar");

        // operações
        ImGui::SameLine();
        if (ImGui::ImageButton("Volume", (ImTextureID)(intptr_t)volumeBtn, ImVec2(24, 24))) selectedTool = ToolType::Volume;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Volume");

        ImGui::SameLine();
        if (ImGui::ImageButton("Area", (ImTextureID)(intptr_t)areaBtn, ImVec2(24, 24))) selectedTool = ToolType::Area;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Área");

        if (ImGui::ImageButton("Wireframe", (ImTextureID)(intptr_t)wireframeBtn, ImVec2(24, 24)) && toggleWireframe) toggleWireframe();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Wireframe");

        ImGui::SameLine();
        if (ImGui::ImageButton("Celulas", (ImTextureID)(intptr_t)celulasBtn, ImVec2(24, 24)) && toggleCell) toggleCell();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Células");

        ImGui::SameLine();
        if (ImGui::ImageButton("RayCast", (ImTextureID)(intptr_t)raycastBtn, ImVec2(24, 24)) && OnToggleRenderType) OnToggleRenderType();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("RayCast");

        ImGui::SameLine();
        if (ImGui::ImageButton("Exportar", (ImTextureID)(intptr_t)exportarBtn, ImVec2(24, 24))) selectedTool = ToolType::Export;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Exportar");

        ImGui::SameLine();
        if (ImGui::ImageButton("Importar", (ImTextureID)(intptr_t)importarBtn, ImVec2(24, 24))) selectedTool = ToolType::Import;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Importar");

        ImGui::SameLine();
        if (ImGui::ImageButton("Print", (ImTextureID)(intptr_t)printBtn, ImVec2(24, 24)) && PrintWingedEdge) PrintWingedEdge();
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Ver no console");
    }

    // extras
    if (ImGui::ImageButton("Camera", (ImTextureID)(intptr_t)cameraBtn, ImVec2(24, 24))) selectedTool = ToolType::Camera;
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Camera");

    ImGui::SameLine();
    if (ImGui::ImageButton("Ocultar", (ImTextureID)(intptr_t)ocultarBtn, ImVec2(24, 24))
        && toggleVisibility) toggleVisibility();
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Ocultar");

    ImGui::SameLine();
    if (ImGui::ImageButton("Deletar", (ImTextureID)(intptr_t)deletarBtn, ImVec2(24, 24))
        && selectedObjectIndex >= 0 && OnDeleteObject ) OnDeleteObject();
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Deletar");
}

void Interface::RenderObjectListSection(const std::vector<std::string>& objectNames) {
    ImGui::Text("OBJETOS");

    ImGui::BeginChild("ObjectList", ImVec2(0.0f, -320.0f), false);
    for (int i = 0; i < static_cast<int>(objectNames.size()); ++i) {
        bool isSelected = (selectedObjectIndex == i);
        if (ImGui::Selectable(objectNames[i].c_str(), isSelected)) {
            selectedObjectIndex = i;
            if (OnSelectObject) {
                OnSelectObject(i);
            }
            ResetInputs();
        }
    }
    ImGui::EndChild();
}

void Interface::RenderDetailsSection(const std::vector<std::string>& objectNames, std::vector<Vertex*> selectedVertices) {
    ImGui::Text("DETALHES");

    switch (selectedTool) {
        case ToolType::Sphere:
            ImGui::InputText("Nome", &label);
            if (selectCollection == ObjectToolCollection::Octree) {
                ImGui::InputFloat("Raio", &sphereRadius);
                ImGui::InputInt("MaxLevel", &maxLevel);
            }

            if (selectCollection == ObjectToolCollection::BREP) {
                ImGui::InputInt("Resolução", &brepResolution);
            }

            if (ImGui::Button("Aplicar") && OnCreateSphere && OnCreateCSGSphere && OnCreateBREPSphere) {
                if (selectCollection == ObjectToolCollection::Octree) OnCreateSphere(label, sphereRadius, maxLevel);
                else if (selectCollection == ObjectToolCollection::CSG) OnCreateCSGSphere(label);
                else if (selectCollection == ObjectToolCollection::BREP) OnCreateBREPSphere(label, brepResolution);
                ResetInputs();
            }
            break;

        case ToolType::Cube:
            ImGui::InputText("Nome", &label);
            if (selectCollection == ObjectToolCollection::Octree) {
                ImGui::InputFloat("Tamanho X", &cubeSizeX);
                ImGui::InputFloat("Tamanho Y", &cubeSizeY);
                ImGui::InputFloat("Tamanho Z", &cubeSizeZ);
                ImGui::InputInt("MaxLevel", &maxLevel);

                ImGui::Separator();
                ImGui::Text("Rotação da primitiva (em graus):");
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("X", &rotationX);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("Y", &rotationY);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("Z", &rotationZ);
            }

            if (ImGui::Button("Aplicar") && OnCreateCube && OnCreateCSGCube && OnCreateBREPCube) {
                if (selectCollection == ObjectToolCollection::Octree) OnCreateCube(label, cubeSizeX, cubeSizeY, cubeSizeZ, maxLevel, rotationX, rotationY, rotationZ);
                else if (selectCollection == ObjectToolCollection::CSG) OnCreateCSGCube(label);
                else if (selectCollection == ObjectToolCollection::BREP) OnCreateBREPCube(label);
                ResetInputs();
            }
            break;

        case ToolType::Cylinder:
            ImGui::InputText("Nome", &label);
            if (selectCollection == ObjectToolCollection::Octree) {
                ImGui::InputFloat("Raio", &cylinderRadius);
                ImGui::InputFloat("Altura", &cylinderHeight);
                ImGui::InputInt("MaxLevel", &maxLevel);
                ImGui::Separator();
                ImGui::Text("Rotação da primitiva (em graus):");
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("X", &rotationX);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("Y", &rotationY);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("Z", &rotationZ);
            }

            if (selectCollection == ObjectToolCollection::BREP) {
                ImGui::InputInt("Resolução", &brepResolution);
            }

            if (ImGui::Button("Aplicar") && OnCreateCylinder && OnCreateCSGCylinder && OnCreateBREPCylinder) {
                if (selectCollection == ObjectToolCollection::Octree) OnCreateCylinder(label, cylinderRadius, cylinderHeight, maxLevel, rotationX, rotationY, rotationZ);
                else if (selectCollection == ObjectToolCollection::CSG) OnCreateCSGCylinder(label);
                else if (selectCollection == ObjectToolCollection::BREP) OnCreateBREPCylinder(label, brepResolution);
                ResetInputs();
            }
            break;

        case ToolType::Cone:
            ImGui::InputText("Nome", &label);
            if (selectCollection == ObjectToolCollection::Octree) {
                ImGui::InputFloat("Raio", &coneRadius);
                ImGui::InputFloat("Altura", &coneHeight);
                ImGui::InputInt("MaxLevel", &maxLevel);
                ImGui::Separator();
                ImGui::Text("Rotação da primitiva (em graus):");
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("X", &rotationX);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("Y", &rotationY);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(60.0f);
                ImGui::InputFloat("Z", &rotationZ);
            }

            if (selectCollection == ObjectToolCollection::BREP) {
                ImGui::InputInt("Resolução", &brepResolution);
            }

            if (ImGui::Button("Aplicar") && OnCreateCone && OnCreateCSGCone) {
                if (selectCollection == ObjectToolCollection::Octree) OnCreateCone(label, coneRadius, coneHeight, maxLevel, rotationX, rotationY, rotationZ);
                else if (selectCollection == ObjectToolCollection::CSG) OnCreateCSGCone(label);
                else if (selectCollection == ObjectToolCollection::BREP) OnCreateBREPCone(label, brepResolution);

                ResetInputs();
            }
            break;

        case ToolType::Union:
        case ToolType::Intersection:
        case ToolType::Difference: {
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione o objeto A na lista acima.");
                break;
            }

            ImGui::BeginChild("Booleans", ImVec2(0.0f, -120.0f), false);
            for (int i = 0; i < static_cast<int>(objectNames.size()); ++i) {
                if (i != selectedObjectIndex) {
                    bool isSelected = (secondSelectedObjectIndex == i);
                    if (ImGui::Selectable(objectNames[i].c_str(), isSelected)) {
                        secondSelectedObjectIndex = i;
                        if (OnSelectSecondObject) {
                            OnSelectSecondObject(i);
                        }
                    }
                }
            }
            ImGui::EndChild();

            bool validSelection = secondSelectedObjectIndex >= 0;

            if (!validSelection) {
                ImGui::TextDisabled("Selecione outro objeto.");
            }

            ImGui::BeginDisabled(!validSelection);
            ImGui::InputText("Nome", &label);

            if (selectCollection == ObjectToolCollection::Octree) {
                ImGui::Checkbox("Nivel dinamico", &useDynamicLevel);
                ImGui::InputInt("MaxLevel", &maxLevel);
            }

            if (ImGui::Button("Aplicar")) {
                if (selectedTool == ToolType::Union && OnUnion && OnCSGUnion) {
                    if (selectCollection == ObjectToolCollection::Octree) OnUnion(label, maxLevel, useDynamicLevel);
                    else if (selectCollection == ObjectToolCollection::CSG) OnCSGUnion(label);
                    ResetInputs();
                } else if (selectedTool == ToolType::Intersection && OnIntersection && OnCSGIntersection) {
                    if (selectCollection == ObjectToolCollection::Octree) OnIntersection(label, maxLevel, useDynamicLevel);
                    else if (selectCollection == ObjectToolCollection::CSG) OnCSGIntersection(label);
                    ResetInputs();
                } else if (selectedTool == ToolType::Difference && OnCSGDifference) {
                    if (selectCollection == ObjectToolCollection::CSG) OnCSGDifference(label);
                    ResetInputs();
                }
            }
            ImGui::EndDisabled();
            break;
        }

        case ToolType::Volume:
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);
                if (ImGui::Button("Aplicar") && OnCalculateVolume) {
                    lastResult = OnCalculateVolume();
                    hasResult = true;
                }
                if (hasResult) {
                    ImGui::Text("Volume: %.4f", lastResult);
                }
            }
            break;

        case ToolType::Area:
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);
                if (ImGui::Button("Aplicar") && OnCalculateArea) {
                    lastResult = OnCalculateArea();
                    hasResult = true;
                }
                if (hasResult) {
                    ImGui::Text("Area: %.4f", lastResult);
                }
            }
            break;

        case ToolType::Move:
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);

                if (selectCollection == ObjectToolCollection::Octree) {
                    const float spacing = ImGui::GetStyle().ItemSpacing.x;
                    const ImVec2 axisButtonSize((ImGui::GetContentRegionAvail().x - spacing * 2.0f) / 3.0f, 0.0f);

                    auto axisButton = [&](const char* label, Axys axis) {
                        bool isSelected = (selectedAxys == axis);
                        if (isSelected) {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
                        }
                        if (ImGui::Button(label, axisButtonSize)) {
                            selectedAxys = axis;
                        }
                        if (isSelected) {
                            ImGui::PopStyleColor();
                        }
                    };

                    axisButton("X", Axys::X);
                    ImGui::SameLine();
                    axisButton("Y", Axys::Y);
                    ImGui::SameLine();
                    axisButton("Z", Axys::Z);

                    // ImGui::SliderFloat("Valor", &translation, -10.0f, 10.0f);
                    ImGui::SliderFloat("##slider", &translation, -10.0f, 10.0f);
                    ImGui::SameLine();
                    ImGui::InputFloat("Valor", &translation);

                    ImGui::Checkbox("Usar transformação global", &useGlobalTransform);
                    // ImGui::Text("");
                } else if (selectCollection == ObjectToolCollection::CSG) {
                    ImGui::InputFloat("X", &csgTransformValues.x);
                    ImGui::InputFloat("Y", &csgTransformValues.y);
                    ImGui::InputFloat("Z", &csgTransformValues.z);
                } else if (selectCollection == ObjectToolCollection::BREP) {
                    ImGui::InputFloat("X", &brepTransformValues.x);
                    ImGui::InputFloat("Y", &brepTransformValues.y);
                    ImGui::InputFloat("Z", &brepTransformValues.z);
                }

                if (ImGui::Button("Aplicar") && OnMoveObject && OnMoveCSGObject && OnMoveBREPObject) {
                    if (selectCollection == ObjectToolCollection::Octree) OnMoveObject(selectedAxys, translation, useGlobalTransform);
                    else if (selectCollection == ObjectToolCollection::CSG) OnMoveCSGObject(csgTransformValues);
                    else if (selectCollection == ObjectToolCollection::BREP) OnMoveBREPObject(brepTransformValues);
                    ResetInputs();
                }
            }
            break;

        case ToolType::Scale:
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);

                if (selectCollection == ObjectToolCollection::Octree) {
                    ImGui::SliderFloat("##slider", &scale, 0.1f, 10.0f);
                    ImGui::SameLine();
                    ImGui::InputFloat("Valor", &scale);

                    ImGui::Checkbox("Usar transformação global", &useGlobalTransform);
                } else if (selectCollection == ObjectToolCollection::CSG) {
                    ImGui::InputFloat("X", &csgTransformValues.x);
                    ImGui::InputFloat("Y", &csgTransformValues.y);
                    ImGui::InputFloat("Z", &csgTransformValues.z);
                } else if (selectCollection == ObjectToolCollection::BREP) {
                    ImGui::InputFloat("X", &brepTransformValues.x);
                    ImGui::InputFloat("Y", &brepTransformValues.y);
                    ImGui::InputFloat("Z", &brepTransformValues.z);
                }

                if (ImGui::Button("Aplicar") && OnScaleObject && OnScaleCSGObject && OnScaleBREPObject) {
                    if (selectCollection == ObjectToolCollection::Octree) OnScaleObject(scale, useGlobalTransform);
                    else if (selectCollection == ObjectToolCollection::CSG) OnScaleCSGObject(csgTransformValues);
                    else if (selectCollection == ObjectToolCollection::BREP) OnScaleBREPObject(brepTransformValues);
                    ResetInputs();
                }
            }
            break;

        case ToolType::Rotate:
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);

                if (selectCollection == ObjectToolCollection::CSG) {
                    ImGui::InputFloat("X", &csgTransformValues.x);
                    ImGui::InputFloat("Y", &csgTransformValues.y);
                    ImGui::InputFloat("Z", &csgTransformValues.z);
                } else if (selectCollection == ObjectToolCollection::BREP) {
                    ImGui::InputFloat("X", &brepTransformValues.x);
                    ImGui::InputFloat("Y", &brepTransformValues.y);
                    ImGui::InputFloat("Z", &brepTransformValues.z);
                }

                if (ImGui::Button("Aplicar") && OnScaleObject && OnScaleCSGObject) {
                    if (selectCollection == ObjectToolCollection::CSG) OnRotateCSGObject(csgTransformValues);
                    else if (selectCollection == ObjectToolCollection::BREP) OnRotateBREPObject(brepTransformValues);
                    ResetInputs();
                }

            }
            break;

        case ToolType::Camera:
            ImGui::Text("Movimentação:");
            ImGui::Text("Setas do teclado e PgUp/PgDown");
            ImGui::Text("Ctrl + Mouse para girar");
            ImGui::Text("Roda do mouse para zoom");

            ImGui::Text("\nResetar:");
            if (ImGui::Button("Posição", {96.0f, 0.0f}) && ResetPosition) ResetPosition();
            ImGui::SameLine();
            if (ImGui::Button("Rotação", {96.0f, 0.0f}) && ResetRotation) ResetRotation();
            ImGui::SameLine();
            if (ImGui::Button("Zoom", {96.0f, 0.0f}) && ResetZoom) ResetZoom();
            break;

        case ToolType::Render:
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);
                ImGui::InputText("Nome", &exportFileName);

                bool validName = !exportFileName.empty();
                if (!validName) ImGui::TextDisabled("Digite um nome de arquivo.");

                ImGui::BeginDisabled(!validName);

                ImGui::InputInt("Res X", &renderResolutionX);
                ImGui::InputInt("Res Y", &renderResolutionY);

                ImGui::Text("Presets");
                if (ImGui::Button("Baixa", {96.0f, 0.0f})) {
                    renderResolutionX = 800;
                    renderResolutionY = 600;
                }
                ImGui::SameLine();
                if (ImGui::Button("Média", {96.0f, 0.0f})) {
                    renderResolutionX = 1280;
                    renderResolutionY = 960;
                }
                ImGui::SameLine();
                if (ImGui::Button("Alta", {96.0f, 0.0f})) {
                    renderResolutionX = 1440;
                    renderResolutionY = 1080;
                }

                ImGui::Text("Raio de Debug");
                ImGui::Checkbox("Usar raio de debug", &useDebugRay);

                if (useDebugRay) {
                    ImGui::Text("Origem (Ponto)");
                    ImGui::SetNextItemWidth(70.0f);
                    ImGui::InputFloat("O.X", &debugRayOrigin.x);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(70.0f);
                    ImGui::InputFloat("O.Y", &debugRayOrigin.y);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(70.0f);
                    ImGui::InputFloat("O.Z", &debugRayOrigin.z);

                    ImGui::Text("Direção (Vetor)");
                    ImGui::SetNextItemWidth(70.0f);
                    ImGui::InputFloat("D.X", &debugRayDirection.x);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(70.0f);
                    ImGui::InputFloat("D.Y", &debugRayDirection.y);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(70.0f);
                    ImGui::InputFloat("D.Z", &debugRayDirection.z);
                }

                if (ImGui::Button("Aplicar") && OnRenderObject) {
                    Ray debugRay(debugRayOrigin, debugRayDirection);
                    OnRenderObject(exportFileName, renderResolutionX, renderResolutionY, useDebugRay, debugRay);
                    ResetInputs();
                }
                ImGui::EndDisabled();
            }
            break;


        case ToolType::Export:
            if (selectCollection == ObjectToolCollection::BREP) {
                ImGui::Text("Para BRep todos os objetos visiveis");
                ImGui::Text("são exportados para um .obj");
                ImGui::InputText("Nome", &exportFileName);

                bool validName = !exportFileName.empty();
                if (!validName) ImGui::TextDisabled("Digite um nome de arquivo.");

                ImGui::BeginDisabled(!validName);
                if (ImGui::Button("Aplicar") && OnExportObject) {
                    OnExportObject(exportFileName, 1);
                    ResetInputs();
                }
                ImGui::EndDisabled();
            } else {
                if (selectedObjectIndex < 0) {
                    ImGui::Text("Selecione um objeto na lista acima.");
                } else {
                    ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);
                    ImGui::InputText("Nome", &exportFileName);

                    bool validName = !exportFileName.empty();
                    if (!validName) ImGui::TextDisabled("Digite um nome de arquivo.");

                    ImGui::BeginDisabled(!validName);
                    if (ImGui::Button("Aplicar") && OnExportObject) {
                        OnExportObject(exportFileName, 0);
                        ResetInputs();
                    }
                    if (selectCollection == ObjectToolCollection::CSG) {
                        ImGui::SameLine();
                        if (ImGui::Button("Aplicar (Legado)") && OnExportLegacyObject) {
                            OnExportLegacyObject(exportFileName);
                            ResetInputs();
                        }
                    }
                    ImGui::EndDisabled();
                }
            }
            break;

        case ToolType::Import:
            ImGui::Text("Arquivos em files/input/");

            ImGui::BeginChild("files", ImVec2(0.0f, -80.0f), false);

            if (std::filesystem::exists("files/input")) {
                for (const auto& entry : std::filesystem::directory_iterator("files/input")) {
                    if (!entry.is_regular_file()) continue;

                    std::string fileName = entry.path().filename().string();
                    bool isSelected = (selectedImportFile == fileName);

                    if (ImGui::Selectable(fileName.c_str(), isSelected)) {
                        selectedImportFile = fileName;
                    }
                }
            } else {
                ImGui::TextDisabled("Pasta files/input nao encontrada.");
            }

            ImGui::EndChild();

            ImGui::BeginDisabled(selectedImportFile.empty());
            if (ImGui::Button("Aplicar") && OnImportObject && OnImportCSGObject && OnImportBREPObject) {
                if (selectCollection == ObjectToolCollection::Octree) OnImportObject(selectedImportFile);
                if (selectCollection == ObjectToolCollection::CSG) OnImportCSGObject(selectedImportFile);
                if (selectCollection == ObjectToolCollection::BREP) OnImportBREPObject(selectedImportFile);
                ResetInputs();
            }
            if (selectCollection == ObjectToolCollection::CSG) {
                ImGui::SameLine();
                if (ImGui::Button("Aplicar (Legado)") && OnImportLegacyCSGObject) {
                    OnImportLegacyCSGObject(selectedImportFile);
                    ResetInputs();
                }
            }
            ImGui::EndDisabled();

            break;

        case ToolType::MVFS:
            if (selectCollection == ObjectToolCollection::BREP) {
                ImGui::InputText("Nome", &label);
                ImGui::Text("Vértice inicial:");
                ImGui::InputFloat("X", &brepTransformValues.x);
                ImGui::InputFloat("Y", &brepTransformValues.y);
                ImGui::InputFloat("Z", &brepTransformValues.z);

                if (ImGui::Button("Aplicar") && OnMVFS) {
                    OnMVFS(label, brepTransformValues);
                    ResetInputs();
                }
            }
            break;

        case ToolType::KVFS:
            if (selectCollection == ObjectToolCollection::BREP) {
                if (selectedVertices[0] != nullptr) {
                    ImGui::Text("Vértice selecionado 1: %d", selectedVertices[0]->id);

                    ImGui::Text("Destruir objeto?");

                    if (ImGui::Button("Aplicar") && OnKVFS) {
                        OnKVFS();
                        ResetInputs();
                    }
                }
                else ImGui::Text("Selecione um vértice");
            }
            break;

        case ToolType::MEV:
            if (selectCollection == ObjectToolCollection::BREP) {
                if (selectedVertices[0] != nullptr) {
                    ImGui::Text("Vértice selecionado 1: %d", selectedVertices[0]->id);

                    ImGui::Text("Novo vértice:");
                    ImGui::InputFloat("X", &brepTransformValues.x);
                    ImGui::InputFloat("Y", &brepTransformValues.y);
                    ImGui::InputFloat("Z", &brepTransformValues.z);

                    if (ImGui::Button("Aplicar") && OnMEV) {
                        OnMEV(brepTransformValues);
                        ResetInputs();
                    }
                }
                else ImGui::Text("Selecione um vértice");
            }
            break;

        case ToolType::KEV:
            if (selectCollection == ObjectToolCollection::BREP) {
                if (selectedVertices[0] != nullptr) {
                    ImGui::Text("Vértice selecionado 1: %d", selectedVertices[0]->id);

                    ImGui::Text("Destruir a vértice e sua aresta?");

                    if (ImGui::Button("Aplicar") && OnKEV) {
                        OnKEV();
                        ResetInputs();
                    }
                } else ImGui::Text("Selecione um vértice");
            }
            break;

        case ToolType::MEF:
            if (selectCollection == ObjectToolCollection::BREP) {
                if (selectedVertices[0] != nullptr && selectedVertices[1] != nullptr) {
                    ImGui::Text("Vértice selecionado 1: %d", selectedVertices[0]->id);
                    ImGui::Text("Vértice selecionado 2: %d", selectedVertices[1]->id);

                    if (ImGui::Button("Aplicar") && OnMEF) {
                        OnMEF();
                        ResetInputs();
                    }
                } else ImGui::Text("Selecione dois vértices");
            }
            break;

        case ToolType::KEF:
            if (selectCollection == ObjectToolCollection::BREP) {
                if (selectedVertices[0] != nullptr && selectedVertices[1] != nullptr) {
                    ImGui::Text("Vértice selecionado 1: %d", selectedVertices[0]->id);
                    ImGui::Text("Vértice selecionado 2: %d", selectedVertices[1]->id);

                    ImGui::Text("Destruir a aresta entre esses vertices?");

                    if (ImGui::Button("Aplicar") && OnKEF) {
                        OnKEF();
                        ResetInputs();
                    }
                } else ImGui::Text("Selecione dois vértices");
            }
            break;

        case ToolType::SelectVertex:
            if (selectCollection == ObjectToolCollection::BREP) {
                ImGui::Text("Ferramenta de seleção de vértice");
                ImGui::Text("Clique na tela para selecionar");
                ImGui::Text("");
                ImGui::Text("Mouse Esq. p/ seleção principal");
                ImGui::Text("Mouse Esq. p/ seleção secundaria");

                if (selectedObjectIndex < 0) ImGui::Text("Selecione um objeto na lista acima.");
                else ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);
            }
            break;

        case ToolType::None:
        default:
            ImGui::TextDisabled("Nenhuma ferramenta selecionada.");
            break;
    }
}


void Interface::DrawCameraGizmo(const Camera& camera, int navBarSize, int viewportWidth) {
    const float gizmoSize = 80.0f;

    ImGui::SetNextWindowPos(ImVec2(navBarSize + viewportWidth - gizmoSize, 0), ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(gizmoSize, gizmoSize), ImGuiCond_Always);

    ImGui::Begin("CameraGizmo", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoScrollbar
    );

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    const ImVec2 center(ImGui::GetWindowPos().x + gizmoSize * 0.5f, ImGui::GetWindowPos().y + gizmoSize * 0.5f);

    glm::mat3 viewRotation = glm::mat3(camera.GetViewMatrix());

    const glm::vec3 axes[3] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};
    const ImU32 colors[3] = {IM_COL32(255, 70, 70, 255), IM_COL32(70, 220, 70, 255), IM_COL32(70, 140, 255, 255)};

    for (int i = 0; i < 3; i++) {
        glm::vec3 direction = viewRotation * axes[i];

        ImVec2 endpoint(center.x + direction.x * 23.0f, center.y - direction.y * 23.0f);
        glm::vec2 arrowDirection = glm::normalize(glm::vec2(endpoint.x - center.x, endpoint.y - center.y));

        ImVec2 lineEnd(endpoint.x - arrowDirection.x * 2.0f, endpoint.y - arrowDirection.y * 2.0f);

        drawList->AddLine(center, lineEnd, colors[i], 3.0f);

        glm::vec2 perpendicular(-arrowDirection.y, arrowDirection.x);

        ImVec2 tip = endpoint;
        ImVec2 base(endpoint.x - arrowDirection.x * 9.0f, endpoint.y - arrowDirection.y * 9.0f);

        drawList->AddTriangleFilled(tip,
            ImVec2(base.x + perpendicular.x * 4.0f, base.y + perpendicular.y * 4.0f),
            ImVec2(base.x - perpendicular.x * 4.0f, base.y - perpendicular.y * 4.0f),
            colors[i]
        );
    }

    ImGui::End();
}

ToolType Interface::getSelectedTool() {
    return selectedTool;
}

void Interface::resetSelectedTool() {
    selectedTool = ToolType::None;
}

void Interface::LoadTextures(Renderer& renderer) {
    areaBtn  = renderer.LoadTexture("assets/area.png");
    cameraBtn  = renderer.LoadTexture("assets/camera.png");
    celulasBtn  = renderer.LoadTexture("assets/celulas.png");
    cilindroBtn  = renderer.LoadTexture("assets/cilindro.png");
    coneBtn  = renderer.LoadTexture("assets/cone.png");
    cuboBtn  = renderer.LoadTexture("assets/cubo.png");
    deletarBtn  = renderer.LoadTexture("assets/deletar.png");
    diferencaBtn  = renderer.LoadTexture("assets/diferenca.png");
    escalarBtn  = renderer.LoadTexture("assets/escalar.png");
    esferaBtn  = renderer.LoadTexture("assets/esfera.png");
    exportarBtn  = renderer.LoadTexture("assets/exportar.png");
    importarBtn  = renderer.LoadTexture("assets/importar.png");
    intersecaoBtn  = renderer.LoadTexture("assets/intersecao.png");
    kefBtn  = renderer.LoadTexture("assets/kef.png");
    kevBtn  = renderer.LoadTexture("assets/kev.png");
    kvfsBtn  = renderer.LoadTexture("assets/kvfs.png");
    mevBtn  = renderer.LoadTexture("assets/mev.png");
    mefBtn  = renderer.LoadTexture("assets/mef.png");
    moverBtn  = renderer.LoadTexture("assets/mover.png");
    mvfsBtn  = renderer.LoadTexture("assets/mvfs.png");
    ocultarBtn  = renderer.LoadTexture("assets/ocultar.png");
    printBtn  = renderer.LoadTexture("assets/print.png");
    raycastBtn  = renderer.LoadTexture("assets/raycast.png");
    renderBtn  = renderer.LoadTexture("assets/render.png");
    rotacionarBtn  = renderer.LoadTexture("assets/rotacionar.png");
    selectBtn  = renderer.LoadTexture("assets/select.png");
    uniaoBtn  = renderer.LoadTexture("assets/uniao.png");
    volumeBtn  = renderer.LoadTexture("assets/volume.png");
    wireframeBtn  = renderer.LoadTexture("assets/wireframe.png");
}