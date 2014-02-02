#ifndef RMSDISTANCE_H_
#define RMSDISTANCE_H_

#include "structure.h"

class RmsDistance {
public:
	RmsDistance() { }

	static void updateAtomToCenterRanks(Structure &structure);

	/**************************************************************************
	 * Purpose: This method calculates the root mean squared distance between
	 *    two structures. This method assumes the updateAtomToCenterRanks
	 *    method has been run on each structure. It also assumes the atom
	 *    distance matrix is up to date on each structure.
	 * Parameters: structure1 and structure2 - the two structures
	 * Returns: the RMS distance between the two structures.
	 */
	static FLOAT calculate(const Structure &structure1, const Structure &structure2);
protected:
	static void sortAtomRanks(unsigned int* rankArray, const unsigned int* atomicNumbers, const FLOAT *distancesToCenterOfMass, int lo, int hi);
	static int compareAtoms(int atom1AtomicNumber, FLOAT atom1Dist, int atom2AtomicNumber, FLOAT atom2Dist);
};

#endif /* RMSDISTANCE_H_ */
