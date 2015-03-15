
#ifndef GAUSSIAN_H_
#define GAUSSIAN_H_

#include "externalEnergy.h"
#include <algorithm>
#include <string>
#include <stdio.h>

class Gaussian : public ExternalEnergy {
public:
	static const char s_sPathToExecutable[];
	static const char s_sInputFileExtension[];
	static const char s_sOutputFileExtension[];

	Gaussian(const ExternalEnergyXml* pExternalEnergyXml);
	~Gaussian();

	static bool m_sRenameCheckpointFile;

	bool createInputFile(const char* inputFileName, const Structure &structure);

	static bool readOutputFile(const char* outputFile, Structure &structure, bool readGeometry);

	bool execute(Structure &structure);

private:
	void searchForCheckPointFile();

	static bool s_bGetStandardOrientation;
	static bool s_bRequireNormalTermination;
	static const std::string s_sAtomsReplacement;
	std::string m_sCheckPointFileExpression;

	bool isFileCharacter(char character);
};

#endif /* GAUSSIAN_H_ */
