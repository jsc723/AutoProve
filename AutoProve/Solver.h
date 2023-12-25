#pragma once
#include "utils.h"
#include "SyntaxTree.h"

class Solver
{
public:
	Solver();
	~Solver();
	static bool provable(TreeNode *tree, bool showTruthTable) {
		set<string> symbols;
		tree->all_symbols(symbols);
		vector<string> list(symbols.begin(), symbols.end());
		unsigned int n = pow(2, list.size());
		bool good = true;
		for (uint32_t i = 0; i < n; i++) {
			for (int k = 0; k < list.size(); k++) {
				tree->assign(list[k], digit(i, k));
				
			}
			bool result = tree->eval();
			if (showTruthTable) {
				for (int k = 0; k < list.size(); k++)
					cout << list[k] << "=" << boolStr(digit(i, k)) << " ";
				cout << tree->toString() << "=" << boolStr(result) << endl;
			}
			if (!result) {
				good = false;
				if (!showTruthTable)
					return false;
			}
		}
		return good;
	}
	
private:
	//return a^n
	static unsigned int pow(int a, int n) {
		unsigned int p = 1;
		while (n-- != 0) {
			p *= a;
		}
		return p;
	}
	//return kth digit from left, k start from 0
	static unsigned int digit(uint32_t n, int k) {
		return (n << (31 - k)) >> 31;
	}

	static string boolStr(int i) {
		return i ? "T" : "F";
	}
};

