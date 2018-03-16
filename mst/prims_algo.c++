#include <iostream>
#include <set>
#include <vector>
#include <list>

#include "../heap/my_heap.h"

using namespace std;

struct edge {
    int u;
    int v;
    int weight;

    edge() {}

    edge(const int& _u, const int& _v, const int& _w) : u(_u), v(_v), weight(_w) {}
};

struct compare_edge {
    bool operator()(const edge& lhs, const edge& rhs) {
        return lhs.weight < rhs.weight;
    }
};

class Graph {
    vector<list<edge>> adj_list;
    vector<edge> mst;
    int num_edges;
public: 
    Graph () : adj_list(), mst(), num_edges(0) {}
    void print_mst() {
        if(!mst.empty()) {
            printf("MST: \n");
            for(const edge& e : mst) {
                printf("(%d)---%d---(%d)\n", e.u, e.weight, e.v);
            }
        }
    }

    void prims_algo() {
        set<int> s;
        my_heap<edge, compare_edge> cheapest_edge;
        vector<list<edge>> adj_list_copy(adj_list);
        mst = vector<edge>();

        int u = 0;
        s.insert(0);
        do {
            while(!adj_list_copy[u].empty()) {
                edge e = adj_list_copy[u].front();
                cheapest_edge.push(e);
                adj_list_copy[u].pop_front();
            }
            edge _e = cheapest_edge.top();
            while(!cheapest_edge.empty() && s.count(_e.v)) {
                cheapest_edge.pop();
                _e = cheapest_edge.top();
            }
            mst.push_back(_e);
            u = _e.v;
            s.insert(u);
        } while (s.size() != adj_list_copy.size());
    
    }

    void add_edge(const int& u, const int& v, const int& w, const bool& doubly_linked = true) {
        while(u >= adj_list.size()) {
            adj_list.push_back(list<edge>());
        }
        edge e(u, v, w);
        adj_list[u].push_back(e);
        if(doubly_linked) {
            --num_edges;
            add_edge(v, u, w, false);
        }
        ++num_edges;
    }
};


int main() {

    Graph g;
    g.add_edge(0,4,10);
    g.add_edge(0,1,1);
    g.add_edge(0,2,4);

    g.add_edge(1,3,2);

    g.add_edge(2,3,3);

    /* Test graph:
        (4)---10---(0)---1---(1)
                    |         |
                    4         2
                    |         |
                   (2)---3---(3)
    */

    g.prims_algo();
    g.print_mst();

    return 0;
}
