//
// Created by Mattheo on 24/07/2022.
//

#include <tuple>
#include <string>
#include <unordered_map>
#include <fstream>
#include <utility>
#include <vector>
#include "../util/util.h"
#include "puzzles.h"

using namespace std;

void solveAll2x2States();

Puzzle newPuzzle(const std::string& def, int preSearchDepth) {

    std::ifstream defFile(def);

    std::string line;

    std::unordered_map<std::string, std::tuple<int, int>> piecesMap;
    std::unordered_map<std::string, int> piecesIndexMap;

    std::unordered_map<std::string, int*> moveMap;

    int *solvedState = nullptr;

    int total = 0;

    std::string puzzleName;
    bool solvedStateDefined = false;

    while (std::getline(defFile, line)) {
        if (line.rfind("Name ", 0) == 0) {
            std::string command = "Name ";
            puzzleName = line.substr(command.size());
        } else if (line.rfind("Set ", 0) == 0) {

            if (solvedStateDefined) {
                //Invalid def file
                exit(400);
            }
            auto array = strSplit(line, " ");
            std::string pieceName = array[1];
            int pieceAmount = stoi(array[2]);
            int orientationAmount = stoi(array[3]);

            piecesMap.insert({pieceName, {pieceAmount, orientationAmount}});
            piecesIndexMap.insert({pieceName, total});

            if (orientationAmount != 1) {
                total += pieceAmount * 2;
            } else {
                total += pieceAmount;
            }
        } else if (line.rfind("Solved", 0) == 0) {
            int* stateArray = new int[total];

            bool done = false;

            while (!done) {
                std::string pieces;
                std::getline(defFile, pieces);

                if (pieces == "End") {
                    done = true;
                    continue;
                }

                std::tuple<int, int> amounts = piecesMap.find(pieces)->second;
                int index = piecesIndexMap.find(pieces)->second;
                int expectedNumbers = std::get<1>(amounts) > 1 ? std::get<0>(amounts) * 2 : std::get<0>(amounts);

                std::vector<int> posVec;

                //TODO Fix missing orientations here (in some way)
                while (posVec.size() < expectedNumbers) {
                    std::string positions;
                    std::getline(defFile, positions);
                    for (const std::string &v : strSplit(positions, " ")) {
                        posVec.push_back(stoi(v));
                    }
                }

                for (int i : posVec) {
                    stateArray[index] = i;
                    index++;
                }
            }

            solvedState = stateArray;
            solvedStateDefined = true;
        } else if (line.rfind("Move ", 0) == 0) {
            if (!solvedStateDefined) {
                //Invalid def file
                defFile.close();
                exit(400);
            }

            std::string moveName = strSplit(line, " ")[1];

            int* moveArray = new int[total];

            bool done = false;

            while (!done) {
                std::string pieces;
                std::getline(defFile, pieces);

                if (pieces == "End") {
                    done = true;
                    continue;
                }

                std::tuple<int, int> amounts = piecesMap.find(pieces)->second;
                int index = piecesIndexMap.find(pieces)->second;
                int expectedNumbers = std::get<1>(amounts) > 1 ? std::get<0>(amounts) * 2 : std::get<0>(amounts);

                std::vector<int> posVec;

                int offset = 0;

                //TODO Fix missing orientations here (in some way)
                while (posVec.size() < expectedNumbers) {
                    std::string positions;
                    std::getline(defFile, positions);
                    for (const std::string &v : strSplit(positions, " ")) {
                        if (offset >= std::get<0>(amounts)) {
                            posVec.push_back(stoi(v));
                        } else {
                            posVec.push_back(stoi(v) +index);
                        }
                        offset++;
                    }
                }

                for (int i : posVec) {
                    moveArray[index] = i;
                    index++;
                }
            }

            moveMap.insert({moveName, moveArray});
        } else if (line.rfind("Ignore ", 0) == 0) {
            //To Be Implemented
        } else if (line.rfind("Block ", 0) == 0) {
            //To Be Implemented
        } else if (line.rfind("ForbiddenPairs ", 0) == 0) {
            //To Be Implemented
        } else if (line.rfind("ForbiddenGroups ", 0) == 0) {
            //To Be Implemented
        } else if ((line.rfind('#', 0) == 0) || line.empty()) {
            //Do nothing
        } else {
            //invalid def file
            defFile.close();
            exit(400);
        }
    }
    defFile.close();

    Puzzle z = Puzzle(total, preSearchDepth, puzzleName, piecesMap, piecesIndexMap, moveMap, solvedState, true);
    z.calcFromSolved("", "", preSearchDepth);
    return z;
}
