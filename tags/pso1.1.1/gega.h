////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing an atom.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __GEGA_H__
#define __GEGA_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "input.h"
#include "typedef.h"

using namespace std;

class Gega
{
public:
	
public:
	Gega();
	~Gega();
	static void tournamentMateSelection(vector<MoleculeSet*> &population, vector<MoleculeSet*> &matingPool);
	static void performMatings(vector<MoleculeSet*> &matingPool, vector<MoleculeSet*> &offSpring, int iNumStructureTypes,
                               int iNumStructuresOfEachType[], Point3D &boxDimensions);
	static void generationReplacement(vector<MoleculeSet*> &population, vector<MoleculeSet*> &offSpring,
	                                  vector<MoleculeSet*> &newPopulation);

	// This function sorts a vector of molecule sets by their energy values
	// (lo should be zero, and hi should be moleculeSets.size() - 1)
	static void sortMoleculeSets(vector<MoleculeSet*> &moleculeSets, int lo, int hi);
	
	// This function copies the best n moleculeSets from moleculeSets to bestN.
	// Note: This function does check to see if there are identical structures in bestN and moleculeSets
	//       before updateing bestN.  Structures in bestN have to be different by fMinDistnaceBetweenSameMoleculeSets
	static void saveBestN(vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestN, int n,
	                      FLOAT fMinDistnaceBetweenSameMoleculeSets, int iNumEnergyFilesToSave, string &sLogFilesDirectory);

private:
};

#endif
