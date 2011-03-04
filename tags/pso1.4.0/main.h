////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MAIN_H_
#define __MAIN_H_

#include "input.h"
#include "argumentParser.h"
#include "energy.h"
#include "myMpi.h"
#include "init.h"
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

/*******************************************************************************
* Purpose: This function performs Simulated Annealing
* Parameters: input - an input object
*             moleculeSets - the population (initizlied inside input if a resume file is read)
*             bestNMoleculeSets - the bestNStructures (initizlied inside input if a resume file is read)
* Returns: true if the run completed
*******************************************************************************/
bool simulatedAnnealing(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets, string &seedFiles, bool bIndependentRun);

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

/*******************************************************************************
* Purpose: This function checks to see how many energy calculations were
*    successful.
* Parameters: the population size
* Returns: true if the percentage is above MIN_CONVERGENCE_PERCENT
*******************************************************************************/
bool energyCalculationSuccess(int populationSize);

/*******************************************************************************
* Purpose: This function reads the input file and initiates the master MPI process.
*    Simulated Annealing and Particle Swarm functions are called from here.
* Parameters: argc - the number of command line arguments
*             argv - the command line arguments
*             rank - mpi rank
* Returns: nothing
*******************************************************************************/
int master(int argc, char *argv[], int rank);

#endif
