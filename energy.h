////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __ENERGY_H__
#define __ENERGY_H__

#include "moleculeSet.h"
#include "input.h"

#include <ctype.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <iostream>

#include <unistd.h>
#include <dirent.h>

using std::ofstream;
using namespace std;

#define ENERGY_TEMP_FILE	"temporaryJob"

#define OPENED_FILE	1
#define READ_ENERGY	2
#define OBTAINED_GEOMETRY	4
#define NORMAL_TERMINATION	8

#define MIN_SUCCESSFULL_ENERGY_CALCULATION_PERCENTAGE	0.9 // 90 percent

class Energy
{
public:
	Energy () {}
	static bool init(const Input &input, int rank);
	static string getGaussianCheckpointFile(const char* gaussianHeader);
	static bool init(const char* mpiInitMessage, int rank);
	static void createMPIInitMessage(char* mpiInitMessage, int maxSize);
	static bool createScratchDir(void);
	static bool deleteScratchDir(void);
	static void createInputFiles(vector<MoleculeSet*> &population);
	static bool createInputFile(MoleculeSet &moleculeSet, int populationMemberNumber, bool resetInputFileName, bool writeMetaData);
	static bool createGaussianInputFile(MoleculeSet &moleculeSet, int populationMemberNumber, bool writeEnergyValueInHeader);
	static int readGaussianOutputFile(const char* logFile, FLOAT &energy, MoleculeSet* pMoleculeSet);
	static void setOutputEnergyFiles(int populationMemberNumber, MoleculeSet &moleculeSet, bool checkExistence);
	static bool doEnergyCalculation(int populationMemberNumber);
	static EnergyProgram* getEnergyProgram() { return &s_energyProgram; }

	static string s_checkPointFileName;

private:
	static void lower(string &s);
	static bool isFileCharacter(char character);

	static EnergyProgram s_energyProgram;
	static string s_header;
	static string s_footer;
	static bool s_bGetStandardOrientation;
	static bool s_bSemiEmpirical;
	static string s_pathToEnergyFiles;
	static int s_iCharge;
	static int s_iMultiplicity;
	static string s_scratchDirectory;
	static string s_fullScratchDirectory;
	static string s_scratchCommand;
};

#endif

