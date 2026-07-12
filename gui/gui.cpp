#include "gui.h"
#include "globals.h"
#include "GraphData.h"
#include <imgui.h>
#include <implot.h>
#include <iostream>
#include <random>
#include <queue>
#include <GL/gl.h>
#include "stb_image.h"


void prepareAlg(){
        vertexCount = manualVertexCount;
        edgeCount = (int)currentGraph.edges.size();
        selectedEdges.clear();
        weightHistory.clear();
        bestChromosome = "{}";
        bestWeight = 0.0;
        bestFitness = 0.0;
        currentGeneration = 0;
        isAlgorithmRunning = false;
        isAlgorithmFinished = false;

        ga.setGraph(currentGraph);
        ga.setPopulationSize(populationSize);
        ga.setTournamentSize(tournamentSize);
        ga.setMutationProbability(mutationProb);
        ga.setCrossoverProbability(crossoverProb);
        ga.setMaxGenerations(maxGenerations);
        ga.setMaxStagnation(noImprovementLimit);
        ga.initialize();

        currentGeneration = ga.getCurrentGeneration();
        bestWeight = ga.getBestIndividual().weight;
        bestFitness = ga.getBestIndividual().fitness;
        weightHistory = ga.getFitnessHistory();
        selectedEdges = ga.getBestIndividual().edges;
        const auto& bestInd = ga.getBestIndividual();
}

bool isGraphConnected() {
    int n = manualVertexCount;
    if (n == 0) return false;
    std::vector<bool> visited(n, false);
    std::queue<int> q;
    q.push(0);
    visited[0] = true;
    while (!q.empty()) {
        int v = q.front(); q.pop();
        for (int u = 0; u < n; ++u) {
            if (manualMatrix[v][u] > 0 && !visited[u]) {
                visited[u] = true;
                q.push(u);
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) return false;
    }
    return true;
}

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
    if (showTree && !selectedEdges.empty()) {
        double totalWeight = 0.0;
        for (int idx : selectedEdges) {
            if (idx < 0 || idx >= (int)currentGraph.edges.size()) continue;
            const Edge& edge = currentGraph.edges[idx];
            if (edge.from < n && edge.to < n) {
                totalWeight += edge.weight;
                ImVec2 p1 = positions[edge.from];
                ImVec2 p2 = positions[edge.to];
                drawList->AddLine(p1, p2, IM_COL32(255, 50, 50, 255), 4.0f);
                // Подпись веса
                ImVec2 mid = ImVec2((p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f);
                std::string weightText = std::to_string(edge.weight);
                drawList->AddText(mid, IM_COL32(255, 255, 255, 255), weightText.c_str());
            }
        }
        // Информация
        std::string info = "Выбранное дерево (рёбер: " + std::to_string(selectedEdges.size()) + 
                        ") Вес: " + std::to_string(totalWeight);
        drawList->AddText(ImVec2(canvasPos.x + 10, canvasPos.y + canvasSize.y - 30), 
                        IM_COL32(255, 255, 255, 255), info.c_str());
    } else {
        drawList->AddText(ImVec2(canvasPos.x + 10, canvasPos.y + canvasSize.y - 30),
                        IM_COL32(200, 200, 200, 255),
                        showTree ? "Дерево не выбрано" : "Показ дерева отключён");
    }
    // Рисует вершинки
    for (int i = 0; i < n; ++i) {
        drawList->AddCircleFilled(positions[i], 20.0f, IM_COL32(70, 150, 255, 255));
        drawList->AddText(ImVec2(positions[i].x - 8.0f, positions[i].y - 10.0f), 
                          IM_COL32(255, 255, 255, 255), std::to_string(i).c_str());
    }

}

void DrawControlPanel(){
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(370, 415), ImGuiCond_Always);
    ImGui::Begin("Управление", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::Text("Данные графа");
    ImGui::Separator();
    if (ImGui::Button("Загрузить из файла")) {
        std::string path = "../file/graph.txt";
        if (loadFromFile(path, currentGraph)) {
            // --- Обновляем глобальные переменные ---
            vertexCount = currentGraph.vertexCount;
            edgeCount = (int)currentGraph.edges.size();
            prepareAlg();
            statusMessage = "Граф загружен из файла и ГА инициализирован";
        } else {
            statusMessage = "Ошибка загрузки файла! Проверьте путь и формат.";
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Случайная генерация")) {
        showRandomInput = true;
    }
    if (ImGui::Button("Ручной ввод")) {
        showManualInput = true;
        manualVertexCount = 5;
        memset(manualMatrix, 0, sizeof(manualMatrix));
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
    // Кнопка "Запустить алгоритм" – полный прогон
        if (ImGui::Button("Запуск")) {
            if (currentGraph.vertexCount > 0) {
                // Если алгоритм не инициализирован, инициализируем
                if (ga.getCurrentPopulation().empty()) {
                    ga.setGraph(currentGraph);
                    ga.setPopulationSize(populationSize);
                    ga.setTournamentSize(tournamentSize);
                    ga.setMutationProbability(mutationProb);
                    ga.setCrossoverProbability(crossoverProb);
                    ga.setMaxGenerations(maxGenerations);
                    ga.setMaxStagnation(noImprovementLimit);
                    ga.initialize();
                }
                isAlgorithmRunning = true;
                isAlgorithmFinished = false;
                std::cout << "=== Starting genetic algorithm ===" << std::endl;
            } else {
                statusMessage = "Ошибка: сначала загрузите или сгенерируйте граф";
                showError(statusMessage);
            }
        }
    } else {

        if (ImGui::Button("Шаг вперед")) {
            if (ga.doOneStep()) {
                currentGeneration = ga.getCurrentGeneration();
                bestWeight = ga.getBestIndividual().weight;
                bestFitness = ga.getBestIndividual().fitness;
                selectedEdges = ga.getBestIndividual().edges;
                weightHistory = ga.getFitnessHistory();
            } else if (isAlgorithmRunning) {
                isAlgorithmFinished = true;
                isAlgorithmRunning = false;

                currentGeneration = ga.getCurrentGeneration();
                bestWeight = ga.getBestIndividual().weight;
                bestFitness = ga.getBestIndividual().fitness;
                selectedEdges = ga.getBestIndividual().edges;
                weightHistory = ga.getFitnessHistory();

                statusMessage = "Алгоритм завершён (поколений: " + std::to_string(currentGeneration) + ")";
            } else {
                showError("Алгоритм завершен");
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Шаг назад")) {
            if (ga.stepBack()) {
                currentGeneration = ga.getCurrentGeneration();
                bestWeight = ga.getBestIndividual().weight;
                bestFitness = ga.getBestIndividual().fitness;
                selectedEdges = ga.getBestIndividual().edges;
                weightHistory = ga.getFitnessHistory();

                isAlgorithmFinished = false;
                isAlgorithmRunning = true;
            } else {
                showError("Вы на начальном поколении");
            }
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Завершить")) {
            ga.run();

            // Обновляем после завершения
            currentGeneration = ga.getCurrentGeneration();
            bestWeight = ga.getBestIndividual().weight;
            bestFitness = ga.getBestIndividual().fitness;
            selectedEdges = ga.getBestIndividual().edges;
            weightHistory = ga.getFitnessHistory();

            isAlgorithmFinished = true;
            isAlgorithmRunning = false;

            statusMessage = "Алгоритм завершён (поколений: " + std::to_string(currentGeneration) + ")";
        }

        
        // Если алгоритм уже завершён, показываем кнопку "Сброс"
        if (isAlgorithmFinished) {
            if (ImGui::Button("Сброс")) {
                isAlgorithmRunning = false;
                isAlgorithmFinished = false;
                currentGeneration = 0;
                weightHistory.clear();
                selectedEdges.clear();
                bestChromosome = "{}";

                if (currentGraph.vertexCount > 0) {
                    // Переинициализация ГА
                    ga.setGraph(currentGraph);
                    ga.setPopulationSize(populationSize);
                    ga.setTournamentSize(tournamentSize);
                    ga.setMutationProbability(mutationProb);
                    ga.setCrossoverProbability(crossoverProb);
                    ga.setMaxGenerations(maxGenerations);
                    ga.setMaxStagnation(noImprovementLimit);
                    ga.initialize();

                    weightHistory = ga.getFitnessHistory();

                    statusMessage = "Состояние сброшено, алгоритм переинициализирован";
                } else {
                    statusMessage = "Граф отсутствует, загрузите граф";
                }
            }
        }
    }
    ImGui::End();
}

void DrawInfoPanel()
{
    ImGui::SetNextWindowPos(ImVec2(390, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(350, 415), ImGuiCond_Always);
    ImGui::Begin("Информация", nullptr, ImGuiWindowFlags_NoResize);

    ImGui::Separator();
    ImGui::Text("Параметры графа");
    ImGui::Separator();

    ImGui::Text("Вершин: %d", vertexCount);
    ImGui::Text("Рёбер: %d", edgeCount);

    ImGui::Separator();
    ImGui::Text("Параметры алгоритма");
    ImGui::Separator();

    ImGui::Text("Поколение: %d", currentGeneration);
    ImGui::Text("Популяция: %d", populationSize);
    //ImGui::PushTextWrapPos(350);
    //ImGui::Text("Лучшая особь в поколении: %s", bestChromosome.c_str());
    ImGui::Text("Лучший вес в поколении: %.2f", bestWeight);
    ImGui::Text("Лучшая МОД по весу: %.2f", bestFitness);

    ImGui::Separator();
    if (isAlgorithmRunning) {
        ImGui::Text("Состояние: %s", isAlgorithmPaused ? "Пауза" : "Выполнение");
    } else if (isAlgorithmFinished) {
        ImGui::Text("Состояние: Завершён");
    } else {
        ImGui::Text("Состояние: Ожидание");
    }
    ImGui::NewLine();
    ImGui::Separator();
    ImGui::PushTextWrapPos(350);
    ImGui::Text("Статус: %s", statusMessage.c_str());
    ImGui::NewLine();
    ImGui::Separator();

    ImGui::End();
}

void DrawVisualizationWindow()
{
    ImGui::SetNextWindowPos(ImVec2(750, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(640, 880), ImGuiCond_Always);
    ImGui::Begin("Визуализация графа", nullptr, ImGuiWindowFlags_NoResize);
    int totalIndividuals;
    std::vector<Individual> population;
    if (ga.getCurrentPopulation().empty()) {
        totalIndividuals = 0;
    } else {
        population = ga.getCurrentPopulation();
    }
    totalIndividuals = (int)population.size();
    if (totalIndividuals > 0) {
        static int selectedIndividualIndex = 0;
        if (selectedIndividualIndex >= totalIndividuals)
            selectedIndividualIndex = totalIndividuals - 1;
        if (selectedIndividualIndex < 0)
            selectedIndividualIndex = 0;

        ImGui::InputInt("Номер особи (0..N-1)", &selectedIndividualIndex);
        if (selectedIndividualIndex < 0) selectedIndividualIndex = 0;
        if (selectedIndividualIndex >= totalIndividuals) selectedIndividualIndex = totalIndividuals - 1;

        selectedEdges = population[selectedIndividualIndex].edges;
        ImGui::Text("Вес особи: %.2f", population[selectedIndividualIndex].weight);
        ImGui::Text("Всего особей: %d", totalIndividuals);
    } else {
        ImGui::Text("Нет популяции для отображения");
        selectedEdges.clear();
    }

    ImGui::Checkbox("Показывать дерево", &showTree);
    drawGraphVisualization();

    ImGui::End();
}

void DrawRandomInputWindow(){
    if (!showRandomInput) return;
    ImGui::SetNextWindowSize(ImVec2(350, 120), ImGuiCond_Always);
    if (ImGui::Begin("Параметры случайного графа", &showRandomInput, ImGuiWindowFlags_NoResize)) {
        ImGui::PushItemWidth(100);
        ImGui::InputInt("Количество вершин", &randomVertexCount);
        if (randomVertexCount < 3) randomVertexCount = 3;
        float minCoef = 2.0/randomVertexCount;
        ImGui::SliderFloat("Коэфициент полноты графа", &randomCoefFull, minCoef, 1.0f, "%.2f");
        if(ImGui::Button("Сохранить")) {
            generateRandomGraphAuto(currentGraph);
            prepareAlg();
            statusMessage = "Граф получен из случайной генерации и ГА инициализирован";
            showRandomInput = false;
        }   
    }
    ImGui::End();
}

void DrawManualInputWindow() {
    if (!showManualInput) return;
    ImGui::SetNextWindowSize(ImVec2(600, 420), ImGuiCond_Always);
    if (ImGui::Begin("Ручной ввод графа", &showManualInput, ImGuiWindowFlags_NoResize)) {
        ImGui::SetNextItemWidth(80);
        ImGui::InputInt("Количество вершин (3-10)", &manualVertexCount);
        if (manualVertexCount < 3) manualVertexCount = 3;
        if (manualVertexCount > 10) manualVertexCount = 10;

        ImGui::BeginChild("MatrixTable", ImVec2(0, 310), true);
        ImGui::Columns(manualVertexCount + 1, "matrix", false);
        ImGui::Text(" ");
        ImGui::NextColumn();
        for (int j = 0; j < manualVertexCount; ++j) {
            ImGui::Text("%d", j);
            ImGui::NextColumn();
        }
        ImGui::Separator();

        for (int i = 0; i < manualVertexCount; ++i) {
            ImGui::Text("%d", i);
            ImGui::NextColumn();
            for (int j = 0; j < manualVertexCount; ++j) {
                if (i < j) {
                    double val = manualMatrix[i][j];
                    ImGui::SetNextItemWidth(80);
                    if (ImGui::InputDouble(("##" + std::to_string(i) + std::to_string(j)).c_str(), &val, 0.0, 0.0, "%.1f")) {
                        if (val < 0) val = 0;
                        manualMatrix[i][j] = val;
                        if (i != j) manualMatrix[j][i] = val;
                    }
                } else {
                    ImGui::Text("%.1f", manualMatrix[i][j]);
                }
                ImGui::NextColumn();
            }
        }
        ImGui::Columns(1);
        ImGui::EndChild();

        if (ImGui::Button("Создать граф")) {
            bool hasEdge = false;
            for (int i = 0; i < manualVertexCount; ++i)
                for (int j = i + 1; j < manualVertexCount; ++j)
                    if (manualMatrix[i][j] > 0) { hasEdge = true; break; }
            if (!hasEdge) {
                showError("Граф не может быть пустым. Добавьте хотя бы одно ребро с весом > 0.");
            } else {
                if (isGraphConnected()){
                    currentGraph.vertexCount = manualVertexCount;
                    currentGraph.edges.clear();
                    for (int i = 0; i < manualVertexCount; ++i)
                        for (int j = i + 1; j < manualVertexCount; ++j)
                            if (manualMatrix[i][j] > 0) {
                                Edge e{i, j, manualMatrix[i][j]};
                                currentGraph.edges.push_back(e);
                            }
                    prepareAlg();
                    statusMessage = "Граф загружен вручную и ГА инициализирован";
                    showManualInput = false;
                } else {
                    showError("Граф должен быть связным для поиска мод");
                }

            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Отмена")) showManualInput = false;
    }
    ImGui::End();
}

void DrawEvolutionPlot()
{
    ImGui::SetNextWindowPos(ImVec2(10, 435), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(730, 455), ImGuiCond_Always);
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

void loadErrorTexture() {
    int width, height, channels;
    // Попробуйте разные пути
    const char* paths[] = {
        "../png/error.jpg"
    };
    unsigned char* data = nullptr;
    for (const char* path : paths) {
        data = stbi_load(path, &width, &height, &channels, 4);
        if (data) {
            std::cout << "Картинка загружена: " << path << std::endl;
            break;
        }
    }
    if (data) {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        stbi_image_free(data);
        errorTexture = (ImTextureID)(uintptr_t)texture;
        errorTextureLoaded = true;
        std::cout << "Текстура создана, ID: " << texture << std::endl;
    } else {
        errorTexture = 0;
        errorTextureLoaded = false;
        std::cout << "Не удалось загрузить картинку ни по одному из путей" << std::endl;
    }
}

void DrawErrorPopup() {
    if (!showErrorPopup) return;

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 100), ImVec2(600, 400));

    if (ImGui::Begin("Ошибка", &showErrorPopup, 
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoCollapse))
    {
        if (errorTextureLoaded && errorTexture != 0) {
            ImGui::Image(errorTexture, ImVec2(100, 100));
            ImGui::SameLine();
            float remainingWidth = ImGui::GetContentRegionAvail().x;
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + remainingWidth);
            ImGui::Text("%s", errorMessage.c_str());
            ImGui::PopTextWrapPos();
        } else {
            float wrapWidth = ImGui::GetContentRegionAvail().x * 0.95f;
            ImGui::PushTextWrapPos(wrapWidth);
            ImGui::Text("%s", errorMessage.c_str());
            ImGui::PopTextWrapPos();
        }

        ImGui::Separator();
        if (ImGui::Button("OK")) {
            showErrorPopup = false;
        }
        ImGui::End();
    }
}