#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GraphData.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

// === Данные графа ===
static Graph currentGraph;               // Хранит загруженный граф
static std::vector<int> selectedEdges;   // Индексы рёбер для подсветки (текущее дерево)

// === Глобальные переменные для имитации состояния ГА ===
static int vertexCount = 0;
static int edgeCount = 0;
static int populationSize = 20;
static int tournamentSize = 3;
static float crossoverProb = 0.8f;
static float mutationProb = 0.1f;
static int maxGenerations = 100;
static int noImprovementLimit = 10;
static int currentGeneration = 0;
static double bestFitness = 0.0;
static double bestWeight = 0.0;
static bool isAlgorithmRunning = false;
static bool isAlgorithmPaused = false;
static bool isAlgorithmFinished = false;
static std::string statusMessage = "Готов к работе";

// Данные для графика (история лучшего веса по поколениям)
static std::vector<float> weightHistory;

// Имитация популяции (для отображения)
static std::vector<std::string> populationDisplay;
static std::string bestChromosome = "{0, 3, 5, 7, 8}";

// === Функция отрисовки визуализации графа (заглушка) ===
void drawGraphVisualization() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    
    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), 
                      IM_COL32(80, 80, 80, 255), 4.0f);
    
    // Текст-заглушка
    drawList->AddText(ImVec2(canvasPos.x + 20, canvasPos.y + 20), 
                      IM_COL32(200, 200, 200, 255), 
                      "Визуализация графа\n(вершины и рёбра)\nТекущее лучшее дерево подсвечено");
    
    // Имитация вершин
    for (int i = 0; i < 6; i++) {
        float x = canvasPos.x + 60 + i * 70;
        float y = canvasPos.y + 80 + (i % 2) * 60;
        drawList->AddCircleFilled(ImVec2(x, y), 20, IM_COL32(70, 150, 255, 255));
        drawList->AddText(ImVec2(x - 8, y - 10), IM_COL32(255, 255, 255, 255), 
                          std::to_string(i).c_str());
    }
}

// === Основная функция ===
int main() {
    // Инициализация GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1400, 900, "Генетический алгоритм для поиска МОД", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Шрифт с кириллицей
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    if (!font) io.Fonts->AddFontDefault();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // === Главный цикл ===
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ===========================================================
        // 1. ЛЕВАЯ ПАНЕЛЬ: Управление и параметры
        // ===========================================================
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_FirstUseEver);
        ImGui::Begin("Управление", nullptr, ImGuiWindowFlags_NoResize);

        // --- Блок: Данные графа ---
        ImGui::Text("Данные графа");
        ImGui::Separator();
        if (ImGui::Button("Загрузить из файла")) {
            // Пока используем жёсткий путь. Позже сделаем диалог выбора файла.
            std::string path = "C:/Users/Admin/Desktop/GIT/Educational-practice/graph.txt";
            
            if (loadFromFile(path, currentGraph)) {
            // Обновляем глобальные переменные для отображения в интерфейсе
            vertexCount = currentGraph.vertexCount;
            edgeCount = (int)currentGraph.edges.size();
        
            // Очищаем выделенные рёбра и историю
            selectedEdges.clear();
            weightHistory.clear();
            bestChromosome = "{}";
            bestWeight = 0.0;
            bestFitness = 0.0;
            currentGeneration = 0;
        
            statusMessage = "Граф загружен! Вершин: " + std::to_string(vertexCount) + ", рёбер: " + std::to_string(edgeCount);
            }
            else {
                statusMessage = "Ошибка загрузки файла! Проверьте путь и формат.";
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Случайная генерация")) {
            // Генерируем полностью случайный граф (размер и плотность — случайны)
            generateRandomGraphAuto(currentGraph);

            // Обновляем глобальные переменные
            vertexCount = currentGraph.vertexCount;
            edgeCount = (int)currentGraph.edges.size();

            // Очищаем выделение и историю
            selectedEdges.clear();
            weightHistory.clear();
            bestChromosome = "{}";
            bestWeight = 0.0;
            bestFitness = 0.0;
            currentGeneration = 0;

            statusMessage = "Случайный граф: " + std::to_string(vertexCount) + " вершин, " + std::to_string(edgeCount) + " рёбер";
        }

        ImGui::Spacing();
        ImGui::Text("Ручное добавление ребра:");
        static int v1 = 0, v2 = 1, weight = 1;
        ImGui::InputInt("Вершина 1", &v1);
        ImGui::InputInt("Вершина 2", &v2);
        ImGui::InputInt("Вес", &weight);
        if (ImGui::Button("Добавить ребро")) {
            std::string msg;
            if (addEdgeToGraph(currentGraph, v1, v2, weight, msg)) {
                // Успешно — обновляем глобальные переменные
                vertexCount = currentGraph.vertexCount;
                edgeCount = (int)currentGraph.edges.size();
                selectedEdges.clear();
                weightHistory.clear();
                bestChromosome = "{}";
                bestWeight = 0.0;
                bestFitness = 0.0;
                currentGeneration = 0;
                statusMessage = msg;
            }       
            else {
                statusMessage = msg; // сообщение об ошибке
            }
        }

        ImGui::Spacing();
        ImGui::Separator();

        // --- Блок: Параметры генетического алгоритма ---
        ImGui::Text("Параметры ГА");
        ImGui::Separator();
        ImGui::InputInt("Размер популяции", &populationSize);
        ImGui::InputInt("Размер турнира", &tournamentSize);
        ImGui::SliderFloat("Вероятность скрещивания", &crossoverProb, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Вероятность мутации", &mutationProb, 0.0f, 1.0f, "%.2f");
        ImGui::InputInt("Макс. поколений", &maxGenerations);
        ImGui::InputInt("Лимит без улучшений", &noImprovementLimit);

        ImGui::Spacing();
        ImGui::Separator();

        // --- Блок: Управление запуском ---
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
                if (isAlgorithmRunning && !isAlgorithmFinished) {
                    // Имитация одного поколения
                    currentGeneration++;
                    double newWeight = 50 + rand() % 50;
                    bestWeight = newWeight;
                    bestFitness = 100.0 - newWeight;
                    weightHistory.push_back((float)newWeight);
                    
                    // Имитация лучшей хромосомы
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

        // ===========================================================
        // 2. ВЕРХНЯЯ ПАНЕЛЬ: Информация
        // ===========================================================
        ImGui::SetNextWindowPos(ImVec2(370, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1020, 180), ImGuiCond_FirstUseEver);
        ImGui::Begin("Информация", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Columns(3, "infoCols");
        ImGui::Text("Вершин: %d", vertexCount);
        ImGui::NextColumn();
        ImGui::Text("Рёбер: %d", edgeCount);
        ImGui::NextColumn();
        ImGui::Text("Популяция: %d", populationSize);
        ImGui::NextColumn();
        ImGui::Text("Поколение: %d", currentGeneration);
        ImGui::NextColumn();
        ImGui::Text("Лучший вес: %.2f", bestWeight);
        ImGui::NextColumn();
        ImGui::Text("Лучшая fitness: %.2f", bestFitness);
        ImGui::NextColumn();
        ImGui::Text("Лучшая особь: %s", bestChromosome.c_str());
        ImGui::NextColumn();
        ImGui::Text("Статус: %s", statusMessage.c_str());
        ImGui::NextColumn();
        if (isAlgorithmRunning) {
            ImGui::Text("Состояние: %s", isAlgorithmPaused ? "Пауза" : "Выполнение");
        } else if (isAlgorithmFinished) {
            ImGui::Text("Состояние: Завершён");
        } else {
            ImGui::Text("Состояние: Ожидание");
        }
        ImGui::Columns(1);

        ImGui::End();

        // ===========================================================
        // 3. ВИЗУАЛИЗАЦИЯ ГРАФА
        // ===========================================================
        ImGui::SetNextWindowPos(ImVec2(370, 200), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1020, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Визуализация графа", nullptr, ImGuiWindowFlags_NoResize);

        drawGraphVisualization();

        ImGui::End();

        // ===========================================================
        // 4. ГРАФИК ЭВОЛЮЦИИ
        // ===========================================================
        ImGui::SetNextWindowPos(ImVec2(370, 610), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1020, 250), ImGuiCond_FirstUseEver);
        ImGui::Begin("График эволюции (вес МОД по поколениям)", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Text("Лучший вес на каждом поколении");
        if (!weightHistory.empty()) {
            ImGui::PlotLines("Вес", weightHistory.data(), (int)weightHistory.size(), 
                             0, nullptr, 0.0f, 100.0f, ImVec2(900, 150));
        } else {
            ImGui::Text("Нет данных для отображения");
        }

        ImGui::End();

        // ===========================================================
        // Рендеринг
        // ===========================================================
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}