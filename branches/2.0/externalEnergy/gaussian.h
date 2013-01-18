/*
 * gaussian.h
 *
 *  Created on: Dec 31, 2012
 *      Author: sethcall
 */

#ifndef GAUSSIAN_H_
#define GAUSSIAN_H_

#include "externalEnergyMethod.h"
#include <algorithm>
#include <string>
#include <stdio.h>

class Gaussian : public ExternalEnergyMethod {
public:
	static const char* s_sPathToExecutable;
	static const char* s_sInputFileExtension;
	static const char* s_sOutputFileExtension;
	static const char* s_optionalOutputFileExtensions[];

	Gaussian(const ExternalEnergy* pExternalEnergy);

	bool createInputFile(Structure &structure,
			unsigned int populationMemberNumber, bool writeEnergyValueInHeader,
			bool writeMetaData);

	static bool readOutputFile(const char* outputFile, Structure structure, bool readGeometry);

	bool doEnergyCalculation(unsigned int populationMemberNumber);

private:
	static bool s_bGetStandardOrientation;
	static bool s_bRequireNormalTermination;
	std::string m_sCheckPointFileName;
	bool m_bSemiEmpirical;

	void getCheckpointFileName();
	bool isFileCharacter(char character);
};

#endif /* GAUSSIAN_H_ */
