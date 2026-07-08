#pragma once

#include <vector>

// Структура ребра
struct Edge
{
    int from;
    int to;
    double weight;
};

// Структура графа
struct Graph
{
    int vertexCount;
    std::vector<Edge> edges;
};

// Структура особи
struct Individual
{
    std::vector<int> edges;   // список ребер
    double weight = 0.0;      // суммарный вес ребер
    double fitness = 0.0;     // параметр приспособленности
};