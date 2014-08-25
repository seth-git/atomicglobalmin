#ifndef __HANDBOOK_H_
#define __HANDBOOK_H_

#include "../translation/strings.h"
#include "../xsd/typedef.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>

class Handbook {
public:
	static bool init();

	static FLOAT getMass(unsigned int atomicNumber);
	static const char* getAtomicSymbol(unsigned int atomicNumber);
	static unsigned int getAtomicNumber(const char* atomicSymbol);

private:
	static bool initAtomicMasses();
	static std::string s_rgAtomcSymbols[MAX_ATOMIC_NUMBERS+1];
	static FLOAT s_rgAtomicMasses[MAX_ATOMIC_NUMBERS+1];
	static std::map<std::string,unsigned int> s_symbolAtomicNumberMap;
};

#endif

