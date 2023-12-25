#pragma once
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include <deque>
#include <set>
#include <typeinfo>
#include <iomanip>

using namespace std;
#define readNewLine(in, line, ss)\
getline(in, line); \
ss.clear();  ss.str(line);

string repeatStr(const char* s, int k);

extern regex atomReg, ctdReg, cnctReg, paraReg, allReg;
