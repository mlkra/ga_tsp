#include "genetic.hpp"

#include "cities.hpp"
#include "solution.hpp"

#include <iostream>
#include <csignal>
#include <cstring>

using namespace std;

uniform_int_distribution<int> disI;
uniform_int_distribution<int> disI2;
uniform_real_distribution<double> disD(0.0, 1.0);

// size of population
int populationSize;
// number of pairs for crossover
int pairs;
// size of array with species
int arraySize;
// probability of mutation
double mutationP;

// kelper arrays with bools
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
  delete[] theBestSolution.order;
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

void initializeSearch() {
  calculateDistances();
  disI.param(uniform_int_distribution<>::param_type{2, n-2});
  // TODO change to something more dynamic
  populationSize = 10;
  disI2.param(uniform_int_distribution<>::param_type{0, populationSize - 1});
  pairs = 4;
  arraySize = (populationSize << 1) + (pairs << 2);
  mutationP = 0.05;
  check1 = new bool[n];
  check2 = new bool[n];
  theBestSolution = createNEHSolution();
  population = new solution_t*[arraySize];
  for (int i = 0; i < populationSize; i++) {
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
  for (int i = populationSize; i < populationSize + (pairs << 1); i++) {
    population[i]->value = calculateDistance(*population[i]);
  }
}

void search() {
  // add some for loop

  // finish implementing
  crossover();

  // DEBUG
  for (int i = 0; i < populationSize + (pairs << 1); i++) {
    for (int j = 0; j <= n; j++) {
      cout << population[i]->order[j] + 1 << " ";
    }
    cout << population[i]->value;
    cout << endl;
  }
  // not implemented yet
  // mutation();
  // selection();
}
