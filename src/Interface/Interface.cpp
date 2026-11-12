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

    ResetTransformValues();
}

void Interface::ResetTransformValues() {
    if (selectedTool == ToolType::Scale) csgTransformValues = {1.0f, 1.0f, 1.0f};
    else csgTransformValues = {0.0f, 0.0f, 0.0f};
}

void Interface::Render(float navBarSize, const std::vector<std::string>& objectNames) {
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
    RenderDetailsSection(objectNames);

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
    const float spacing    = ImGui::GetStyle().ItemSpacing.x;
    const float availWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 buttonSize((availWidth - spacing * 3.0f) / 4.0f, 40.0f);

    if (selectCollection == ObjectToolCollection::Octree) {
        // primitivas
        if (ImGui::Button("Esfera", buttonSize)) selectedTool = ToolType::Sphere;
        ImGui::SameLine();
        if (ImGui::Button("Cubo", buttonSize)) selectedTool = ToolType::Cube;
        ImGui::SameLine();
        if (ImGui::Button("Cone", buttonSize)) selectedTool = ToolType::Cone;
        ImGui::SameLine();
        if (ImGui::Button("Cilindro", buttonSize)) selectedTool = ToolType::Cylinder;

        // operações
        if (ImGui::Button("Uniao", buttonSize)) selectedTool = ToolType::Union;
        ImGui::SameLine();
        if (ImGui::Button("Intersec.", buttonSize)) selectedTool = ToolType::Intersection;
        ImGui::SameLine();
        if (ImGui::Button("Volume", buttonSize)) selectedTool = ToolType::Volume;
        ImGui::SameLine();
        if (ImGui::Button("Area", buttonSize)) selectedTool = ToolType::Area;

        // ferramentas
        if (ImGui::Button("Mover", buttonSize)) selectedTool = ToolType::Move;
        ImGui::SameLine();
        if (ImGui::Button("Escalar", buttonSize)) selectedTool = ToolType::Scale;
        ImGui::SameLine();
        if (ImGui::Button("Exportar", buttonSize)) selectedTool = ToolType::Export;
        ImGui::SameLine();
        if (ImGui::Button("Importar", buttonSize)) selectedTool = ToolType::Import;

        // visualização
        if (ImGui::Button("Wireframe", buttonSize) && toggleWireframe) toggleWireframe();
        ImGui::SameLine();
        if (ImGui::Button("Celulas", buttonSize) && toggleCell) toggleCell();
        ImGui::SameLine();
        if (ImGui::Button("RayCast", buttonSize) && OnToggleRenderType) OnToggleRenderType();
    } else if (selectCollection == ObjectToolCollection::CSG) {
        // primitivas
        if (ImGui::Button("Esfera", buttonSize)) selectedTool = ToolType::Sphere;
        ImGui::SameLine();
        if (ImGui::Button("Cubo", buttonSize)) selectedTool = ToolType::Cube;
        ImGui::SameLine();
        if (ImGui::Button("Cone", buttonSize)) selectedTool = ToolType::Cone;
        ImGui::SameLine();
        if (ImGui::Button("Cilindro", buttonSize)) selectedTool = ToolType::Cylinder;

        // operações
        if (ImGui::Button("Uniao", buttonSize)) selectedTool = ToolType::Union;
        ImGui::SameLine();
        if (ImGui::Button("Intersec.", buttonSize)) selectedTool = ToolType::Intersection;
        ImGui::SameLine();
        if (ImGui::Button("Dif.", buttonSize)) selectedTool = ToolType::Difference;

        if (ImGui::Button("Volume", buttonSize)) selectedTool = ToolType::Volume;
        ImGui::SameLine();
        if (ImGui::Button("Area", buttonSize)) selectedTool = ToolType::Area;

        // ferramentas
        if (ImGui::Button("Mover", buttonSize)) {selectedTool = ToolType::Move; ResetTransformValues();}
        ImGui::SameLine();
        if (ImGui::Button("Escalar", buttonSize)) {selectedTool = ToolType::Scale; ResetTransformValues();}
        ImGui::SameLine();
        if (ImGui::Button("Rotac.", buttonSize)) {selectedTool = ToolType::Rotate; ResetTransformValues();}

        if (ImGui::Button("Exportar", buttonSize)) selectedTool = ToolType::Export;
        ImGui::SameLine();
        if (ImGui::Button("Importar", buttonSize)) selectedTool = ToolType::Import;
        ImGui::SameLine();
        if (ImGui::Button("Render", buttonSize)) selectedTool = ToolType::Render;
        ImGui::SameLine();
        if (ImGui::Button("RayCast", buttonSize) && OnToggleRenderType) OnToggleRenderType();
    } else if (selectCollection == ObjectToolCollection::BREP) {
        ImGui::Text("Em breve");
    }

    if (ImGui::Button("Camera", buttonSize)) selectedTool = ToolType::Camera;
    ImGui::SameLine();
    // extras
    if (ImGui::Button("Ocultar", buttonSize) && toggleVisibility) toggleVisibility();
    ImGui::SameLine();
    if (ImGui::Button("Deletar", buttonSize) && selectedObjectIndex >= 0 && OnDeleteObject) OnDeleteObject();
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

void Interface::RenderDetailsSection(const std::vector<std::string>& objectNames) {
    ImGui::Text("DETALHES");

    switch (selectedTool) {
        case ToolType::Sphere:
            ImGui::InputText("Nome", &label);
            if (selectCollection == ObjectToolCollection::Octree) {
                ImGui::InputFloat("Raio", &sphereRadius);
                ImGui::InputInt("MaxLevel", &maxLevel);
            }
            if (ImGui::Button("Aplicar") && OnCreateSphere && OnCreateCSGSphere) {
                if (selectCollection == ObjectToolCollection::Octree) OnCreateSphere(label, sphereRadius, maxLevel);
                else if (selectCollection == ObjectToolCollection::CSG) OnCreateCSGSphere(label);
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

            if (ImGui::Button("Aplicar") && OnCreateCube && OnCreateCSGCube) {
                if (selectCollection == ObjectToolCollection::Octree) OnCreateCube(label, cubeSizeX, cubeSizeY, cubeSizeZ, maxLevel, rotationX, rotationY, rotationZ);
                else if (selectCollection == ObjectToolCollection::CSG) OnCreateCSGCube(label);
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

            if (ImGui::Button("Aplicar") && OnCreateCylinder && OnCreateCSGCylinder) {
                if (selectCollection == ObjectToolCollection::Octree) OnCreateCylinder(label, cylinderRadius, cylinderHeight, maxLevel, rotationX, rotationY, rotationZ);
                else if (selectCollection == ObjectToolCollection::CSG) OnCreateCSGCylinder(label);
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

            if (ImGui::Button("Aplicar") && OnCreateCone && OnCreateCSGCone) {
                if (selectCollection == ObjectToolCollection::Octree) OnCreateCone(label, coneRadius, coneHeight, maxLevel, rotationX, rotationY, rotationZ);
                else if (selectCollection == ObjectToolCollection::CSG) OnCreateCSGCone(label);

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
                }

                if (ImGui::Button("Aplicar") && OnMoveObject && OnMoveCSGObject) {
                    if (selectCollection == ObjectToolCollection::Octree) OnMoveObject(selectedAxys, translation, useGlobalTransform);
                    else if (selectCollection == ObjectToolCollection::CSG) OnMoveCSGObject(csgTransformValues);
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
                }

                if (ImGui::Button("Aplicar") && OnScaleObject && OnScaleCSGObject) {
                    if (selectCollection == ObjectToolCollection::Octree) OnScaleObject(scale, useGlobalTransform);
                    else if (selectCollection == ObjectToolCollection::CSG) OnScaleCSGObject(csgTransformValues);
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

                    if (ImGui::Button("Aplicar") && OnScaleObject && OnScaleCSGObject) {
                        OnRotateCSGObject(csgTransformValues);
                        ResetInputs();
                    }
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
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);
                ImGui::InputText("Nome", &exportFileName);

                bool validName = !exportFileName.empty();
                if (!validName) ImGui::TextDisabled("Digite um nome de arquivo.");

                ImGui::BeginDisabled(!validName);
                if (ImGui::Button("Aplicar") && OnExportObject) {
                    OnExportObject(exportFileName);
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
            break;

        case ToolType::Import: {
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
            if (ImGui::Button("Aplicar") && OnImportObject && OnImportCSGObject) {
                if (selectCollection == ObjectToolCollection::Octree) OnImportObject(selectedImportFile);
                if (selectCollection == ObjectToolCollection::CSG) OnImportCSGObject(selectedImportFile);
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
        }

        case ToolType::None:
        default:
            ImGui::TextDisabled("Nenhuma ferramenta selecionada.");
            break;
    }
}