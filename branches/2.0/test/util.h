
#ifndef __UNIT_UTIL_H_
#define __UNIT_UTIL_H_

#include "settings.h"
#include "../input.h"
#include "../handbook/handbook.h"

using std::ofstream;

FLOAT** getDistanceMatrix(const Structure &structure, bool globalOrLocal);

void deleteDistanceMatrix(FLOAT** matrix, unsigned int size);

bool distancesWithinTolerance(FLOAT distance1, FLOAT distance2, FLOAT tolerance);

// This verifies that rotation and translation were performed correctly.
// The correctness is verified by examining distances between atoms within molecules.
// These should be the same before and after translation and rotation.
bool transformationCheck(const Structure &moleculeSet);

/*************************************************************************************************************
 * Purpose: This function verifies that minimum and maximum distance constraints have been enforced correctly.
 * Parameters: moleculeSet: the set of molecules
 *             pGeneralMin: a general minimum distance constraint or NULL if there is no such constraint
 *             pMinAtomicDistances: specific minimum distance constraints of NULL if no such constraints exist
 *             pMaxDist: a maximum distance constraint or NULL if there is no such constraint
 * Returns: true if the constraints are satisfied.
 *************************************************************************************************************/
bool distanceConstraintsOK(const Structure &structure, FLOAT *pGeneralMin,
		std::map<unsigned int, std::map<unsigned int,FLOAT> > *pMinAtomicDistances, FLOAT *pMaxDist);

bool structureInsideCube(const Structure &structure, FLOAT halfCubeLWH);

void makeWaterSet(Structure &structure, unsigned int number);

static const FLOAT FLOAT_ERROR = 0.000001;

bool floatsEqual(FLOAT a, FLOAT b);

#endif
