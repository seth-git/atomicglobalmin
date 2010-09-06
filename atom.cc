////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages an atom.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "atom.h"

Atom::Atom ()
{
	m_iAtomicNumber = 0;
	m_iMoleculeSetIndex = -1;
}

Atom::~Atom (void)
{
}

void Atom::copy(Atom &atom)
{
	m_iAtomicNumber = atom.m_iAtomicNumber;
	m_iMoleculeSetIndex = atom.m_iMoleculeSetIndex;
	m_localPoint = atom.m_localPoint;
	m_globalPoint = atom.m_globalPoint;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This is a helper function that prints a floating point
//    number with the right number of decimals.
// Parameters: number - the FLOAT
// Returns: a character array with the number
const char *Atom::printFloat(FLOAT number)
{
	static char tempString[100];
	int i;
	sprintf(tempString,"%0.15f", number);

	// Remove trailing zeros
	i = 0;
	while (tempString[i] != '\0')
		++i;
	--i;
	while ((tempString[i] == '0') && !((i > 0) && (tempString[i-1] == '.'))) {
		tempString[i] = '\0';
		--i;
	}

	return tempString;
}


FLOAT Atom::s_rgAtomicMasses[MAX_ATOMIC_NUMBERS+1]; // You need this to make sure the program compiles.

void Atom::initAtomicMasses(string fileName)
{
	const int MAX_LINE_LENGTH = 500;
	char fileLine[MAX_LINE_LENGTH];
	char* myString;
	int atomicNumber;
	
	ifstream infile(fileName.c_str());
	if (!infile)
	{
		cout << "Can't open the input file:" << fileName << endl;
		exit(0);
	}
	
	// Read two blank line
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		exit(0);
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		exit(0);
	while(true)
	{
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
			exit(0);
		if (strlen(fileLine) == 0)
			break;
		myString = strtok(fileLine, ",");
		if (myString == NULL)
			break;
		atomicNumber = atoi(myString);
		if (atomicNumber > MAX_ATOMIC_NUMBERS)
		{
			cout << "Atomic numbers above " << MAX_ATOMIC_NUMBERS << " are not allowed." << endl;
			exit(0);
		}
		if (atomicNumber < 1)
		{
			cout << "Atomic numbers below 1 are not allowed." << endl;
			exit(0);
		}
		myString = strtok(NULL, ",");
		if (myString == NULL)
			break;
		s_rgAtomicMasses[atomicNumber] = atof(myString);
//		cout << "atomicNumber,atomicMass = " << atomicNumber << ", " << s_rgAtomicMasses[atomicNumber] << endl;
	}
	infile.close();
}

FLOAT Atom::getMass()
{
	return s_rgAtomicMasses[m_iAtomicNumber];
}

void Atom::sortAtoms(int *rankArray, Atom **atomsArray, FLOAT *distanceArray, int lo, int hi)
{
	int left, right;
	FLOAT fMedianDist;
	int iMedianAtomicNumber;
	int tempAtom;
	
	if( hi > lo ) // if at least 2 elements, then
	{
		left=lo; right=hi;
		
		// We don't know what the median is, so we'll just pick something
		tempAtom = rankArray[(lo+hi)/2];
		fMedianDist = distanceArray[tempAtom];
		iMedianAtomicNumber = atomsArray[tempAtom]->m_iAtomicNumber;
		
		while(right >= left)
		{
			while(compareAtoms(atomsArray[rankArray[left]]->m_iAtomicNumber, distanceArray[rankArray[left]],
			                   iMedianAtomicNumber, fMedianDist) < 0)
				left++;
			while(compareAtoms(atomsArray[rankArray[right]]->m_iAtomicNumber, distanceArray[rankArray[right]],
			                   iMedianAtomicNumber, fMedianDist) > 0)
				right--;
			
			if(left > right)
				break;
			
			// Swap
			tempAtom=rankArray[left];
			rankArray[left]=rankArray[right];
			rankArray[right]=tempAtom;
			
			left++;
			right--;
		}
		
		sortAtoms(rankArray, atomsArray, distanceArray, lo, right);
		sortAtoms(rankArray, atomsArray, distanceArray, left, hi);
	}
}

int Atom::compareAtoms(int atom1AtomicNumber, FLOAT atom1Dist, int atom2AtomicNumber, FLOAT atom2Dist)
{
	if (atom1AtomicNumber != atom2AtomicNumber)
		return atom1AtomicNumber - atom2AtomicNumber;
	else if (atom1Dist < atom2Dist)
		return -1;
	else if (atom1Dist == atom2Dist)
		return 0;
	else // if (atom1Dist > atom2Dist)
		return 1;
}


FLOAT Atom::getMinDistance(Atom &otherAtom)
{
	return getMinAtomicDistance(m_iAtomicNumber,otherAtom.m_iAtomicNumber);
}

FLOAT Atom::s_rgMinAtomicDistances[MAX_ATOMIC_NUMBERS+1][MAX_ATOMIC_NUMBERS+1]; // Do this so the program compiles.
FLOAT Atom::s_fDefaultMinAtomicDistance;

void Atom::initMinAtomicDistances(FLOAT fDefaultMinAtomicDistance)
{
	int i, j;
	for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i)
		for (j = 1; j <= MAX_ATOMIC_NUMBERS; ++j)
			s_rgMinAtomicDistances[i][j] = -1;
	s_fDefaultMinAtomicDistance = fDefaultMinAtomicDistance;
}

void Atom::setMinAtomicDistance(int atomicNumber1, int atomicNumber2, FLOAT distance)
{
	if ((atomicNumber1 < 1) || (atomicNumber1 > MAX_ATOMIC_NUMBERS) ||
	    (atomicNumber2 < 1) || (atomicNumber2 > MAX_ATOMIC_NUMBERS) || (distance <= 0))
		return;
	s_rgMinAtomicDistances[atomicNumber1][atomicNumber2] = distance;
	s_rgMinAtomicDistances[atomicNumber2][atomicNumber1] = distance;
}

FLOAT Atom::getMinAtomicDistance(int atomicNumber1, int atomicNumber2)
{
	if ((atomicNumber1 < 1) || (atomicNumber1 > MAX_ATOMIC_NUMBERS) ||
	    (atomicNumber2 < 1) || (atomicNumber2 > MAX_ATOMIC_NUMBERS)) {
		cout << "Invalid atomic number in getMinAtomicDistance" << endl;
		exit(0);
	} else if (s_rgMinAtomicDistances[atomicNumber1][atomicNumber2] >= 0)
		return s_rgMinAtomicDistances[atomicNumber1][atomicNumber2];
	else
		return s_fDefaultMinAtomicDistance;
}

void Atom::outputMinDistances(ofstream &outFile)
{
	int i, j;
	for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i)
		for (j = i; j <= MAX_ATOMIC_NUMBERS; ++j)
			if (s_rgMinAtomicDistances[i][j] != -1)
				outFile << i << " " << j << " " << printFloat(s_rgMinAtomicDistances[i][j]) << endl;
}



