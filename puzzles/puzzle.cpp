//
// Created by Mattheo on 24/07/2022.
//

#include <tuple>
#include <string>
#include <map>
#include <unordered_map>
#include <fstream>
#include <utility>
#include "../util/util.cpp"



struct Puzzle {
    int size;

    std::string name;

    std::map<std::string, std::tuple<int, int>> piecesMap;
    std::map<std::string, int> piecesIndexMap;

    std::unordered_map<int, vector<string>> fromSolvedTable;

    std::map<int, int> orientationMap;
    std::map<std::string, std::string> moveInverseMap;

    std::map<std::string, int*> moveMap;

    int* solvedState;

    int* currentPos;

    int hashPosition(const int* pos) const {
        int total = 0;
        for (int i = 0; i < size; i++) {
            total = total * size + pos[i];
        }
        return total;
    }

    void applyMove(const std::string& move) {
        int* moveArr = moveMap.find(move)->second;

        int* buff = (int*)malloc(size * sizeof(int));
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

    bool isSolved() const {
        bool solved = true;
        for (int i = 0; i < size; i++) {
            solved = solved && currentPos[i] == solvedState[i];
        }
        return solved;
    }

    void generateMovePowers() {
        std::map<std::string, int*> extraMoveMap;

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

            Puzzle p = Puzzle(size, name, piecesMap, piecesIndexMap, moveMap, indexedSolvedState, false);
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
    string inverseMove(const std::string& move) {
        return moveInverseMap.find(move)->second;
    }

    string inverseAlg(const std::string& alg) {
        vector<string> all = strSplit(alg, " ");
        std::reverse(all.begin(), all.end());
        std::string res = "";

        for (int i = 0; i < all.size(); i++) {
            res += inverseMove(all.at(i)) + " ";
        }
        return trim(res);
    }

    void calcFromSolved(std::string soFar, const std::string& lastMove, int maxDepth) {
        if (maxDepth != 0) {
            for (auto const&[key, value] : moveMap) {
                if (!canCancel(key, lastMove)) {
                    applyMove(key);
                    soFar += " " + key;

                    //Add soFar to hashmap
                    int hash = hashPosition(currentPos);

                    if (fromSolvedTable.count(hash) == 0) {
                        vector<std::string> vec;
                        vec.push_back(soFar);
                        fromSolvedTable.insert({hash, vec});
                    } else {
                        auto vec = fromSolvedTable.find(hash)->second;
                        vec.push_back(soFar);
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

    Puzzle(int size, string name, const map<std::string, std::tuple<int, int>> &piecesMap,
           const map<std::string, int> &piecesIndexMap, const map<std::string, int *> &moveMap, int *solvedState, bool generatePowers)
            : size(size), name(std::move(name)), piecesMap(piecesMap), piecesIndexMap(piecesIndexMap), moveMap(moveMap),
              solvedState(solvedState) {
        currentPos = (int*)malloc(size * sizeof(int));
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

Puzzle newPuzzle(const std::string& def, int preSearchDepth) {

    std::ifstream defFile(def);

    std::string line;

    std::map<std::string, std::tuple<int, int>> piecesMap;
    std::map<std::string, int> piecesIndexMap;

    std::map<std::string, int*> moveMap;

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

                vector<int> posVec;

                //TODO Fix missing orientations here (in some way)
                while (posVec.size() < expectedNumbers) {
                    std::string positions;
                    std::getline(defFile, positions);
                    for (const string &v : strSplit(positions, " ")) {
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

                vector<int> posVec;

                int offset = 0;

                //TODO Fix missing orientations here (in some way)
                while (posVec.size() < expectedNumbers) {
                    std::string positions;
                    std::getline(defFile, positions);
                    for (const string &v : strSplit(positions, " ")) {
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

    Puzzle z = Puzzle(total, puzzleName, piecesMap, piecesIndexMap, moveMap, solvedState, true);
    z.calcFromSolved("", "", preSearchDepth);
    return z;
}
