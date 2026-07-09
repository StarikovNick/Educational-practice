#pragma once
#include <vector>
#include <string>

struct Edge {
    int from;
    int to;
    double weight;
};

struct Graph {
    int vertexCount;
    std::vector<Edge> edges;
};

struct Individual
{
    std::vector<int> edges;   // список ребер
    double weight = 0.0;      // суммарный вес ребер
    double fitness = 0.0;     // параметр приспособленности
};

// Функция для загрузки графа из файла (объявление)
bool loadFromFile(const std::string& filename, Graph& outGraph);
void generateRandomGraph(int vertexCount, int extraEdges, Graph& outGraph);
// Генерирует полностью случайный граф (размер и плотность выбираются автоматически)
void generateRandomGraphAuto(Graph& outGraph);
// Добавляет ребро в граф, если оно корректно и не дублируется.
// Возвращает true, если добавление успешно, иначе false.
// В параметре message возвращается описание результата.
bool addEdgeToGraph(Graph& graph, int from, int to, double weight, std::string& message);