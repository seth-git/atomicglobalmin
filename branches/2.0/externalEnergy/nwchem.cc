#include "nwchem.h"
#include "externalEnergyXml.h"

const char Nwchem::s_sPathToExecutable[] = "/Full/path/to/NWChem/executable";
const char Nwchem::s_sInputFileExtension[] = "nw";
const char Nwchem::s_sOutputFileExtension[] = "nwo";

const std::string Nwchem::s_sAtomsReplacement = "#atoms";

Nwchem::Nwchem(const ExternalEnergyXml* pExternalEnergyXml) : ExternalEnergy(pExternalEnergyXml) {
}

Nwchem::~Nwchem() {
}

bool Nwchem::createInputFile(const char* inputFileName, const Structure &structure) {
	std::string atoms;
	unsigned int i;
	unsigned int n = structure.getNumberOfAtoms();
	const unsigned int* atomicNumbers = structure.getAtomicNumbers();
	const COORDINATE4* const* coordinates = structure.getAtomCoordinates();
	const FLOAT* coordinate;
	char buffer[81];
	atoms.reserve(n*80);
	for (i = 0; i < n; ++i) {
		coordinate = *coordinates[i];
		snprintf(buffer, sizeof(buffer), "%-2s %0.6lf %0.6lf %0.6lf\n", Handbook::getAtomicSymbol(atomicNumbers[i]), coordinate[0], coordinate[1], coordinate[2]);
		atoms.append(buffer);
	}

	std::string header = m_pExternalEnergyXml->m_sHeader;
	replace(header, s_sAtomsReplacement, atoms);

	FILE *fp = fopen(inputFileName, "w");
	if (fp == NULL) {
		printf(strings::ErrorWritingFile, inputFileName);
		return false;
	}
	fputs(header.c_str(), fp);
	fclose(fp);
	return true;
}

bool Nwchem::execute(Structure &structure) {
	using namespace std;
	time_t start = time(NULL);
	std::string inputFileName = structure.m_sFilePrefix + "." + s_sInputFileExtension;
	std::string outputFileName = structure.m_sFilePrefix + "." + s_sOutputFileExtension;
	bool haveTempDir = m_pExternalEnergyXml->m_sTemporaryDir.length() > 0;
	bool haveResultsDir = m_pExternalEnergyXml->m_sResultsDir.length() > 0;

	std::string cleanCommand, runCommand;
	if (haveTempDir)
		cleanCommand = "rm -f *.*";
	else
		cleanCommand.append("rm -f ").append(structure.m_sFilePrefix).append(".*");
	runCommand.append(s_sPathToExecutable, sizeof(s_sPathToExecutable)-1)
		.append(" ").append(inputFileName).append(" > ").append(outputFileName);

	if (!FileUtils::executeCommand(cleanCommand.c_str()))
		return false;
	if (!createInputFile(inputFileName.c_str(), structure))
		return false;
	if (!FileUtils::executeCommand(runCommand.c_str()))
		return false;
	if (!readOutputFile(outputFileName.c_str(), structure, m_bReadGeometry))
		return false;
	if (haveTempDir && haveResultsDir) {
		if (!FileUtils::changeDirectory(structure.m_sFilePrefix, m_pExternalEnergyXml->m_sResultsDir))
			return false;
	} else if (haveResultsDir)
		FileUtils::deleteFile(inputFileName.c_str());
	else
		structure.m_sFilePrefix = "";

	time_t elapsed = time(NULL) - start;
	if (elapsed > m_tLongestExecutionTime)
		m_tLongestExecutionTime = elapsed;

	return true;
}

