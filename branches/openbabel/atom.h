////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing an atom.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ATOM_H__
#define __ATOM_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include "typedef.h"

using namespace std;

const int MAX_ATOMIC_NUMBERS = 200;

class Atom
{
public:
	int m_iAtomicNumber;
	int m_iMoleculeSetIndex;
	Point3D m_localPoint;
	Point3D m_globalPoint;
	vector<int> m_bondedAtoms; // index to other atoms as contained in the molecule object
	static string s_rgAtomcSymbols[MAX_ATOMIC_NUMBERS+1];
	static FLOAT s_rgAtomicMasses[MAX_ATOMIC_NUMBERS+1];
	static int s_iMaxAtomicNumber;

private:
	// Note: if your not familiar with static, it means these variables belong to the class and not any particular object
	//       of the class.  It also means they can be called from anywhere without needing an object of the class.  Cool ha!
	static FLOAT s_rgMinAtomicDistances[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1];
	static FLOAT s_fDefaultMinAtomicDistance;
	
public:
	Atom();
	~Atom();
	static bool initAtomicMasses(void);
	void copy(Atom &atom);
	FLOAT getMass();
	static const char *printFloat(FLOAT number);

	////////////////////////////////////////////////////////////////////////////////
	// Purpose: This recursive function sorts atoms first on their atomic number and second on their distance.
	//    This is a generic function that is used in multiple ways.  It implements the generic quick sort alrogirthm
	//    which is very fast.
	// Parameters: rankArray - The results of the function are stored in this array.  Specifically this array stores
	//                         the rank of each of the atoms in m_prgAtoms.
	//             atomsArray - an array of pointers to all the atoms (gives us atomic number)
	//             distanceArray - The distances of atoms in m_prgAtoms
	//             lo - the lowest index in rankArray to be sorted (see classic quick sort algorithm)
	//             hi - the highest index in rankArray to be sorted (see classic quick sort algorithm)
	// Returns: nothing (results stored in rankArary)
	static void sortAtoms(int *rankArray, Atom **allAtomsArray, FLOAT *distanceArray, int lo, int hi);
	
	static void initMinAtomicDistances(FLOAT fDefaultMinAtomicDistance);
	static void setMinAtomicDistance(int atomicNumber1, int atomicNumber2, FLOAT distance);
	static FLOAT getMinAtomicDistance(int atomicNumber1, int atomicNumber2);
	static void outputMinDistances(ofstream &outFile);
	
	FLOAT getMinDistance(Atom &otherAtom);

private:
	////////////////////////////////////////////////////////////////////////////////
	// Purpose: This function is associated with sortAtoms.  It compares two atoms based first on
	//    atomic numbers and second on their distances.
	// Parameters: atom1AtomicNumber - atomic number of the first atom
	//             atom1Dist - distance of the first atom
	//             atom2AtomicNumber - atomic number of the second atom
	//             atom2Dist - distance of the second atom
	// Returns: a number > 0 if atom 1 has a higher atomic number or larger distance than atom 2
	//                     0 if the atom are the same
	//          a number < 0 if atom 2 has a higher atomic number or larger distance than atom 1
	static int compareAtoms(int atom1AtomicNumber, FLOAT atom1Dist, int atom2AtomicNumber, FLOAT atom2Dist);

};

#endif
