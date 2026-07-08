#include "disjoint_set.h"

DisjointSet::DisjointSet(int size)
{
    parent.resize(size);
    rank.resize(size, 0);

    std::iota(parent.begin(), parent.end(), 0);
}

int DisjointSet::find(int vertex)
{
    if (parent[vertex] == vertex)
        return vertex;

    return parent[vertex] = find(parent[vertex]);
}

bool DisjointSet::unite(int a, int b)
{
    a = find(a);
    b = find(b);

    if (a == b)
        return false;

    if (rank[a] < rank[b])
        std::swap(a, b);

    parent[b] = a;
    if (rank[a] == rank[b])
        rank[a]++;

    return true;
}