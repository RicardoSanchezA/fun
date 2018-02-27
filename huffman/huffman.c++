#include <iostream>
#include <unordered_map>

#include "../heap/my_heap.h"

using namespace std;

struct node {
	char c;
	int freq;
	node* left;
	node* right;

	node() : c(-1), freq(0), left(nullptr), right(nullptr) {}
	node(const int& _fq) : c(-1), freq(_fq), left(nullptr), right(nullptr) {}
	node(const char& _c, const int& _fq) : c(_c), freq(_fq), left(nullptr), right(nullptr) {}
};

struct node_compare { 
    bool operator()(node* lhs, node* rhs) {
        return lhs->freq < rhs->freq;
    }
};

class Huffman {
	node* root;
	my_heap<node*, node_compare> h;
	vector<char> chars;
	vector<int> freq;
	unordered_map<char, string> code;
	void build_heap() {
		for(int i = 0; i < chars.size(); ++i) {
			node* n = new node(chars[i], freq[i]);
			h.push(n);
		}
	}
	void create_codes(string str, node* n) {
		if(n->left) {
			create_codes(str + "0", n->left);
		}
		if(n->right) {
			create_codes(str + "1", n->right);
		}
		if(!n->left && !n->right) {
			code[n->c] = str; 
		}
	}
	void build_tree() {
		while(h.size() > 1) {
			node* l = h.top();
			h.pop();
			node* r = h.top();
			h.pop();
			node* n = new node(l->freq + r->freq);
			n->left  = l;
			n->right = r;
			h.push(n);
		}
		if(h.empty()) { printf(" ** Something went wrong... **\n"); }
		else {
			root = h.top();
		}
	}
	char code_to_char(const string& str, int& idx) const {
		char c = -1;
		node* n = root;
		while(n) {
			if(n->c != -1) {
				c = n->c;
				break;
			}
			else if(str[idx] == '0') {
				n = n->left;
			}
			else if(str[idx] == '1') {
				n = n->right;
			}
			++idx;
		}
		return c;
	}
	void delete_tree(node* n) {
		if(n->left) {
			delete_tree(n->left);
		}
		if(n->right) {
			delete_tree(n->right);
		}
		delete n;
	}
public:
	Huffman() : root(nullptr), h(), chars(), freq(), code() {}
	Huffman(vector<char> _chars, vector<int> _freq) : root(nullptr), h(), chars(_chars), freq(_freq), code() {
		build_heap();
		build_tree();
		create_codes("", root);
	}
	void print_codes() const {
		printf(" char | code\n");
		for(auto it : code) {
			printf("  \'%c\' - %s\n", it.first, it.second.c_str());
		}
	}
	string encode(const string& message) {
		string result = "";
		for(char c : message) {
			if(code.find(c) != code.end()) {
				result += code[c];
			}
		}
		return result;
	}
	string decode(const string& message) const {
		string result = "";
		int i = 0;
		while(i < message.size()) {
			char c = code_to_char(message, i);
			result.push_back(c);
		}
		return result;
	}
	~Huffman() {
		delete_tree(root);
	}
};

vector<int> get_chars_and_freq(const string& message, vector<char>& chars, vector<int>& freq) {
	unordered_map<char, int> m;

	for(char c : message) { 
		++m[c];
	}

	for(auto it : m) {
		chars.push_back(it.first);
		freq.push_back(it.second);
	}
}


int main() {

	string message;
	printf("Enter message to encode: ");
	getline(cin, message);

	vector<char> chars;
	vector<int>  freq;
	get_chars_and_freq(message, chars, freq);

	Huffman huff(chars, freq);
	huff.print_codes();

	string e_message = huff.encode(message);
	printf("Encoded message: %s\n", e_message.c_str());
	printf("Decoded message: %s\n", huff.decode(e_message).c_str());

	return 0;
}