/*
 * Gamess.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: sethcall
 */

#include "gamess.h"
#include "externalEnergyXml.h"

const char* Gamess::s_sPathToExecutable = "/Full/path/to/Gamess/executable";
const char* Gamess::s_sInputFileExtension = "com";
const char* Gamess::s_sOutputFileExtension = "log";
const char* Gamess::s_optionalOutputFileExtensions[] = {"chk"};

Gamess::Gamess(const ExternalEnergyXml* pExternalEnergyXml) : ExternalEnergy(pExternalEnergyXml) {
}

bool Gamess::createInputFile(Structure &structure,
		unsigned int populationMemberNumber, bool writeEnergyValueInHeader,
		bool writeMetaData) {
	return true;
}

bool Gamess::execute(Structure &structure) {
	return true;
}

