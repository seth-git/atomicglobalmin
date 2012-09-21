////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __INIT_H_
#define __INIT_H_

#include "input.h"
#include "energyProgram.h"
#include <unistd.h>
#include <errno.h>


using std::ofstream;
using namespace std;

#define INITIALIZATION_TRIES	500

class Init
{
public:
	/*******************************************************************************
	* Purpose: This function initializes a random population
	* Parameters: input - an input object specifying what type of structures
	*                     to initialize, linear, planer, etc.
	*             moleculeSets - the population to initialize
	* Returns: true on success
	*******************************************************************************/
	static bool initializePopulation(Input &input, vector<MoleculeSet*> &moleculeSets);
	
	static bool initProgram(int mpiRank);

	static bool initLinearMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions);
	
	static bool initPlanarMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions);
	
	static bool init3DMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions);
	
	static bool init3DMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Point3D &boxDimensions, FLOAT maxAtomDist);
	
	static bool init3DNonFragMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Point3D &boxDimensions, FLOAT maxAtomDist);
};
#endif
