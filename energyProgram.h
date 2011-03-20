////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code identifying an energy program
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENERGY_PROGRAM_H__
#define __ENERGY_PROGRAM_H__

#define LENNARD_JONES                       1
#define GAUSSIAN                            2
#define GAUSSIAN_WITH_CCLIB                 3
#define GAMESS_US                           4

#define MAX_OUTPUT_FILE_TYPES               10

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;


class EnergyProgram
{
public:
	string m_sName;
	bool m_bUsesMPI;
	bool m_bUsesCclib; // See: http://sourceforge.net/apps/mediawiki/cclib/index.php?title=Main_Page
	string m_sPathToExecutable;
	int m_iProgramID;
	string m_sInputFileExtension;
	int m_iNumOutputFileTypes;
	string m_sOutputFileTypeExtensions[MAX_OUTPUT_FILE_TYPES];
	bool m_bOutputFileTypeRequired[MAX_OUTPUT_FILE_TYPES];
	
	EnergyProgram (const char* name, bool useMPI, bool bUsesCclib, const char* pathToExecutable, int programID, const char* inputFileExtension);
	EnergyProgram (void);
	void copy(EnergyProgram &energyProgram);
	string toString();
	bool set(vector<char*> parameters);
	int getNumParameters();

	static void init(void);
	static void addOutputFileType(const char* fileExtension, bool required);
	static void cleanUp(void);
	
	static vector<EnergyProgram*> s_energyPrograms;
	static const char* cclibPythonScript;
};

#endif
