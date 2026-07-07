#pragma once

#include "graph.h"

// Основной класс алгоритма
class GeneticAlgorithm
{
public:
    GeneticAlgorithm();

    // Загрузка графа
    void setGraph(const Graph& graph);

    // Настройка параметров алгоритма
    void setPopulationSize(int size);
    void setMutationProbability(double probability);
    void setCrossoverProbability(double probability);
    void setMaxGenerations(int generations);
    void setTournamentSize(int size);

    // Управление алгоритмом
    void initialize();   // создание начальной популяции
    bool doOneStep();    // выполнить одно поколение
    void run();          // выполнить до завершения

    // Получение результатов
    const std::vector<Individual>& getCurrentPopulation() const;
    const Individual& getBestIndividual() const;
    const std::vector<double>& getFitnessHistory() const;
    const std::vector<std::vector<Individual>>& getPopulationHistory() const;
    int getCurrentGeneration() const;

    // Проверка критерия окончания
    bool isFinished() const;

private:
    // Создание начальной популяции
    void createInitialPopulation();

    // Оценка популяции
    void evaluatePopulation();
    void updateBestIndividual();

    // Генетические операторы
    Individual selection();
    std::pair<Individual, Individual> crossover(
        const Individual& parent1,
        const Individual& parent2);
    void mutate(Individual& individual);

    // Функция исправления решения
    void repair(Individual& individual);

    // Вспомогательные вычисления
    double calculateWeight(const Individual& individual);
    double calculateFitness(const Individual& individual);
    bool isTree(const Individual& individual) const;

    Graph graph;
    std::vector<Individual> population;
    Individual best_individual;

    // История популяций и изменения лучшего fitness
    std::vector<double> fitness_history;
    std::vector<std::vector<Individual>> population_history;

    int generation = 0;
    int population_size = 100;
    int max_generations = 500;
    int tournament_size = 3;

    double mutation_probability = 0.05;
    double crossover_probability = 0.8;
};