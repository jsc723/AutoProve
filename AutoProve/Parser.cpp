#include "Parser.h"



Parser::Parser()
{
}


Parser::~Parser()
{
}



TreeNode *Parser::parse(deque<string> &tokens) {
	takeOutPara(tokens);
	if (tokens.size() == 0)
		return NULL;
	if (tokens.size() == 1 && regex_match(tokens[0], ctdReg)) {
		return new CtdNode();
	}
	if (tokens.size() == 1 && regex_match(tokens[0], atomReg)) {
		return new AtomNode(tokens[0]);
	}
	auto second = nextArg(tokens, begin(tokens));
	if (second == end(tokens)) {
		if (tokens[0] == "!") {
			deque<string> rest(tokens.begin() + 1, tokens.end());
			if (second == end(tokens)) {
				return new NotNode(parse(rest));
			}
		}
		else {
			cout << "malformed formula" << endl;
			return NULL;
		}
	}
	auto third = nextArg(tokens, second);
	if (third == end(tokens)) {
		cout << "malformed formula" << endl;
		return NULL;
	}
	deque<string> left(begin(tokens), second);
	deque<string> right(third, end(tokens));
	TreeNode *leftTree = parse(left);
	TreeNode *rightTree = parse(right);
	if (*second == "^") {
		return new AndOpNode(leftTree, rightTree);
	}
	else if (*second == "v") {
		return new OrOpNode(leftTree, rightTree);
	}
	else if (*second == "->") {
		return new IfOpNode(leftTree, rightTree);
	}
	else if (*second == "<->") {
		return new EqOpNode(leftTree, rightTree);
	}
	cout << "malformed formula" << endl;
	return NULL;
}

deque<string>::iterator Parser::nextArg(deque<string> &tokens, deque<string>::iterator it)
{
	int count = 0;
	if (it == end(tokens))
		return it;
	if (*it == "!")
		return nextArg(tokens, it + 1);
	while (it != end(tokens)) {
		if (*it == "(")
			count++;
		else if (*it == ")")
			count--;
		if (count < 0) {
			cout << "parathesis does not match!" << endl;
			return end(tokens);
		}
		++it;
		if (count == 0) {
			return it;
		}
	}
}

void Parser::takeOutPara(deque<string>& tokens)
{
	if (tokens.size() < 2 || tokens[0] != "(" || tokens[tokens.size() - 1] != ")")
		return;
	int count = 0;
	for (auto it = begin(tokens); it != end(tokens); ++it) {
		if (*it == "(")
			count++;
		else if (*it == ")")
			count--;
		if (count == 0 && it + 1 != end(tokens))
			return;
	}
	if (count != 0) {
		cout << "parathesis does not match!" << endl;
		return;
	}
	tokens.pop_front();
	tokens.pop_back();
}
