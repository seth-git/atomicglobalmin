////////////////////////////////////////////////////////////////////////////////
// Purpose: This file performs energy calculations on multiple nodes using
//    Gaussian.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "mpi.h"
#include "energy.h"
#include "typedef.h"

using namespace std;

int main(int argc, char *argv[])
{
	MPI::Init(argc, argv);
	string sPathToEnergyProgram;
	string sFileNamePrefix;
	char commandLine[500];
	char logFile[300];

	int rank = MPI::COMM_WORLD.Get_rank();
	int size = MPI::COMM_WORLD.Get_size();
	
	int numCalculations;
	string scratchDir;
	string scratchCommand;
	int i, j;
	int energyReturned = 0;
	FLOAT energy;

	if ((argc != 4) && (argc != 6)) {
		cout << "Incorrect number of arguments to mpi (" << argc << ")..." << endl;
		MPI::Finalize();
		return(0);
	}
	sPathToEnergyProgram = argv[1];
	sFileNamePrefix = argv[2];
	numCalculations = atoi(argv[3]);
	if (argc == 6) {
		snprintf(commandLine, 500, "%s/%s%d", argv[4], argv[5], rank);
		scratchDir = commandLine;
		scratchCommand = "export GAUSS_SCRDIR=" + scratchDir + " && ";
//		scratchCommand = "env SCRDIR=" + scratchDir + " ";
		snprintf(commandLine, 500, "mkdir %s", scratchDir.c_str());
		system(commandLine);
	} else {
		scratchDir = "";
		scratchCommand = "";
	}
	for (i = rank; i < numCalculations; i += size) {
		snprintf(logFile, 300, "%s%d.log", sFileNamePrefix.c_str(), (i+1));
		j = 0;
		do {
			++j;
			if (j > 1)
				cerr << "Error " << energyReturned << " in reading file '" << logFile << "', starting try # " << j << "..." << endl;
			snprintf(commandLine, 500, "%s%s < %s%d.com > %s%d.log", scratchCommand.c_str(),
			         sPathToEnergyProgram.c_str(), sFileNamePrefix.c_str(), (i+1), sFileNamePrefix.c_str(), (i+1));
			system(commandLine);
			energyReturned = Energy::readGaussianLogFile(logFile, energy, NULL, false, false);
		} while (!((energyReturned & OPENED_FILE) && (energyReturned & READ_ENERGY)) && (j < 2));
	}
	if (argc == 6) {
		snprintf(commandLine, 500, "rm -rf %s", scratchDir.c_str());
		system(commandLine);
	}
//	cout << "Hello World! I am " << rank << " of " << size << endl;

	MPI::Finalize();
	return(0);
}

