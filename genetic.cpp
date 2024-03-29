#include "genetic.hpp"

#include "cities.hpp"
#include "solution.hpp"

#include <iostream>
#include <csignal>
#include <cstring>
#include <algorithm>

using namespace std;

uniform_int_distribution<int> disI;
uniform_int_distribution<int> disI2;
uniform_int_distribution<int> disI3;
uniform_real_distribution<double> disD(0.0, 1.0);

// size of population
int populationSize;
// number of pairs for crossover
int pairs;
// size after crossover
int crossoverSize;
// size of array with species
int arraySize;
// probability of mutation
double mutationP;
// iterations counter
int k;
int better;
int iterations;
int stopCondition;

// helper arrays with bools
bool *check1;
bool *check2;

solution_t **population;
solution_t theBestSolution;

void printResult() {
  cout << calculateDistance(theBestSolution) << endl;
  for (int i = 0; i <= n; i++) {
    cerr << theBestSolution.order[i] + 1 << " ";
  }
  cerr << endl;
}

void handler(int signum) {
  printResult();
  exit(0);
}

void setupHandler() {
  struct sigaction act;
  act.sa_handler = handler;
  sigaction(SIGINT, &act, NULL);
}

inline solution_t *copyFromBest() {
  solution_t *solution = new solution_t;
  solution->order = new int[n + 1];
  solution->value = theBestSolution.value;
  memcpy(solution->order, theBestSolution.order, (n + 1) * sizeof(int));
  return solution;
}

void initializeSearch() {
  calculateDistances();
  theBestSolution = createNEHSolution();

  if (n < 200) {
    populationSize = 30;
    pairs = populationSize;
    iterations = 40000;
  } else if (n < 700) {
    populationSize = 30;
    pairs = populationSize * 2;
    iterations = 20000;
  } else if (n < 1050) {
    populationSize = 60;
    pairs = populationSize;
    iterations = 20000;
  } else if (n < 1700) {
    populationSize = 60;
    pairs = populationSize;
    iterations = 10000;
  } else if (n < 12000) {
    populationSize = 30;
    pairs = populationSize;
    iterations = 2000;
  } else {
    populationSize = 30;
    pairs = populationSize;
    iterations = 500;
  }

  mutationP = 0.5;
  stopCondition = iterations / 4;

  crossoverSize = populationSize + (pairs << 1);
  arraySize = (populationSize + (pairs << 1)) << 1;
  better = 0;
  disI.param(uniform_int_distribution<>::param_type{2, n-2});
  disI2.param(uniform_int_distribution<>::param_type{0, populationSize - 1});
  disI3.param(uniform_int_distribution<>::param_type{1, n-1});
  check1 = new bool[n];
  check2 = new bool[n];
  population = new solution_t*[arraySize];
  population[0] = copyFromBest();
  for (int i = 1; i < populationSize; i++) {
    population[i] = createRandomSolution();
  }
  for (int i = populationSize; i < arraySize; i++) {
    solution_t *solution = new solution_t;
    solution->order = new int[n + 1];
    solution->order[0] = 0;
    solution->order[n] = 0;
    solution->value = 0;
    population[i] = solution;
  }
}

inline permutation_t generatePermutation() {
  int a = disI3(generator);
  int b = disI3(generator);
  while (a == b) {
    b = disI3(generator);
  }
  permutation_t permutation;
  if (a > b) {
    permutation.a = b;
    permutation.b = a;
  } else {
    permutation.a = a;
    permutation.b = b;
  }
  return permutation;
}

inline void copyToBest(solution_t *solution) {
  theBestSolution.value = solution->value;
  memcpy(theBestSolution.order, solution->order, (n + 1) * sizeof(int));
  better = k;
}

void crossover() {
  for (int i = 0; i < pairs; i++) {
    int a = disI2(generator);
    int b;
    while ((b = disI2(generator)) == a);
    solution_t *p1 = population[a];
    solution_t *p2 = population[b];
    solution_t *c1 = population[populationSize + (i << 1)];
    solution_t *c2 = population[populationSize + (i << 1) + 1];
    int cutPoint = disI(generator);

    memset(check1, false, sizeof(bool) * n);
    check1[0] = true;
    memset(check2, false, sizeof(bool) * n);
    check2[0] = true;

    // copy cities before cut
    for (int j = 1; j < cutPoint; j++) {
      int ord1 = p1->order[j];
      c1->order[j] = ord1;
      check1[ord1] = true;
      int ord2 = p2->order[j];
      c2->order[j] = ord2;
      check2[ord2] = true;
    }

    // copy cities after cut
    int currentPosition1 = cutPoint;
    int currentPosition2 = cutPoint;
    for (int j = cutPoint; j < n; j++) {
      int ord2 = p2->order[j];
      if (!check1[ord2]) {
        c1->order[currentPosition1++] = ord2;
        check1[ord2] = true;
      }
      int ord1 = p1->order[j];
      if (!check2[ord1]) {
        c2->order[currentPosition2++] = ord1;
        check2[ord1] = true;
      }
    }

    // fix them
    int j = cutPoint;
    while ((j < n) && (currentPosition1 < n) && (currentPosition2 < n)) {
      int ord1 = p1->order[j];
      int ord2 = p2->order[j++];
      if (!check1[ord1]) {
        c1->order[currentPosition1++] = ord1;
        check1[ord1] = true;
      }
      if (!check2[ord2]) {
        c2->order[currentPosition2++] = ord2;
        check2[ord2] = true;
      }
    }
    if (currentPosition1 < n) {
      while ((j < n) && (currentPosition1 < n)) {
        int ord1 = p1->order[j++];
        if (!check1[ord1]) {
          c1->order[currentPosition1++] = ord1;
          check1[ord1] = true;
        }
      }
    }
    if (currentPosition2 < n) {
      while ((j < n) && (currentPosition2 < n)) {
        int ord2 = p2->order[j++];
        if (!check2[ord2]) {
          c2->order[currentPosition2++] = ord2;
          check2[ord2] = true;
        }
      }
    }
  }
  for (int i = populationSize; i < crossoverSize; i++) {
    double distance = calculateDistance(*population[i]);
    population[i]->value = distance;
    if (distance < theBestSolution.value) {
      copyToBest(population[i]);
    }
  }
}

int mutation() {
  int i = crossoverSize;
  for (int j = 0; j < crossoverSize; j++) {
    if (disD(generator) < mutationP) {
      permutation_t permutation = generatePermutation();
      population[i]->value = calculateNeighbourDistance(*population[j], permutation);
      swap2(population[i], population[j], permutation);
      i++;
    }
  }
  return i - crossoverSize;
}

int compare(const void* p1, const void* p2) {
  solution_t *s1 = *(solution_t **) p1;
  solution_t *s2 = *(solution_t **) p2;

  if (s1->value < s2->value) {
    return -1;
  } else if (s1->value > s2->value) {
    return 1;
  } else {
    return 0;
  }
}

void selection(int currentSize) {
  while (currentSize >= (populationSize << 1)) {
    random_shuffle(population, population + currentSize);
    int halfSize = currentSize >> 1;
    for (int i = 0; i < halfSize; i++) {
      solution_t *temp = population[i];
      if (population[(i << 1)]->value > population[(i << 1) + 1]->value) {
        population[i] = population[(i << 1) + 1];
        population[(i << 1) + 1] = temp;
      } else {
        population[i] = population[(i << 1)];
        population[(i << 1)] = temp;
      }
    }
    if (currentSize % 2 == 1) {
      solution_t *temp = population[halfSize];
      population[halfSize] = population[currentSize - 1];
      population[currentSize - 1] = temp;
      currentSize = halfSize + 1;
    } else {
      currentSize = halfSize;
    }
  }
  if (currentSize > populationSize) {
    qsort(population, currentSize, sizeof(solution_t *), compare);
  }
}

void search() {
  for (k = 0; k < iterations; k++) {
    crossover();
    int mutated = mutation();
    selection(crossoverSize + mutated);
    if (k - better >= stopCondition) {
      break;
    }
  }

  post(&theBestSolution, disI3);
  printResult();

  delete[] check1;
  delete[] check2;
  for (int i = 0; i < arraySize; i++) {
    delete[] population[i]->order;
    delete population[i];
  }
  delete[] population;
}
