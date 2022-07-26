#include <iostream>
#include <string>
#include "commands/commands.h"
#include "puzzles/puzzle.cpp"

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
    string path = "DefFile/Path";
    Puzzle p = newPuzzle(path + "2x2RUF.def");
    p.del();

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
