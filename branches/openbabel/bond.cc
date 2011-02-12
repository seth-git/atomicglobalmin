////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages a bond.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "bond.h"

Bond::Bond()
{
	m_iAtom1 = -1;
	m_iAtom2 = -1;
}

Bond::Bond(int iAtom1, int iAtom2, bool bSingle, bool bDouble, bool bTriple, bool bOnRing, bool bRotatable)
{
	m_iAtom1 = iAtom1;
	m_iAtom2 = iAtom2;
	m_bSingle = bSingle;
	m_bSingle = bSingle;
	m_bDouble = bDouble;
	m_bTriple = bTriple;
	m_bOnRing = bOnRing;
	m_bRotatable = bRotatable;
}

Bond::~Bond (void)
{
}

void Bond::copy(Bond &bond)
{
	m_iAtom1 = bond.m_iAtom1;
	m_iAtom2 = bond.m_iAtom2;
	m_bSingle = bond.m_bSingle;
	m_bDouble = bond.m_bDouble;
	m_bTriple = bond.m_bTriple;
	m_bOnRing = bond.m_bOnRing;
	m_bRotatable = bond.m_bRotatable;
}

// Do this so the program compiles.
FLOAT Bond::singleMinDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
FLOAT Bond::singleMaxDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
FLOAT Bond::doubleMinDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
FLOAT Bond::doubleMaxDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
FLOAT Bond::tripleMinDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
FLOAT Bond::tripleMaxDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];

bool Bond::readBondDistanceFile(string fileName)
{
	const int MAX_LINE_LENGTH = 500;
	char fileLine[MAX_LINE_LENGTH];
	int lineNumber = 0;
	int atomicNumber1, atomicNumber2;
	char atomicSymbol1[MAX_LINE_LENGTH], atomicSymbol2[MAX_LINE_LENGTH];
	char bondType;
	FLOAT minDist, maxDist;
	int i;
	
	ifstream infile(fileName.c_str());
	if (!infile)
	{
		cout << "Can't open the input file:" << fileName << endl;
		return false;
	}
	for (atomicNumber1 = 0; atomicNumber1 < MAX_ATOMIC_NUMBERS; ++atomicNumber1) {
		for (atomicNumber2 = 0; atomicNumber2 < MAX_ATOMIC_NUMBERS; ++atomicNumber2) {
			singleMinDist[atomicNumber1][atomicNumber2] = 0;
			singleMaxDist[atomicNumber1][atomicNumber2] = 0;
			doubleMinDist[atomicNumber1][atomicNumber2] = 0;
			doubleMaxDist[atomicNumber1][atomicNumber2] = 0;
			tripleMinDist[atomicNumber1][atomicNumber2] = 0;
			tripleMaxDist[atomicNumber1][atomicNumber2] = 0;
		}
	}
	
	while(true)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
			break;
		if (strlen(fileLine) == 0)
			break;
		if (sscanf(fileLine,"%s\t%s\t%c\t%lf-%lf", atomicSymbol1, atomicSymbol2, &bondType, &minDist, &maxDist) != 5) {
			cout << "Incorrect number of arguments read from line " << lineNumber << " in file: " << fileName << endl;
			return false;
		}
		atomicNumber1 = atoi(atomicSymbol1);
		if (atomicNumber1 < 1) {
			for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i)
				if (strcmp(atomicSymbol1,Atom::s_rgAtomcSymbols[i].c_str()) == 0) {
					atomicNumber1 = i;
					break;
				}
			if (atomicNumber1 < 1) {
				cout << "Unidentified element symbol (" << atomicSymbol1 << ") in file: " << fileName << endl;
				return false;
			}
		} else if (atomicNumber1 > MAX_ATOMIC_NUMBERS) {
			cout << "In the file " << fileName << ", there is no information available the element with atomic number " << atomicNumber1 << "." << endl;
			cout << "Please update the periodic table file." << endl;
			return false;
		}
		atomicNumber2 = atoi(atomicSymbol2);
		if (atomicNumber2 < 1) {
			for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i)
				if (strcmp(atomicSymbol2,Atom::s_rgAtomcSymbols[i].c_str()) == 0) {
					atomicNumber2 = i;
					break;
				}
			if (atomicNumber2 < 1) {
				cout << "Unidentified element symbol (" << atomicSymbol2 << ") in file: " << fileName << endl;
				return false;
			}
		} else if (atomicNumber2 > MAX_ATOMIC_NUMBERS) {
			cout << "In the file " << fileName << ", there is no information available the element with atomic number " << atomicNumber2 << "." << endl;
			cout << "Please update the periodic table file." << endl;
			return false;
		}
		if (minDist > maxDist) {
			FLOAT temp = minDist;
			minDist = maxDist;
			maxDist = temp;
		}
		if (bondType == 's') {
			singleMinDist[atomicNumber1][atomicNumber2] = minDist;
			singleMaxDist[atomicNumber1][atomicNumber2] = maxDist;
			singleMinDist[atomicNumber2][atomicNumber1] = minDist;
			singleMaxDist[atomicNumber2][atomicNumber1] = maxDist;
		} else if (bondType == 'd') {
			doubleMinDist[atomicNumber1][atomicNumber2] = minDist;
			doubleMaxDist[atomicNumber1][atomicNumber2] = maxDist;
			doubleMinDist[atomicNumber2][atomicNumber1] = minDist;
			doubleMaxDist[atomicNumber2][atomicNumber1] = maxDist;
		} else if (bondType == 't') {
			tripleMinDist[atomicNumber1][atomicNumber2] = minDist;
			tripleMaxDist[atomicNumber1][atomicNumber2] = maxDist;
			tripleMinDist[atomicNumber2][atomicNumber1] = minDist;
			tripleMaxDist[atomicNumber2][atomicNumber1] = maxDist;
		} else {
			cout << "Unknown bond type(" << bondType << ") on line " << lineNumber << " in file: " << fileName << endl;
			return false;
		}
	}
	infile.close();
	return true;
}

