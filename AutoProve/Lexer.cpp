#include "Lexer.h"



Lexer::Lexer()
{
}


Lexer::~Lexer()
{
}

vector< deque<string> > Lexer::tokenlize(const char *src)
{
	ifstream in(src);
	stringstream ss;
	string line;
	
	vector< deque<string> > problem;
	if (!in.is_open()) {
		exit(1);
	}
	while (!in.eof()) {
		deque<string> tokens;
		getline(in, line);
		cout << "input: " << line << endl;
		auto words_begin = sregex_iterator(line.begin(), line.end(), allReg);
		auto words_end = sregex_iterator();
		for (sregex_iterator i = words_begin; i != words_end; ++i) {
			smatch match = *i;
			string match_str = match.str();
			//cout << match_str << '\n';
			tokens.push_back(match_str);
		}
		if (tokens.size()) {
			problem.push_back(tokens);
		}
	}
	return problem;
}
