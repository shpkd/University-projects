#ifndef __PROGTEST__
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <vector>
#include <map>

using namespace std;
using Symbol = char;
using Word = std::vector<Symbol>;

struct Grammar {
    std::set<Symbol> m_Nonterminals;
    std::set<Symbol> m_Terminals;
    std::vector<std::pair<Symbol, std::vector<Symbol>>> m_Rules;
    Symbol m_InitialSymbol;
};
#endif


std::vector<size_t> trace(const Grammar& grammar, const Word& word){
    vector<vector<map<Symbol,vector<size_t>>>> position(word.size(),vector<map<Symbol,vector<size_t>>>(word.size()));
    vector<size_t> result;
    if(word.empty()){
        for(size_t i = 0; i< grammar.m_Rules.size(); i++){
            if(grammar.m_Rules[i].second.empty()){
                result.push_back(i);
                return result;
            }
        }
        return result;
    }

    for(size_t i=0; i<word.size(); i++){
        for(size_t j=0; j<grammar.m_Rules.size(); j++){
            if(grammar.m_Rules[j].second.size()==1 && grammar.m_Rules[j].second[0]== word[i]){
                position[i][0][grammar.m_Rules[j].first].push_back(j);
            }
        }
    }
    for (size_t i=1; i<word.size(); i++){
        for(size_t j=0; j<word.size()-i; j++){
            for(size_t k=0;k<=i-1; k++){
                for(auto l:position[j][k]){
                   for(auto m:position[j+k+1][i-k -1]){
                       for(size_t d=0; d<grammar.m_Rules.size(); d++){
                           if(grammar.m_Rules[d].second.size()==2 && grammar.m_Rules[d].second[0] == l.first && grammar.m_Rules[d].second[1] == m.first){
                               if(position[j][i].find(grammar.m_Rules[d].first) == position[j][i].end()){
                                   position[j][i][grammar.m_Rules[d].first].push_back(d);
                                   for(auto p : l.second){
                                       position[j][i][grammar.m_Rules[d].first].push_back(p);
                                   }
                                   for(auto p : m.second){
                                       position[j][i][grammar.m_Rules[d].first].push_back(p);
                                   }
                               }
                           }
                       }
                   }
                }

            }

        }
    }
    for(auto n : position[0][word.size()-1]){
        if(n.first==grammar.m_InitialSymbol)
            return n.second;
    }
    return result;

}




#ifndef __PROGTEST__
int main()
{
    Grammar g0{
            {'A', 'B', 'C', 'S'},
            {'a', 'b'},
            {
                    {'S', {'A', 'B'}},
                    {'S', {'B', 'C'}},
                    {'A', {'B', 'A'}},
                    {'A', {'a'}},
                    {'B', {'C', 'C'}},
                    {'B', {'b'}},
                    {'C', {'A', 'B'}},
                    {'C', {'a'}},
            },
            'S'};

//    assert(trace(g0, {'b', 'a', 'a', 'b', 'a'}) == std::vector<size_t>({0, 2, 5, 3, 4, 6, 3, 5, 7}));
//    assert(trace(g0, {'b'}) == std::vector<size_t>({}));
//    assert(trace(g0, {'a'}) == std::vector<size_t>({}));
    assert(trace(g0, {}) == std::vector<size_t>({}));
//    assert(trace(g0, {'a', 'a', 'a', 'a', 'a'}) == std::vector<size_t>({1, 4, 6, 3, 4, 7, 7, 7, 7}));
    assert(trace(g0, {'a', 'b'}) == std::vector<size_t>({0, 3, 5}));
    assert(trace(g0, {'b', 'a'}) == std::vector<size_t>({1, 5, 7}));
//    assert(trace(g0, {'c', 'a'}) == std::vector<size_t>({}));

    Grammar g1{
            {'A', 'B'},
            {'x', 'y'},
            {
                    {'A', {}},
                    {'A', {'x'}},
                    {'B', {'x'}},
                    {'A', {'B', 'B'}},
                    {'B', {'B', 'B'}},
            },
            'A'};

    assert(trace(g1, {}) == std::vector<size_t>({0}));
    assert(trace(g1, {'x'}) == std::vector<size_t>({1}));
    assert(trace(g1, {'x', 'x'}) == std::vector<size_t>({3, 2, 2}));
//    assert(trace(g1, {'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 2, 2, 2}));
//    assert(trace(g1, {'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 2, 2, 2, 2}));
//    assert(trace(g1, {'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 2, 2, 2, 2, 2}));
//    assert(trace(g1, {'x', 'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2}));
//    assert(trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2}));
//    assert(trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2}));
//    assert(trace(g1, {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}) == std::vector<size_t>({3, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2}));

    Grammar g2{
            {'A', 'B'},
            {'x', 'y'},
            {
                    {'A', {'x'}},
                    {'B', {'x'}},
                    {'A', {'B', 'B'}},
                    {'B', {'B', 'B'}},
            },
            'B'};

    assert(trace(g2, {}) == std::vector<size_t>({}));
    assert(trace(g2, {'x'}) == std::vector<size_t>({1}));
    assert(trace(g2, {'x', 'x'}) == std::vector<size_t>({3, 1, 1}));
    assert(trace(g2, {'x', 'x', 'x'}) == std::vector<size_t>({3, 3, 1, 1, 1}));

    Grammar g3{
            {'A', 'B', 'C', 'D', 'E', 'S'},
            {'a', 'b'},
            {
                    {'S', {'A', 'B'}},
                    {'S', {'S', 'S'}},
                    {'S', {'a'}},
                    {'A', {'B', 'S'}},
                    {'A', {'C', 'D'}},
                    {'A', {'b'}},
                    {'B', {'D', 'D'}},
                    {'B', {'b'}},
                    {'C', {'D', 'E'}},
                    {'C', {'b'}},
                    {'C', {'a'}},
                    {'D', {'a'}},
                    {'E', {'S', 'S'}},
            },
            'S'};

//    assert(trace(g3, {}) == std::vector<size_t>({}));
//    assert(trace(g3, {'b'}) == std::vector<size_t>({}));
//    assert(trace(g3, {'a', 'b', 'a', 'a', 'b'}) == std::vector<size_t>({1, 2, 0, 3, 7, 1, 2, 2, 7}));
//    assert(trace(g3, {'a', 'b', 'a', 'a', 'b', 'a', 'b', 'a', 'b', 'a', 'a'}) == std::vector<size_t>({1, 1, 0, 4, 8, 11, 12, 0, 5, 6, 11, 11, 0, 4, 9, 11, 7, 11, 7, 2, 2}));
}
#endif

