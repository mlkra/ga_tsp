#include "solution.hpp"

#include "cities.hpp"

#include <cstring>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <random>

using namespace std;

random_device rd;
mt19937 generator(rd());

solution_t *createRandomSolution() {
  uniform_int_distribution<int> disI(1, n - 1);
  solution_t *solution = new solution_t;
  int *order = new int[n+1];
  for (int i = 0; i < n; i++) {
    order[i] = i;
  }
  order[n] = 0;

  for (int i = 0; i < n*log(n); i++) {
    int a = disI(generator);
    int b = disI(generator);
    int temp = order[a];
    order[a] = order[b];
    order[b] = temp;
  }

  solution->order = order;
  solution->value = calculateDistance(*solution);

  return solution;
}

solution_t createNEHSolution() {
  solution_t solution;
  double value = 0;
  int *order = new int[n + 1];
  memset(order, -1, (n + 1) * sizeof(int));
  order[0] = 0; order[1] = 1; order[2] = 0;
  value += getDistance(0, 1) * 2;

  for (int i = 2; i < n; i++) {
    double min = FLT_MAX;
    int minPos;
    for (int j = 1; j <= i; j++) {
      double distance = getDistance(order[j-1], i) + getDistance(i, order[j]) - getDistance(order[j-1], order[j]);
      if (distance < min) {
        min = distance;
        minPos = j;
      }
    }
    for (int j = n; j > minPos; j--) {
      order[j] = order[j-1];
    }
    order[minPos] = i;
    value += min;
  }

  solution.order = order;
  solution.value = value;
  return solution;
}

solution_t *createNEHSolution2() {
  solution_t *solution = new solution_t;
  double value = 0;
  int *order = new int[n + 1];
  memset(order, -1, (n + 1) * sizeof(int));
  order[0] = 0; order[1] = 1; order[2] = 0;
  value += getDistance(0, 1) * 2;

  for (int i = 2; i < n; i++) {
    double min = FLT_MAX;
    int minPos;
    for (int j = 1; j <= i; j++) {
      double distance = getDistance(order[j-1], i) + getDistance(i, order[j]) - getDistance(order[j-1], order[j]);
      if (distance < min) {
        min = distance;
        minPos = j;
      }
    }
    for (int j = n; j > minPos; j--) {
      order[j] = order[j-1];
    }
    order[minPos] = i;
    value += min;
  }

  solution->order = order;
  solution->value = value;

  return solution;
}

double calculateDistance(solution_t solution) {
  double distance = 0;
  for (int i = 1; i <= n; i++) {
    distance += getDistance(solution.order[i-1], solution.order[i]);
  }
  return distance;
}

double calculateDistance2(solution_t solution) {
  double distance = 0;
  for (int i = 1; i <= n; i++) {
    city_t c1 = cities[solution.order[i-1]];
    city_t c2 = cities[solution.order[i]];
    distance += sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2));
  }
  return distance;
}

double calculateNeighbourDistance(solution_t solution, permutation_t permutation) {
  double distance = solution.value;
  distance -= getDistance(solution.order[permutation.a-1], solution.order[permutation.a]);
  distance -= getDistance(solution.order[permutation.b], solution.order[permutation.b+1]);
  distance += getDistance(solution.order[permutation.a-1], solution.order[permutation.b]);
  distance += getDistance(solution.order[permutation.a], solution.order[permutation.b+1]);
  return distance;
}

void swap(solution_t *solution, permutation_t permutation) {
  int length = (permutation.b - permutation.a + 1) / 2;
  for (int i = 0; i < length; i++) {
    int temp = solution->order[permutation.a+i];
    solution->order[permutation.a+i] = solution->order[permutation.b-i];
    solution->order[permutation.b-i] = temp;
  }
}

void swap2(solution_t *dest, solution_t *src, permutation_t permutation) {
  memcpy(dest->order, src->order, (permutation.a) * sizeof(int));
  int length = permutation.b - permutation.a + 1;
  for (int i = 0; i < length; i++) {
    dest->order[permutation.a + i] = src->order[permutation.b - i];
  }
  memcpy(dest->order + permutation.b + 1, src->order + permutation.b + 1, (n + 1 - (permutation.b + 1)) * sizeof(int));
}

inline permutation_t generatePermutation(uniform_int_distribution<int> disI3) {
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

void post(solution_t *solution, std::uniform_int_distribution<int> disI3) {
  int m;
  if (n < 7000) {
    m = 1000000;
  } else {
    m = 20000000;
  }
  for (int i = 0; i < m; i++) {
    permutation_t permutation = generatePermutation(disI3);
    double distance = calculateNeighbourDistance(*solution, permutation);
    if (distance < solution->value) {
      solution->value = distance;
      swap(solution, permutation);
    }
  }
}
