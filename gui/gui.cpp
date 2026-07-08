#include "gui.h"
#include "globals.h"
#include "GraphData.h"
#include <imgui.h>
#include <implot.h>
#include <iostream>
#include <random>

void initTestTrees() {
    testTrees.clear();

    testTrees.push_back(std::vector<Edge>{ 
        Edge{0, 1, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{0, 2, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{0, 3, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{0, 4, static_cast<double>(1 + std::rand() % 20)} 
    });
    testTrees.push_back(std::vector<Edge>{ 
        Edge{0, 1, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{1, 2, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{2, 3, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{3, 4, static_cast<double>(1 + std::rand() % 20)} 
    });
    testTrees.push_back(std::vector<Edge>{ 
        Edge{0, 1, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{0, 3, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{1, 2, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{2, 4, static_cast<double>(1 + std::rand() % 20)} 
    });
    testTrees.push_back(std::vector<Edge>{ 
        Edge{0, 2, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{1, 3, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{2, 4, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{3, 4, static_cast<double>(1 + std::rand() % 20)} 
    });
    testTrees.push_back(std::vector<Edge>{ 
        Edge{0, 1, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{1, 3, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{2, 3, static_cast<double>(1 + std::rand() % 20)}, 
        Edge{3, 4, static_cast<double>(1 + std::rand() % 20)} 
    });
}

void initTestGraph() {
    currentGraph.vertexCount = 5;
    currentGraph.edges.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 20);
    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
            Edge e;
            e.from = i;
            e.to = j;
            e.weight = static_cast<double>(dis(gen));
            currentGraph.edges.push_back(e);
        }
    }
    vertexCount = 5;
    edgeCount = (int)currentGraph.edges.size();
}

void drawGraphVisualization() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), 
                      IM_COL32(80, 80, 80, 255), 4.0f);

    int n = currentGraph.vertexCount;
    if (n == 0) {
        drawList->AddText(ImVec2(canvasPos.x + 20, canvasPos.y + 20), 
                          IM_COL32(200, 200, 200, 255), 
                          "Загрузите или сгенерируйте граф");
        return;
    }

    const float PI = 3.14159265359f;
    ImVec2 center = ImVec2(canvasPos.x + canvasSize.x * 0.5f, canvasPos.y + canvasSize.y * 0.5f);
    float radius = std::min(canvasSize.x, canvasSize.y) * 0.35f;

    std::vector<ImVec2> positions(n);
    for (int i = 0; i < n; ++i) {
        float angle = 2.0f * PI * i / n - PI / 2.0f;
        positions[i] = ImVec2(center.x + radius * cosf(angle), center.y + radius * sinf(angle));
    }

    // Рисует основной граф сереньким
    for (const auto& edge : currentGraph.edges) {
        if (edge.from < n && edge.to < n) {
            drawList->AddLine(positions[edge.from], positions[edge.to], 
                              IM_COL32(150, 150, 150, 255), 1.5f);
        }
    }

    // Рисует выбранное дерево с обозначением весов
    if (!testTrees.empty() && selectedTreeIndex >= 0 && selectedTreeIndex < (int)testTrees.size()) {
        const auto& tree = testTrees[selectedTreeIndex];
        double totalWeight = 0;         // Коля везде где эта переменная меняется заменяй ее на ту что в структуре особи(дерева)
        for (const auto& edge : tree) {
            if (edge.from < n && edge.to < n) {
                totalWeight+=edge.weight;
                ImVec2 p1 = positions[edge.from];
                ImVec2 p2 = positions[edge.to];
                drawList->AddLine(p1, p2, IM_COL32(255, 50, 50, 255), 4.0f);
                // Подпись веса
                ImVec2 mid = ImVec2((p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f);
                std::string weightText = std::to_string(edge.weight);
                drawList->AddText(mid, IM_COL32(255, 255, 255, 255), weightText.c_str());
            }
        }
        // Инфа по дереву
        if (!testTrees.empty() && selectedTreeIndex < (int)testTrees.size()) {
            std::string info = "Дерево " + std::to_string(selectedTreeIndex + 1) + 
                            " (рёбер: " + std::to_string(testTrees[selectedTreeIndex].size()) + ")" +
                            " Вес дерева = " + std::to_string(totalWeight);
            drawList->AddText(ImVec2(canvasPos.x + 10, canvasPos.y + canvasSize.y - 30), 
                            IM_COL32(255, 255, 255, 255), info.c_str());
        }
    }

    // Рисует вершинки
    for (int i = 0; i < n; ++i) {
        drawList->AddCircleFilled(positions[i], 20.0f, IM_COL32(70, 150, 255, 255));
        drawList->AddText(ImVec2(positions[i].x - 8.0f, positions[i].y - 10.0f), 
                          IM_COL32(255, 255, 255, 255), std::to_string(i).c_str());
    }

}

void DrawControlPanel()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_Always);
    ImGui::Begin("Управление", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::Text("Данные графа");
    ImGui::Separator();
    if (ImGui::Button("Загрузить из файла")) {
        std::string path = "./data/graph.txt";
        if (loadFromFile(path, currentGraph)) {
            vertexCount = currentGraph.vertexCount;
            edgeCount = (int)currentGraph.edges.size();
            selectedEdges.clear();
            weightHistory.clear();
            bestChromosome = "{}";
            bestWeight = 0.0;
            bestFitness = 0.0;
            currentGeneration = 0;
            statusMessage = "Граф загружен! Вершин: " + std::to_string(vertexCount) + ", рёбер: " + std::to_string(edgeCount);
        } else {
            statusMessage = "Ошибка загрузки файла! Проверьте путь и формат.";
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Случайная генерация")) {
        generateRandomGraphAuto(currentGraph);
        vertexCount = currentGraph.vertexCount;
        edgeCount = (int)currentGraph.edges.size();
        selectedEdges.clear();
        weightHistory.clear();
        bestChromosome = "{}";
        bestWeight = 0.0;
        bestFitness = 0.0;
        currentGeneration = 0;
        statusMessage = "Случайный граф: " + std::to_string(vertexCount) + " вершин, " + std::to_string(edgeCount) + " рёбер";
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Параметры ГА");
    ImGui::Separator();
    ImGui::PushItemWidth(100);
    ImGui::InputInt("Размер популяции", &populationSize);
    ImGui::InputInt("Размер турнира", &tournamentSize);
    ImGui::SliderFloat("Вероятность скрещивания", &crossoverProb, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Вероятность мутации", &mutationProb, 0.0f, 1.0f, "%.2f");
    ImGui::InputInt("Макс. поколений", &maxGenerations);
    ImGui::InputInt("Лимит без улучшений", &noImprovementLimit);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Управление");
    ImGui::Separator();

    if (!isAlgorithmRunning && !isAlgorithmFinished) {
        if (ImGui::Button("Запустить алгоритм")) {
            isAlgorithmRunning = true;
            isAlgorithmPaused = false;
            isAlgorithmFinished = false;
            currentGeneration = 0;
            bestFitness = 0.0;
            bestWeight = 0.0;
            weightHistory.clear();
            statusMessage = "Алгоритм запущен (заглушка)";
        }
    } else {
        if (isAlgorithmRunning && !isAlgorithmPaused) {
            if (ImGui::Button("Пауза")) {
                isAlgorithmPaused = true;
                statusMessage = "Алгоритм на паузе";
            }
        } else if (isAlgorithmRunning && isAlgorithmPaused) {
            if (ImGui::Button("Продолжить")) {
                isAlgorithmPaused = false;
                statusMessage = "Алгоритм продолжен";
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Один шаг")) {
            // Логику шага вынесем в GA, но пока оставим заглушку
            if (isAlgorithmRunning && !isAlgorithmFinished) {
                currentGeneration++;
                double newWeight = 50 + rand() % 50;
                bestWeight = newWeight;
                bestFitness = 100.0 - newWeight;
                weightHistory.push_back((double)newWeight);
                bestChromosome = "{";
                for (int i = 0; i < vertexCount - 1; i++) {
                    bestChromosome += std::to_string(rand() % edgeCount) + (i < vertexCount - 2 ? ", " : "");
                }
                bestChromosome += "}";
                if (currentGeneration >= maxGenerations || weightHistory.size() > 10) {
                    isAlgorithmFinished = true;
                    statusMessage = "Алгоритм завершён по критерию остановки";
                } else {
                    statusMessage = "Шаг " + std::to_string(currentGeneration) + " выполнен";
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Завершить")) {
            if (isAlgorithmRunning) {
                isAlgorithmFinished = true;
                statusMessage = "Алгоритм завершён досрочно";
            }
        }
        if (ImGui::Button("Сброс")) {
            isAlgorithmRunning = false;
            isAlgorithmPaused = false;
            isAlgorithmFinished = false;
            currentGeneration = 0;
            bestFitness = 0.0;
            bestWeight = 0.0;
            weightHistory.clear();
            statusMessage = "Состояние сброшено";
        }
    }

    ImGui::End();
}

void DrawInfoPanel()
{
    ImGui::SetNextWindowPos(ImVec2(370, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_Always);
    ImGui::Begin("Информация", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::Text("Вершин: %d", vertexCount);
    ImGui::Text("Поколение: %d", currentGeneration);
    ImGui::Text("Лучшая особь: %s", bestChromosome.c_str());
    ImGui::Separator();
    ImGui::Text("Рёбер: %d", edgeCount);
    ImGui::Text("Лучший вес: %.2f", bestWeight);
    ImGui::Text("Статус: %s", statusMessage.c_str());
    ImGui::Separator();
    ImGui::Text("Популяция: %d", populationSize);
    ImGui::Text("Лучшая МОД по весу: %.2f", bestFitness);
    ImGui::Separator();
    if (isAlgorithmRunning) {
        ImGui::Text("Состояние: %s", isAlgorithmPaused ? "Пауза" : "Выполнение");
    } else if (isAlgorithmFinished) {
        ImGui::Text("Состояние: Завершён");
    } else {
        ImGui::Text("Состояние: Ожидание");
    }
    ImGui::Separator();

    ImGui::Text("Ручное добавление ребра:");
    static int v1 = 0, v2 = 1, weight = 1;
    ImGui::PushItemWidth(150);
    ImGui::InputInt("Вершина 1", &v1);
    ImGui::InputInt("Вершина 2", &v2);
    ImGui::InputInt("Вес", &weight);
    if (ImGui::Button("Добавить ребро")) {
        if (v1 != v2 && weight > 0) {
            statusMessage = "Ребро добавлено (заглушка)";
            edgeCount++;
        } else {
            statusMessage = "Некорректные данные ребра";
        }
    }

    ImGui::End();
}

void DrawVisualizationWindow()
{
    ImGui::SetNextWindowPos(ImVec2(730, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(660, 880), ImGuiCond_Always);
    ImGui::Begin("Визуализация графа", nullptr, ImGuiWindowFlags_NoResize);
    
    // Выбор номера особи (1-based)
    int totalTrees = (int)testTrees.size();
    if (totalTrees > 0) {
        int displayIndex = selectedTreeIndex + 1; // преобразуем в 1-based
        ImGui::InputInt("Номер особи (1..N)", &displayIndex);
        // Коррекция диапазона
        if (displayIndex < 1) displayIndex = 1;
        if (displayIndex > totalTrees) displayIndex = totalTrees;
        selectedTreeIndex = displayIndex - 1; // обратно в 0-based
        ImGui::Text("Всего деревьев: %d", totalTrees);
    } else {
        ImGui::Text("Нет деревьев для отображения");
        selectedTreeIndex = 0;
    }

    drawGraphVisualization();

    ImGui::End();
}

void DrawEvolutionPlot()
{
    ImGui::SetNextWindowPos(ImVec2(10, 420), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(710, 470), ImGuiCond_Always);
    ImGui::Begin("График эволюции (вес МОД по поколениям)", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::Text("Лучший вес на каждом поколении");
    if (!weightHistory.empty()) {
        if (ImPlot::BeginPlot("##График", ImVec2(680, 410))) {
            ImPlot::SetupAxis(ImAxis_X1, "Поколение");
            ImPlot::SetupAxis(ImAxis_Y1, "Вес");
            ImPlot::PlotLine("Вес МОД", weightHistory.data(), weightHistory.size());
            ImPlot::PlotScatter("Вес МОД", weightHistory.data(), weightHistory.size());
            ImPlot::EndPlot();
        }
    } else {
        ImGui::Text("Нет данных для отображения");
    }

    ImGui::End();
}