#include "SyntaxTree.h"
#include "utils.h"
#include <memory>
#include <unordered_set>

string ProofLine::toString(int i) {
	stringstream ss;
	ss << std::left << setw(15) << TreeNode::set_to_string(dep);
	ss << setw(8) << std::left << "(" + to_string(i) + ")";
	ss << setw(40) << std::left << formula->toString();
	ss << setw(20)<< std::left << TreeNode::vector_to_string(args) + " " + tag;
	return ss.str();
}
void ProofLine::printProof(Proof &p) {
	cout << "-----proof-----" << endl;
	for (int i = 0; i < p.size(); i++) {
		cout << p[i].toString(i) << endl;
	}
	cout << endl;
}

int TreeNode::find(set<int> dep_most, TreeNode *formula, vector<ProofLine> proof) {
	for (int i = 0; i < proof.size(); i++) {
		if (proof[i].formula->equals(formula) && is_subset(proof[i].dep, dep_most)) {
			return i;
		}
	}
	return -1;
}

bool TreeNode::is_subset(set<int> &a, set<int> &b)
{
	for (auto i : a) {
		if (b.find(i) == b.end())
			return false;
	}
	return true;
}

set<int> TreeNode::union_set(set<int>& a, set<int>& b)
{
	set<int> c(begin(a), end(a));
	c.insert(begin(b), end(b));
	return c;
}

set<int> TreeNode::substract_set(set<int>& a, set<int>& b)
{
	set<int> c(begin(a), end(a));
	for (auto i : b) {
		c.erase(i);
	}
	return c;
}

string TreeNode::set_to_string(set<int>& a)
{
	stringstream ss;
	for (auto i : a) {
		ss << i << ",";
	}
	string s = ss.str();
	if (s.size() > 0) {
		s = s.substr(0, s.size() - 1);
	}
	return s;
}
string TreeNode::vector_to_string(vector<int>& a)
{
	stringstream ss;
	for (auto i : a) {
		ss << i << ",";
	}
	string s = ss.str();
	if (s.size() > 0) {
		s = s.substr(0, s.size() - 1);
	}
	return s;
}

int TreeNode::pgen(int max_level, set<int> dep, vector<ProofLine> &proof) {
	static vector<string> thinking_stack;
	string pgs = pgen_arg_str(dep, proof);
	if (std::find(thinking_stack.begin(), thinking_stack.end(), pgs) != thinking_stack.end()) {
		return -1;
	}
	cout << repeatStr(" ", 20 - max_level) << "proving " << toString() << endl;
	thinking_stack.push_back(pgs);
	int i = find(dep, this, proof), j;
	int result = -1;
	if (i != -1) {
		thinking_stack.pop_back();
		cout << repeatStr(" ", 20 - max_level) << "proved " << toString() << endl;
		return i;
	}
	TreeNode *any = new AnyNode();
	TreeNode *and_left = new AndOpNode(this, any);
	TreeNode *and_right = new AndOpNode(any, this);
	TreeNode *if_node = new IfOpNode(any, this);
	TreeNode *not_node = new NotNode(this);
	TreeNode *dnot_node = new NotNode(not_node);

	vector<TreeNode *> and_left_matches, and_right_matches;
	vector<TreeNode *> if_matches, dnot_matches;
	if (and_left->helpful(proof, and_left_matches)) {
		for (auto m : and_left_matches) {
			if ((i = m->pgen_checked(max_level, dep, proof)) != -1) {
				ProofLine p(proof[i].dep, this, "^E");
				p.arg(i);
				proof.push_back(p);
				result = proof.size() - 1;
				goto end;
			}
		}
	}
	if (and_right->helpful(proof, and_right_matches)) {
		for (auto m : and_right_matches) {
			if ((i = m->pgen_checked(max_level, dep, proof)) != -1) {
				ProofLine p(proof[i].dep, this, "^E");
				p.arg(i);
				proof.push_back(p);
				result = proof.size() - 1;
				goto end;
			}
		}
	}

	if (if_node->helpful(proof, if_matches)) {
		for (auto m : if_matches) {
			if ((i = m->pgen_checked(max_level, dep, proof)) != -1) {
				IfOpNode *cond = dynamic_cast<IfOpNode *>(proof[i].formula);
				if ((j = cond->left->pgen_checked(max_level, dep, proof)) != -1) {
					ProofLine p(union_set(proof[i].dep, proof[j].dep), this, "->E");
					p.arg(i).arg(j);
					proof.push_back(p);
					result = proof.size() - 1;;
					goto end;
				}
			}
		}
	}

	
	if (dnot_node->helpful(proof, dnot_matches)) {
		TreeNode *m = dnot_matches[0];
		if ((i = m->pgen_checked(max_level, dep, proof)) != -1) {
			ProofLine p(proof[i].dep, this, "DN");
			p.arg(i);
			proof.push_back(p);
			result = proof.size() - 1;
			goto end;
		}
	}

	result = pgen_neg_asp(max_level, dep, proof);


end:
	thinking_stack.pop_back();
	delete and_left, and_right, if_node;
	delete not_node, dnot_node;
	//delete any;
	if (result != -1) {
		cout << repeatStr(" ", 20 - max_level) << "proved " << toString() << endl;
	}
	return result;
}

int TreeNode::pgen_neg_asp(int max_level, set<int> dep, vector<ProofLine>& proof)
{
	static int max_neg_asp = 1;
	
	if (max_neg_asp <= 0 || type == "CtdNode")
		return -1;

	max_neg_asp--;
	
	Proof proof_copy = proof;
	set<int> d; d.insert(proof.size());
	int i = proof.size();
	TreeNode *n = type == "NotNode" ? dynamic_cast<NotNode *>(this)->child : new NotNode(this);


	cout << repeatStr(" ", 20 - max_level) << "asuming " << n->toString() << endl;
	ProofLine asp(d, n, "ASP!");
	proof.push_back(asp);
	TreeNode *ctd = new CtdNode();
	int j = ctd->pgen_checked(max_level, union_set(dep, d), proof);

	if (j != -1) {
		TreeNode *dn = n->type == "NotNode" ? dynamic_cast<NotNode*>(n)->child : new NotNode(n);
		ProofLine p(dep, dn, "!I");
		p.arg(i).arg(j);
		proof.push_back(p);

		cout << repeatStr(" ", 20 - max_level) << "proved " << toString() << endl;
		max_neg_asp++;
		return proof.size() - 1;
	}
	proof = proof_copy;

	max_neg_asp++;

	return -1;
}

int CtdNode::pgen(int max_level, set<int> dep, vector<ProofLine> &proof) {
	static int ctd_max_level = 1;

	if (ctd_max_level <= 0) {
		return -1;
	}
	ctd_max_level--;
	int i, j;

	Proof proof_copy = proof;

	set<string> symbols_set;
	for (int i = 0; i < proof.size() && proof[i].tag.starts_with("P"); i++) {
		proof[i].formula->all_symbols(symbols_set);
	}
	vector<string> symbols(symbols_set.begin(), symbols_set.end());
	for (const auto &symbol : symbols) {

		TreeNode* pos = new AtomNode(symbol);
		TreeNode* neg = new NotNode(pos);
		
		int i = pos->pgen_checked(max_level, dep, proof);
		int j = neg->pgen_checked(max_level, dep, proof);

		if (i != -1 && j != -1) {
			ProofLine pline(union_set(proof[i].dep, proof[j].dep), this, "!E");
			pline.arg(i).arg(j);
			proof.push_back(pline);
			cout << repeatStr(" ", 20 - max_level) << "proved " << toString() << endl;
			ctd_max_level++;
			return proof.size() - 1;
		}
		delete pos;
		delete neg;
	}
	proof = proof_copy;

	for (i = 0; i < proof.size() && proof[i].tag.starts_with("P"); i++) {
		TreeNode* np = new NotNode(proof[i].formula);
		if (int k = np->pgen_checked(max_level, dep, proof);  k != -1) {
			ProofLine pline(proof[k].dep, this, "!E");
			pline.arg(i).arg(k);
			proof.push_back(pline);
			cout << repeatStr(" ", 20 - max_level) << "proved " << toString() << endl;
			ctd_max_level++;
			return proof.size() - 1;
		}
		delete np;
	}
	proof = proof_copy;

	ctd_max_level++;

	return -1;
}
