/*
 * Gaussian.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: sethcall
 */

#include "gaussian.h"
#include "externalEnergyXml.h"

const char* Gaussian::s_sPathToExecutable = "/Full/path/to/Gaussian/executable";
const char* Gaussian::s_sInputFileExtension = "com";
const char* Gaussian::s_sOutputFileExtension = "log";
const char* Gaussian::s_optionalOutputFileExtensions[] = {"chk"};

bool Gaussian::s_bGetStandardOrientation = false;
bool Gaussian::s_bRequireNormalTermination = false;

Gaussian::Gaussian(const ExternalEnergyXml* pExternalEnergyXml) : ExternalEnergy(pExternalEnergyXml) {
	getCheckpointFileName();
}

void Gaussian::getCheckpointFileName()
{
	std::string lowerHeader = m_pExternalEnergyXml->m_sHeader;
	std::transform(lowerHeader.begin(), lowerHeader.end(), lowerHeader.begin(), ::tolower);

	std::string::size_type pos;
	const char *header;
	char fileName[500];
	unsigned int fileNameLength;

	m_sCheckPointFileName = "";
	pos = lowerHeader.find("% chk");
	if (pos == std::string::npos) // if we didn't find it
		pos = lowerHeader.find("%chk");
	if (pos != std::string::npos) { // if we found it
		// find the beginning of the name
		header = m_pExternalEnergyXml->m_sHeader.c_str();
		header = &header[pos];
		while (*header != '=')
			++header;
		while ((*header != '\0') && !isFileCharacter(*header))
			++header;
		if (*header == '\0')
			return;
		fileNameLength = 1;
		while (isFileCharacter(header[fileNameLength]) && fileNameLength < sizeof(fileName))
			++fileNameLength;

		strncpy(fileName,header,fileNameLength);
		fileName[fileNameLength] = '\0';
		m_sCheckPointFileName = fileName;
	}

	m_bSemiEmpirical = ((lowerHeader.find("#p") != std::string::npos) || (lowerHeader.find("# p") != std::string::npos));
}

bool Gaussian::isFileCharacter(char character)
{
	return (((character >= 'a') && (character <= 'z')) ||
		    ((character >= 'A') && (character <= 'Z')) ||
		    ((character >= '0') && (character <= '9')) ||
		    (character == '.') || (character == ',') || (character == '_'));
}

bool Gaussian::createInputFile(Structure &structure,
		unsigned int populationMemberNumber, bool writeEnergyValueInHeader,
		bool writeMetaData) {
/*	std::string header = m_pExternalEnergy->m_sHeader;
	std::string footer = m_pExternalEnergy->m_sFooter;
	std::string::size_type pos;

	std::string fullCheckPointFileName;
	if (m_sCheckPointFileName.length() > 0) {
		fullCheckPointFileName.append(m_sCheckPointFileName).append(populationMemberNumber);
		pos = header.find(m_sCheckPointFileName);
		header.replace(pos,s_checkPointFileName.length(), fullCheckPointFileName);
		if (footer.length() > 0) {
			pos = footer.find(m_sCheckPointFileName);
			if (pos != string::npos) // If we found it
				footer.replace(pos,m_sCheckPointFileName.length(), fullCheckPointFileName);
		}
	}

	FILE *fp = fopen(structure.getInputEnergyFile(), "w");
	if (fp == NULL) {
		printf("Unable to write to file: %1$s\n", structure.getInputEnergyFile());
		return false;
	}
	fprintf(fp, "%s\n", header.c_str());
	if (writeEnergyValueInHeader) {
		char energyStr[100];
		XsdTypeUtil::doubleToString(structure.getEnergy(), energyStr);
		fprintf(fp, "This is a computer generated structure with energy: %s\n\n", energyStr);
	} else {
		fprintf(fp, "This is a computer generated structure.\n\n", energyStr);
	}
	fprintf(fp, "%d %u\n", m_pExternalEnergy->m_iCharge, m_pExternalEnergy->m_iMultiplicity);

//	structure.writeCoordinatesToInputFileWithAtomicNumbers(fout);

	fprintf(fp, "\n");

	if (footer.length() > 0)
		fprintf(fp, "%s\nTitle\n\n\d \u\n", footer.c_str(), m_pExternalEnergy->m_iCharge, m_pExternalEnergy->m_iMultiplicity);
	fclose(fp);*/
	return true;
}

bool Gaussian::readOutputFile(const char* outputFile, Structure &structure, bool readGeometry) {
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

