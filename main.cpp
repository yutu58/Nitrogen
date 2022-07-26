#include <iostream>
#include <string>
#include "puzzles/puzzle.cpp"
#include "puzzles/solve.cpp"

using namespace std;

enum command_code {
    eQuit,
    eHelp,
    eGenerate,
    eTrain,
    eViewCase,
    eUnknown,
};

command_code hashCommand (std::string const& inString) {
    if (inString == "quit") return eQuit;
    if (inString == "help") return eHelp;
    if (inString == "generate") return eGenerate;
    if (inString == "train") return eTrain;
    if (inString == "viewCase") return eViewCase;
    return eUnknown;
}

int main() {
    Puzzle p = newPuzzle("defFilePath/2x2RUF.depth", 5);

    vector<string> solutions = solveToVec(p, "R U R' U R U2 R'", 7);

    for (const string& s : solutions) {
        std::cout << s << std::endl;
    }

    return 1;

//    bool running = true;
//
//    while (running) {
//        string fullCommand;
//        std::cin >> fullCommand;
//
//        string command = fullCommand.substr(0, fullCommand.find(' '));
//        switch (hashCommand(command)) {
//            case eQuit:
//                running = false;
//                break;
//            case eHelp:
//                help(command);
//                break;
//            case eGenerate:
//                generate(command);
//                break;
//            case eTrain:
//                train(command);
//                break;
//            case eViewCase:
//                viewCase(command);
//                break;
//            case eUnknown:
//                std::cout << "Unknown command: " << command << std::endl;
//                break;
//            default:
//                std::cout << "Unknown command: " << command << std::endl;
//        }
//    }
//    return 0;
}
