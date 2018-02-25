#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

class Solution {

public: 
	static void get_input(vector<int>& coins, int& target) {
		printf("Enter coins available (-1 to stop): \n");
		int input;
		do {
			printf(">> ");
			cin >> input;
			coins.push_back(input);
		} while(input != -1);
		coins.pop_back();

		printf("Enter target: ");
		cin >> target;
		printf("\n");
	}

	static vector<int> min_coins(const vector<int>& coins, const int& target) {
		vector<int> dp(target + 1, target);
		vector<int> path_to_success(target + 1);
		
		dp[0] = 0;
		for(int x = 1; x <= target; ++x) {
			int smallest = target;
			int best_coin = -1;
			for(int coin : coins) {
				if(x >= coin) {
					if(dp[x - coin] + 1 < smallest) {
						smallest = dp[x - coin] + 1;
						best_coin = coin;
					}
				}
			}
			dp[x] = smallest;
			path_to_success[x] = best_coin;
		}

		vector<int> coins_needed;
		int amount_left = target;
		while (amount_left > 0) {
			int next_coint = path_to_success[amount_left];
			if(next_coint == -1){
				/* If no solution, return emtpy vector. */
				return vector<int>(0);
			}
			coins_needed.push_back(next_coint);
			amount_left -= next_coint;
		}

		return coins_needed;
	}

	static void validate_solution(const vector<int>& solution) {
		if(solution.size() == 0) {
			printf("No solution found.\n");
		}
		else {
			printf("Minimum coins needed: %lu\n", solution.size());
			printf("Specifically: ");
			for(int coin : solution) {
				printf("%d, ", coin);
			}
			printf("\n");
		}
	}

};

int main () {
	vector<int> coins;
	int target;

	/* Get input */
	Solution::get_input(coins, target);

	vector<int> solution;
	if(coins.size() > 0 && target >= 0)
		solution = Solution::min_coins(coins, target);

	Solution::validate_solution(solution);

	return 0;
}

