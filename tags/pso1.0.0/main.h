////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing a molecule or a set of adams that
//    belong in a group.
// Author: Seth Call
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

#define INIT_LINEAR_TASK 1
#define INIT_PLANAR_TASK 2
#define INIT_3D_TASK 3
#define INIT_3D_WITH_MAX_DIST_TASK 4
#define INIT_3D_NONFRAG_WITH_MAX_DIST_TASK 5
#define TRANSFORM_TASK 6
#define SWARM_TASK 7
#define CALCULATE_ENERGY_TASK 8
#define INITIALIZATION_TRIES 500
#define MIN_CONVERGENCE_PERCENT 15

bool g_bErrorToStopFor;

typedef struct
{
	int task;
	vector<MoleculeSet*> *moleculeSets; // The population
	Point3D boxDimensions;
	Energy* energy;
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

bool initLinearMoleculeSet(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, bool useLocalOptimization, bool readInOptimizedCoordinates);

bool initPlanarMoleculeSet(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, bool useLocalOptimization, bool readInOptimizedCoordinates);

bool init3DMoleculeSet(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, bool useLocalOptimization, bool readInOptimizedCoordinates);

bool init3DMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, FLOAT maxAtomDist, bool useLocalOptimization, bool readInOptimizedCoordinates);

bool init3DNonFragMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, FLOAT maxAtomDist, bool useLocalOptimization, bool readInOptimizedCoordinates);

MoleculeSet *transformMoleculeSet(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions,
                                  FLOAT deltaForCoordinates, FLOAT deltaForAnglesInRad, FLOAT temperature,
                                  FLOAT boltzmanConstant, bool usePreviousWaveFunction, bool bPerformNonFragmentedSearch,
                                  bool bPerformBasinHopping, FLOAT maxDist);

void swarmMoleculeSet(MoleculeSet &moleculeSet, MoleculeSet &minDistMoleculeSet, MoleculeSet &populationBestSet,
                      MoleculeSet &individualBestSet, Energy &energy, FLOAT coordInertia,
                      FLOAT coordIndividualMinimumAttraction, FLOAT coordPopulationMinimumAttraction,
                      FLOAT coordMaxVelocity, FLOAT angleInertia, FLOAT angleIndividualMinimumAttraction,
                      FLOAT anglePopulationMinimumAttraction, FLOAT angleMaxVelocity, Point3D &boxDimensions,
                      bool enforceMinDistOnACopy, bool usePreviousWaveFunction, FLOAT attractionRepulsion,
                      FLOAT fIndividualBestUpdateDist, bool useLocalOptimization);

/*******************************************************************************
* Purpose: This tread function performs the task specified in nodeData
* Parameters: nodeData - the data for this node
*******************************************************************************/
void * performTask(void * nodeData);

/*******************************************************************************
* Purpose: This function performs the task specified in nodeDataTemplate for all
*    the moleculeSets in nodeDataTemplate.  It does this by calling threads.
* Parameters: nodeDataTemplate - tempalate structure to be passed into each node
*             energyObjects - a set of energy objects, one for each node
*             numEnergyObjectsOrNodes - the number of nodes which should be equal to the numbe of energy objects
*******************************************************************************/
void performAllTasks(NodeData_t &nodeDataTemplate, Energy **energyObjects, int numEnergyObjectsOrNodes);

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
void initializePopulation(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets,
                          NodeData_t &nodeData, Energy** energyObjects, bool useLocalOptimization,
			  bool readInOptimizedCoordinates);

/*******************************************************************************
* Purpose: This function performs Simulated Annealing
* Parameters: input - an input object
*             moleculeSets - the population (initizlied inside input if a resume file is read)
*             bestNMoleculeSets - the bestNStructures (initizlied inside input if a resume file is read)
* Returns: nothing
*******************************************************************************/
void simulatedAnnealing(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets, bool usingSeeding);

/*******************************************************************************
* Purpose: This function performs Particle Swarm Optimization
* Parameters: input - an input object
*             moleculeSets - the population (initizlied inside input if a resume file is read)
*             bestNMoleculeSets - the bestNStructures (initizlied inside input if a resume file is read)
*             bestIndividualMoleculeSets - the best structures seen by each individual in the population
* Returns: nothing
*******************************************************************************/
void particleSwarmOptimization(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets,
                               vector<MoleculeSet*> &bestIndividualMoleculeSets, bool usingSeeding);

/*******************************************************************************
* Purpose: This function finds the visibility RMS distance parameter input.m_fLocalMinDist
*    based on a desired percentage of visibility.
* Parameters: population - the individuals in the population
*             desiredPercentConnectivity - the desired percentage of visibility between particles
*             desiredDist - return value which is the distance desired
*             maxDist - value which is the maximum distance
*******************************************************************************/
FLOAT getConnectivityDistance(vector<MoleculeSet*> &population, FLOAT desiredPercentConnectivity);

/*******************************************************************************
 * Purpose: This function finds the best solution within a local distance of each individual solution.
 * Parameters: population - the individuals in the population
 *             localBestMoleculeSets - where the best local solution for each individual is stored
 *             distanceMatrix        - distance matrix for the population
 *             distance - the maximum distance between two solutions required for them to be "local" to one another
 * Returns: nothing
 ******************************************************************************/
FLOAT findLocalBestMoleculeSets(vector<MoleculeSet*> &population, vector<MoleculeSet*> bestIndividualMoleculeSets,
                                vector<MoleculeSet*> &localBestMoleculeSets, FLOAT **distanceMatrix, FLOAT distance);

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
void gega(Input &input, vector<MoleculeSet*> &population, vector<MoleculeSet*> &bestNMoleculeSets, bool usingSeeding);


/*******************************************************************************
* Purpose: This function deletes the old energy files before beginning calculations.
* Parameters: moleculeSets - the population
*             energyObjects - the energy objects
*             numEnergyObjects - the number of energy objects
*             usePreviousWaveFunction - true if we're using previous wave functions
* Returns: nothing
*******************************************************************************/
void deleteEnergyFiles(vector<MoleculeSet*> &moleculeSets, Energy** energyObjects, int numEnergyObjects,
                       bool usePreviousWaveFunction);

#endif
