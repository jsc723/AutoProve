#pragma once
#include "utils.h"
#include "SyntaxTree.h"
class Parser
{
public:
	Parser();
	~Parser();
	static TreeNode *parse(deque<string> &tokens);
private:
	static deque<string>::iterator nextArg(deque<string> &tokens, deque<string>::iterator it);
	static void takeOutPara(deque<string> &tokens);
};

