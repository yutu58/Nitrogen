#ifndef NITROGEN_PUZZLES_H
#define NITROGEN_PUZZLES_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include "../util/util.h"

void solveAll2x2States(int maxLength, int nTopAlgs, std::ofstream &destFile);

struct Puzzle {
    int size;
    int preSearchDepth;

    int* buff;

    std::string name;

    std::unordered_map<std::string, std::tuple<int, int>> piecesMap;
    std::unordered_map<std::string, int> piecesIndexMap;

    std::unordered_map<long long, std::vector<std::string>> fromSolvedTable;

    std::unordered_map<int, int> orientationMap;
    std::unordered_map<std::string, std::string> moveInverseMap;

    std::unordered_map<std::string, int*> moveMap;

    int* solvedState;

    int* currentPos;

    //TODO: fix this to some better encoder type so it works 100%
    [[nodiscard]] long long hashPosition() const {
        long long total = 0;
        for (int i = 0; i < size; i++) {
            total = total * size + currentPos[i];
        }
        return total;
    }

    void applyMoves(const std::string& moves) {
        for (const std::string& s : strSplit(moves, " ")) {
            if (!s.empty()) {
                applyMove(s);
            }
        }
    }

    void applyMove(const std::string& move) {
        int* moveArr = moveMap.find(move)->second;

        memcpy(buff, currentPos, size * sizeof(int));

        for (auto const& [key, val] : piecesMap) {
            int startIndex = piecesIndexMap.find(key)->second;
            int amount = std::get<0>(val);
            int nOrientations = std::get<1>(val);

            if (nOrientations > 1) {
                for (int i = startIndex; i < startIndex+amount; i++) {
                    currentPos[i] = buff[moveArr[i] - 1];
                    currentPos[i+amount] = (buff[moveArr[i] + amount -1] + moveArr[i+amount]) % nOrientations;
                }
            } else {
                for (int i = startIndex; i < startIndex+amount; i++) {
                    currentPos[i] = buff[moveArr[i] - 1];
                }
            }
        }
    }

    [[nodiscard]] bool isSolved() const {
        bool solved = true;
        for (int i = 0; i < size; i++) {
            solved = solved && currentPos[i] == solvedState[i];
        }
        return solved;
    }

    void generateMovePowers() {
        std::unordered_map<std::string, int*> extraMoveMap;

        //Check how many times a move can be done to return back to solved
        for (auto const& [key, val] : moveMap) {
            this->resetToSolved();
            this->applyMove(key);
            int amount = 1;

            while (!this->isSolved()) {
                this->applyMove(key);
                amount++;
            }
            int* indexedSolvedState = new int[size];
            for (int i=0; i < size; i++) {
                if (orientationMap.count(i)) {
                    indexedSolvedState[i] = 0;
                } else {
                    indexedSolvedState[i] = i+1;
                }
            }

            //Create a "mock" puzzle with similar properties to check
            Puzzle p = Puzzle(size, preSearchDepth, name, piecesMap, piecesIndexMap, moveMap, indexedSolvedState, false);
            p.applyMove(key);
            moveInverseMap.insert({key, key+"'"});


            for (int i = 2; i < amount; i++) {
                p.applyMove(key);

                int* moveArray = new int[size];
                memcpy(moveArray, p.currentPos, size * sizeof(int));

                if (i < amount / 2) {
                    extraMoveMap.insert({key + std::to_string(i), moveArray});
                    moveInverseMap.insert({key + std::to_string(i), key + std::to_string(i) + "'"});
                } else if (i == amount / 2) {
                    extraMoveMap.insert({key + std::to_string(i), moveArray});
                    moveInverseMap.insert({key + std::to_string(i), key + std::to_string(i)});
                } else {
                    int n = amount - i;
                    if (n > 1) {
                        extraMoveMap.insert({key + std::to_string(i) + "'", moveArray});
                        moveInverseMap.insert({key + std::to_string(i) + "'", key + std::to_string(i)});
                    } else {
                        extraMoveMap.insert({key + "'", moveArray});
                        moveInverseMap.insert({key+"'", key});
                    }
                }
            }
            p.del();
        }
        moveMap.insert(extraMoveMap.begin(), extraMoveMap.end());
    }

    void resetToSolved() {
        delete[](currentPos);
        currentPos = (int*)malloc(size * sizeof(int));
        memcpy(currentPos, solvedState, size * sizeof(int));
    }


    //Note: this method only works for a single move
    //Use inverseAlg for inversion of an alg
    std::string inverseMove(const std::string& move) {
        return moveInverseMap.find(move)->second;
    }

    std::string inverseAlg(const std::string& alg) {
        std::vector<std::string> all = strSplit(alg, " ");
        std::reverse(all.begin(), all.end());
        std::string res = "";

        for (int i = 0; i < all.size(); i++) {
            res += inverseMove(all.at(i)) + " ";
        }
        return trim(res);
    }

    void calcFromSolved(std::string soFar2, const std::string& lastMove, int maxDepth) {
        if (maxDepth != 0) {
            for (auto const&[key, value] : moveMap) {
                if (!canCancel(key, lastMove)) {
                    applyMove(key);
                    std::string soFar = soFar2 + " " + key;
                    soFar = trim(soFar);

                    long long hash = hashPosition();

                    if (fromSolvedTable.count(hash) == 0) {
                        std::vector<std::string> vec;
                        vec.push_back(inverseAlg(soFar));
                        fromSolvedTable.insert({hash, vec});
                    } else {
                        auto& vec = fromSolvedTable.find(hash)->second;
                        vec.push_back(inverseAlg(soFar));
                    }

                    calcFromSolved(soFar, key, maxDepth - 1);
                    applyMove(inverseMove(key));
                }
            }
        }
        else {
            return;
        }
    }

    Puzzle(int size, int preSearchDepth, std::string name, const std::unordered_map<std::string, std::tuple<int, int>> &piecesMap,
           const std::unordered_map<std::string, int> &piecesIndexMap, const std::unordered_map<std::string, int *> &moveMap, int *solvedState, bool generatePowers)
            : size(size), preSearchDepth(preSearchDepth), name(std::move(name)), piecesMap(piecesMap), piecesIndexMap(piecesIndexMap), moveMap(moveMap),
              solvedState(solvedState) {
        currentPos = (int*)malloc(size * sizeof(int));
        buff = (int*) malloc(size * sizeof(int));
        memcpy(currentPos, solvedState, size * sizeof(int));

        for (auto const& [key, val] : piecesMap) {
            int nPieces = std::get<0>(val);
            int nOrientations = std::get<1>(val);
            if (nOrientations > 1) {
                int index = piecesIndexMap.find(key)->second;
                for (int i = index + nPieces; i < index + nPieces*2; i++) {
                    orientationMap.insert({i, nOrientations});
                }
            }
        }

        if (generatePowers) {
            this->generateMovePowers();
        }
    }

    void del() {
        delete[](solvedState);
        delete[](currentPos);
    }

    //This is a temporary method
    //TODO: Create a proper method for checking for cancellations
    static bool canCancel(std::string moveA, std::string moveB) {
        if (moveA.empty() || moveB.empty()) {
            return false;
        }
        moveA = removeNumbersAndApostrophs(moveA);
        moveB = removeNumbersAndApostrophs(moveB);
        return moveA == moveB;

    }
};
Puzzle newPuzzle(const std::string& def, int preSearchDepth);

void solveHelper(Puzzle& puzzle, const std::string& soFar2, const std::string& lastMove, int depthToGo, std::vector<std::string>& allSolutions);

std::vector<std::string> solveSetupToVec(Puzzle& puzzle, const std::string& setup, int maxDepth);

#endif //NITROGEN_PUZZLES_H
