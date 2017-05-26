#include "cities.hpp"

#include <iostream>
#include <cmath>

using namespace std;

short n;
city_t* cities;
float **distances;

inline float calculateDistanceBetweenCities(int city1, int city2) {
  city_t c1 = cities[city1];
  city_t c2 = cities[city2];
  return sqrt(pow(c1.x - c2.x, 2) + pow(c1.y - c2.y, 2));
}

void calculateDistances() {
  distances = new float*[n];
  for (int i = 0; i < n; i++) {
    distances[i] = new float[n-i];
  }
  for (int i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      float distance = calculateDistanceBetweenCities(i, j);
      distances[i][j-i] = distance;
    }
  }
}

void loadCities() {
  cin >> n;
  cities = new city_t[n];
  for (int i = 0; i < n; i++) {
    cin >> cities[i].label;
    cin >> cities[i].x;
    cin >> cities[i].y;
  }
}

float getDistance(int c1, int c2) {
  if (c1 < c2) {
    return distances[c1][c2-c1];
  } else {
    return distances[c2][c1-c2];
  }
}
