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

const char* ExternalEnergy::cclibPythonScript = "/home/sethcall/Chemistry/cclib-1.0.1/atomicGlobalMin.py";
unsigned int ExternalEnergy::s_iMaxEnergyCalcFailuresOnStructure = 2;

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

void ExternalEnergy::replace(std::string src, const char symbol, std::map<const char*, const char*, cmp_str> &map, std::string &dest) {
	const char* c = src.c_str();
	const char* r;
	std::string temp;
	size_t i = 0;
	bool stop;
	while (c[i] != '\0') {
		if (c[i] == symbol) {
			if (i != 0) {
				dest.append(c, i);
				c += i;
				i = 0;
			}
			++c;
			stop = false;
			do {
				switch (c[i]) {
				case ' ':
				case '\t':
				case '\n':
				case '\r':
				case '\f':
				case '\0':
					stop = true;
					break;
				default:
					++i;
				}
			} while (!stop);
			if (i > 0) {
				temp = "";
				temp.append(c, i);
				r = map[temp.c_str()];
				if (NULL != r)
					dest.append(r);
				c += i;
				i = 0;
			}
			continue;
		}
		++i;
	}
	if (0 != i)
		dest.append(c, i);
}
