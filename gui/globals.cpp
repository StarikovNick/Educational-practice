#include "Globals.h"
#include "GraphData.h"

Graph currentGraph;
std::vector<std::vector<Edge>> testTrees;
int selectedTreeIndex = 0;
std::vector<int> selectedEdges;
int vertexCount = 0;
int edgeCount = 0;
int populationSize = 20;
int tournamentSize = 3;
float crossoverProb = 0.8f;
float mutationProb = 0.1f;
int maxGenerations = 100;
int noImprovementLimit = 10;
int currentGeneration = 0;
double bestFitness = 0.0;
double bestWeight = 0.0;
bool isAlgorithmRunning = false;
bool isAlgorithmPaused = false;
bool isAlgorithmFinished = false;
std::string statusMessage = "Готов к работе";
std::vector<double> weightHistory;
std::vector<std::string> populationDisplay;
std::string bestChromosome = "{0, 3, 5, 7, 8}";
bool showManualInput = false;
double manualMatrix[10][10] = {};
int manualVertexCount = 5;
bool showTree = true;