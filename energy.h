////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENERGY_H__
#define __ENERGY_H__

#include "moleculeSet.h"
#include "input.h"

#include <ctype.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

using std::ofstream;
using namespace std;

#define SINGLE_POINT_ENERGY_CALCULATION 1
#define OPTIMIZE_BUT_DONT_READ 2
#define OPTIMIZE_AND_READ 3
#define TRANSITION_STATE_SEARCH 4

#define ENERGY_TEMP_FILE	"temporaryJob"
#define LENNARD_JONES_EPSILON   1.0
#define LENNARD_JONES_SIGMA     1.0

#define OPENED_FILE	1
#define READ_ENERGY	2
#define OBTAINED_GEOMETRY	4
#define NORMAL_TERMINATION	8

class Energy
{
public:
	Energy ();
	~Energy ();
	
public:
/*******************************************************************************
 * Purpose: This function calculates energies for a population of structures.
 * Parameters: energyCalculationType
 *                SINGLE_POINT_ENERGY_CALCULATION: single ponit energy calculation
 *                OPTIMIZE_BUT_DONT_READ: perform optimization, but don't obtain the optimized geometry
 *                OPTIMIZE_AND_READ: perform optimization, and store the optimized geometry in optimizedPopulation
 *                TRANSITION_STATE_SEARCH: write the energy file footer at the end of the input file
 *                      and get the optimized geometry if the structure is a transition state (NULL otherwise).
 *                      (not implmented with the Lennard Jones potential)
 *             input - an input object
 *             population - the molecule sets for which energies will be calculated
 *             optimizedPopulation - the new coordinates resulting from optimization
 *                   Note: this function assumes that optimizedPopulation is an vector
 *                   of pointers that is already the same size as population.
 * Returns: the percentage of the energy calculations that were successful
 *******************************************************************************/
	static FLOAT calculateEnergies (int energyCalculationType, const Input &input, vector<MoleculeSet*> &population, vector<MoleculeSet*> &optimizedPopulation);


	static void createGaussianInputFile(string &inputFileName, string &sCheckPointFileName, string &sId, const Input &input, MoleculeSet &moleculeSet, bool writeEnergyValueInHeader);
	static int readGaussianLogFile(const string &logFile, FLOAT &energy, MoleculeSet* pMoleculeSet, bool getStandardOrientation, bool semiEmpirical);

	static void lower(string &s);
	static bool isFileCharacter(char character);
	static string fileWithoutPath(const string &s);
	static void removeFirstNode(const Input &input);
};

#endif

