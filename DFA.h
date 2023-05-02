//
// Created by Siebe Mees on 09/03/2023.
//

#ifndef SUBSETCONSTRUCTION_DFA_H
#define SUBSETCONSTRUCTION_DFA_H

#include <string>
#include <vector>
#include <map>

using namespace std;

class DFA {
private:
    vector<string> states;
    vector<char> alfabet;
    map<pair<string, char>, string> transitionFunction;
    string startState;
    vector<string> acceptStates;
public:
    // Default constructor
    DFA();
    // Inputfile constructor
    DFA(const string inputFile);

    DFA(const DFA &dfa1, const DFA &dfa2, bool doorsnede);

    bool accepts(string input);

    void print();

    // Setters
    void setStates(const vector<string> &states);
    void addState(const string &state);
    void setAlfabet(const string &alfabet);
    void setTransitionFunction(const map<pair<string, char>, string> &transitionFunction);
    void addTransition(const string &fromState, const char &input, const string &toState);
    void setStartState(const string &startState);
    void setAcceptStates(const vector<string> &acceptStates);
    void addAcceptState(const string &state);

    // Getters
    const vector<string> &getStates() const;

    const vector<char> &getAlfabet() const;

    const string &getStartState() const;

    bool isAcceptState(const string &state) const;

    string getTransition(const string &fromState, const char &input) const;
};


#endif //SUBSETCONSTRUCTION_DFA_H
