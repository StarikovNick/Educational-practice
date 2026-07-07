#include "gui.h"
#include "globals.h"
#include "GraphData.h"
#include <imgui.h>
#include <implot.h>
#include <iostream>
#include <random>

static void DrawGraphVisualization(ImDrawList* drawList, ImVec2 canvasPos, ImVec2 canvasSize)
{
    // Отрисовываем рамку
    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                      IM_COL32(80, 80, 80, 255), 4.0f);

    // Пока рисуем фиксированный граф из 7 вершин (можно позже переделать на currentGraph)
    const int n = 7;
    const float PI = 3.14159265359f;
    ImVec2 center = ImVec2(canvasPos.x + canvasSize.x * 0.5f, canvasPos.y + canvasSize.y * 0.5f);
    float radius = std::min(canvasSize.x, canvasSize.y) * 0.4f;

    std::vector<ImVec2> positions(n);
    for (int i = 0; i < n; ++i) {
        float angle = 2.0f * PI * i / n - PI / 2.0f;
        positions[i] = ImVec2(center.x + radius * cosf(angle), center.y + radius * sinf(angle));
    }

    // Рёбра (пока все возможные, потом можно брать из currentGraph)
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            drawList->AddLine(positions[i], positions[j], IM_COL32(180, 180, 180, 255), 2.0f);
        }
    }

    // Вершины
    for (int i = 0; i < n; ++i) {
        drawList->AddCircleFilled(positions[i], 10.0f, IM_COL32(70, 150, 255, 255));
        ImVec2 textPos = ImVec2(positions[i].x - 20.0f, positions[i].y - 10.0f);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), std::to_string(i).c_str());
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
                weightHistory.push_back((float)newWeight);
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
    ImGui::SetNextWindowSize(ImVec2(650, 880), ImGuiCond_Always);
    ImGui::Begin("Визуализация графа", nullptr, ImGuiWindowFlags_NoResize);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    DrawGraphVisualization(drawList, canvasPos, canvasSize);

    ImGui::End();
}

void DrawEvolutionPlot()
{
    ImGui::SetNextWindowPos(ImVec2(10, 420), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(710, 480), ImGuiCond_Always);
    ImGui::Begin("График эволюции (вес МОД по поколениям)", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::Text("Лучший вес на каждом поколении");
    if (!weightHistory.empty()) {
        if (ImPlot::BeginPlot("##График", ImVec2(550, 380))) {
            ImPlot::SetupAxis(ImAxis_X1, "Поколение");
            ImPlot::SetupAxis(ImAxis_Y1, "Вес");
            ImPlot::PlotLine("Вес МОД", weightHistory.data(), (int)weightHistory.size());
            ImPlot::PlotScatter("Вес МОД", weightHistory.data(), (int)weightHistory.size());
            ImPlot::EndPlot();
        }
    } else {
        ImGui::Text("Нет данных для отображения");
    }

    ImGui::End();
}