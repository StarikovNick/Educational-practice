#pragma once

#include "GraphData.h"
#include "disjoint_set.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <limits>
#include <utility>

// Основной класс алгоритма
class GeneticAlgorithm
{
public:
    GeneticAlgorithm();

    // Настройка параметров алгоритма
    void setGraph(const Graph& graph);
    void setPopulationSize(int size);
    void setMutationProbability(double probability);
    void setCrossoverProbability(double probability);
    void setMaxGenerations(int generations);
    void setMaxStagnation(int generations);
    void setTournamentSize(int size);

    // Получение результатов
    const std::vector<Individual>& getCurrentPopulation() const;
    const Individual& getBestIndividual() const;
    const std::vector<double>& getFitnessHistory() const;
    const std::vector<std::vector<Individual>>& getPopulationHistory() const;
    int getCurrentGeneration() const;

    // Управление алгоритмом
    void initialize();   // создание начальной популяции
    bool doOneStep();    // выполнить одно поколение
    void run();          // выполнить до завершения

    // Проверка критерия окончания
    bool isFinished() const;

private:
    // Создание начальной популяции
    void createInitialPopulation();

    // Оценка популяции
    void evaluatePopulation();
    void updateBestIndividual();

    // Вспомогательные вычисления
    double calculateWeight(const Individual& individual);
    double calculateFitness(const Individual& individual);

    // Генетические операторы
    Individual selection();
    std::pair<Individual, Individual> crossover(
        const Individual& parent1, const Individual& parent2);
    void mutate(Individual& individual);

    // Функция исправления решения
    void repair(Individual& individual);

    Graph graph;
    std::vector<Individual> population;
    Individual bestIndividual;

    // История популяций и изменения лучшего fitness
    std::vector<double> fitnessHistory;
    std::vector<std::vector<Individual>> populationHistory;

    int generation = 0;
    int populationSize = 100;
    int maxGenerations = 500;
    int maxStagnation = 50;
    int stagnationCounter = 0;
    int tournamentSize = 3;

    double mutationProbability = 0.05;
    double crossoverProbability = 0.8;

    std::mt19937 randomGenerator{std::random_device{}()};
};