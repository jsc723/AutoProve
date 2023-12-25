#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "Solver.h"
using namespace std;
#define readNewLine(in, line, ss)\
getline(in, line); \
ss.clear();  ss.str(line);

int main(int argc, char *argv[]) {
	if (argc != 2) {
		return 0;
	}
	vector< deque<string> > tokens = Lexer::tokenlize(argv[1]);
	TreeNode *conclusion = Parser::parse(*(tokens.end() - 1));
	Proof proof;
	set<int> dep;
	cout << "problem: ";
	for (int i = 0; i < tokens.size() - 1; i++) {
		TreeNode *premise = Parser::parse(tokens[i]);
		cout << premise->toString();
		if (i < tokens.size() - 2)
			cout << ", ";
		set<int> d;
		d.insert(i);
		dep.insert(i);
		ProofLine p(d, premise, "P");
		proof.push_back(p);
	}
	cout << " |- " << conclusion->toString() << endl;

	const int sz = proof.size();
	for (int i = 0; i < sz; i++) {
		const auto& p = proof[i];
		if (p.formula->type == "OrOpNode") {
			auto c = (OrOpNode *) p.formula;
			if (c->left->type == "NotNode") {
				auto inode = new IfOpNode(((NotNode*)c->left)->child, c->right);
				set<int> d{ p.dep };
				ProofLine p(d, inode, "P-alt");
				proof.push_back(p);
			}
			if (c->right->type == "NotNode") {
				auto inode = new IfOpNode(((NotNode*)c->right)->child, c->left);
				set<int> d{ p.dep };
				ProofLine p(d, inode, "P-alt");
				proof.push_back(p);
			}
		}
	}
	
	
	conclusion->pgen_checked(20, dep, proof);
	ProofLine::printProof(proof);
	system("pause");
	return 0; 
}