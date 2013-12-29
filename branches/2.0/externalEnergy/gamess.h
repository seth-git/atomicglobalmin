/*
 * gaussian.h
 *
 *  Created on: Jan 18, 2012
 *      Author: sethcall
 */

#ifndef GAMESS_H_
#define GAMESS_H_

#include "externalEnergy.h"
#include <algorithm>
#include <string>
#include <stdio.h>

class Gamess : public ExternalEnergy {
public:
	static const char* s_sPathToExecutable;
	static const char* s_sInputFileExtension;
	static const char* s_sOutputFileExtension;
	static const char* s_optionalOutputFileExtensions[];

	Gamess(const ExternalEnergyXml* pExternalEnergyXml);

	bool createInputFile(Structure &structure,
			unsigned int populationMemberNumber, bool writeEnergyValueInHeader,
			bool writeMetaData);

	bool setup() { return true; }
	bool execute(Structure &structure);
	bool cleanup() { return true; }
};

#endif /* GAMESS_H_ */
