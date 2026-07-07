#pragma once

#include <vector>

// Структура ребер
struct Edge
{
    int from;
    int to;
    double weight;
};

// Структура графа
struct Graph
{
    int vertex_count;
    std::vector<Edge> edges;
};

// Структура особи
struct Individual
{
    std::vector<int> edges;   // индексы ребер
    double weight = 0.0;      // сумма весов дерева
    double fitness = 0.0;     // значение функции качества
};