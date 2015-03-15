#ifndef NWCHEM_H_
#define NWCHEM_H_

#include "externalEnergy.h"
#include <algorithm>
#include <string>
#include <stdio.h>

class Nwchem : public ExternalEnergy {
public:
	static const char s_sPathToExecutable[];
	static const char s_sInputFileExtension[];
	static const char s_sOutputFileExtension[];

	Nwchem(const ExternalEnergyXml* pExternalEnergyXml);
	~Nwchem();

	bool createInputFile(const char* inputFileName, const Structure &structure);

	bool execute(Structure &structure);

private:
	static const std::string s_sAtomsReplacement;
};

#endif /* NWCHEM_H_ */
