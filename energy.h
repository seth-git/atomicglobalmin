////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "moleculeSet.h"
#include "input.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using std::ofstream;
using namespace std;

#define	GAUSSIAN		1
#define	LENNARD_JONES	2

#define	LENNARD_JONES_EPSILON	1.0
#define	LENNARD_JONES_SIGMA		1.0
#define ENERGY_TEMP_FILE	"temporaryJob"

class Energy
{
private:
	int m_iEnergyFunctionToUse;
	FLOAT m_fEnergy;
	bool m_bConverged;
	int m_iTimesCalculatedEnergy;
	int m_iTimesConverged;
	const Input* m_pInput;
	string m_sRun; //abinitio or semi
	int m_iEnergyObjectId;
	string m_sEnergyObjectId;
	const char *m_nodeName;
	string m_sGeneralEnergyFileHeader;
	string m_sGeneralCheckPointFileName;
	
public:
	Energy (const Input *pInput, int fileNameIndex, const char *nodeName, string energyFunction);
	~Energy ();
	void calculateEnergy(MoleculeSet &moleculeSet, bool usePreviousWaveFunctionThisTime, bool performLocalOptimization, bool readOptimizedCoordinates);
	bool getConverged() { return m_bConverged; }
	FLOAT getEnergy() { return m_fEnergy; }
	int getTimesCalculatedEnergy() { return m_iTimesCalculatedEnergy; };
	int getTimesConverged() { return m_iTimesConverged; };
	void resetTimesCalculatedEnergy() { m_iTimesCalculatedEnergy = 0; m_iTimesConverged = 0; };
	void backupWaveFunctionFile(MoleculeSet &moleculeSet); // Backs up the checkpoint file for the associated moleculeSet
	void restoreWaveFunctionFile(MoleculeSet &moleculeSet); // This replaces the checkpoint file with the backup
	void deleteEnergyFiles(MoleculeSet *moleculeSet);
	void getStandardOrientation(const string &logFile, MoleculeSet &moleculeSet);

private:
	bool Energy::isFileCharacter(char character);
	void writeOne(const string &fileToMake, int config, const string &sEnergyFileHeader,
                  int charge, int multiplicity, MoleculeSet &moleculeSet);
	string int2string(long long number);
	bool getEsp(const string &logFile, FLOAT &answer);
	bool  extractE(const string &logFile, FLOAT &answer);
};



