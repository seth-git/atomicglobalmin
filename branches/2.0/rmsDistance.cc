
#include "rmsDistance.h"

void RmsDistance::updateAtomToCenterRanks(Structure &structure) {
	unsigned int iNumberOfAtoms = structure.getNumberOfAtoms();
	unsigned int atomToCenterRanks[iNumberOfAtoms];
	COORDINATE3 centerOfMass;
	FLOAT distancesToCenterOfMass[iNumberOfAtoms];
	const COORDINATE4* const* coordinates = structure.getAtomCoordinates();
	const FLOAT* coordinate;
	FLOAT diffX, diffY, diffZ;

	structure.getCenterOfMass(centerOfMass);
	for (unsigned int i = 0; i < iNumberOfAtoms; ++i) {
		coordinate = *(coordinates[i]);
		diffX = coordinate[0] - centerOfMass[0];
		diffY = coordinate[1] - centerOfMass[1];
		diffZ = coordinate[2] - centerOfMass[2];
		distancesToCenterOfMass[i] = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
		atomToCenterRanks[i] = i;
	}
	sortAtomRanks(atomToCenterRanks,structure.getAtomicNumbers(),distancesToCenterOfMass,0,iNumberOfAtoms-1);
	structure.setAtomToCenterRanks(atomToCenterRanks);
}

void RmsDistance::sortAtomRanks(unsigned int* rankArray,
		const unsigned int* atomicNumbers,
		const FLOAT *distancesToCenterOfMass, int lo, int hi) {
	int left, right; // these must be signed
	FLOAT fMedianDist;
	int iMedianAtomicNumber;
	int tempAtom;

	if( hi > lo ) // if at least 2 elements, then
	{
		left=lo; right=hi;

		// We don't know what the median is, so we'll just pick something
		tempAtom = rankArray[(lo+hi)/2];
		fMedianDist = distancesToCenterOfMass[tempAtom];
		iMedianAtomicNumber = (int)atomicNumbers[tempAtom];

		while(right >= left)
		{
			while(compareAtoms((int)atomicNumbers[rankArray[left]], distancesToCenterOfMass[rankArray[left]],
			                   iMedianAtomicNumber, fMedianDist) < 0)
				left++;
			while(compareAtoms((int)atomicNumbers[rankArray[right]], distancesToCenterOfMass[rankArray[right]],
			                   iMedianAtomicNumber, fMedianDist) > 0)
				right--;

			if(left > right)
				break;

			// Swap
			tempAtom=rankArray[left];
			rankArray[left]=rankArray[right];
			rankArray[right]=tempAtom;

			left++;
			right--;
		}

		sortAtomRanks(rankArray, atomicNumbers, distancesToCenterOfMass, lo, right);
		sortAtomRanks(rankArray, atomicNumbers, distancesToCenterOfMass, left, hi);
	}
}

int RmsDistance::compareAtoms(int atomicNumber1, FLOAT dist1,
		int atomicNumber2, FLOAT dist2) {
	if (atomicNumber1 != atomicNumber2)
		return atomicNumber1 - atomicNumber2;
	else if (dist1 < dist2)
		return -1;
	else if (dist1 == dist2)
		return 0;
	else // if (atom1Dist > atom2Dist)
		return 1;
}

FLOAT RmsDistance::calculate(const Structure &structure1, const Structure &structure2) {
	unsigned int iNumberOfAtoms = structure1.getNumberOfAtoms();
	unsigned int totalComparisons = ((iNumberOfAtoms-1) * iNumberOfAtoms / 2);
	const unsigned int* atomToCenterRanks1 = structure1.getAtomToCenterRanks();
	const unsigned int* atomToCenterRanks2 = structure2.getAtomToCenterRanks();
	const FLOAT* const* atomDistances1 = structure1.getAtomDistanceMatrix();
	const FLOAT* const* atomDistances2 = structure2.getAtomDistanceMatrix();
	unsigned int iRank, jRank;
	FLOAT diff;
	FLOAT distance = 0;
	unsigned int atom1Index, atom2Index;
	unsigned int otherAtom1Index, otherAtom2Index;

	unsigned int iNumberOfAtomsM1 = iNumberOfAtoms-1;
	for (iRank = 0; iRank < iNumberOfAtomsM1; ++iRank) {
		atom1Index = atomToCenterRanks1[iRank];
		otherAtom1Index = atomToCenterRanks2[iRank];
		for (jRank = iRank+1; jRank < iNumberOfAtoms; ++jRank) {
			atom2Index = atomToCenterRanks1[jRank];
			otherAtom2Index = atomToCenterRanks2[jRank];

			diff = atomDistances1[atom1Index][atom2Index] -
			       atomDistances2[otherAtom1Index][otherAtom2Index];

			distance += diff*diff;
		}
	}
	distance /= totalComparisons;
	distance = sqrt(distance);
	return distance;
}
