/*
 * Gaussian.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: sethcall
 */

#include "gaussian.h"
#include "externalEnergyXml.h"
#include <list>
#include <set>

const char Gaussian::s_sPathToExecutable[] = "/Full/path/to/Gaussian/executable";
const char Gaussian::s_sInputFileExtension[] = "com";
const char Gaussian::s_sOutputFileExtension[] = "log";
const char* Gaussian::s_optionalOutputFileExtensions[] = {"chk"};

bool Gaussian::s_bGetStandardOrientation = false;
bool Gaussian::s_bRequireNormalTermination = false;

Gaussian::Gaussian(const ExternalEnergyXml* pExternalEnergyXml) : ExternalEnergy(pExternalEnergyXml) {
	searchForCheckPointFile();
}

Gaussian::~Gaussian() {
}

void Gaussian::searchForCheckPointFile() {
	std::string lowerHeader;
	lowerHeader.resize(m_pExternalEnergyXml->m_sHeader.length()+1);
	std::transform(m_pExternalEnergyXml->m_sHeader.begin(), m_pExternalEnergyXml->m_sHeader.end(), lowerHeader.begin(), ::tolower);

	std::string::size_type pos;
	const char *header;
	unsigned int i;

	m_sCheckPointFileName = "";
	pos = lowerHeader.find("% chk");
	if (pos == std::string::npos) // if we didn't find it
		pos = lowerHeader.find("%chk");
	if (pos != std::string::npos) { // if we found it
		// find the beginning of the name
		header = m_pExternalEnergyXml->m_sHeader.c_str() + pos;
		while (*header != '=') {
			++header;
			if (header == '\0')
				return;
		}
		while (!isFileCharacter(*header)) {
			++header;
			if (header == '\0')
				return;
		}
		i = 1;
		while (isFileCharacter(header[i]))
			++i;

		m_sCheckPointFileName.append(header,i).append(".").append(s_optionalOutputFileExtensions[0]);
	}
}

bool Gaussian::isFileCharacter(char character)
{
	return (((character >= 'a') && (character <= 'z')) ||
		    ((character >= 'A') && (character <= 'Z')) ||
		    ((character >= '0') && (character <= '9')) ||
		    (character == '.') || (character == ',') || (character == '_'));
}

bool Gaussian::createInputFile(const char* inputFileName, const Structure &structure) {
	std::string atoms;
	unsigned int i, j;
	unsigned int n = structure.getNumberOfAtoms();
	const unsigned int* atomicNumbers = structure.getAtomicNumbers();
	const COORDINATE4* const* coordinates = structure.getAtomCoordinates();
	const FLOAT* coordinate;
	char buffer[100];
	size_t len;
	atoms.reserve(n*80);
	for (i = 0; i < n; ++i) {
		snprintf(buffer, sizeof(buffer), "%u", atomicNumbers[i]);
		atoms.append(buffer);
		coordinate = *coordinates[i];
		for (j = 0; j < 3; ++j) {
			len = XsdTypeUtil::createFloat(coordinate[j], buffer, sizeof(buffer));
			atoms.append(" ").append(buffer, len);
		}
		atoms.append("\n");
	}

	std::map<const char*, const char*, cmp_str> map;
	map[strings::pAtoms] = atoms.c_str();
	std::string header;
	header.reserve(m_pExternalEnergyXml->m_sHeader.length()*2 + atoms.length()*2);
	replace(m_pExternalEnergyXml->m_sHeader, '!', map, header);

	FILE *fp = fopen(inputFileName, "w");
	if (fp == NULL) {
		printf(strings::ErrorWritingFile, inputFileName);
		return false;
	}
	fputs(header.c_str(), fp);
	fclose(fp);
	return true;
}

bool Gaussian::readOutputFile(const char* outputFile, Structure &structure, bool readGeometry) {
	using std::ifstream;
	ifstream fin(outputFile);
	char line[300];
	std::string stringLine;
	std::string nextLine;
	std::string pieceLine;
	int dummy;
	FLOAT* c;
	std::vector<FLOAT*> coordinatesVector;
	std::vector<unsigned int> atomicNumbersVector;
	unsigned int atomicNumber;
	std::string::size_type startingIndex, endIndex;
	unsigned int i;
	bool bSemiEmpericalKnown = false;
	bool bSemiEmperical;

	bool readEnergy = false;
	bool obtainedGeometry = false;
	bool normalTermination = false;

	if (!fin) {
		printf("Unable to open file: %s\n", outputFile);
		return false;
	}

	structure.setIsTransitionState(false);
	while(fin.getline(line, sizeof(line)))
	{
		stringLine=line;
		if (bSemiEmpericalKnown) {
			if (bSemiEmperical)
				startingIndex=stringLine.find("Energy=   ");
			else
				startingIndex=stringLine.find("SCF Done:");
		} else {
			startingIndex=stringLine.find("Energy=   ");
			if (startingIndex != std::string::npos) { // If we found it
				bSemiEmpericalKnown = true;
				bSemiEmperical = true;
			} else {
				startingIndex=stringLine.find("SCF Done:");
				if (startingIndex != std::string::npos) { // If we found it
					bSemiEmpericalKnown = true;
					bSemiEmperical = false;
				}
			}
		}

		if (startingIndex != std::string::npos) // If we found it
		{
			startingIndex=stringLine.find("=");
			startingIndex=stringLine.find("-",startingIndex); //energy should be negative.
			if (startingIndex < 0) { // Sometimes the energy isn't negative with semi-emperical methods
				structure.setEnergy(0);
				readEnergy=true;
				continue;
			}
			endIndex=stringLine.find(" ",startingIndex);

			pieceLine=stringLine.substr(startingIndex, endIndex-startingIndex);
			structure.setEnergy(atof(pieceLine.c_str()));
			readEnergy=true;
		}
		if (readGeometry) {
			startingIndex=stringLine.find(" 1 imaginary frequencies (negative Signs) ");
			if (startingIndex != std::string::npos) // if we found it
				structure.setIsTransitionState(true);

			if (s_bGetStandardOrientation)
				startingIndex=stringLine.find("Standard orientation");
			else
				startingIndex=stringLine.find("Input orientation");

			if (startingIndex != std::string::npos)
			{
				obtainedGeometry = true;
				fin.getline(line, sizeof(line)); //get ----
				fin.getline(line, sizeof(line)); //get first line of text
				fin.getline(line, sizeof(line)); //get second line of text
				fin.getline(line, sizeof(line)); //get ----

				atomicNumbersVector.clear();
				for (i = 0; i < coordinatesVector.size(); ++i)
					delete[] coordinatesVector[i];
				coordinatesVector.clear();

				while (fin.getline(line, sizeof(line))) {
					c = new FLOAT[3];
					if (sscanf(line, "%d %u %d %lf %lf %lf", &dummy, &atomicNumber,
						&dummy, &(c[0]), &(c[1]), &(c[2])) != 6) {
						delete[] c;
						break;
					} else {
						coordinatesVector.push_back(c);
						atomicNumbersVector.push_back(atomicNumber);
					}
				}
			}
		}
		startingIndex=stringLine.find("Normal termination");
		if (startingIndex != std::string::npos) // if we found it
			normalTermination = true;
	}
	fin.close();

	if (readGeometry && obtainedGeometry) {
		COORDINATE4 coordinates[coordinatesVector.size()];
		FLOAT* coordinate;
		unsigned int atomicNumbers[coordinatesVector.size()];

		for (i = 0; i < coordinatesVector.size(); ++i) {
			atomicNumbers[i] = atomicNumbersVector[i];
			coordinate = coordinates[i];
			memcpy(coordinate, coordinatesVector[i], SIZEOF_COORDINATE3);
			coordinate[3] = 1;
		}
		structure.setAtoms(coordinatesVector.size(), coordinates, atomicNumbers);
	}

	// deallocate memory
	atomicNumbersVector.clear();
	for (i = 0; i < coordinatesVector.size(); ++i)
		delete[] coordinatesVector[i];
	coordinatesVector.clear();

	if (!readEnergy) {
		printf("Unable to read energy from file: %1$s\n", outputFile);
		return false;
	}

	if (readGeometry && !obtainedGeometry) {
		printf("Unable to read geometry from file: %1$s\n", outputFile);
		return false;
	}

	if (s_bRequireNormalTermination && !normalTermination) {
		printf("Normal termination is required, but Gaussian didn't terminate normally, see: %1$s\n", outputFile);
		return false;
	}

	return true;
}

bool Gaussian::execute(Structure &structure) {
	return true;
}

