#include <iostream>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include <omp.h>

using namespace std;

struct Queen
{
    unsigned char x, y;
};

using Configuration = vector<Queen>;
string getSortedString(Configuration config)
{
    // configurations are already sorted
    string ret = "";
    for (const Queen &q : config)
    {
        ret += q.x;
        ret += q.y;
    }
    return ret;
}

unordered_set<string> existingConfigurations;

bool collides(const Queen &a, const Queen &b)
{
    return a.x == b.x ||
           a.y == b.y ||
           abs(a.x - b.x) == abs(a.y - b.y);
}

bool isValidPlacement(const Configuration &placedQueens, const Queen &newQueen)
{
    for (const Queen &placedQueen : placedQueens)
        if (collides(placedQueen, newQueen))
            return false;
    return true;
}

void printConfig(const Configuration &config)
{
    for (const Queen &q : config)
        printf("x: %d, y: %d\n", q.x, q.y);
    printf("\n\n");
}

void insertSorted(Configuration &newConfiguration, Queen &newQueen)
{
    Configuration::iterator it;
    for (it = newConfiguration.begin(); it < newConfiguration.end(); ++it)
    {
        if (newQueen.x < it->x)
            break;
        if (newQueen.x == it->x && newQueen.y == it->y)
            break;
    }

    newConfiguration.insert(it, newQueen);
}

int getNumSolutions(unsigned char boardSideLength, const Configuration &placedQueens)
{
    int numPlacedQueens = placedQueens.size();

    if (numPlacedQueens == boardSideLength)
        return 1;

    int ret = 0;
    for (unsigned char x = 0; x < boardSideLength; ++x)
    {
        for (unsigned char y = 0; y < boardSideLength; ++y)
        {
            Queen newQueen{x, y};
            if (!isValidPlacement(placedQueens, newQueen))
                continue;

            Configuration newConfiguration(placedQueens);
            insertSorted(newConfiguration, newQueen);
            // newConfiguration.push_back(newQueen);
            string sortedString = getSortedString(newConfiguration);

            if (existingConfigurations.count(sortedString) == 0)
            {
                existingConfigurations.insert(sortedString);
                ret += getNumSolutions(boardSideLength, newConfiguration);
            }
        }
    }

    return ret;
}

string dumpExistingConfigurations()
{
    string ret = "";
    for (const string &s : existingConfigurations)
        ret += s + "\n";
    return ret;
}

int main(int argc, char **argv)
{
    int N = 4;
    if (argc > 1)
        N = stoi(argv[1]);

    double time_before = omp_get_wtime();
    int numSolutions = getNumSolutions(N, {});
    double time_after = omp_get_wtime();

    printf("Number of solutions for %d-Queens: %d\nComputation took %lf seconds\n", N, numSolutions, time_after - time_before);
    return 0;
}