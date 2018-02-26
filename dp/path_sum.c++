#include <iostream>
#include <utility>
#include <vector>
#include <deque>
#include <climits>

/* set PRINT_PATH to 1 to print the path used to get the solution. */
#define PRINT_PATH 0

using namespace std;

class Solution {
	static pair<int,int> max_adj(const int& i, const vector<int>& row) {
		int max = INT_MIN;
		int max_idx = -1;

		if(i < row.size() && row[i] > max) {
			max = row[i];
			max_idx = i;
		}
		if(i >= 1 && row[i-1] > max) {
			max = row[i-1];
			max_idx = i-1;
		}
		return make_pair(max, max_idx);
	}
	static void build_path(int start, const vector<vector<int>>& history, deque<int>& path) {
		int row = history.size() - 1;
		while(row > 0) {
			path.push_front(start);
			start = history[row][start];
			--row;
		}
		path.push_front(start);
	}
public:
	static int max_sum(const vector<vector<int>>& triangle, deque<int>& path) {
		/* Corner case: empty input */
		if(triangle.size() < 1) { return -1; }

		vector<vector<int>> history(triangle.size());
		vector<vector<int>> dp(triangle.size());
		/* dp[i][j] = max./best sum of all paths that end at dp[i][j]. */
		/* Base case:  */
		dp[0].push_back(triangle[0][0]);
		/* Fill the dp matrix */
		for(int i = 1; i < triangle.size(); ++i) {
			for(int j = 0; j < triangle[i].size(); ++j) {
				pair<int,int> p = max_adj(j, dp[i-1]);
				history[i].push_back(p.second);
				dp[i].push_back(p.first + triangle[i][j]);
			}
		}
		/* Get the max. number of the bottom row of the dp matrix */
		int _max = INT_MIN;
		int _max_idx = -1;
		const vector<int>& last_row = dp[dp.size()-1];
		for(int i = 0; i < last_row.size(); ++i) {
			int leaf = last_row[i];
			if(leaf > _max) {
				_max = leaf;
				_max_idx = i;
			}
		}
		/* build the path followed to achieve the max. sum */
		build_path(_max_idx, history, path);

		return _max;
	}

	static void read_input(const int& rows, vector<vector<int>>& triangle) {
		int items = 1;
		for(int i = 0; i < rows; ++i, ++items) {
			for(int j = 0; j < items; ++j) {
				int num;
				cin >> num;
				triangle[i].push_back(num);
			}
		}
	}

	static void print_triangle(const vector<vector<int>>& triangle, const deque<int>& path) {
		for(int i = 0; i < triangle.size(); ++i) {
			for(int j = 0; j < triangle[i].size(); ++j) {
				if(j == path[i]) { printf("["); }
				printf("%d", triangle[i][j]);
				if(j == path[i]) { printf("]"); }
				printf(" ");
			}
			printf("\n");
		}
	}
};

int main() {

	int test_cases;
	cin >> test_cases;

	for(int i = 0; i < test_cases; ++i) {
		int rows;
		cin >> rows;

		vector<vector<int>> triangle(rows);
		Solution::read_input(rows, triangle);

		deque<int> path;
		int answer = Solution::max_sum(triangle, path);

		printf("%d\n", answer);
		if(PRINT_PATH)
			Solution::print_triangle(triangle, path);
	}


	return 0;
}
