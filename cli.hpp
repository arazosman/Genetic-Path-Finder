// A simple command line interface for the program.

#ifndef CLI_H
#define CLI_H

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h> // for Windows OS
#else
#include <unistd.h> // for Posix
#endif

// an array which represents directions, elements are left, up, right and down respectively
const int dir[4][2] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };

std::string drawLine(int N);
void printMaze(const std::vector <std::vector <char>> &maze)
{
	std::string line = drawLine(maze.size());
	std::cout << line;

	for (const auto &arr : maze)
	{
		std::cout << "\t| ";

		for (const char &val : arr)
			std::cout << val << " | ";

		std::cout << line;
	}

	std::cout << '\n';
}

std::string drawLine(int N)
{
	std::string line("\n\t");

	for (int i = 0; i < 4 * N + 1; ++i)
		line.push_back('-');

	return line + "\n";
}

void printBanner();
void printPattern(const std::string &pattern, int pos);
void waitForMilliSeconds(int milliSeconds);
void visualizePath(std::vector <std::vector <char>> &maze, const std::string &pattern, int fitness)
{
	int x = maze.size() / 2, y = x;
	printBanner();
	std::cout << "\tVisualizing the path:\n\n";
	std::cout << "\t(1: left, 2: up, 3: right, 4: down)\n";
	printPattern(pattern, -1);
	printMaze(maze);
	waitForMilliSeconds(1000);

	for (size_t i = 0; i < pattern.size(); ++i)
	{
		printBanner();
		std::cout << "\tVisualizing the path:\n\n";

		maze[x][y] = ' ';

		x += dir[pattern[i] - '1'][0];
		y += dir[pattern[i] - '1'][1];

		maze[x][y] = 'P';

		std::cout << "\t(1: left, 2: up, 3: right, 4: down)\n";
		printPattern(pattern, i);
		printMaze(maze);
		waitForMilliSeconds(1000);
	}

	std::cout << "\tLength of the path: " << pattern.size() << "\n";
	std::cout << "\tFitness of the path: " << fitness;
	std::cout << " (# of foods + distance between path length and M)\n\n";
}

void printPattern(const std::string &pattern, int pos)
{
	std::cout << "\n\t";

	for (int i = 0; i < pos; ++i)
		std::cout << pattern[i] << " ";

	if (pos >= 0)
		std::cout << "(" << pattern[pos] << ") ";

	for (size_t i = pos + 1; i < pattern.size(); ++i)
		std::cout << pattern[i] << " ";

	std::cout << "\n";
}

void clearScreen()
{
#ifdef _WIN32           // for Windows OS
	system("cls");
#else
	system("clear");    // for Posix
#endif
}

void waitForMilliSeconds(int milliSeconds)
{
	std::cout.flush();

#ifdef _WIN32                   // for Windows OS
	Sleep(milliSeconds);
#else
	usleep(1000 * milliSeconds);  // for Posix
#endif
}

void printBanner()
{
	clearScreen();
	printf("\n\t################################################\n");
	printf("\t############## Genetic Path Finder #############\n");
	printf("\t################################################\n\n");
}

#endif
