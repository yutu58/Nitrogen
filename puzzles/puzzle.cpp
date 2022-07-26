//
// Created by Mattheo on 24/07/2022.
//

#include <tuple>
#include <string>
#include <map>
#include <fstream>
#include <utility>
#include <unordered_set>
#include "../util/util.cpp"

typedef struct Puzzle {
    int size;

    std::string name;

    std::map<std::string, std::tuple<int, int>> piecesMap;
    std::map<std::string, int> piecesIndexMap;

    std::map<int, int> orientationMap;

    std::map<std::string, int*> moveMap;

    int* solvedState;

    int* currentPos;

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

        //Check how many times a move can be done to return back to solved

        //Calculate powers based on that
    }

    void resetToSolved() {
        delete[](currentPos);
        currentPos = (int*)malloc(size * sizeof(int));
        memcpy(currentPos, solvedState, size * sizeof(int));
    }

    Puzzle(int size, string name, const map<std::string, std::tuple<int, int>> &piecesMap,
           const map<std::string, int> &piecesIndexMap, const map<std::string, int *> &moveMap, int *solvedState)
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
    }

    void del() {
        delete[](solvedState);
        delete[](currentPos);
    }
};

Puzzle newPuzzle(const std::string& def) {

    std::ifstream defFile(def);

    std::string line;

    std::map<std::string, std::tuple<int, int>> piecesMap;
    std::map<std::string, int> piecesIndexMap;
    std::map<int, int> orientationMap;

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
    return Puzzle(total, puzzleName, piecesMap, piecesIndexMap, moveMap, solvedState);
}
