#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include "ArgumentManager.h"

using namespace std;

template <class t>
struct node {
    t data;
    node<t> *next;
};

template<class t>
class stack {
private:
    node<t>* head;

public:
    stack() {
       head = NULL;
    }

    t top() {
        return head->data;
    }

    void push(const t& d) {
        node<t> *n;
        n = new node<t>;
        n->data = d;
        n->next = head;
        head = n;
    }

    void pop() {
        node<t> *temp;
        if (head != NULL) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }

    bool empty() const {
        return head == NULL;
    }
};


int reduce(string line, map<char,int>& prev_map, bool first);
void parseFile(string inFileName, string outFileName);


void parseFile(string inFileName, string outFileName)  {
    ifstream inFile(inFileName);
    ofstream outFile(outFileName);
    int line_num = 0;
    bool errored = false;
    bool same = true;
    bool first_line;
    string line;
    map<char,int> previous;

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        line_num++;
        int similar = reduce(line, previous, first_line);
        if (similar == -1) {
            errored = true;
            outFile << "Error at: " << line_num << endl;
        }
        if (similar == 1) {
            same = false;
        }
        if (first_line) {
            first_line = false;
        }
    }

    if (errored) {
        return;
    }
    if (same) {
        outFile << "yes" << endl;
    }
    else {
        outFile << "no" << endl;
    }
}


int reduce(string line, map<char,int>& prev_map, bool first) {
    stack<char> brackets;
    stack<bool> pos;
    bool positive = true;
    char prev = ' ';
    char sign = ' ';
    int digit = 0;
    bool error = false;
    int change = 0;
    map<char,int> var_map;

    for (char c : line) {
        if (c == '\n') {
            continue;
        }
        else if (c == '+') {
            if (prev == '-') {
                sign = '-';
                c = '-';
            }
            else {
                sign = '+';
            }
        }
        else if (c == '-') {
            if (prev == '-') {
                sign = '+';
                c = '+';
            }
            else {
                sign = '-';
            }
        }
        else if (c == '(' || c == '{' || c == '[') {
            if (prev == '-') {
                positive = !positive;
                pos.push(false);
            }
            brackets.push(c);
        }
        else if (c == ')') {
            if (brackets.empty() || brackets.top() != '(') {
                return -1;
            }
            brackets.pop();
            if (!pos.empty()) {
                pos.pop();
            }
            if (pos.empty() && !positive) {
                positive = !positive;
            }
        }
        else if (c == ']') {
            if (brackets.empty() || brackets.top() != '[') {
                return -1;
            }
            brackets.pop();
            if (!pos.empty()) {
                pos.pop();
            }
            if (pos.empty() && !positive) {
                positive = !positive;
            }
        }
        else if (c == '}') {
            if (brackets.empty() || brackets.top() != '{') {
                return -1;
            }
            brackets.pop();
            if (!pos.empty()) {
                pos.pop();
            }
            if (pos.empty() && !positive) {
                positive = !positive;
            }
        }
        else {
            if (prev == '(' || prev == '{' || prev == '[') {
                sign = '+';
            }
            if (sign == '+' && !positive) {
                sign = '-';
            }
            else if (sign == '-' && !positive) {
                sign = '+';
            }
            else if (sign == ' ' && !positive) {
                sign = '-';
            }
            else if (sign == ' ') {
                sign = '+';
            }
            if (isdigit(c)) {
                if (sign == '+') {
                    digit += (c - '0');
                }
                else {
                    digit -= (c - '0');
                }
            }
            else if (isalpha(c)) {
                if (sign == '+') {
                    change = 1;
                }
                else {
                    change = -1;
                }
                if (var_map.count(c) > 0) {
                    var_map[c] += change;
                }
                var_map.insert({c, change});
            }
        }
        prev = c;
    }

    if (!brackets.empty()) {
        return -1;
    }

    var_map.insert({' ', digit});
    if (first) {
        prev_map = var_map;
        return 0;
    }

    for (int ci = 65; ci <= 122; ci++) {
        char cc = char(ci);
        if (!isalpha(cc)) {
            continue;
        }

        if (prev_map.count(cc) == 0 && var_map.count(cc) == 0) {
            continue;
        }

        if (prev_map.count(cc) > 0 && var_map.count(cc) == 0) {
            return 1;
        }

        if (prev_map.count(cc) == 0 && var_map.count(cc) > 0) {
            return 1;
        }

        if (prev_map[cc] != var_map[cc]) {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {

    ArgumentManager am(argc, argv);
    string inFileName = am.get("input");
    string outFileName = am.get("output");

    parseFile(inFileName, outFileName);

    return 0;
}
