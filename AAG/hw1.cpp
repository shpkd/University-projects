#ifndef __PROGTEST__
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <vector>

using State = unsigned;
using Symbol = uint8_t;
using Word = std::vector<Symbol>;

struct MISNFA {
    std::set<State> states;
    std::set<Symbol> alphabet;
    std::map<std::pair<State, Symbol>, std::set<State>> transitions;
    std::set<State> initialStates;
    std::set<State> finalStates;
};

struct DFA {
    std::set<State> states;
    std::set<Symbol> alphabet;
    std::map<std::pair<State, Symbol>, State> transitions;
    State initialState;
    std::set<State> finalStates;

    bool operator==(const DFA& x) const = default;
};
#endif

DFA determinizace(const MISNFA& nfa){
    DFA dfa;
    dfa.alphabet = nfa.alphabet;
    std::map<std::pair<std::set<State>, Symbol>, std::set<State>> transitionGraph;

    //BFS
    std::set<std::set<State>> visitedStates;
    std::queue<std::set<State>> stateQueue;


    std::set<State> initialSet(nfa.initialStates.begin(), nfa.initialStates.end());
    visitedStates.insert(initialSet);
    stateQueue.push(initialSet);

    std::map<std::set<State>, State> stateMapping;
    int nextDFAState = 0;
    stateMapping[initialSet] = nextDFAState++;
    dfa.initialState = stateMapping[initialSet];

    while (!stateQueue.empty()) {
        std::set<State> currentSet = stateQueue.front();
        stateQueue.pop();

        for (const auto & symbol : dfa.alphabet) {
            std::set<State> nextStateSet;

            for (const auto & state : currentSet) {
                auto transition = nfa.transitions.find({state, symbol});

                if (transition != nfa.transitions.end()) {
                    nextStateSet.insert(transition->second.begin(), transition->second.end());
                }
            }

            if (!nextStateSet.empty()) {
                transitionGraph[{currentSet, symbol}] = nextStateSet;
                if (visitedStates.find(nextStateSet) == visitedStates.end()) {
                    visitedStates.insert(nextStateSet);
                    stateQueue.push(nextStateSet);

                    stateMapping[nextStateSet] = nextDFAState++;
                }
            }
        }
    }



    //new transitions
    for(const auto & [statePair, target] : transitionGraph){
        State dfaSource=stateMapping[statePair.first];
        Symbol symbol=statePair.second;
        State dfaTarget = stateMapping[target];


        dfa.transitions[{dfaSource,symbol}]=dfaTarget;
    }


    for (const auto& [stateSet, dfaState] : stateMapping) {
        dfa.states.insert(dfaState);
    }

    for (const auto & [nfaStateSet, dfaState] : stateMapping){
        //new final state
        for (const auto & finalState : nfa.finalStates) {
            if (nfaStateSet.find(finalState) != nfaStateSet.end()) {
                dfa.finalStates.insert(dfaState);
            }
        }
    }

    return dfa;
}


//BFS zbytecny stavy
std::set<State> findUsefulStates(const DFA& dfa){
    std::set<State> usefulStates;
    std::queue<State> sQueue;

    for(const auto & finalState : dfa.finalStates){
        sQueue.push(finalState);
        usefulStates.insert(finalState);
    }

    while(!sQueue.empty()){
        auto currentState=sQueue.front();
        sQueue.pop();

        for(const auto & symbol : dfa.alphabet){
            for(const auto & [pairTrans, stateTarget] : dfa.transitions){
                if(pairTrans.second==symbol && stateTarget==currentState){
                    auto statePrev=pairTrans.first;

                    if(usefulStates.find(statePrev)==usefulStates.end()){
                        usefulStates.insert(statePrev);
                        sQueue.push(statePrev);
                    }
                }
            }
        }
    }
    return usefulStates;
}


DFA complement(const MISNFA& nfa){
    DFA dfa= determinizace(nfa);

    //uplny automat
    bool needQ0=false;
    std::map<std::pair<State, Symbol>, State> missingTransitions;


    for (const auto& state : dfa.states) {
        for (const auto& symbol : dfa.alphabet) {
            if (dfa.transitions.find({state, symbol}) == dfa.transitions.end()) {
                needQ0 = true;
                missingTransitions[{state, symbol}] = dfa.states.size();
            }
        }
    }

    if(needQ0){
        State q0 = dfa.states.size();
        dfa.states.insert(q0);

        for(const auto & key : missingTransitions){
            dfa.transitions[key.first]=q0;
        }

        for (const auto& symbol : dfa.alphabet) {
            dfa.transitions[{q0, symbol}] = q0;
        }
    }

    //doplnek
    std::set<State> stateFinalNew;

    for(const auto & state : dfa.states){
        if(dfa.finalStates.find(state)==dfa.finalStates.end()){
            stateFinalNew.insert(state);
        }
    }

    dfa.finalStates=stateFinalNew;


    //delete zbytecny
    std::set<State> usefulStates= findUsefulStates(dfa);

    std::set<State> updatedStates;
    std::map<std::pair<State, Symbol>, State> updateTransition;
    std::set<State> updateFinal;


    //new state
    for(const auto & state : dfa.states){
        if(usefulStates.find(state) != usefulStates.end()){
            updatedStates.insert(state);
        }
    }

    //new transitions
    for(const auto & [pairTrans, stateTarget] : dfa.transitions){
        if(usefulStates.find(pairTrans.first) != usefulStates.end() && usefulStates.find(stateTarget) != usefulStates.end()){
            updateTransition[pairTrans]=stateTarget;
        }
    }

    //new final states
    for(const auto & state : dfa.finalStates){
        if(usefulStates.find(state)!=usefulStates.end()){
            updateFinal.insert(state);
        }
    }

    //update dfa
    dfa.states=updatedStates;
    dfa.transitions=updateTransition;
    dfa.finalStates=updateFinal;

    //check if initial state zbytecny stav
    if (usefulStates.find(dfa.initialState) == usefulStates.end()) {
        dfa.states.clear();
        dfa.transitions.clear();
        dfa.finalStates.clear();
    }

    if(dfa.transitions.empty()){
        dfa.states.insert(0);
        dfa.initialState = 0;
        dfa.alphabet = nfa.alphabet;
        return dfa;
    }

    return dfa;
}

bool run(const DFA& dfa, const Word& word){
    State current=dfa.initialState;

    for(const auto & symbol : word){
        auto transition=dfa.transitions.find({current,symbol});

        if(transition!=dfa.transitions.end()){
            current=transition->second;
        }
        else{
            return false;
        }
    }

    return dfa.finalStates.find(current)!=dfa.finalStates.end();
}

#ifndef __PROGTEST__

int main()
{
    MISNFA in0 = {
            {0, 1, 2},
            {'e', 'l'},
            {
             {{0, 'e'}, {1}},
                    {{0, 'l'}, {1}},
                   {{1, 'e'}, {2}},
                    {{2, 'e'}, {0}},
                    {{2, 'l'}, {2}},
            },
            {0},
            {1, 2},
    };
    auto out0 = complement(in0);
    assert(run(out0, {}) == true);
    assert(run(out0, {'e', 'l'}) == true);

    assert(run(out0, {'l'}) == false);
    assert(run(out0, {'l', 'e', 'l', 'e'}) == true);
    MISNFA in1 = {
            {0, 1, 2, 3},
            {'g', 'l'},
            {
             {{0, 'g'}, {1}},
                    {{0, 'l'}, {2}},
                   {{1, 'g'}, {3}},
                      {{1, 'l'}, {3}},
                    {{2, 'g'}, {1}},
                    {{2, 'l'}, {0}},
                    {{3, 'l'}, {1}},
            },
            {0},
            {0, 2, 3},
    };
    auto out1 = complement(in1);
    assert(run(out1, {}) == false);
    assert(run(out1, {'g'}) == true);
    assert(run(out1, {'g', 'l'}) == false);
    assert(run(out1, {'g', 'l', 'l'}) == true);
    MISNFA in2 = {
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
            {'a', 'b'},
            {
             {{0, 'a'}, {1}},
                    {{0, 'b'}, {2}},
                   {{1, 'a'}, {3}},
                      {{1, 'b'}, {4}},
                         {{2, 'a'}, {5}},
                            {{2, 'b'}, {6}},
                               {{3, 'a'}, {7}},
                                  {{3, 'b'}, {8}},
                                     {{4, 'a'}, {9}},
                                        {{5, 'a'}, {5}},
                                           {{5, 'b'}, {10}},
                                               {{6, 'a'}, {8}},
                    {{6, 'b'}, {8}},
                    {{7, 'a'}, {11}},
                    {{8, 'a'}, {3}},
                    {{8, 'b'}, {9}},
                    {{9, 'a'}, {5}},
                    {{9, 'b'}, {5}},
                    {{10, 'a'}, {1}},
                    {{10, 'b'}, {2}},
                    {{11, 'a'}, {5}},
                    {{11, 'b'}, {5}},
            },
            {0},
            {5, 6},
    };
    auto out2 = complement(in2);
    assert(run(out2, {}) == true);
    assert(run(out2, {'a'}) == true);
    assert(run(out2, {'a', 'a', 'a', 'a', 'a', 'b'}) == true);
    assert(run(out2, {'a', 'a', 'a', 'c', 'a', 'a'}) == false);
    MISNFA in3 = {
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
            {'e', 'j', 'k'},
            {
             {{0, 'e'}, {1}},
                    {{0, 'j'}, {2}},
                    {{0, 'k'}, {3}},
                      {{1, 'e'}, {2}},
                         {{1, 'j'}, {4}},
                            {{1, 'k'}, {5}},
                               {{2, 'e'}, {6}},
                                  {{2, 'j'}, {0}},
                                     {{2, 'k'}, {4}},
                                        {{3, 'e'}, {7}},
                    {{3, 'j'}, {2}},
                    {{3, 'k'}, {1}},
                    {{4, 'e'}, {4}},
                    {{4, 'j'}, {8}},
                    {{4, 'k'}, {7}},
                    {{5, 'e'}, {4}},
                    {{5, 'j'}, {0}},
                    {{5, 'k'}, {4}},
                    {{6, 'e'}, {7}},
                    {{6, 'j'}, {8}},
                    {{6, 'k'}, {4}},
                    {{7, 'e'}, {3}},
                    {{7, 'j'}, {1}},
                    {{7, 'k'}, {8}},
                    {{8, 'e'}, {2}},
                    {{8, 'j'}, {4}},
                    {{8, 'k'}, {9}},
                    {{9, 'e'}, {4}},
                    {{9, 'j'}, {0}},
                    {{9, 'k'}, {4}},
            },
            {0},
            {1, 6, 8},
    };
    auto out3 = complement(in3);
    assert(run(out3, {}) == true);
    assert(run(out3, {'b', 'e', 'e'}) == false);
    assert(run(out3, {'e', 'e', 'e'}) == false);
    assert(run(out3, {'e', 'j'}) == true);
    assert(run(out3, {'e', 'k', 'j', 'e', 'j', 'j', 'j', 'e', 'k'}) == true);
    MISNFA in4 = {
            {0, 1, 2, 3, 4, 5},
            {'e', 'n', 'r'},
            {
             {{0, 'e'}, {1}},
                    {{0, 'n'}, {1}},
                    {{0, 'r'}, {2}},
                      {{1, 'e'}, {2}},
                         {{1, 'n'}, {3}},
                            {{1, 'r'}, {3}},
                    {{2, 'e'}, {3}},
                    {{2, 'n'}, {3}},
                    {{2, 'r'}, {1}},
                    {{3, 'e'}, {1}},
                    {{3, 'n'}, {1}},
                    {{3, 'r'}, {2}},
                    {{4, 'r'}, {5}},
            },
            {0},
            {4, 5},
    };
    auto out4 = complement(in4);
    assert(run(out4, {}) == true);
    assert(run(out4, {'e', 'n', 'r', 'e', 'n', 'r', 'e', 'n', 'r', 'e', 'n', 'r'}) == true);
    assert(run(out4, {'n', 'e', 'r', 'n', 'r', 'r', 'r', 'n', 'e', 'n', 'n', 'm', '\x0c', '\x20'}) == false);
    assert(run(out4, {'r', 'r', 'r', 'e', 'n'}) == true);
    MISNFA in5 = {
            {0, 1, 2, 3, 4, 5, 6},
            {'l', 'q', 't'},
            {
             {{0, 'l'}, {2, 4, 5}},
                    {{0, 'q'}, {2}},
                    {{0, 't'}, {1}},
                      {{1, 'l'}, {0, 2}},
                         {{1, 'q'}, {1, 4}},
                            {{1, 't'}, {0, 2}},
                               {{2, 'l'}, {5}},
                    {{2, 'q'}, {0, 4}},
                    {{2, 't'}, {0}},
                    {{3, 'l'}, {3, 4}},
                    {{3, 'q'}, {0}},
                    {{3, 't'}, {0, 2}},
                    {{4, 't'}, {4}},
                    {{5, 'l'}, {0, 2}},
                    {{5, 'q'}, {4, 5}},
                    {{5, 't'}, {0, 2}},
                    {{6, 'l'}, {4, 6}},
                    {{6, 'q'}, {0}},
                    {{6, 't'}, {0, 2}},
            },
            {2, 4},
            {0, 1, 3, 5, 6},
    };
    auto out5 = complement(in5);
    assert(run(out5, {}) == true);
    assert(run(out5, {'q', 'q'}) == true);
    assert(run(out5, {'q', 'q', 'l'}) == false);
    assert(run(out5, {'q', 'q', 'q'}) == false);
}
#endif