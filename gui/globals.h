#pragma once

#include <string>
#include <vector>
#include "GraphData.h"
#include "genetic_algorithm.h"
#include <imgui.h> 

extern GeneticAlgorithm ga;
extern Graph currentGraph;
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
extern bool showManualInput;
extern double manualMatrix[10][10];
extern int manualVertexCount;
extern bool showTree;

extern bool showErrorPopup;
extern std::string errorMessage;

extern ImTextureID errorTexture;
extern bool errorTextureLoaded;

void showError(const std::string& msg);

extern int idxPopulation;

extern bool showRandomInput;

extern int randomVertexCount;
extern float randomCoefFull;