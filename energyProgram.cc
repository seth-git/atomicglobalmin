////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for the available energy programs.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "energyProgram.h"

vector<EnergyProgram*> EnergyProgram::s_energyPrograms;
const char* EnergyProgram::cclibPythonScript = "/Full/path/to/cclib-1.0/atomicGlobalMin.py";

void EnergyProgram::init() {
	cleanUp();
	s_energyPrograms.push_back(new EnergyProgram(true, false, "/Full/path/to/gaussian", GAUSSIAN, "com"));
	addOutputFileType("log", true);
	addOutputFileType("chk", false);
	
	s_energyPrograms.push_back(new EnergyProgram(true, true, "/Full/path/to/gaussian", GAUSSIAN_WITH_CCLIB, "com"));
	addOutputFileType("log", true);
	addOutputFileType("chk", false);
	
	// US GAMESS
	s_energyPrograms.push_back(new EnergyProgram(true, true, "/Full/path/to/gamess", GAMESS_US, "gamin"));
	addOutputFileType("gamout", true);
	
	s_energyPrograms.push_back(new EnergyProgram(false, false, "", LENNARD_JONES, ""));
}

EnergyProgram::EnergyProgram(bool useMPI, bool bUsesCclib, const char* pathToExecutable, int programID, const char* inputFileExtension) {
	m_bUsesMPI = useMPI;
	m_bUsesCclib = bUsesCclib;
	m_sPathToExecutable = pathToExecutable;
	m_iProgramID = programID;
	m_sInputFileExtension = inputFileExtension;
	m_iNumOutputFileTypes = 0;
}

EnergyProgram::EnergyProgram(void) {
	m_bUsesMPI = false;
	m_iProgramID = -1;
	m_iNumOutputFileTypes = 0;
}

const char* EnergyProgram::getName(const Strings* messages) {
	switch (m_iProgramID) {
	case GAUSSIAN:
		return messages->m_sGaussian.c_str();
	case GAUSSIAN_WITH_CCLIB:
		return messages->m_sGaussianWithCclib.c_str();
	case GAMESS_US:
		return messages->m_sGAMESS.c_str();
	case LENNARD_JONES:
		return messages->m_sLennardJones.c_str();
	default:
		return "Unknown";
	}
}

const char* EnergyProgram::getName() {
	return getName(Strings::instance());
}

void EnergyProgram::addOutputFileType(const char* fileExtension, bool required) {
	EnergyProgram *pEnergyProgram;
	if (s_energyPrograms.size() == 0)
		return;
	pEnergyProgram = s_energyPrograms[s_energyPrograms.size()-1];
	if (pEnergyProgram->m_iNumOutputFileTypes >= MAX_OUTPUT_FILE_TYPES)
		return;
	pEnergyProgram->m_sOutputFileTypeExtensions[pEnergyProgram->m_iNumOutputFileTypes] = fileExtension;
	pEnergyProgram->m_bOutputFileTypeRequired[pEnergyProgram->m_iNumOutputFileTypes] = required;
	++pEnergyProgram->m_iNumOutputFileTypes;
}

void EnergyProgram::cleanUp() {
	for (unsigned int i = 0; i < s_energyPrograms.size(); ++i)
		delete s_energyPrograms[i];
	s_energyPrograms.clear();
}

void EnergyProgram::copy(EnergyProgram &energyProgram) {
	m_bUsesMPI = energyProgram.m_bUsesMPI;
	m_bUsesCclib = energyProgram.m_bUsesCclib;
	m_sPathToExecutable = energyProgram.m_sPathToExecutable;
	m_iProgramID = energyProgram.m_iProgramID;
	m_sInputFileExtension = energyProgram.m_sInputFileExtension;
	m_iNumOutputFileTypes = energyProgram.m_iNumOutputFileTypes;
	for (int i = 0; i < m_iNumOutputFileTypes; ++i) {
		m_sOutputFileTypeExtensions[i] = energyProgram.m_sOutputFileTypeExtensions[i];
		m_bOutputFileTypeRequired[i] = energyProgram.m_bOutputFileTypeRequired[i];
	}
}

string EnergyProgram::toString() {
	char prefix[1000];
	snprintf(prefix, sizeof(prefix), "%d|%s|%d|%s|%d", 
	         m_bUsesMPI,
	         m_sPathToExecutable.c_str(),
	         m_iProgramID,
	         m_sInputFileExtension.c_str(),
	         m_iNumOutputFileTypes);
	string returnString = prefix;
	for (int i = 0; i < m_iNumOutputFileTypes; ++i) {
		snprintf(prefix, sizeof(prefix), "|%s|%d",
		         m_sOutputFileTypeExtensions[i].c_str(), m_bOutputFileTypeRequired[i]);
		returnString += prefix;
	}
	return returnString;
}

bool EnergyProgram::set(vector<char*> parameters) {
	int i, j;
	if (parameters.size() < 5)
		return false;
	m_bUsesMPI = parameters[0];
	m_sPathToExecutable = parameters[1];
	m_iProgramID = atoi(parameters[2]);
	m_sInputFileExtension = parameters[3];
	m_iNumOutputFileTypes = atoi(parameters[4]);
	if ((signed int)parameters.size() < getNumParameters())
		return false;
	for (i = 0; i < m_iNumOutputFileTypes; ++i) {
		j = 5 + (i*2);
		m_sOutputFileTypeExtensions[i] = parameters[j];
		m_bOutputFileTypeRequired[i] = atoi(parameters[j+1]);
	}
	return true;
}

int EnergyProgram::getNumParameters() {
	return 6 + (m_iNumOutputFileTypes*2);
}

