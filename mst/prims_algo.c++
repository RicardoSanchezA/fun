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
public: 
	void print_mst() {
		printf("MST: \n");
        for(const edge& e : mst) {
	        printf("(%d)---%d---(%d)\n", e.u, e.weight, e.v);
        }
	}

    void prims_algo() {
        set<int> s;
        my_heap<edge, compare_edge> cheapest_edge;

        mst = vector<edge>();

        int u = 0;
        s.insert(u);
        while(!adj_list[u].empty()) {
            edge e = adj_list[u].front();
            cheapest_edge.push(e);
            adj_list[u].pop_front();
        }

        while(s.size() != adj_list.size()) {
            edge _e = cheapest_edge.top();
            while(!cheapest_edge.empty() && s.count(_e.v)) {
                cheapest_edge.pop();
                _e = cheapest_edge.top();
            }
            mst.push_back(_e);
            s.insert(_e.v);

            u = _e.v;
            while(!adj_list[u].empty()) {
                edge e = adj_list[u].front();
                cheapest_edge.push(e);
                adj_list[u].pop_front();
            }
        }
    }
    void add_edge(const int& u, const int& v, const int& w) {
		while(u >= adj_list.size()) {
			adj_list.push_back(list<edge>());
		}
		edge e(u, v, w);
		adj_list[u].push_back(e);
	}
};


int main() {

	Graph g;
    g.add_edge(0,4,10);
    g.add_edge(0,1,1);
    g.add_edge(0,2,4);

    g.add_edge(1,0,1);
    g.add_edge(1,3,2);

    g.add_edge(2,0,4);
    g.add_edge(2,3,3);

    g.add_edge(3,1,2);
    g.add_edge(3,2,3);

	g.add_edge(4,0,10);

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
