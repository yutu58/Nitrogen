#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "puzzles.h"
#include "../algRaters/AlgRater2x2.h"
#include "../util/util.h"


void solveHelper(Puzzle& puzzle, const std::string& soFar2, const std::string& lastMove, int depthToGo, std::vector<std::string>& allSolutions) {
    for (auto const& [key, value] : puzzle.moveMap) {
        if (Puzzle::canCancel(lastMove, key)) {
            continue;
        }

        puzzle.applyMove(key);
        std::string soFar = soFar2 + " " + key;
        soFar = trim(soFar);


        if (depthToGo - puzzle.preSearchDepth == 1) {
            if (puzzle.fromSolvedTable.count(puzzle.hashPosition())) {
                std::vector<std::string> solutions = puzzle.fromSolvedTable.find(puzzle.hashPosition())->second;
//                filter out all solutions that either cancel or < length 5
                for (const std::string& s : solutions) {
                    std::vector<std::string> moves = strSplit(s, " ");
                    if (moves.size() == puzzle.preSearchDepth && !Puzzle::canCancel(moves.at(0), key)) {
                        allSolutions.push_back(soFar + " " + s);
                    }
                }
            }
        } else {
            solveHelper(puzzle, soFar, key, depthToGo-1, allSolutions);
        }
        puzzle.applyMove(puzzle.inverseMove(key));
    }
}

std::vector<std::string> solveSetupToVec(Puzzle& puzzle, const std::string& setup, int maxDepth) {
    puzzle.resetToSolved();
    puzzle.applyMoves(setup);
    std::vector<std::string> res;

    if (puzzle.fromSolvedTable.count(puzzle.hashPosition())) {
        std::vector<std::string> solutions = puzzle.fromSolvedTable.find(puzzle.hashPosition())->second;

        //TODO: sort by length;
        res.insert(res.end(), solutions.begin(), solutions.end());
    }

    int startDepth = puzzle.preSearchDepth + 1;

    for (int i = startDepth; i <= maxDepth; i++) {
//        std::cout << "Searching depth: " << i << std::endl;
        solveHelper(puzzle, "", "", i, res);
    }
    return res;
}

bool next2x2Case(int* p) {
    if (p[7] == 2 &&
        p[8] == 2 &&
        p[9] == 2 &&
        p[10] == 2 &&
        p[11] == 2 &&
        p[12] == 2 &&
        p[13] == 1) {
        p[7] = 0;
        p[8] = 0;
        p[9] = 0;
        p[10] = 0;
        p[11] = 0;
        p[12] = 0;
        p[13] = 0;
        return std::next_permutation(p, p+4);
    } else {
        //Next orientation
        p[12] += 1;
        p[13] = (p[13] + 2) % 3;

        for (int i = 13; i > 7; i--) {
            if (p[i] == 3) {
                p[i] = 0;
                p[i-1] += 1;
                p[13] = (p[13] + 2) % 3;
            }
        }
    }
    return true;

}

void solveAll2x2States(int maxLength, int nTopAlgs, std::ofstream &destFile) {
    Puzzle puzzle = newPuzzle(R"(Q:\CubingSoftware\Nitrogen\puzzles\2x2RUF.def)", 4);
    std::cout << "done preloading" << std::endl;

    if (!destFile.is_open()) {
        return;
    }

    long casesDone = 0;
    int solvedState[]{1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0};

    while (next2x2Case(solvedState)) {
        long long hash = 0;
        memcpy(puzzle.currentPos, solvedState, puzzle.size * sizeof(int));
        hash = puzzle.hashPosition();

        std::vector<std::string> res;

        if (puzzle.fromSolvedTable.count(puzzle.hashPosition())) {
            std::vector<std::string> solutions = puzzle.fromSolvedTable.find(puzzle.hashPosition())->second;
            res.insert(res.end(), solutions.begin(), solutions.end());
        }

        for (int i = puzzle.preSearchDepth + 1; i <= maxLength; i++) {

            if (res.size() > 100) {
                continue;
            }
            solveHelper(puzzle, "", "", i, res);
        }

        rateAndSort(res);

        destFile << '"' << std::to_string(hash) << '"' << ":[\"";
        for (int i = 0; i < nTopAlgs; i++) {
            if (i >= res.size()) continue;
            destFile << res.at(i) << ",";
        }
        destFile << "\"]" << std::endl;

        casesDone++;

        if (casesDone % 100 == 0) {
            std::cout << casesDone << " cases done!" << std::endl;
        }
    }
}