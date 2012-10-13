////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __UNIT_UTIL_H_
#define __UNIT_UTIL_H_

#include "../input.h"
#include "../init.h"
#include "../argumentParser.h"
#include "../energy.h"
#include "../gega.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FRAGMENTED           1
#define PARTIALLY_FRAGMENTED 2
#define NOT_FRAGMENTED       3

using std::ofstream;
using namespace std;

FLOAT** getDistanceMatrix(MoleculeSet &moleculeSet, bool globalOrLocal);

void deleteDistanceMatrix(FLOAT** matrix, unsigned int size);

bool distancesWithinTolerance(FLOAT distance1, FLOAT distance2, FLOAT tolerance);

// This verifies that rotation and translation were performed correctly.
// The correctness is verified by examining distances between atoms within molecules.
// These should be the same before and after translation and rotation.
bool transformationCheck(MoleculeSet &moleculeSet);

/*************************************************************************************************************
 * Purpose: This function verifies that minimum and maximum distance constraints have been enforced correctly.
 * Parameters: moleculeSet: the set of molecules
 *             pGeneralMin: a general minimum distance constraint or NULL if there is no such constraint
 *             pMinAtomicDistances: specific minimum distance constraints of NULL if no such constraints exist
 *             pMaxDist: a maximum distance constraint or NULL if there is no such constraint
 *             fragmentation: 1 means complete fragmentation (no maximum distance constraint)
 *                            2 means partial fragmentation (all molecules are within the maximum distance of
 *                              at least one other molecule, but pairs of molecules can be isolated from the group)
 *                            3 means no fragmentation (max distance enforced with all molecules in one group)
 * Returns: true if the constraints are satisfied.
 *************************************************************************************************************/
bool distanceConstraintsOK(MoleculeSet &moleculeSet, FLOAT *pGeneralMin,
		map<unsigned int, map<unsigned int,FLOAT> > *pMinAtomicDistances, FLOAT *pMaxDist, unsigned int fragmentation);

bool moleculeSetInsideCube(MoleculeSet &moleculeSet, Point3D &boxDimensions);

void printPoint(const Point3D &point);

void printDetailedInfo(MoleculeSet &moleculeSet, bool printBondLengths);

void makeWater(Molecule &molecule);

void makeWaterSet(MoleculeSet &moleculeSet, Molecule &water, unsigned int number);

void setupMinDistances(FLOAT generalMin, map<unsigned int, map<unsigned int,FLOAT> > &minAtomicDistances);

#endif
