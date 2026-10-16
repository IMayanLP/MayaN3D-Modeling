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

    ImGui::Text("FERRAMENTAS");
    ImGui::Separator();

    RenderToolsSection(navBarSize);
    ImGui::Separator();
    RenderObjectListSection(objectNames);
    ImGui::Separator();
    RenderDetailsSection(objectNames);

    ImGui::EndChild();
    ImGui::PopStyleColor();

    ImGui::End();
}

void Interface::RenderToolsSection(float navBarSize) {
    const float spacing    = ImGui::GetStyle().ItemSpacing.x;
    const float availWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 buttonSize((availWidth - spacing * 3.0f) / 4.0f, 40.0f);

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
    if (ImGui::Button("Camera", buttonSize)) selectedTool = ToolType::Camera;
    ImGui::SameLine();

    ImGui::Text("");

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
            ImGui::InputFloat("Raio", &sphereRadius);
            ImGui::InputInt("MaxLevel", &maxLevel);
            if (ImGui::Button("Aplicar") && OnCreateSphere) {
                OnCreateSphere(label, sphereRadius, maxLevel);
                ResetInputs();
            }
            break;

        case ToolType::Cube:
            ImGui::InputText("Nome", &label);
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

            if (ImGui::Button("Aplicar") && OnCreateCube) {
                OnCreateCube(label, cubeSizeX, cubeSizeY, cubeSizeZ, maxLevel, rotationX, rotationY, rotationZ);
                ResetInputs();
            }
            break;

        case ToolType::Cylinder:
            ImGui::InputText("Nome", &label);
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

            if (ImGui::Button("Aplicar") && OnCreateCylinder) {
                OnCreateCylinder(label, cylinderRadius, cylinderHeight, maxLevel, rotationX, rotationY, rotationZ);
                ResetInputs();
            }
            break;

        case ToolType::Cone:
            ImGui::InputText("Nome", &label);
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

            if (ImGui::Button("Aplicar") && OnCreateCone) {
                OnCreateCone(label, coneRadius, coneHeight, maxLevel, rotationX, rotationY, rotationZ);
                ResetInputs();
            }
            break;

        case ToolType::Union:
        case ToolType::Intersection: {
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
            ImGui::Checkbox("Nivel dinamico", &useDynamicLevel);
            ImGui::InputInt("MaxLevel", &maxLevel);
            if (ImGui::Button("Aplicar")) {
                if (selectedTool == ToolType::Union && OnUnion) {
                    OnUnion(label, maxLevel, useDynamicLevel);
                    ResetInputs();
                } else if (selectedTool == ToolType::Intersection && OnIntersection) {
                    OnIntersection(label, maxLevel, useDynamicLevel);
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

                if (ImGui::Button("Aplicar") && OnMoveObject) {
                    OnMoveObject(selectedAxys, translation, useGlobalTransform);
                    ResetInputs();
                }
            }
            break;

        case ToolType::Scale:
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);

                ImGui::SliderFloat("##slider", &scale, 0.1f, 10.0f);
                ImGui::SameLine();
                ImGui::InputFloat("Valor", &scale);

                ImGui::Checkbox("Usar transformação global", &useGlobalTransform);

                if (ImGui::Button("Aplicar") && OnScaleObject) {
                    OnScaleObject(scale, useGlobalTransform);
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

        case ToolType::Export:
            if (selectedObjectIndex < 0) {
                ImGui::Text("Selecione um objeto na lista acima.");
            } else {
                ImGui::Text("Objeto selecionado: %d", selectedObjectIndex);
                ImGui::InputText("Nome", &exportFileName);

                bool validName = !exportFileName.empty();
                if (!validName) {
                    ImGui::TextDisabled("Digite um nome de arquivo.");
                }

                ImGui::BeginDisabled(!validName);
                if (ImGui::Button("Aplicar") && OnExportObject) {
                    OnExportObject(exportFileName);
                    ResetInputs();
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
            if (ImGui::Button("Gerar Octree") && OnImportObject) {
                OnImportObject(selectedImportFile);
                ResetInputs();
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