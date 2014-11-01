/*
 * ExternalEnergyMethod.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: sethcall
 */

#include "externalEnergy.h"
#include "externalEnergyXml.h"
#include "gaussian.h"
#include "gamess.h"
#include <mpi.h>
#include <unistd.h>

const char* ExternalEnergy::cclibPythonScript = "/path/to/cclib/atomicGlobalMin.py";
unsigned int ExternalEnergy::s_iMaxEnergyCalcFailuresOnStructure = 2;

bool ExternalEnergy::s_bReadGeometry = true;

ExternalEnergy::ExternalEnergy(const ExternalEnergyXml* pExternalEnergyXml) : Energy() {
	m_pExternalEnergyXml = pExternalEnergyXml;
}

ExternalEnergy* ExternalEnergy::instance(Impl impl,
		const ExternalEnergyXml* pExternalEnergyXml) {
	switch(impl) {
	case GAUSSIAN:
		return new Gaussian(pExternalEnergyXml);
	case GAMESS:
		return new Gamess(pExternalEnergyXml);
	default:
		printf("There is no method for creating '%1$s' input files. ", getEnumString(impl));
		printf("Please create a new class for this method that extends ExternalEnergyMethod.\n");
		return NULL;
	}
}

bool ExternalEnergy::readOutputFile(Impl impl, const char* outputFile,
		Structure &structure, bool readGeometry) {
	switch (impl) {
	case GAUSSIAN:
		return Gaussian::readOutputFile(outputFile, structure, readGeometry);
	default:
		return readOutputFileWithCCLib(outputFile, structure, readGeometry);
	}
}

bool ExternalEnergy::isCCLibInstalled(std::string &error) {
	char cmd[500];
	char line[500];
	static const char* testFile = "";
	snprintf(cmd, sizeof(cmd), "python %s %s scfenergies_last_au isTransitionState natom atomnos atomcoords_last", cclibPythonScript, testFile);
	FILE* pipe = NULL;
	pipe = popen(cmd, "r");
	error = "";
	if (pipe == NULL) {
		error = "Unable to call cclib.";
	} else if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL)) {
		error = "Unable get output from cclib.";
	}
	if (pipe != NULL)
		pclose(pipe);
	return error.length() == 0;
}

bool ExternalEnergy::readOutputFileWithCCLib(const char* fileName,
		Structure &structure, bool readGeometry) {
	char cmd[500];
	char line[500];
	unsigned int numAtoms;
	unsigned int lineLength;
	COORDINATE4 *cartesianPoints = NULL;
	FLOAT* cartesianPoint;
	unsigned int *atomicNumbers = NULL;
	unsigned int i;
	char* myString;
	const char* infoNotFoundFlag = "not found\n";

	bool error = false;
	bool readEnergy = false;
	bool obtainedGeometry = false;
	structure.setEnergy(0);
	structure.setIsTransitionState(false);

	if (readGeometry)
		snprintf(cmd, sizeof(cmd), "python %s %s scfenergies_last_au isTransitionState natom atomnos atomcoords_last", cclibPythonScript, fileName);
	else
		snprintf(cmd, sizeof(cmd), "python %s %s scfenergies_last_au", cclibPythonScript, fileName);
//	printf("executing: %s\n", cmd);
	FILE* pipe = NULL;
	try {
		pipe = popen(cmd, "r");
		if (pipe == NULL) {
			printf("Please install cclib.");
			throw "";
		}
		if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL)) {
			printf("Please install cclib.");
			throw "";
		}

		if ((strncmp("I/O error 2 (", line, 13) == 0) && (strstr(line, fileName) != 0)) {
			printf("Unable to open this file with cclib: %1$s\n", fileName);
			throw "";
		}

		if (strcmp(line,infoNotFoundFlag) == 0) {
			printf("Unable to read energy from file: %1$s\n", fileName);
			throw "";
		}
		structure.setEnergy(atof(line));
		readEnergy = true;

		if (readGeometry) {
			if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
				throw "Error reading transition state information from cclib.";
			if (strcmp(line,infoNotFoundFlag) == 0)
				structure.setIsTransitionState(false);
			else
				structure.setIsTransitionState((bool)atoi(line));

			if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
				throw "Error reading the number of atoms from cclib.";
			if (strcmp(line,infoNotFoundFlag) == 0)
				throw "The number of atoms was not found in the output file.";
			numAtoms = atoi(line);
			if (numAtoms == 0)
				throw "The number of atoms returned from cclib must be greater than zero.";

			atomicNumbers = new unsigned int[numAtoms];
			i = 0;
			while (i < numAtoms) {
				if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
					throw "Error reading atomic number line from cclib.";
				if (strcmp(line,infoNotFoundFlag) == 0)
					throw "The atomic numbers were not found in the output file.";
				lineLength = strlen(line);
				if (lineLength < 2)
					throw "cclib atomic number line length is less than the minimum.";

				// get rid of brakets
				line[0] = ' ';
				if (line[lineLength-2] == ']')
					line[lineLength-2] = ' ';

				myString = strtok(line, " ");
				while ((myString != NULL) && (i < numAtoms)) {
					atomicNumbers[i] = atoi(myString);
					++i;
					myString = strtok(NULL, " ");
				}
			}
			if (i != numAtoms)
				throw "Failed to read the atomic numbers in ExternalEnergyMethod::readOutputFileWithCCLib.";

			cartesianPoints = new COORDINATE4[numAtoms];
			for (i = 0; i < numAtoms; ++i) {
				if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
					throw "Error reading geometry line from cclib.";
				if (strcmp(line,infoNotFoundFlag) == 0)
					throw "The geometry was not found in the output file.";
				lineLength = strlen(line);
				if (lineLength < 8)
					throw "cclib geometry line length is less than the minimum.";

				// get rid of brakets
				line[0] = ' ';
				line[1] = ' ';
				line[lineLength-2] = ' ';
				if (line[lineLength-3] == ']')
					line[lineLength-3] = ' ';
				cartesianPoint = cartesianPoints[i];
				if (sscanf(line, "%lf %lf %lf", &cartesianPoint[0], &cartesianPoint[1], &cartesianPoint[2]) != 3)
					throw "Error reading Cartesian coordinates in ExternalEnergyMethod::readOutputFileWithCCLib";
				cartesianPoint[3] = 1;
			}
			structure.setAtoms(numAtoms, cartesianPoints, atomicNumbers);
			obtainedGeometry = true;
		}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			std::cerr << "Caught message: '" << message << "'" << std::endl;
		error = true;
	}
	if (atomicNumbers != NULL)
		delete[] atomicNumbers;
	if (cartesianPoints != NULL)
		delete[] cartesianPoints;
	if (pipe != NULL)
		pclose(pipe);

	return !error && readEnergy && (!readGeometry || obtainedGeometry);
}

const char* ExternalEnergy::getOutputFileExtension(Impl impl) {
	switch (impl) {
	case GAUSSIAN:
		return Gaussian::s_sOutputFileExtension;
	case GAMESS:
		return Gamess::s_sOutputFileExtension;
	default:
		printf("Unknown output file extension for method: %s. ", getEnumString(impl));
		printf("Please create a new class for this method that extends ExternalEnergyMethod.\n");
		return NULL;
	}
}

bool ExternalEnergy::getEnum(const char* attributeName, const char* stringValue, Impl &result, const rapidxml::xml_node<>* pElem) {
	using namespace strings;
	const char* methods[] = {pADF, pGAMESS, pGAMESSUK, pGaussian,
			pFirefly, pJaguar, pMolpro, pORCA};
	return XsdTypeUtil::getEnumValue(attributeName, stringValue, result, pElem, methods);
}

const char* ExternalEnergy::getEnumString(Impl enumValue) {
	using namespace strings;
	const char* methods[] = {pADF, pGAMESS, pGAMESSUK, pGaussian,
			pFirefly, pJaguar, pMolpro, pORCA};
	return methods[enumValue];
}

void ExternalEnergy::replace(std::string &str, const std::string &find, const std::string &replace) {
	size_t pos = 0;
	while ((pos = str.find(find, pos)) != std::string::npos) {
		str.replace(pos, find.length(), replace);
		pos += find.length();
	}
}

bool ExternalEnergy::setup() {
	using namespace std;
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	m_sCalcDirectory = "";
	if (m_pExternalEnergyXml->m_sTemporaryDir.length() > 0) {
		m_sCalcDirectory.append(m_pExternalEnergyXml->m_sTemporaryDir.c_str())
			.append("/").append(strings::pResults, sizeof(strings::pResults)-1)
			.append(ToString(rank)).append("_").append(ToString(getpid()));
		if (!FileUtils::exists(m_sCalcDirectory.c_str()))
			if (!FileUtils::makeDirectory(m_sCalcDirectory.c_str()))
				return false;
	} else {
		m_sCalcDirectory = m_pExternalEnergyXml->m_sResultsDir;
	}
	m_bMoveFilesToResultsDir =
			m_pExternalEnergyXml->m_sTemporaryDir.length() > 0 &&
			m_pExternalEnergyXml->m_sResultsDir.length() > 0;

	if (chdir(m_sCalcDirectory.c_str()) == -1) {
		printf("Couldn't cd to directory: '%s'.\n", m_sCalcDirectory.c_str());
		return false;
	}
	return true;
}

bool ExternalEnergy::cleanup() {
	if (m_pExternalEnergyXml->m_sTemporaryDir.length() > 0) {
		if (chdir("..") == -1) {
			printf("Couldn't cd out of directory: '%s'.\n", m_sCalcDirectory.c_str());
			return false;
		}
		return FileUtils::deleteFile(m_sCalcDirectory.c_str());
	}
	return true;
}

