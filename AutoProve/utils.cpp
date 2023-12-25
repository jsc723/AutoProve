#include "utils.h"
regex atomReg("[A-Z][0-9]*");
regex ctdReg("/\\\\");
regex cnctReg("!|\\^|v|(->)|(<->)");
regex paraReg("\\(|\\)");
regex allReg("([A-Z][0-9]*)|(/\\\\)|!|\\^|v|(->)|(<->)|\\(|\\)");

string repeatStr(const char* s, int k) {
	string res;

	for (int i = 0; i < k; i++) {
		res += s;
	}
	return res;
}