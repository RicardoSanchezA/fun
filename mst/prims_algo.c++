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
};

struct compare_edge {
	bool operator()(const edge& lhs, const edge& rhs) {
		return lhs.weight < rhs.weight;
	}
};

class Prim_MST {

public: 
	static void get_mst(vector<list<edge>> adj_list) {
		set<int> s;
		vector<list<edge>> result(adj_list.size());
		my_heap<edge, compare_edge> h;

		int u = 0;
		s.insert(u);
		while(!adj_list[u].empty()) {
			edge e = adj_list[u].front();
			h.push(e);
			adj_list[u].pop_front();
		}

		while(s.size() != adj_list.size()) {
			edge _e = h.top();
			while(!h.empty() && s.count(_e.v)) {
				h.pop();
				_e = h.top();
			}
			result[_e.u].push_back(_e);
			s.insert(_e.v);

			u = _e.v;
			while(!adj_list[u].empty()) {
				edge e = adj_list[u].front();
				h.push(e);
				adj_list[u].pop_front();
			}
		}

		printf("MST: \n");
		for(auto l : result) {
			while(!l.empty()) {
				edge e = l.front();
				l.pop_front();
				printf("%d---[%d]---%d\n", e.u, e.weight, e.v);
			}
		}
	}
};


int main() {
	vector<list<edge>> adj_list;
	{
		edge e1;
		e1.u = 0; e1.v = 4; e1.weight = 10;
		edge e2;
		e2.u = 0; e2.v = 1; e2.weight = 1;
		edge e3;
		e3.u = 0; e3.v = 2; e3.weight = 4;
		edge my_edges[] { e1, e2, e3};
		list<edge> l(my_edges, my_edges + sizeof(my_edges) / sizeof(edge) );
		adj_list.push_back(l);
	}
	{
		edge e1;
		e1.u = 1; e1.v = 0; e1.weight = 1;
		edge e2;
		e2.u = 1; e2.v = 3; e2.weight = 2;
		edge my_edges[] { e1, e2};
		list<edge> l(my_edges, my_edges + sizeof(my_edges) / sizeof(edge) );
		adj_list.push_back(l);
	}
	{
		edge e1;
		e1.u = 2; e1.v = 0; e1.weight = 4;
		edge e2;
		e2.u = 2; e2.v = 3; e2.weight = 3;
		edge my_edges[] { e1, e2};
		list<edge> l(my_edges, my_edges + sizeof(my_edges) / sizeof(edge) );
		adj_list.push_back(l);
	}
	{
		edge e1;
		e1.u = 3; e1.v = 1; e1.weight = 2;
		edge e2;
		e2.u = 3; e2.v = 2; e2.weight = 3;
		edge my_edges[] { e1, e2};
		list<edge> l(my_edges, my_edges + sizeof(my_edges) / sizeof(edge) );
		adj_list.push_back(l);
	}
	{
		edge e1;
		e1.u = 4; e1.v = 0; e1.weight = 10;
		edge my_edges[] { e1 };
		list<edge> l(my_edges, my_edges + sizeof(my_edges) / sizeof(edge) );
		adj_list.push_back(l);
	}

	/* Test graph:
	    (4)---[10]---(0)---[1]---(1)
		              |           |
                     [4]         [2]
                      |           |
                     (2)---[3]---(3)
	*/

	Prim_MST::get_mst(adj_list);

	return 0;
}
