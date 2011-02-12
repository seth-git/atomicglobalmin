////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages an atom.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
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
	m_bondedAtoms.clear();
	
	for (int i = 0; i < (signed int)atom.m_bondedAtoms.size(); ++i)
		m_bondedAtoms.push_back(atom.m_bondedAtoms[i]);
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


string Atom::s_rgAtomcSymbols[MAX_ATOMIC_NUMBERS+1]; // You need this to make sure the program compiles.
FLOAT Atom::s_rgAtomicMasses[MAX_ATOMIC_NUMBERS+1]; // You need this to make sure the program compiles.
int Atom::s_iMaxAtomicNumber = 0;

bool Atom::initAtomicMasses(void)
{
	const int MAX_LINE_LENGTH = 500;
	char fileLine[MAX_LINE_LENGTH];
	char* myString;
	char tempString[100];
	int atomicNumber;
	int massNumber;
	FLOAT mass, abundance, highestAbundance;
	string fileName = "periodic_table.txt";
	
	ifstream infile(fileName.c_str());
	if (!infile)
	{
		cout << "Can't open the file:" << fileName << endl;
		return false;
	}
	
	// Read one blank line
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Can't read the file:" << fileName << endl;
		return false;
	}
	while(true)
	{
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
			break;
		if (strlen(fileLine) == 0)
			break;
		if (fileLine[0] != '\t') {
			myString = strtok(fileLine, "\t");
			if (myString == NULL)
				break;
			atomicNumber = atoi(myString);
			if (atomicNumber > s_iMaxAtomicNumber)
				s_iMaxAtomicNumber = atomicNumber;
			highestAbundance = -1;
			if (atomicNumber > MAX_ATOMIC_NUMBERS)
			{
				cout << "Haven't allocated enough memeory for atomic numbers above " << MAX_ATOMIC_NUMBERS << "." << endl;
				cout << "Please change the value for MAX_ATOMIC_NUMBERS in atom.h." << endl;
				return false;
			}
			if (atomicNumber < 1)
			{
				cout << "Atomic numbers below 1 are not allowed." << endl;
				return false;
			}
			myString = strtok(NULL, "\t"); // get the element name
			if (myString == NULL) {
				cout << "Missing element name in " << fileName << endl;
				return false;
			}
			myString = strtok(NULL, "\t"); // get the element symbol
		} else {
			myString = strtok(fileLine, "\t"); // get the element symbol
		}

		if (myString == NULL) {
			cout << "Missing element symbol in " << fileName << endl;
			return false;
		}
		if (sscanf(myString, "%d%s", &massNumber, tempString) != 2) {
			cout << "Unable to read element symbol in " << fileName << endl;
			return false;
		}
		s_rgAtomcSymbols[atomicNumber] = tempString;
		myString = strtok(NULL, "\t"); // mass
		if (myString == NULL) {
			cout << "Unable to read element mass in " << fileName << endl;
			return false;
		}
		mass = atof(myString);
		myString = strtok(NULL, "\t"); // abundance
		if (myString == NULL) {
			cout << "Unable to read element abundance in " << fileName << endl;
			return false;
		}
		abundance = atof(myString);
		if (abundance > highestAbundance) {
			highestAbundance = abundance;
			s_rgAtomicMasses[atomicNumber] = mass;
		}
//		cout << "atomicNumber,massNumber,atomicSymbol,abundance,atomicMass = " << atomicNumber << "," << massNumber << ","
//		     << s_rgAtomcSymbols[atomicNumber] << "," << abundance << "," << s_rgAtomicMasses[atomicNumber] << endl;
	}
	infile.close();
	return true;
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
	if ((atomicNumber1 < 1) || (atomicNumber1 > s_iMaxAtomicNumber) ||
	    (atomicNumber2 < 1) || (atomicNumber2 > s_iMaxAtomicNumber) || (distance <= 0))
		return;
	s_rgMinAtomicDistances[atomicNumber1][atomicNumber2] = distance;
	s_rgMinAtomicDistances[atomicNumber2][atomicNumber1] = distance;
}

FLOAT Atom::getMinAtomicDistance(int atomicNumber1, int atomicNumber2)
{
	if ((atomicNumber1 < 1) || (atomicNumber1 > s_iMaxAtomicNumber) ||
	    (atomicNumber2 < 1) || (atomicNumber2 > s_iMaxAtomicNumber)) {
		cout << "Invalid atomic number in getMinAtomicDistance" << endl;
		return -1;
	} else if (s_rgMinAtomicDistances[atomicNumber1][atomicNumber2] >= 0)
		return s_rgMinAtomicDistances[atomicNumber1][atomicNumber2];
	else
		return s_fDefaultMinAtomicDistance;
}

void Atom::outputMinDistances(ofstream &outFile)
{
	int i, j;
	for (i = 1; i <= s_iMaxAtomicNumber; ++i)
		for (j = i; j <= s_iMaxAtomicNumber; ++j)
			if (s_rgMinAtomicDistances[i][j] != -1)
				outFile << i << " " << j << " " << printFloat(s_rgMinAtomicDistances[i][j]) << endl;
}



