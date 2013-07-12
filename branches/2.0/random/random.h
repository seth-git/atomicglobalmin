/*
 * Random.h
 *
 *  Created on: Jun 15, 2013
 *      Author: sethcall
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include "randomc.h"
#include "../xsd/typedef.h"

class Random {
private:
	static CRandomMersenne s_cRandomMersenne;
public:
	static bool init(int mpiRank);
	static int getInt(int min, int max);
	static FLOAT getFloat(FLOAT min, FLOAT max);
};

#endif /* RANDOM_H_ */
