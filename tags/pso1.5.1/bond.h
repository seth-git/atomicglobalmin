////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing an atom.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __BOND_H__
#define __BOND_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "typedef.h"
#include "atom.h"

using namespace std;

class Bond
{
public:
	int m_iAtom1;
	int m_iAtom2;
	bool m_bSingle;
	bool m_bDouble;
	bool m_bTriple;
	bool m_bOnRing;
	bool m_bRotatable;

	// Note: if your not familiar with static, it means these variables belong to the class and not any particular object
	//       of the class.  It also means they can be called from anywhere without needing an object of the class.  Cool ha!
	static FLOAT singleMinDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
	static FLOAT singleMaxDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
	static FLOAT doubleMinDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
	static FLOAT doubleMaxDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
	static FLOAT tripleMinDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
	static FLOAT tripleMaxDist[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
	
public:
	Bond();
	Bond(int iAtom1, int iAtom2, bool bSingle, bool bDouble, bool bTriple, bool bOnRing, bool bRotatable);
	~Bond();
	void copy(Bond &bond);
	static bool readBondDistanceFile(string fileName);
};

#endif

