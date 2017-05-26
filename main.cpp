#include "genetic.hpp"

#include <iostream>
#include <ctime>
#include <csignal>
#include <wait.h>
#include <unistd.h>

using namespace std;

int main(int argc, char const *argv[]) {
  time_t start = time(NULL);
  // TODO load cities
  loadCities();
  int limit;
  cin >> limit;
  pid_t ppid = getpid();
  pid_t pid = fork();
  if (pid == 0) {
    time_t currentTime;
    while (1) {
      sleep(1);
      currentTime = time(NULL);
      if (currentTime - start >= limit - 1) {
        kill(ppid, SIGINT);
        break;
      }
    }
    wait(NULL);
  } else {
    // TODO start evolution
    initializeSearch();
    setupHandler();
    search();
    kill(pid, SIGINT);
  }
  return 0;
}
