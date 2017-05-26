#include "tsp.hpp"

#include "cities.hpp"
#include "solution.hpp"

#include <iostream>
#include <csignal>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <random>
#include <functional>
#include <ctime>
#include <cfloat>

#define divisor 64
#define alpha1 0.9999999
#define minTemp 100
#define maxN 3000
#define maxW 600000
#define maxR 10000
#define restart 15

using namespace std;

double initialTemperature;
solution_t theBestSolution;

uniform_real_distribution<double> disD(0.0, 1.0);
uniform_int_distribution<int> disI;

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

void initializeSearch() {
  disI.param(uniform_int_distribution<>::param_type{1, n-1});
  calculateDistances();
  initialTemperature = divisor;
  if (initialTemperature < minTemp) {
    initialTemperature = minTemp;
  }
  theBestSolution = createNEHSolution();
}

void setupHandler() {
  struct sigaction act;
  act.sa_handler = handler;
  sigaction(SIGINT, &act, NULL);
}

inline permutation_t generatePermutation() {
  int a = disI(generator);
  int b = disI(generator);
  while (a == b) {
    b = disI(generator);
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

void search() {
  auto doubleRand = bind(disD, generator);
  solution_t currentSulution;
  currentSulution.order = new int[n+1];
  memcpy(currentSulution.order, theBestSolution.order, (n+1)*sizeof(int));
  currentSulution.value = theBestSolution.value;
  double temperature = initialTemperature;
  int wCounter = 0;
  if (n < maxN) {
    while (temperature > 1) {
      permutation_t permutation = generatePermutation();
      double distance = calculateNeighbourDistance(currentSulution, permutation);
      double delta = distance - currentSulution.value;
      if (delta < 0) {
        swap(&currentSulution, permutation);
        currentSulution.value = distance;
        if (currentSulution.value < theBestSolution.value) {
          wCounter = 0;
          memcpy(theBestSolution.order, currentSulution.order, (n+1)*sizeof(int));
          theBestSolution.value = currentSulution.value;
        }
      } else if ((1 / (1 + exp(delta / temperature))) > doubleRand()) {
        wCounter++;
        swap(&currentSulution, permutation);
        currentSulution.value = distance;
      }
      temperature *= alpha1;
      if (wCounter > maxW) {
        break;
      }
    }
  } else {
    int restarts = 0;
    while (temperature > 1) {
      permutation_t permutation = generatePermutation();
      double distance = calculateNeighbourDistance(currentSulution, permutation);
      double delta = distance - currentSulution.value;
      if (delta < 0) {
        swap(&currentSulution, permutation);
        currentSulution.value = distance;
        if (currentSulution.value < theBestSolution.value) {
          restarts = 0;
          wCounter = 0;
          memcpy(theBestSolution.order, currentSulution.order, (n+1)*sizeof(int));
          theBestSolution.value = currentSulution.value;
        }
      } else if ((1 / (1 + exp(delta / temperature))) > doubleRand()) {
        wCounter++;
        swap(&currentSulution, permutation);
        currentSulution.value = distance;
      }
      if (wCounter > restart) {
        restarts++;
        wCounter = 0;
        memcpy(currentSulution.order, theBestSolution.order, (n+1)*sizeof(int));
        currentSulution.value = theBestSolution.value;
        if (restarts > maxR) {
          break;
        }
      }
      temperature *= alpha1;
    }
  }
  delete[] currentSulution.order;
  printResult();
}
