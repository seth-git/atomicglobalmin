
////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MPI_H__
#define __MPI_H__

#include "energy.h"
#include <time.h>
#include <mpi.h>

#define SINGLE_POINT_ENERGY_CALCULATION 1
#define OPTIMIZE_BUT_DONT_READ 2
#define OPTIMIZE_AND_READ 3
#define TRANSITION_STATE_SEARCH 4

#define LENNARD_JONES_EPSILON   1.0
#define LENNARD_JONES_SIGMA     1.0
#define PRINT_MPI_MESSAGES	false

#define DIETAG 1
#define WORKTAG 2

using namespace std;

class Mpi
{
public:
	Mpi ();
	~Mpi ();
	
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
 * Returns: true if all the calculations were completed
 *******************************************************************************/
	static bool calculateEnergies (int energyCalculationType, const Input &input, vector<MoleculeSet*> &population, vector<MoleculeSet*> &optimizedPopulation);
	static bool masterSetup(int populationSize, bool independent, bool masterDistributingTasks, int rank);
	static bool setQuitFlag(string &endIfFileExists);
	static bool master(int energyCalculationType, vector<MoleculeSet*> &population, vector<MoleculeSet*> &optimizedPopulation);
	static void end(int rank);
	static void slave(int rank, bool masterDistributingTasks);

	static time_t s_wallTime;
	static bool s_timeToFinish;
	static double s_percentageOfSuccessfulCalculations;
private:
	static bool readOutputFile(int energyCalculationType, vector<MoleculeSet*> &population, vector<MoleculeSet*> &optimizedPopulation, int fileIndex, int &converged);
	
	static time_t s_longestiCalculationTime;
	static bool s_independent;
	static bool s_masterDistributingTasks;
	static string s_EndIfFileExists;
	static MPI_Request s_mpiRequest;
};

#endif

