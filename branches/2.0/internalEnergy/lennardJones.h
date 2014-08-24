
#ifndef LENNARD_JONES_H_
#define LENNARD_JONES_H_

#include "internalEnergy.h"
#include <cmath>

class LennardJones : public InternalEnergy {
public:
	LennardJones(const InternalEnergyXml* pInternalEnergyXml);
	~LennardJones();

	bool calculateEnergy(Structure &structure);
	bool localOptimization(Structure &structure);

private:
	const static FLOAT epsilon;
	const static FLOAT sigma;
	const static FLOAT a;
	const static FLOAT b;
};

#endif /* LENNARD_JONES_H_ */
