
#include "lennardJones.h"
#include "internalEnergyXml.h"

const FLOAT LennardJones::epsilon = 1;
const FLOAT LennardJones::sigma = 1;
const FLOAT LennardJones::a = 4 * epsilon * pow(sigma,12);
const FLOAT LennardJones::b = 4 * epsilon * pow(sigma,6);

LennardJones::LennardJones(const InternalEnergyXml* pInternalEnergyXml) : InternalEnergy(pInternalEnergyXml) {
}

LennardJones::~LennardJones() {
}

bool LennardJones::calculateEnergy(Structure &structure) {
	unsigned int iAtoms = structure.getNumberOfAtoms();
	unsigned int iAtomsM1 = iAtoms - 1;
	const FLOAT* const* distances = structure.getAtomDistanceMatrix();
	unsigned int i, j;

	FLOAT energy = 0;
	FLOAT pow1, pow2;
	for (i = 0; i < iAtomsM1; ++i) {
		for (j = i + 1; j < iAtoms; ++j) {
			pow1 = distances[i][j];
			pow1 = pow1 * pow1 * pow1 * pow1 * pow1 * pow1;
			pow2 = pow1 * pow1;
			energy += (a / pow2) - (b / pow1);
		}
	}
	structure.setEnergy(energy);
	return true;
}

bool LennardJones::localOptimization(Structure &structure) {
	printf("Local optimization with the Lennard Jones potential is not yet implemented.\n");
	return false;
}
