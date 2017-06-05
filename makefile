all: main.cpp genetic.o solution.o cities.o
	g++ -O3 -std=c++11 main.cpp genetic.o solution.o cities.o -o main

genetic.o: genetic.cpp
	g++ -c -O3 -std=c++11 genetic.cpp

solution.o: solution.cpp
	g++ -c -O3 -std=c++11 solution.cpp

cities.o: cities.cpp
	g++ -c -O3 -std=c++11 cities.cpp

clean:
	rm -rf *.out *.o *.gch
	rm main
