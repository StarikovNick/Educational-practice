#include "g_alg.h"
#include "globals.h"
#include <cstdlib>
#include <string>

void StartAlgorithm()
{
    isAlgorithmRunning = true;
    isAlgorithmPaused = false;
    isAlgorithmFinished = false;
    currentGeneration = 0;
    bestFitness = 0.0;
    bestWeight = 0.0;
    weightHistory.clear();
    statusMessage = "Алгоритм запущен (заглушка)";
}

void PauseAlgorithm()
{
    if (isAlgorithmRunning && !isAlgorithmPaused) {
        isAlgorithmPaused = true;
        statusMessage = "Алгоритм на паузе";
    }
}

void ResumeAlgorithm()
{
    if (isAlgorithmRunning && isAlgorithmPaused) {
        isAlgorithmPaused = false;
        statusMessage = "Алгоритм продолжен";
    }
}

void StepAlgorithm()
{
    if (!isAlgorithmRunning || isAlgorithmFinished) return;

    // Здесь должна быть реальная логика одного поколения ГА.

    currentGeneration++;
    double newWeight = 50 + rand() % 50;
    bestWeight = newWeight;
    bestFitness = 100.0 - newWeight;
    weightHistory.push_back((float)newWeight);

    bestChromosome = "{";
    for (int i = 0; i < vertexCount - 1; i++) {
        bestChromosome += std::to_string(rand() % edgeCount) + (i < vertexCount - 2 ? ", " : "");
    }
    bestChromosome += "}";

    // Проверка критериев остановки
    if (currentGeneration >= maxGenerations || weightHistory.size() > 10) {
        isAlgorithmFinished = true;
        statusMessage = "Алгоритм завершён по критерию остановки";
    } else {
        statusMessage = "Шаг " + std::to_string(currentGeneration) + " выполнен";
    }
}

void StopAlgorithm()
{
    if (isAlgorithmRunning) {
        isAlgorithmFinished = true;
        statusMessage = "Алгоритм завершён досрочно";
    }
}

void ResetAlgorithm()
{
    isAlgorithmRunning = false;
    isAlgorithmPaused = false;
    isAlgorithmFinished = false;
    currentGeneration = 0;
    bestFitness = 0.0;
    bestWeight = 0.0;
    weightHistory.clear();
    statusMessage = "Состояние сброшено";
}

bool IsAlgorithmFinished()
{
    return isAlgorithmFinished;
}