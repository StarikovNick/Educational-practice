#include <fstream>
#include <iostream>

#include "genetic_algorithm.h"

int main()
{
    std::ifstream file("graph.txt");

    if (!file.is_open()) {
        std::cout << "Cannot open graph.txt\n";
        return 1;
    }

    Graph graph;
    int edgeCount;
    file >> graph.vertexCount >> edgeCount;
    graph.edges.reserve(edgeCount);

    for (int i = 0; i < edgeCount; i++) {
        Edge edge;

        file >> edge.from >> edge.to >> edge.weight;

        graph.edges.push_back(edge);
    }

    file.close();

    GeneticAlgorithm algorithm;

    algorithm.setGraph(graph);
    algorithm.setPopulationSize(100); // значения из gui
    algorithm.setMaxGenerations(500);
    algorithm.setMutationProbability(0.05);
    algorithm.setCrossoverProbability(0.8);
    algorithm.setTournamentSize(3);

    algorithm.initialize();

    algorithm.doOneStep();

    const Individual& best = algorithm.getBestIndividual();
    std::cout << "Best tree\n\n";
    std::cout << "Weight = " << best.weight << "\n\n";
    std::cout << "Edges:\n";

    for (int edgeIndex : best.edges) {
        const Edge& edge = graph.edges[edgeIndex];
        std::cout << edge.from << " " << edge.to << " " << edge.weight << '\n';
    }

    return 0;
}