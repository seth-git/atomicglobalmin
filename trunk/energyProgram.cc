////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for the available energy programs.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "energyProgram.h"

vector<EnergyProgram> EnergyProgram::s_energyPrograms;

void EnergyProgram::init(void) {
	EnergyProgram energyProgram;

	energyProgram.m_sName = "Gaussian";
	energyProgram.m_bUsesMPI = true;
	energyProgram.m_sPathToExecutable = "/fslapps/chem/bin/rung03";
	energyProgram.m_iProgramID = GAUSSIAN;
	energyProgram.m_sInputFileExtension = "com";
	energyProgram.m_iNumOutputFileTypes = 0;
	energyProgram.m_sOutputFileTypeExtensions[energyProgram.m_iNumOutputFileTypes] = "log";
	energyProgram.m_bOutputFileTypeRequired[energyProgram.m_iNumOutputFileTypes] = true;
	++energyProgram.m_iNumOutputFileTypes;
	energyProgram.m_sOutputFileTypeExtensions[energyProgram.m_iNumOutputFileTypes] = "chk";
	energyProgram.m_bOutputFileTypeRequired[energyProgram.m_iNumOutputFileTypes] = false;
	++energyProgram.m_iNumOutputFileTypes;
	s_energyPrograms.push_back(energyProgram);
	
	energyProgram.m_sName = "Lennard Jones";
	energyProgram.m_bUsesMPI = false;
	energyProgram.m_sPathToExecutable = "";
	energyProgram.m_sInputFileExtension = "";
	energyProgram.m_iProgramID = LENNARD_JONES;
	energyProgram.m_iNumOutputFileTypes = 0;
	s_energyPrograms.push_back(energyProgram);
}

string EnergyProgram::toString() {
	char prefix[1000];
	snprintf(prefix, sizeof(prefix), "%s|%d|%s|%d|%s|%d", 
	         m_sName.c_str(),
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
	if (parameters.size() < 6)
		return false;
	m_sName = parameters[0];
	m_bUsesMPI = parameters[1];
	m_sPathToExecutable = parameters[2];
	m_iProgramID = atoi(parameters[3]);
	m_sInputFileExtension = parameters[4];
	m_iNumOutputFileTypes = atoi(parameters[5]);
	if ((signed int)parameters.size() < getNumParameters())
		return false;
	for (i = 0; i < m_iNumOutputFileTypes; ++i) {
		j = 6 + (i*2);
		m_sOutputFileTypeExtensions[i] = parameters[j];
		m_bOutputFileTypeRequired[i] = atoi(parameters[j+1]);
	}
	return true;
}

int EnergyProgram::getNumParameters() {
	return 6 + (m_iNumOutputFileTypes*2);
}

