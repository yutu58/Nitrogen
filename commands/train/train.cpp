#include <fstream>
#include <chrono>
#include <random>
#include "../../puzzles/puzzles.h"

using namespace std;

void generateCase(Puzzle& p, vector<string> algs, vector<string> preAUF, vector<string> postAUF, int minMoves, string& dest) {
    string setup = preAUF.at(rand() % preAUF.size()) + " " + algs.at(rand() % algs.size()) + " " + postAUF.at(rand() % postAUF.size());

    p.resetToSolved();
    p.applyMoves(setup);

    int maxDepth = 9;

    vector<string> scrambles;
    while (scrambles.size() < 20) {
        scrambles = solveToVec(p, setup, maxDepth);
        maxDepth++;
    }

    bool valid = false;
    string scramble;
    while (!valid) {
        scramble = scrambles.at(rand() % scrambles.size());

        if (scramble.size() > minMoves) {
            valid = true;
        }
    }
    dest = scramble;
}

bool train() {
    //Initialize random number seed
    srand(chrono::system_clock::now().time_since_epoch().count());

    string defFile;

    vector<string> algs;

    vector<string> preAUF;
    vector<string> postAUF;

    //SELECT A PUZZLE
    std::cout << "Path to the definition file: ";
    std::cin >> defFile;
    std::cout << "Preloading optimization tables, please wait..." << std::endl;

    Puzzle p = newPuzzle(defFile, 6);

    std::cout << "Puzzle " << p.name << "loaded!";


    //SELECT CASES
    std::cout << "Enter d when done" << std::endl;

    bool done = false;
    while (!done) {
        std::cout << "Path to (another) txt file to include: ";
        string temp;
        std::cin >> temp;

        if (temp == "d") {
            done = true;
        } else {
            ifstream file(temp);
            if (file.fail()) {
                std::cout << "Specified file does not exist, please try again" << std::endl;
                continue;
            }

            string temp2;
            while (getline(file, temp2)) {
                algs.push_back(temp2);
            }
        }
    }

    //Determine PreAUF and postAUF moves
//    std::cout << "What AUF moves can be used? (in the format \"X X' X2\")" << std::endl;
//    string aufs;
//    std::cin >> aufs;
//    vector<string> aufVec = strSplit(aufs, " ");
    preAUF = vector<string>{"", "U", "U'", "U2"};
    postAUF = vector<string>{"", "U", "U'", "U2"};

    std::cout << "All set to start, happy training!" << std::endl;
    std::cout << "Press q to leave when done" << std::endl;
    std::cin.ignore(INT_MAX, '\n');

    //TODO Increase font size?

    bool trainingDone = false;
    while (!trainingDone) {
        std::cin.clear();

        string setup;
        generateCase(p, algs, preAUF, postAUF, 7, setup);
        std::cout << setup << std::endl;

        string input;
        getline(cin, input);

        if (input == "q") {
            trainingDone = true;
            std::cout << "Going back to main menu..." << std::endl;
            continue;
        }
    }


    //Start()

    return true;
}