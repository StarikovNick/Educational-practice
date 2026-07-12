#include "genetic_algorithm.h"

// Конструктор
GeneticAlgorithm::GeneticAlgorithm() {}

// Сеттеры
void GeneticAlgorithm::setGraph(const Graph& graph)
{
    this->graph = graph;
}
void GeneticAlgorithm::setPopulationSize(int size)
{
    populationSize = size;
}
void GeneticAlgorithm::setMutationProbability(double probability)
{
    mutationProbability = probability;
}
void GeneticAlgorithm::setCrossoverProbability(double probability)
{
    crossoverProbability = probability;
}
void GeneticAlgorithm::setMaxGenerations(int generations)
{
    maxGenerations = generations;
}
void GeneticAlgorithm::setMaxStagnation(int generations)
{
    maxStagnation = generations;
}
void GeneticAlgorithm::setTournamentSize(int size)
{
    tournamentSize = size;
}

// Геттеры
const std::vector<Individual>& GeneticAlgorithm::getCurrentPopulation() const
{
    return population;
}
const Individual& GeneticAlgorithm::getBestIndividual() const
{
    return bestIndividual;
}
const std::vector<double>& GeneticAlgorithm::getFitnessHistory() const
{
    return fitnessHistory;
}
const std::vector<GenerationState>&
GeneticAlgorithm::getGenerationHistory() const
{
    return generationHistory;
}
int GeneticAlgorithm::getCurrentGeneration() const
{
    return generation;
}

// Создание начальной популяции (инициализация)
void GeneticAlgorithm::initialize()
{
    generation = 0;
    
    population.clear();
    fitnessHistory.clear();
    generationHistory.clear();
    bestIndividual.edges.clear();
    bestIndividual.weight = 0;
    bestIndividual.fitness = 0;
    stagnationCounter = 0;

    createInitialPopulation();
    evaluatePopulation();
    updateBestIndividual();
}

// Создание начальной популяции
void GeneticAlgorithm::createInitialPopulation()
{
    std::vector<int> edgeIndices(graph.edges.size());

    std::iota(edgeIndices.begin(), edgeIndices.end(), 0);

    while (population.size() < static_cast<size_t>(populationSize)) {
        Individual individual;
        DisjointSet dsu(graph.vertexCount);

        std::shuffle(edgeIndices.begin(), edgeIndices.end(), randomGenerator);
        for (int edgeIndex : edgeIndices) {
            const Edge& edge = graph.edges[edgeIndex];

            if (dsu.unite(edge.from, edge.to)) {
                individual.edges.push_back(edgeIndex);

                if (individual.edges.size() ==
                    static_cast<size_t>(graph.vertexCount - 1))
                    break;
            }
        }

        population.push_back(individual);
    }
}

// Функция приспособленности
void GeneticAlgorithm::evaluatePopulation()
{
    for (Individual& individual : population) {
        individual.weight = calculateWeight(individual);
        individual.fitness = calculateFitness(individual);
    }
}
// Обновление лучшего решения
void GeneticAlgorithm::updateBestIndividual()
{
    if (population.empty())
        return;

    bool improved = false;

    if (generation == 0 && bestIndividual.edges.empty()) {
        bestIndividual = population.front();
        improved = true;
    }

    for (const Individual& individual : population) {
        if (individual.fitness < bestIndividual.fitness) {
            bestIndividual = individual;
            improved = true;
        }
    }

    if (improved)
        stagnationCounter = 0;
    else
        stagnationCounter++;
}

// Вычисление веса
double GeneticAlgorithm::calculateWeight(const Individual& individual)
{
    double totalWeight = 0.0;

    for (int edgeIndex : individual.edges) {
        totalWeight += graph.edges[edgeIndex].weight;
    }

    return totalWeight;
}
// Вычисление приспособленности
double GeneticAlgorithm::calculateFitness(const Individual& individual)
{
    return individual.weight;
}

// Турнирный отбор
Individual GeneticAlgorithm::selection()
{
    std::uniform_int_distribution<int> distribution(
        0, static_cast<int>(population.size()) - 1);
    Individual best = population[distribution(randomGenerator)];

    for (int i = 1; i < tournamentSize; i++) {
        const Individual& candidate = population[distribution(randomGenerator)];

        if (candidate.fitness < best.fitness)
            best = candidate;
    }

    return best;
}

// Равномерное скрещивание
std::pair<Individual, Individual> GeneticAlgorithm::crossover(
    const Individual& parent1, const Individual& parent2)
{
    std::bernoulli_distribution crossoverDistribution(crossoverProbability);

    if (!crossoverDistribution(randomGenerator))
        return {parent1, parent2};

    Individual child1;
    Individual child2;

    std::bernoulli_distribution geneChoice(0.5);

    size_t chromosomeSize = parent1.edges.size();
    child1.edges.resize(chromosomeSize);
    child2.edges.resize(chromosomeSize);

    for (size_t i = 0; i < chromosomeSize; i++) {
        if (geneChoice(randomGenerator)) {
            child1.edges[i] = parent1.edges[i];
            child2.edges[i] = parent2.edges[i];
        }
        else {
            child1.edges[i] = parent2.edges[i];
            child2.edges[i] = parent1.edges[i];
        }
    }

    return {child1, child2};
}

// Мутация
void GeneticAlgorithm::mutate(Individual& individual)
{
    std::bernoulli_distribution mutation(mutationProbability);

    if (!mutation(randomGenerator))
        return;

    std::uniform_int_distribution<int> removeDistribution(
        0, static_cast<int>(individual.edges.size()) - 1);
    int removeIndex = removeDistribution(randomGenerator);
    individual.edges.erase(individual.edges.begin() + removeIndex);
}

// Функция исправления
void GeneticAlgorithm::repair(Individual& individual)
{
    // Исправление дубликатов
    std::sort(individual.edges.begin(), individual.edges.end());
    individual.edges.erase(std::unique(individual.edges.begin(),
            individual.edges.end()), individual.edges.end());

    // Исправление циклов
    DisjointSet dsu(graph.vertexCount);
    std::vector<int> repairedEdges;

    for (int edgeIndex : individual.edges) {
        const Edge& edge = graph.edges[edgeIndex];

        if (dsu.unite(edge.from, edge.to))
            repairedEdges.push_back(edgeIndex);
    }

    // Исправление несвязности
    std::vector<int> sortedEdgeIndices(graph.edges.size());

    std::iota(sortedEdgeIndices.begin(), sortedEdgeIndices.end(), 0);

    std::sort(sortedEdgeIndices.begin(), sortedEdgeIndices.end(),
        [&](int left, int right) {
            return graph.edges[left].weight < graph.edges[right].weight; });

    for (int edgeIndex : sortedEdgeIndices) {
        if (repairedEdges.size() ==
            static_cast<size_t>(graph.vertexCount - 1))
            break;

        const Edge& edge = graph.edges[edgeIndex];

        if (dsu.unite(edge.from, edge.to))
            repairedEdges.push_back(edgeIndex);
    }

    // Запись результата
    std::sort(repairedEdges.begin(), repairedEdges.end());
    individual.edges = std::move(repairedEdges);
    individual.weight = calculateWeight(individual);
    individual.fitness = calculateFitness(individual);
}

// Запуск одного шага
bool GeneticAlgorithm::doOneStep()
{
    if (isFinished())
        return false;

    // Сохранение текущего состояния
    GenerationState state;
    state.population = population;
    state.bestIndividual = bestIndividual;
    state.generation = generation;
    state.stagnationCounter = stagnationCounter;
    generationHistory.push_back(std::move(state));

    std::vector<Individual> newPopulation;
    std::bernoulli_distribution crossoverDistribution(crossoverProbability);

    while (newPopulation.size() < static_cast<size_t>(populationSize)) {
        // Выбор родителей
        Individual parent1 = selection();
        Individual parent2 = selection();

        Individual child1 = parent1;
        Individual child2 = parent2;

        // Скрещивание
        auto children = crossover(parent1, parent2);
        child1 = std::move(children.first);
        child2 = std::move(children.second);

        // Мутация
        mutate(child1);
        mutate(child2);

        // Исправление
        repair(child1);
        repair(child2);

        newPopulation.push_back(std::move(child1));

        if (newPopulation.size() < static_cast<size_t>(populationSize))
            newPopulation.push_back(std::move(child2));
    }

    population = std::move(newPopulation);

    evaluatePopulation();
    updateBestIndividual();

    fitnessHistory.push_back(bestIndividual.fitness);
    generation++;

    return !isFinished();
}

// Полный запуск
void GeneticAlgorithm::run()
{
    while (doOneStep()) {}
}

// Шаг назад по алгоритму
bool GeneticAlgorithm::stepBack()
{
    if (generationHistory.empty())
        return false;

    GenerationState state = std::move(generationHistory.back());

    generationHistory.pop_back();

    population = std::move(state.population);
    bestIndividual = std::move(state.bestIndividual);

    generation = state.generation;
    stagnationCounter = state.stagnationCounter;

    if (!fitnessHistory.empty())
        fitnessHistory.pop_back();

    return true;
}

// Критерий остановки
bool GeneticAlgorithm::isFinished() const
{
    return generation >= maxGenerations || stagnationCounter >= maxStagnation;
}