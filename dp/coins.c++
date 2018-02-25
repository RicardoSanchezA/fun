#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

class Solution {

public: 
	static int min_coins(vector<int>& coins, const int& target) {
		vector<int> dp(target + 1);
		dp[0] = 0;
		for(int x = 1; x <= target; ++x) {
			int baby = INT_MAX;
			for(int coin : coins) {
				if(x >= coin) {
					baby = min(baby, dp[x - coin] + 1);
				}
			}
			dp[x] = baby; 
		}


		return dp[target];
	}

};

int main () {
	vector<int> coins = {1,4,5};

	cout << Solution::min_coins(coins, 11) << endl;

	return 0;
}

