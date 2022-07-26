#include <vector>
#include <algorithm>
#include <cctype>
#include "util.h"

using namespace std;

//source https://stackoverflow.com/a/46931770/14209629
vector<string> strSplit(const string& s, const string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

//source https://www.codevscolor.com/c-plus-plus-remove-all-numbers-string
string removeNumbersAndApostrophs(string str) {
    int current = 0;
    for(int i = 0; i< str.length(); i++){
        if(!isdigit(str[i]) && str[i] != '\''){
            str[current] = str[i];
            current++;
        }
    }

    return str.substr(0,current);
}

//source https://stackoverflow.com/a/17976541
std::string trim(const std::string &s) {
    auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
    auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
    return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}