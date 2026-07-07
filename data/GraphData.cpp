#include "GraphData.h"
#include <fstream>
#include <iostream>
#include <random>
#include <set>

bool loadFromFile(const std::string& filename, Graph& outGraph) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return false;
    }

    int V, E;
    if (!(file >> V >> E)) {
        std::cerr << "Ошибка: некорректный формат файла (ожидались V и E)" << std::endl;
        return false;
    }

    outGraph.vertexCount = V;
    outGraph.edges.clear();
    outGraph.edges.reserve(E); // резервируем память для скорости

    for (int i = 0; i < E; ++i) {
        int u, v, w;
        if (!(file >> u >> v >> w)) {
            std::cerr << "Ошибка: неполные данные ребра на строке " << (i+2) << std::endl;
            return false;
        }
        outGraph.edges.push_back({u, v, static_cast<double>(w)});
    }

    std::cout << "Файл успешно считан" << std::endl;

    file.close();
    return true;
}

void generateRandomGraph(int vertexCount, int extraEdges, Graph& outGraph) {
    // Проверка на минимальное число вершин
    if (vertexCount < 2) vertexCount = 2;
    if (extraEdges < 0) extraEdges = 0;

    // Максимально возможное число рёбер в простом графе
    int maxPossibleEdges = vertexCount * (vertexCount - 1) / 2;
    // Минимальное дерево требует (vertexCount - 1) рёбер
    int minEdges = vertexCount - 1;
    // Общее количество рёбер = дерево + дополнительные
    int totalEdges = minEdges + extraEdges;
    if (totalEdges > maxPossibleEdges) totalEdges = maxPossibleEdges;

    outGraph.vertexCount = vertexCount;
    outGraph.edges.clear();
    outGraph.edges.reserve(totalEdges);

    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> weightDist(1, 100); // веса от 1 до 100

    // --- Этап 1: Создаём остовное дерево (гарантирует связность) ---
    // Используем простой подход: соединяем вершину i с i+1 (путь)
    for (int i = 0; i < vertexCount - 1; ++i) {
        int weight = weightDist(gen);
        outGraph.edges.push_back({i, i + 1, static_cast<double>(weight)});
    }

    // --- Этап 2: Добавляем случайные дополнительные рёбра ---
    // Чтобы избежать дубликатов, будем хранить уже существующие пары (u,v) в множестве
    std::set<std::pair<int, int>> existingEdges;
    for (const auto& e : outGraph.edges) {
        int u = std::min(e.from, e.to);
        int v = std::max(e.from, e.to);
        existingEdges.insert({u, v});
    }

    int added = 0;
    int attempts = 0;
    const int maxAttempts = 10000; // защита от бесконечного цикла

    while (added < extraEdges && attempts < maxAttempts) {
        attempts++;
        // Случайные две разные вершины
        int u = std::uniform_int_distribution<int>(0, vertexCount - 1)(gen);
        int v = std::uniform_int_distribution<int>(0, vertexCount - 1)(gen);
        if (u == v) continue;
        int a = std::min(u, v);
        int b = std::max(u, v);

        // Если ребро уже существует — пропускаем
        if (existingEdges.find({a, b}) != existingEdges.end()) continue;

        // Добавляем новое ребро
        int weight = weightDist(gen);
        outGraph.edges.push_back({a, b, static_cast<double>(weight)});
        existingEdges.insert({a, b});
        added++;
    }

    // Если не удалось добавить нужное количество рёбер (например, граф стал полным),
    // это нормально — просто выходим.
}

void generateRandomGraphAuto(Graph& outGraph) {
    // Генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    // Случайное число вершин от 4 до 12 (можно изменить диапазон)
    std::uniform_int_distribution<int> vertexDist(4, 50);
    int vCount = vertexDist(gen);

    // Случайное количество дополнительных рёбер от 0 до 5
    std::uniform_int_distribution<int> extraDist(100, 200);
    int eExtra = extraDist(gen);

    // Вызываем основную функцию генерации с этими параметрами
    generateRandomGraph(vCount, eExtra, outGraph);
}

bool addEdgeToGraph(Graph& graph, int from, int to, double weight, std::string& message) {
    // Проверка корректности данных
    if (from == to) {
        message = "Ошибка: вершины должны быть разными";
        return false;
    }
    if (from < 0 || to < 0 || weight <= 0) {
        message = "Ошибка: вершины >= 0, вес > 0";
        return false;
    }

    // Проверка на существование такого же ребра (с учётом порядка)
    for (const Edge& e : graph.edges) {
        if ((e.from == from && e.to == to) || (e.from == to && e.to == from)) {
            message = "Ребро " + std::to_string(from) + "-" + std::to_string(to) + " уже существует";
            return false;
        }
    }

    // Вычисляем новое количество вершин (если появится новая вершина)
    int newVertexCount = graph.vertexCount;
    int maxVertex = std::max(from, to);
    if (maxVertex >= newVertexCount) {
        newVertexCount = maxVertex + 1;
    }

    // Максимальное число рёбер для простого графа
    int maxPossible = newVertexCount * (newVertexCount - 1) / 2;
    if ((int)graph.edges.size() + 1 > maxPossible) {
        message = "Нельзя добавить ребро: достигнут максимум (" + std::to_string(maxPossible) + ") для " +
                  std::to_string(newVertexCount) + " вершин";
        return false;
    }

    // Добавляем ребро
    graph.edges.push_back({from, to, weight});
    if (maxVertex >= graph.vertexCount) {
        graph.vertexCount = newVertexCount;
    }

    message = "Ребро добавлено: " + std::to_string(from) + "-" + std::to_string(to) +
              " вес " + std::to_string(weight);
    return true;
}