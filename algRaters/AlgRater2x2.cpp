//This is a C++ version of an old algrater available at yutu58.github.io/algrater


#include <string>
#include <vector>
#include <unordered_map>
#include "AlgRater2x2.h"
#include "../util/util.h"

using namespace std;

void doTurn(const string& turn, int &Rthumb, string& lastMove, int &Points) {
    if (Rthumb == 0) {
        if (turn == "R"){
            Rthumb = 1;
            Points += 5;
        } else if (turn == "R'"){
            Rthumb = 3;
            Points += 5;
        } else if (turn == "R2"){
            Rthumb = 2;
            Points += 9;
        } else if (turn == "U"){
            Points += 5;
        } else if (turn == "U'"){
            Points += 5;
        } else if (turn == "U2"){
            Points += 7;
        } else if (turn == "F"){
            if (lastMove == "R'" || lastMove == "R") {
                Points += 11;
            } else {
                Points += 15;
            }
        } else if (turn == "F'"){
            if (lastMove == "R") {
                Points += 10;
            } else {
                Points += 14;}
        } else if (turn == "F2"){
            Points += 22;
        }
    }
    else if (Rthumb == 1) {
        if (turn == "R"){
            Rthumb = 2;
            Points += 9;
        } else if (turn == "R'"){
            Rthumb = 0;
            Points += 5;
        } else if (turn == "R2"){
            Rthumb = 3;
            Points += 9;
        } else if (turn == "U"){
            Points += 6;
        } else if (turn == "U'"){
            Points += 5;
        } else if (turn == "U2"){
            Points += 9;
        } else if (turn == "F"){
            Points += 17;
        } else if (turn == "F'"){
            if (lastMove == "R") {
                Points += 9;
            } else {
                Points += 14;
            }
        } else if (turn == "F2"){
            Points += 19;
        }
    }
    else if (Rthumb == 3) {
        if (turn == "R"){
            Rthumb = 0;
            Points += 5;
        } else if (turn == "R'"){
            Rthumb = 2;
            Points += 9;
        } else if (turn == "R2"){
            Rthumb = 1;
            Points += 8;
        } else if (turn == "U"){
            Points += 7;
        } else if (turn == "U'"){
            Points += 5;
        } else if (turn == "U2"){
            Points += 9;
        } else if (turn == "F"){
            Points += 5;
        } else if (turn == "F'"){
            Points += 8;
        } else if (turn == "F2"){
            Points += 7;
        }
    }
    else if (Rthumb == 2) {
        Rthumb = 0;
        Points += 20;
        doTurn(turn, Rthumb, lastMove, Points);
    }
    lastMove = turn;
}



int algRater2x2(const string& alg) {
    vector<string> moves = strSplit(alg, " ");

    int lowest = INT_MAX;

    for (int i = 0; i < 4; i++) {
        int Rthumb = i;
        int Points = 0;
        string lastMove;

        for (const string& s : moves) {
            doTurn(s, Rthumb, lastMove, Points);
        }

        if (Points < lowest) {
            lowest = Points;
        }
    }
    return lowest;
}


