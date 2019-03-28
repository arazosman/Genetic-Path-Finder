/*
    Artificial Intelligence - Assignment 1
    Genetic Path Finder

    @author:
    Name & Surname: Osman Araz
    Student ID: 16011020
    Delivery Date: 28.03.2019
*/

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>
#include "cli.hpp"

// a structure to keep path elements
struct Path
{
    std::string pattern; // path pattern, like "143241331242134"
    int weight; // # of eaten foods
    int fitness; // # of foods + distance between path length and initial path length
};

void clearScreen();
void fillMaze(std::vector <std::vector <char>> &maze, int N, int K);
void initilizePaths(std::vector <Path> &paths, std::vector <std::vector <char>> &maze, int M, int K);
std::vector <Path> evolution(std::vector <std::vector <char>> &maze, std::vector <Path> &paths, int K, int M);
void resizePatterns(std::vector <std::vector <char>> &maze, std::vector <Path> &paths, int K, int M);
int main()
{
    srand(time(NULL));

    int N, K, M; // N: matrix size, K: # of foods, M: length of paths

    printBanner();

    do
    {
        std::cout << "\tEnter the matrix size (N): ";
        std::cin >> N;
        std::cout << "\tEnter the number of foods (K): ";
        std::cin >> K;
        std::cout << "\tEnter the length of paths (M): ";
        std::cin >> M;

        if (K >= N*N)
            std::cout << "\n\tNumber of foods should be less than the number of cells in the maze!\n\n";
        
        if (M < 1)
            std::cout << "\n\tLength of the paths must be a positive integer!\n\n";
    }
    while (K >= N*N || M < 1);

    // generating the maze matrix, all cells initalized with a space character:
    std::vector <std::vector <char>> maze(N, std::vector <char> (N, ' '));

    printBanner();
    std::cout << "\tGenerated maze ('P' represents the hunter, '*'s represents foods):\n";
    fillMaze(maze, N, K); // filling the maze with foods
    printMaze(maze);
    std::cout << "\tTo contiune, press ENTER on your keyboard. ";
    getchar();
    getchar();

    printBanner();

    // generating and initializing 100 paths for the first generation:
    std::vector <Path> paths(100);
    initilizePaths(paths, maze, M, K);

    // sorting the paths accorrding to their fitnesses:
    std::sort(paths.begin(), paths.end(), [] (const Path &lhs, const Path &rhs) { return lhs.fitness > rhs.fitness; });

    int i = 1, j = 0;
    char ch = 'Y';

    // searching for a appropriate path:
    while (toupper(ch) == 'Y' && paths[0].weight < K)
    {
        // trying for 10000 generations
        while (j < 10000*i && paths[0].weight < K)
        {
            std::cout << "\tGeneration #" << ++j << ": " << paths[0].pattern << " (" << paths[0].fitness << ")\n";
            paths = evolution(maze, paths, K, M); // paths are evolving
            // sorting the paths accorrding to their fitnesses:
            std::sort(paths.begin(), paths.end(), [] (const Path &lhs, const Path &rhs) { return lhs.fitness > rhs.fitness; });
        }

        if (paths[0].weight < K) // if an appropriate path could not be found:
        {
            ++i;
            std::cout << "\n\tPath could not be found up to generation #" << j  << ".\n";
            std::cout << "\tIt may not be possible to find a path with the length you have specified.\n";
            std::cout << "\tDo you want continue to seek for a path? (Y/N): ";
            std::cin >> ch;
        }
    }

    if (paths[0].weight == K) // if a path found
    {
        Path shortestPath = paths[0]; // making the found path as the best path

        // if the length of the best path is less than the initial path length, then shrink lengths of all 
        // the paths to the length of the best path:
        if (shortestPath.pattern.size() < M)
            resizePatterns(maze, paths, K, M);

        do
        {
            // if the length of the found path is less than the best path length, then make the found path as
            // the best path an shrink lengths of all the paths to the length of the found path:
            if (paths[0].fitness > shortestPath.fitness)
            {
                shortestPath = paths[0];
                resizePatterns(maze, paths, K, M);
            }

            std::cout << "\n\n\t" << j << " generations have passed.\n";
            std::cout << "\tBest Path: " << shortestPath.pattern << "\n";
            std::cout << "\tPath length: " << shortestPath.pattern.size() << "\n";
            std::cout << "\tPath fitness: " << shortestPath.fitness << " (# of foods + distance between path length and M)\n";
            std::cout << "\n\tDo you want to continue to seek for a shorter path? (Y/N): ";

            std::cin >> ch;

            if (toupper(ch) == 'Y')
            {
                // trying for 1000 generations
                for (int i = 0; i < 1000 && paths[0].fitness <= shortestPath.fitness; ++i)
                {
                    std::cout << "\n\tGeneration #" << ++j << ": " << paths[0].pattern << " (fitness: " << paths[0].fitness << ")";
                    paths = evolution(maze, paths, K, M); // paths are evolving
                    // sorting the paths accorrding to their fitnesses:
                    std::sort(paths.begin(), paths.end(), [] (const Path &lhs, const Path &rhs) { return lhs.fitness > rhs.fitness; });
                }
            }
        }
        while (toupper(ch) == 'Y');

        visualizePath(maze, shortestPath.pattern, shortestPath.fitness); // visualizing the best path
    }
    else // informing the user to say there is no appropriate path
    {
        printBanner();
        std::cout << "\tCouldn't find a path to solve this maze with lentgh " << M << " :(\n";
        printMaze(maze);
    }

    return 0;
}

/*
    A function to fill the maze elements with foods.
    @param maze: the maze matrix
    @param N: size of the maze
    @param K: # of foods
*/
void fillMaze(std::vector <std::vector <char>> &maze, int N, int K)
{
    maze[N/2][N/2] = 'P';

    for (int i = 0; i < K; ++i)
    {
        // randomly selecting a cell in the maze:
        int x, y;

        // the cell must be empty
        do
        {
            x = rand() % N;
            y = rand() % N;
        }
        while (maze[x][y] != ' ');

        maze[x][y] = '*';
    }
}

/*
    A function to initialize paths.
    @param paths: array of the paths
    @param maze: the maze matrix
    @param M: the length of the initial path
    @param K: # of foods
*/
void fixPattern(std::string &pattern);
bool calculateFitness(std::vector <std::vector <char>> &maze, Path &path, int K, int M);
void initilizePaths(std::vector <Path> &paths, std::vector <std::vector <char>> &maze, int M, int K)
{
    for (Path &path: paths) // for every path
    {
        do
        {
            path.pattern.clear();

            // randomly inserting directions to the path
            for (int i = 0; i < M; ++i)
                path.pattern.push_back((1 + rand() % 4) + '0');

            // fixing the path if there is a consecutive recursion:
            fixPattern(path.pattern);
        }
        while (!calculateFitness(maze, path, K, M));
        // while the path is not dead (if crosses with walls)
    }
}

/*
    A function to evolve paths to make them beter and better.
    @param maze: the maze matrix
    @param paths: array of the paths
    @param K: # of foods
    @param M: the length of the initial path
    @return newPaths: the new generation
*/
void selectParents(int &p1, int &p2);
Path crossover(const std::string &parentPattern1, const std::string &parentPattern2);
std::vector <Path> evolution(std::vector <std::vector <char>> &maze, std::vector <Path> &paths, int K, int M)
{
    // generating the new generation
    std::vector <Path> newPaths(paths.size());

    for (int i = 0; i < paths.size(); ++i)
    {
        do
        {
            int p1, p2;
            selectParents(p1, p2); // selecting parents for each individual of new generation
            // parent genes crossovers to make the child:
            newPaths[i] = crossover(paths[p1].pattern, paths[p2].pattern);
            // fixing the path if there is a consecutive recursion:
            fixPattern(newPaths[i].pattern);
        }
        while (!calculateFitness(maze, newPaths[i], K, M));
        // while the path is not dead (if crosses with walls)
    }

    return newPaths;
}

/*
    A function to select parents randomly over the generation.
    @param p1: the first parent
    @param p2: the second parent
*/
void selectParents(int &p1, int &p2)
{
    if (rand() % 2 == 0) // with 0.5 probability
        p1 = rand() % 10, p2 = rand() % 10; // two of first 10 parents
    else if (rand() % 2 == 0) // with 0.5*0.5 probability
        p1 = rand() % 25, p2 = rand() % 25; // two of first 25 parents
    else  // with 0.5*0.5 probability
        p1 = rand() % 100, p2 = rand() % 100; // two of first 100 parents
}

// a function to check whether we cross with the wall or not:
inline bool isDead(int x, int y, int N) { return x < 0 || x >= N || y < 0 || y >= N; }

/*
    A function to calculate fitness value of a path.
    @param maze: the maze matrix
    @param path: the path
    @param K: # of foods
    @param M: the length of the initial path
    @return: whether the path is dead or not (true: alive, false: dead)
*/
bool calculateFitness(std::vector <std::vector <char>> &maze, Path &path, int K, int M)
{
    int j = 0, x = maze.size()/2, y = x;
    // a hash table to keep eaten foods to prevent eating a food for multiple times
    std::set <std::pair <int, int>> eatenFoods;

    while (j < path.pattern.size() && eatenFoods.size() < K)
    {
        // dir[][] array: {{0, -1}, {-1, 0}, {0, 1}, {1, 0}} -> left, up, right, down
        // moving the positions:
        x += dir[path.pattern[j]-'1'][0];
        y += dir[path.pattern[j]-'1'][1];

        // if we cross with walls, then we are dead :(
        if (isDead(x, y, maze.size()))
            return false;

        // if we find a food, then let's eat it:
        if (maze[x][y] == '*')
            eatenFoods.insert({x, y});

        ++j;
    }

    path.weight = eatenFoods.size();

    // if we eat all of the foods early (before reach to initial lentgh),
    // then our fitness value will be increased as the difference bitween
    // the initial length and the new length of the path,
    // and also we need to shrink the size of the path

    // if we couldn't eat foods before reach to initial length, the lines 
    // of codes below will also be harmless

    path.fitness = path.weight + M - j;
    path.pattern.resize(j);

    return true; // we are alive
}

/*
    A function which crossovers the patterns to create a new pattern.
    @param parentPattern1: parent pattern 1
    @param parentPattern2: parent pattern 2
    @return childPath: the created path
*/
Path crossover(const std::string &parentPattern1, const std::string &parentPattern2)
{
    Path childPath;

    // randomly selecting a threshold point,
    int threshold = rand() % parentPattern1.size();

    for (int i = 0; i < threshold; ++i)
    {
        if (rand() % 10 < 2) // mutation
            childPath.pattern.push_back((1 + rand() % 4) + '0');
        else // child gets a chromosome from the first parent
            childPath.pattern.push_back(parentPattern1[i]);
    }

    for (int i = threshold; i < parentPattern1.size(); ++i)
    {
        if (rand() % 10 < 2) // mutation
            childPath.pattern.push_back((1 + rand() % 4) + '0');
        else // child gets a chromosome from the first parent
            childPath.pattern.push_back(parentPattern2[i]);
    }

    // fixing the path if there is a consecutive recursion:
    fixPattern(childPath.pattern);

    return childPath;
}

/*
    A function which fixes consecutive recursions of paths.
    If the difference between consecutive directions is 2, then there is a recursion.
    For example, 1 represents left and 3 represents right. We "usually" don't want to 
    go somewhere and then immediately come back to the previous position.
    @param pattern: the path pattern
*/
void fixPattern(std::string &pattern)
{
    for (int i = 1; i < pattern.size(); ++i)
        if (abs(pattern[i]-pattern[i-1]) == 2)
            pattern[i] = (pattern[i] % 4) + 1 + '0';
}

/*
    A function to resize the lenght of the paths.
    @param maze: the maze matrix
    @param paths: array of the paths
    @param K: # of foods
    @param M: the length of the initial path
*/
void resizePatterns(std::vector <std::vector <char>> &maze, std::vector <Path> &paths, int K, int M)
{
    for (Path &path: paths)
    {
        path.pattern.resize(paths[0].pattern.size());
        calculateFitness(maze, path, K, M);
    }

    // sorting the paths accorrding to their fitnesses:
    std::sort(paths.begin(), paths.end(), [] (const Path &lhs, const Path &rhs) { return lhs.fitness > rhs.fitness; });
}