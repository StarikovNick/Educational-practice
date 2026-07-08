#pragma once

#include <string>
#include <vector>
#include "GraphData.h"

extern Graph currentGraph;
extern std::vector<std::vector<Edge>> testTrees;
extern int selectedTreeIndex;
extern std::vector<int> selectedEdges;
extern int vertexCount;
extern int edgeCount;
extern int populationSize;
extern int tournamentSize;
extern float crossoverProb;
extern float mutationProb;
extern int maxGenerations;
extern int noImprovementLimit;
extern int currentGeneration;
extern double bestFitness;
extern double bestWeight;
extern bool isAlgorithmRunning;
extern bool isAlgorithmPaused;
extern bool isAlgorithmFinished;
extern std::string statusMessage;
extern std::vector<double> weightHistory;
extern std::vector<std::string> populationDisplay;
extern std::string bestChromosome;