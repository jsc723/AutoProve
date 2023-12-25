#pragma once
#include "utils.h"
class TreeNode;
class AtomNode;
class AnyNode;
class CtdNode;
class UniOpNode;
class NotNode;
class BiOpNode;
class AndOpNode;
class OrOpNode;
class IfOpNode;
class EqOpNode;
struct ProofLine;
typedef vector<ProofLine> Proof;
struct ProofLine {
	ProofLine(set<int> dep, TreeNode *fml, string tag) {
		this->dep = dep;
		this->formula = fml;
		this->tag = tag;
	}
	set<int> dep;
	//int lineNum;
	TreeNode *formula;
	vector<int> args;
	string tag;
	ProofLine &arg(int i) { args.push_back(i); return *this; }
	string toString(int i);
	static void printProof(Proof &p);
	
};
class TreeNode {
public:
	TreeNode() {}
	virtual string toString() = 0;
	virtual bool eval() { return false; }
	virtual void assign(string symbol, bool value) {}
	virtual void all_symbols(set<string> &symbols) {}
	virtual bool equals(TreeNode *tree) { return (tree->type == "AnyNode" || type == tree->type); }
	virtual bool contains(TreeNode *tree, TreeNode **match = NULL) {
		bool eq = equals(tree);
		if (match != NULL && eq) {
			*match = this;
		}
		return eq;
	}
	virtual void find_all_match(TreeNode *partten, vector<TreeNode *> &matches) {
		if (equals(partten))
			matches.push_back(this);
	}
	int pgen_checked(int maxLevel, set<int> dep, vector<ProofLine>& proof) {
		if (maxLevel <= 0) {
			return -1;
		}
		return pgen(maxLevel - 1, dep, proof);
	}
	int pgen_checked_basic(int maxLevel, set<int> dep, vector<ProofLine>& proof) {
		if (maxLevel <= 0) {
			return -1;
		}
		return TreeNode::pgen(maxLevel - 1, dep, proof);
	}
	virtual int pgen(int maxLevel, set<int> dep, vector<ProofLine> &proof);
	int pgen_neg_asp(int maxLevel, set<int> dep, vector<ProofLine> &proof);
	bool helpful(vector<ProofLine> &proof, vector<TreeNode *> &matches) {
		bool result = false;
		for (auto p : proof) {
			if (p.formula->contains(this)) {
				p.formula->find_all_match(this, matches);
				result = true;
			}
		}
		return result;
	}
	
	string type;
	static int find(set<int> dep_most, TreeNode *formula, vector<ProofLine> proof);
	static bool is_subset(set<int> &a, set<int> &b);
	static set<int> union_set(set<int> &a, set<int> &b);
	static set<int> substract_set(set<int> &a, set<int> &b);
	static string set_to_string(set<int> &a);
	static string vector_to_string(vector<int>& a);
	
	string pgen_arg_str(set<int> &dep, vector<ProofLine> &proof) {
		stringstream ss;
		ss << proof.size();
		return set_to_string(dep) + " " + this->toString() + " " + ss.str();
	}
	
};

class AtomNode : public TreeNode {
public:
	AtomNode(string symbol) :TreeNode() {
		this->symbol = symbol;
		this->value = false;
		this->type = "AtomNode";
	}
	string toString() {
		return symbol;
	}
	void assign(string symbol, bool value) {
		if (this->symbol == symbol) {
			this->value = value;
		}
	}
	void all_symbols(set<string> &symbols) {
		symbols.insert(symbol);
	}
	bool equals(TreeNode *tree) {
		if (tree->type == "AnyNode")
			return true;
		if (tree->type == this->type) {
			AtomNode *t = dynamic_cast<AtomNode *>(tree);
			return t->symbol == this->symbol;
		}
		return false;
	}
	bool eval() { return value; }
	string symbol;
	bool value;
};

class AnyNode : public TreeNode {
public:
	AnyNode() :TreeNode() {
		this->type = "AnyNode";
	}
	string toString() {
		return "*";
	}
	bool equals(TreeNode *tree) {
		return true;
	}
	bool contains(TreeNode *tree, TreeNode **match = NULL) {
		if (match != NULL) {
			*match = this;
		}
		return true;
	}
	void find_all_match(TreeNode *partten, vector<TreeNode *> &matches) {
		throw new exception("AnyNode can't call find_all_match()");
	}
	int pgen(int max_level, set<int> dep, vector<ProofLine> &proof) {
		throw new exception("AnyNode can't call pgen()");
	}
};

class CtdNode : public TreeNode {
public:
	CtdNode() : TreeNode() {
		this->type = "CtdNode";
	}
	string toString() {
		return "/\\";
	}
	int pgen(int max_level, set<int> dep, vector<ProofLine> &proof);
};

class UniOpNode : public TreeNode {
public:
	UniOpNode(TreeNode *child) : child(child) {};
	string toString() {
		return symbol + child->toString();
	}
	virtual void assign(string symbol, bool value) {
		child->assign(symbol, value);
	}
	void all_symbols(set<string> &symbols) {
		child->all_symbols(symbols);
	}
	bool equals(TreeNode *tree) {
		if (tree->type == "AnyNode")
			return true;
		if (tree->type == this->type) {
			TreeNode *c = dynamic_cast<UniOpNode *>(tree)->child;
			return child->equals(c);
		}
		return false;
	}
	bool contains(TreeNode *tree, TreeNode **match = NULL) {
		if (equals(tree)) {
			if (match != NULL)
				*match = this;
			return true;
		}
		return child->contains(tree, match);
	}
	virtual void find_all_match(TreeNode *partten, vector<TreeNode *> &matches) {
		if (equals(partten)) {
			matches.push_back(this);
		}
		else {
			child->find_all_match(partten, matches);
		}
	}
	TreeNode *child;
	string symbol;
};

class NotNode : public UniOpNode {
public:
	NotNode(TreeNode *child) : UniOpNode(child) {
		symbol = "!";
		this->type = "NotNode";
	}
	bool eval() {
		return !child->eval();
	}
};

class BiOpNode : public TreeNode {
public:
	BiOpNode(TreeNode *left, TreeNode *right) :
		left(left), right(right) {};
	string toString() {
		return "(" + left->toString() + symbol + right->toString() + ")";
	}
	virtual void assign(string symbol, bool value) {
		left->assign(symbol, value);
		right->assign(symbol, value);
	}
	void all_symbols(set<string> &symbols) {
		left->all_symbols(symbols);
		right->all_symbols(symbols);
	}
	bool equals(TreeNode *tree) {
		if (tree->type == "AnyNode")
			return true;
		if (tree->type == this->type) {
			TreeNode *cl = dynamic_cast<BiOpNode *>(tree)->left;
			TreeNode *cr = dynamic_cast<BiOpNode *>(tree)->right;
			return left->equals(cl) && right->equals(cr);
		}
		return false;
	}
	bool contains(TreeNode *tree, TreeNode **match = NULL) {
		if (equals(tree)) {
			if (match != NULL) {
				*match = this;
			}
			return true;
		}
		return left->contains(tree, match) || right->contains(tree, match);
	}
	virtual void find_all_match(TreeNode *partten, vector<TreeNode *> &matches) {
		if (equals(partten)) {
			matches.push_back(this);
		}
		left->find_all_match(partten, matches);
		right->find_all_match(partten, matches);
	}
	string symbol;
	TreeNode *left, *right;
};

class AndOpNode : public BiOpNode {
public:
	AndOpNode(TreeNode *left, TreeNode *right) :
		BiOpNode(left, right) {
		symbol = "^";
		this->type = "AndOpNode";
	}
	bool eval() {
		return left->eval() && right->eval();
	}
	int pgen(int max_level, set<int> dep, vector<ProofLine>& proof) {
		Proof proof_copy = proof;
		int i = left->pgen_checked(max_level, dep, proof);
		int j = right->pgen_checked(max_level, dep, proof);
		if (i != -1 && j != -1) {
			ProofLine p(union_set(proof[i].dep, proof[j].dep), this, "^I");
			p.arg(i).arg(j);
			proof.push_back(p);
			return proof.size() - 1;
		}
		proof = proof_copy;

		if (int k = this->TreeNode::pgen_checked_basic(max_level, dep, proof); k != -1) {
			return k;
		}
		return -1;
	}
};

class OrOpNode : public BiOpNode {
public:
	OrOpNode(TreeNode *left, TreeNode *right) :
		BiOpNode(left, right) {
		symbol = "v";
		this->type = "OrOpNode";
	}
	bool eval() {
		return left->eval() || right->eval();
	}
	int pgen(int max_level, set<int> dep, vector<ProofLine>& proof) {
		Proof proof_copy = proof;

		int i = left->pgen_checked(max_level, dep, proof);
		if (i != -1) {
			ProofLine p(proof[i].dep, this, "vI");
			p.arg(i);
			proof.push_back(p);
			cout << repeatStr(" ", 20 - max_level) << "proved: " << toString() << endl;
			return proof.size() - 1;
		}
		proof = proof_copy;

		i = right->pgen_checked(max_level, dep, proof);
		if (i != -1) {
			ProofLine p(proof[i].dep, this, "vI");
			p.arg(i);
			proof.push_back(p);
			cout << repeatStr(" ", 20 - max_level) << "proved: " << toString() << endl;
			return proof.size() - 1;
		}

		proof = proof_copy;

		if (int k = this->TreeNode::pgen_checked_basic(max_level, dep, proof); k != -1) {
			return k;
		}
		return -1;
	}
};

class IfOpNode : public BiOpNode {
public:
	IfOpNode(TreeNode *left, TreeNode *right) :
		BiOpNode(left, right) {
		symbol = "->";
		this->type = "IfOpNode";
	}
	bool eval() {
		return !(left->eval() && !right->eval());
	}
	int pgen(int max_level, set<int> dep, Proof &proof) {
		/*Proof proof_copy = proof;
		set<int> d;
		d.insert(proof.size());
		bool alreadyAssumed = false;
		for (const auto& p : proof) {
			if (p.formula->equals(left)) {
				alreadyAssumed = true;
			}
		}
		if (!alreadyAssumed) {
			ProofLine asp(d, left, "ASP->");
			cout<< repeatStr(" ", 20 - max_level) << "assuming " << left->toString() << endl;
			proof.push_back(asp);
			int i = proof.size() - 1, j;
			if ((j = right->pgen_checked(max_level, union_set(dep, d), proof)) != -1) {
				ProofLine p(substract_set(proof[j].dep, d), this, "->I");
				p.arg(i).arg(j);
				proof.push_back(p);
				return proof.size() - 1;
			}
		}
		proof = proof_copy;*/

		if (int k = this->TreeNode::pgen_checked_basic(max_level, dep, proof); k != -1) {
			return k;
		}
		return -1;
	}
};

class EqOpNode : public BiOpNode {
public:
	EqOpNode(TreeNode *left, TreeNode *right) :
		BiOpNode(left, right) {
		symbol = "<->";
		this->type = "EqOpNode";
	}
	bool eval() {
		return left->eval() == right->eval();
	}
};
