#ifndef GAMESS_UK_H_
#define GAMESS_UK_H_

#include "externalEnergy.h"
#include <algorithm>
#include <string>
#include <stdio.h>

class GamessUK : public ExternalEnergy {
public:
	static const char s_sPathToExecutable[];
	static const char s_sInputFileExtension[];
	static const char s_sOutputFileExtension[];

	GamessUK(const ExternalEnergyXml* pExternalEnergyXml);
	~GamessUK();

	bool createInputFile(const char* inputFileName, const Structure &structure);

	bool execute(Structure &structure);

private:
	static const std::string s_sAtomsReplacement;
};

#endif /* GAMESS_UK_H_ */
