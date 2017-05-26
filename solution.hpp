#pragma once

#include <random>

typedef struct {
  double value;
  int *order;
} solution_t;

typedef struct {
  int a, b;
} permutation_t;

extern std::mt19937 generator;

solution_t *createRandomSolution();
solution_t createNEHSolution();
double calculateDistance(solution_t solution);
double calculateDistance2(solution_t solution);
double calculateNeighbourDistance(solution_t solution, permutation_t permutation);
void swap(solution_t *solution, permutation_t permutation);
