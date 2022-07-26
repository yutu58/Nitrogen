#include <map>
#include <unordered_map>

void solveHelper(Puzzle puzzle, const std::string& soFar2, const std::string& lastMove, int depthToGo, std::vector<std::string>& allSolutions) {
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

std::vector<std::string> solveToVec(Puzzle& puzzle, const std::string& setup, int maxDepth) {
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
        solveHelper(puzzle, "", "", i, res);
    }
    return res;
}