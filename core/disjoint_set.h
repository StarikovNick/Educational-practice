#pragma once

#include <vector>
#include <numeric>

// Вспомогательный класс cистемы непересекающихся множеств
class DisjointSet
{
public:
    DisjointSet(int size);

    int find(int vertex);
    
    bool unite(int a, int b);

private:
    std::vector<int> parent;
    std::vector<int> rank;
};