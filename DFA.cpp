//
// Created by Siebe Mees on 09/03/2023.
//

#include "DFA.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <queue>
#include "json.hpp"
using namespace std;

using json = nlohmann::json;

DFA::DFA() {}

DFA::DFA(const string inputFile) {
    // inlezen uit file
    ifstream input(inputFile);

    json j;
    input >> j;

    // Access the elements of the "alphabet" array
    string alfabet;
    for (const auto& letter : j["alphabet"]) {
        alfabet += letter;
    }
    setAlfabet(alfabet);

    // Access the elements of the "states" array
    vector<string> states;
    string startState;
    vector<string> acceptStates;
    for (const auto& state : j["states"]) {
        states.push_back(state["name"]);
        if (state["starting"] == true){
            startState = state["name"];
        }
        if (state["accepting"] == true){
            acceptStates.push_back(state["name"]);
        }
    }
    setStates(states);
    setAcceptStates(acceptStates);
    setStartState(startState);

    // Access the elements of the "transitions" array
    // Create a map to hold the transition function
    map<pair<string, char>, string> transitionFunction;
    // Access the elements of the "transitions" array
    for (const auto& transition : j["transitions"]) {
        // Get the "from" state
        string fromState = transition["from"];
        // Get the input character
        string input = transition["input"];
        char inputChar = input[0];
        // Get the "to" state
        string toState = transition["to"];
        // Add the transition to the map
        transitionFunction[make_pair(fromState, inputChar)] += toState;
    }
    setTransitionFunction(transitionFunction);
}

string statePairToString(const pair<string, string> &statePair) {
    return "(" + statePair.first + "," + statePair.second + ")";
}

bool hasInitialState(const DFA &dfa, const string &state) {
    return state == dfa.getStartState();
}

DFA::DFA(const DFA &dfa1, const DFA &dfa2, bool doorsnede) {
    // Alfabet
    for (char c1 : dfa1.getAlfabet()) {
        if (find(alfabet.begin(), alfabet.end(), c1) == alfabet.end()) {
            alfabet.push_back(c1);
        }
    }
    for (char c2 : dfa2.getAlfabet()) {
        if (find(alfabet.begin(), alfabet.end(), c2) == alfabet.end()) {
            alfabet.push_back(c2);
        }
    }

    // Transities & staten
    queue<pair<string, string>> stateQueue;
    set<pair<string, string>> visitedStates;

    stateQueue.push(make_pair(dfa1.getStartState(), dfa2.getStartState()));
    // 1. Voeg de sartstaen van beide DFA's samen
    startState = statePairToString(make_pair(dfa1.getStartState(), dfa2.getStartState()));

    // 2. Zoalang de queue niet leeg is pa je volgende logica toe
    while (!stateQueue.empty()) {
        // 2.1 Verwijder de element uit de queue
        pair<string, string> statePair = stateQueue.front();
        stateQueue.pop();

        // 2.2 Bepaal de transities waar we naartoe kunnen
        pair<string, string> reachableStates;
        for (char input : alfabet) {
            string nextState1 = dfa1.getTransition(statePair.first, input); // voeg het paar toe aan de set van toegevoegde staten
            string nextState2 = dfa2.getTransition(statePair.second, input);
            reachableStates = make_pair(nextState1, nextState2);
            // 2.2.1 Kijkt of het bereikbare statenpaar al bezocht is
            if (visitedStates.count(reachableStates) == 0) {
                stateQueue.push(reachableStates);
            }
            // 2.2.2 Maak de transitiefunctie
            transitionFunction[make_pair(statePairToString(statePair), input)] = statePairToString(reachableStates);
        }

        // 2.3 Voeg de staten toe aan de set van toegevoegde staten
        if (find(states.begin(), states.end(), statePairToString(statePair)) == states.end()) {
            states.push_back(statePairToString(statePair));
        }
        // 2.3.1 Bepaal of het een start/accepterende staat is
        bool isAcceptState = (dfa1.isAcceptState(statePair.first) && dfa2.isAcceptState(statePair.second));
        if (doorsnede) {
            if (isAcceptState) {
                acceptStates.push_back(statePairToString(statePair));
            }
        } else {
            if (isAcceptState || dfa1.isAcceptState(statePair.first) || dfa2.isAcceptState(statePair.second)) {
                acceptStates.push_back(statePairToString(statePair));
            }
        }
        visitedStates.insert(statePair);
    }
}

bool DFA::accepts(string input) {
    string currentState = startState;
    // Loop over the input
    for (char c : input) {
        // Kijkt of er een transitie bestaat op het staat, input paar
        if (transitionFunction.count({currentState, c}) == 0) {
            // Doe niets, current staat blijft hetzelfde
            currentState = currentState;
        }
        // Als er een transitiefunctie bestaat dan verander je de
        // current staat naar de nieuwe staat volgens de transitiefunctie
        currentState = transitionFunction[{currentState, c}];
    }
    // zoek of we uiteindelijk in een accept state zijn belandt
    return find(acceptStates.begin(), acceptStates.end(), currentState) != acceptStates.end();
}

void DFA::print(){
    // manueel aanmaken
    json j;
    // type
    j["type"] = "DFA";

    // alphabet
    vector<string> alphabetString;
    for (char c : alfabet) {
        alphabetString.push_back(string(1, c));
    }
    j["alphabet"] = alphabetString;

    // states
    json states_array = json::array();
    for (const auto& state : states) {
        json state_obj;
        state_obj["name"] = state;
        state_obj["starting"] = (state == startState);
        state_obj["accepting"] = (find(acceptStates.begin(), acceptStates.end(), state) != acceptStates.end());
        states_array.push_back(state_obj);
    }
    j["states"] = states_array;

    // transitions
    json transitions_array = json::array();
    for (const auto& transition : transitionFunction) {
        json transition_obj;
        transition_obj["from"] = transition.first.first;
        transition_obj["input"] = string(1, transition.first.second);
        transition_obj["to"] = transition.second;
        transitions_array.push_back(transition_obj);
    }
    j["transitions"] = transitions_array;

    // Print JSON object
    cout << setw(4) << j << endl;
}

// Setters
void DFA::setStates(const vector<string> &states) {
    DFA::states = states;
}

void DFA::addState(const std::string &state) {
    DFA::states.push_back(state);
}

void DFA::setAlfabet(const string &alfabet) {
    vector<char> chars;
    for (int i = 0; i < alfabet.size(); ++i) {
        chars.push_back(alfabet[i]);
    }
    DFA::alfabet = chars;
}

void DFA::setTransitionFunction(const map<pair<string, char>, string> &transitionFunction) {
    DFA::transitionFunction = transitionFunction;
}

void DFA::addTransition(const string &fromState, const char &input, const string &toState) {
    DFA::transitionFunction[{fromState, input}] = toState;
}

void DFA::setStartState(const string &startState) {
    DFA::startState = startState;
}

void DFA::setAcceptStates(const vector<string> &acceptStates) {
    DFA::acceptStates = acceptStates;
}

void DFA::addAcceptState(const std::string &state) {
    DFA::acceptStates.push_back(state);
}

const vector<string> &DFA::getStates() const {
    return states;
}

const vector<char> &DFA::getAlfabet() const {
    return alfabet;
}

const string &DFA::getStartState() const {
    return startState;
}

bool DFA::isAcceptState(const string &state) const {
    return find(acceptStates.begin(), acceptStates.end(), state) != acceptStates.end();
}

string DFA::getTransition(const string &fromState, const char &input) const {
    auto it = transitionFunction.find({fromState, input});
    if (it == transitionFunction.end()) {
        return "";
    } else {
        return it->second;
    }
}