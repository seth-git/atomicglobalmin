////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MAIN_H_
#define __MAIN_H_

#include "input.h"
#include "energy.h"
#include "gega.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


using std::ofstream;
using namespace std;

#define INITIALIZATION_TRIES	500
#define MIN_CONVERGENCE_PERCENT	15


typedef struct
{
	int task;
	vector<MoleculeSet*> *moleculeSets; // The population
	MoleculeSet** transitionStatesFound; // The transition state found for each population member
	Point3D boxDimensions;
	pthread_mutex_t* mutex;
	FLOAT maxAtomDistance; // Used for initialization
	
	// Used for simulated annealing
	bool bPerformNonFragmentedSearch;
	bool bPerformBasinHopping;
	FLOAT deltaForCoordinates;
	FLOAT deltaForAnglesInRad;
	FLOAT temperature;
	FLOAT fBoltzmanConstant;

	// Used for PSO
	vector<MoleculeSet*> *moleculeSetsMinDistEnforced; // a version of the population (moleculeSets) in which the min. distance constraint is enforced
	bool bEnforceMinDistOnCopy; // If true, minimum distance constraints for PSO will be enforced on moleculeSetsMinDistEnforced
	                            // but not on moleculeSets
	vector<MoleculeSet*> *bestIndividualMoleculeSets; // The best solution found for each individual
	bool usePopulationBestAndNotLocalBest;
	vector<MoleculeSet*> *localBestMoleculeSets; // The solution near each individual that is the best
	MoleculeSet* populationBestMoleculeSet; // The best to date in the population
	FLOAT fCoordInertia;
	FLOAT fCoordIndividualMinimumAttraction;
	FLOAT fCoordPopulationMinimumAttraction;
	FLOAT fCoordMaximumVelocity;
	FLOAT fAngleInertia;
	FLOAT fAngleIndividualMinimumAttraction;
	FLOAT fAnglePopulationMinimumAttraction;
	FLOAT fAngleMaximumVelocity;
	FLOAT fAttractionRepulsion;
	FLOAT fIndividualBestUpdateDist;
	
	bool usePreviousWaveFunction;
	bool useLocalOptimization;
	bool readInOptimizedCoordinates;
} NodeData_t;

bool initLinearMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions);

bool initPlanarMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions);

bool init3DMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions);

bool init3DMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Point3D &boxDimensions, FLOAT maxAtomDist);

bool init3DNonFragMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Point3D &boxDimensions, FLOAT maxAtomDist);

MoleculeSet *transformMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions,
                                  FLOAT deltaForCoordinates, FLOAT deltaForAnglesInRad, FLOAT temperature,
                                  FLOAT boltzmanConstant, bool usePreviousWaveFunction, bool bPerformNonFragmentedSearch,
                                  bool bPerformBasinHopping, FLOAT maxDist);

/*******************************************************************************
* Purpose: This function performs a random search.  It is used to search for
*    transition states.
* Parameters: moleculeSet - the molecule set which will undergo a random change
*             energy - the energy object
*             boxDimensions - the length, width, and height of the search cube
*             deltaForCoordinates - the amount to change coordinates
*             deltaForAnglesInRad - the amount to change angles
*             bPerformNonFragmentedSearch - if this is true, a maximum distance will be observed
*             maxDist - the maximum distance
*             transitionsFound - an array of pointers.  If a transition state
*                       is found, a pointer to the structure will be stored in
*                       the index appropriate for moleculeSet, otherwise NULL will be
*                       stored at that index.
*******************************************************************************/
MoleculeSet *findTransitionState(MoleculeSet &moleculeSet, Point3D &boxDimensions,
                                 FLOAT deltaForCoordinates, FLOAT deltaForAnglesInRad,
                                 bool bPerformNonFragmentedSearch, FLOAT maxDist, MoleculeSet** transitionsFound);

void swarmMoleculeSet(MoleculeSet &moleculeSet, MoleculeSet &minDistMoleculeSet, MoleculeSet &populationBestSet,
                      MoleculeSet &individualBestSet, FLOAT coordInertia,
                      FLOAT coordIndividualMinimumAttraction, FLOAT coordPopulationMinimumAttraction,
                      FLOAT coordMaxVelocity, FLOAT angleInertia, FLOAT angleIndividualMinimumAttraction,
                      FLOAT anglePopulationMinimumAttraction, FLOAT angleMaxVelocity, Point3D &boxDimensions,
                      bool enforceMinDistOnACopy, bool usePreviousWaveFunction, FLOAT attractionRepulsion,
                      FLOAT fIndividualBestUpdateDist, bool useLocalOptimization);

/*******************************************************************************
* Purpose: This function initializes a population
* Parameters: input - an input object
*             moleculeSets - the population to initialize
*             bestNMoleculeSets - where to store the list of best n solutions
*             fout - the output file to print the input and other information
*             nodeData - structure used for threads
*             energyObjects - used to calculate energy
* Returns: nothing
*******************************************************************************/
bool initializePopulation(Input &input, vector<MoleculeSet*> &moleculeSets);

/*******************************************************************************
* Purpose: This function performs Simulated Annealing
* Parameters: input - an input object
*             moleculeSets - the population (initizlied inside input if a resume file is read)
*             bestNMoleculeSets - the bestNStructures (initizlied inside input if a resume file is read)
* Returns: nothing
*******************************************************************************/
void simulatedAnnealing(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets, string &seedFiles);

FLOAT probabilitySum(vector<FLOAT> &deltaEnergies, FLOAT scalingFactor, FLOAT temperature);

/*******************************************************************************
* Purpose: This function performs Particle Swarm Optimization
* Parameters: input - an input object
*             moleculeSets - the population (initizlied inside input if a resume file is read)
*             bestNMoleculeSets - the bestNStructures (initizlied inside input if a resume file is read)
*             bestIndividualMoleculeSets - the best structures seen by each individual in the population
* Returns: nothing
*******************************************************************************/
void particleSwarmOptimization(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets,
                               vector<MoleculeSet*> &bestIndividualMoleculeSets, string &seedFiles);

void quickSort(FLOAT *array, int lo, int hi);

//FLOAT getVisibilityDistance(vector<MoleculeSet*> &population, FLOAT desiredPercentVisibility);
FLOAT getVisibilityDistance(vector<MoleculeSet*> &population, FLOAT **distanceMatrix, FLOAT desiredPercentVisibility);

/*******************************************************************************
 * Purpose: This function finds the best solution within a local distance of each individual solution.
 * Parameters: population - the individuals in the population
 *             localBestMoleculeSets - where the best local solution for each individual is stored
 *             distanceMatrix        - distance matrix for the population
 *             distance - the maximum distance between two solutions required for them to be "local" to one another
 * Returns: nothing
 ******************************************************************************/
FLOAT findLocalBestMoleculeSets(vector<MoleculeSet*> &population, vector<MoleculeSet*> bestIndividualMoleculeSets,
                                vector<MoleculeSet*> &localBestMoleculeSets, FLOAT **distanceMatrix, FLOAT distance, Point3D &boxDimensions);

/*******************************************************************************
 * Purpose: This function computes distances between each solution in the
 *    population and returns and updates the distance matrix.
 * Parameters: population - the individuals in the population
 *             distanceMatrix - a 2D distance matrix
 * Returns: nothing
 ******************************************************************************/
void getPopulationDistanceMatrix(vector<MoleculeSet*> &population, FLOAT **distanceMatrix);

/*******************************************************************************
 * Purpose: This function measures the diversity in the population and returns
 *    a diversity measure.
 * Parameters: distanceMatrix - a 2D distance matrix
 * Returns: the diversity measure
 ******************************************************************************/
FLOAT calculateDiversity(int populationSize, FLOAT **distanceMatrix, FLOAT cubeLength);

/*******************************************************************************
* Purpose: This function performs a genetic algorithm.
* Parameters: input - an input object
*             moleculeSets - the population (initizlied inside input if a resume file is read)
*             bestNMoleculeSets - the bestNStructures (initizlied inside input if a resume file is read)
* Returns: nothing
*******************************************************************************/
void gega(Input &input, vector<MoleculeSet*> &population, vector<MoleculeSet*> &bestNMoleculeSets, string &seedFiles);

/*******************************************************************************
* Purpose: This function optimizes the structures in the popultaion and places optimized structures in the list of best structures.
* Parameters: input - an input object
*             moleculeSets - the structures to be optimized
*             bestNMoleculeSets - the optimized structures
* Returns: nothing
*******************************************************************************/
void optimizeBestStructures(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets);

#endif
