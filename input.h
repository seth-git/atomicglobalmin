////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing a molecule or a set of adams that
//    belong in a group.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __INPUT_H__
#define __INPUT_H__

#include "typedef.h"
#include "molecule.h"
#include "moleculeSet.h"
#include "strings.h"
#include <stdio.h>
#include <cstdio>
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <vector>
#include <cctype> // for tolower
#include <algorithm>

using std::ofstream;
using std::ifstream;
using std::ostream;
using namespace std;
using std::setw;
using std::time;
using std::ostringstream;

#define PROGRAM_VERSION "1.5.0"
#define INPUT_FILE_VERSION "1.5"
#define SIMULATED_ANNEALING 1
#define PARTICLE_SWARM_OPTIMIZATION 2
#define GENETIC_ALGORITHM 3

#define ACCEPTANCE_RATIO_NUM_ITERATIONS 100
#define NUM_ITERATIONS_TO_SET_SCALING_FACTOR 200
#define SET_SCALING_FACTOR_EVERY 20

#define HARTREE_TO_JOULES       2622950

#define PRINT_CATCH_MESSAGES	false

template < class T >
string ToString(const T &arg)
{
	ostringstream	out;

	out << arg;

	return(out.str());
}

// This class reads user input from a file in a specific format
// and can also copy input parameters to a new file.
class Input
{
public:
	string m_sLanguageCode; // 2 characters long
	const Strings* m_messages; // messages for m_sLanguageCode
	const Strings* m_messagesDL; // messages for the default language


	// This class reads the following parameters from the input file.
	// These are public member variables for the sake of easy access.

	// Variables specific to simulated annealing
	bool m_bPerformNonFragSearch;
	bool m_bPerformBasinHopping;
	bool m_bTransitionStateSearch;
	FLOAT m_fStartingTemperature;
	FLOAT m_fBoltzmanConstant;
	FLOAT m_fDesiredAcceptedTransitions; // By giving this variable a value, the program will automatically set m_fBoltzmanConstant
	FLOAT m_fAcceptanceRatio;
	int m_iNumIterationsBeforeDecreasingTemp;
	FLOAT m_fQuenchingFactor;
	FLOAT m_fMinTemperatureToStop;
	FLOAT m_fMinAcceptedTransitions;
	FLOAT m_fNumPerterbations;
	FLOAT m_fStartCoordinatePerturbation;
	FLOAT m_fStartAnglePerturbation;
	FLOAT m_fMinCoordinatePerturbation;
	FLOAT m_fMinAnglePerturbation;
	
	// Variables used in the simulated annealing resume file
	bool m_bDecreasingTemp;
	int *m_prgAcceptedTransitions;
	int m_iAcceptedTransitionsIndex;
	
	// PSO variables
	FLOAT m_fStartCoordInertia;
	FLOAT m_fEndCoordInertia;
	FLOAT m_fCoordIndividualMinimumAttraction;
	FLOAT m_fCoordPopulationMinimumAttraction;
	FLOAT m_fCoordMaximumVelocity;
	FLOAT m_fStartAngleInertia;
	FLOAT m_fEndAngleInertia;
	FLOAT m_fAngleIndividualMinimumAttraction;
	FLOAT m_fAnglePopulationMinimumAttraction;
	FLOAT m_fAngleMaximumVelocity;
	int m_iReachEndInertiaAtIteration;
	FLOAT m_fSwitchToRepulsionWhenDiversityIs;
	int m_iSwitchToRepulsionWhenNoProgress;
	FLOAT m_fSwitchToAttractionWhenDiversityIs;
	int m_iSwitchToAttractionReplaceBest;
	FLOAT m_fIndividualBestUpdateDist;
	bool m_bEnforceMinDistOnCopy;
	FLOAT m_fStartVisibilityDistance;
	bool m_bStartingVisibilityAuto;
	FLOAT m_fVisibilityDistanceIncrease;
	bool m_bUseLocalOptimization;
	
	// PSO variables in the resume file
	FLOAT m_fVisibilityDistance;
	int m_iNumIterationsBestEnergyHasntChanged;
	FLOAT m_fAttractionRepulsion; // Either 1 or -1
	bool m_bRunComplete;
	
	// General variables
	string m_sInputFileName;
	EnergyProgram *m_pSelectedEnergyProgram;
	string m_sPathToEnergyFiles;
	string m_sPathToScratch;
	int m_iAlgorithmToDo;
	bool m_bResumeFileRead;
	bool m_bOptimizationFileRead;
	string m_sOutputFileName;
	string m_sResumeFileName;
	int m_iResumeFileNumIterations;
	int m_iCharge;
	int m_iMultiplicity;
	int m_iMaxIterations;
	int m_iLinearSructures;
	FLOAT m_fLinearBoxHeight;
	int m_iPlanarStructures;
	int m_i3DStructures;
	int m_i3DStructuresWithMaxDist;
	int m_i3DNonFragStructuresWithMaxDist;
	int m_iTotalPopulationSize;
	FLOAT m_fMinDistnaceBetweenSameMoleculeSets; // When comparing 2 moleculeSets, if the total distance between
	                                             // all atoms is greater than this value, the sets are "different".
	int m_iNumberOfBestStructuresToSave;
	int m_iNumberOfLogFilesToSave;
	string m_sSaveLogFilesInDirectory;
	FLOAT m_fMaxAtomDistance;
	FLOAT m_fGeneralMinAtomDistance;
	Point3D m_boxDimensions; // this defines the box within which all atoms must remain
	MoleculeSet m_tempelateMoleculeSet;
	int m_iPrintSummaryInfoEveryNIterations;
	string m_sNodesFile;
	vector<char*> m_srgNodeNames;
	string m_sEnergyFileHeader;
	string m_sEnergyFileFooter;
	
	int m_iIteration;
	int m_iFreezeUntilIteration;
	time_t m_tElapsedSeconds;
	int m_iNumEnergyEvaluations;
	
	// Structure variables
	int m_iNumStructureTypes;
	int* m_iNumStructuresOfEachType;
	string* m_sStructureFormats;
	vector<Point3D> *m_cartesianPoints; // an array of vectors
	vector<int> *m_atomicNumbers; // an array of vectors

	// Used in optimization files
	int m_iStructuresToOptimizeAtATime;

	bool m_bTestMode;
	FILE* m_testFile;
	
	static string s_program_directory; // the directory where the pso application is

public:
	Input ();
	~Input ();
	bool readFile(ifstream &infile, bool setMinDistances, bool bReadNodesFile);
	bool hasBeenInitialized() { return (m_sInputFileName != ""); } // returns true if a file has been successfully read
	bool save(const char *outputFileName);
	bool save();
	void printToFile(ofstream &outFile);
	void printInputParamsToFile(ofstream &outFile);
	void writeResumeFile(string &fileName, vector<MoleculeSet*> &moleculeSets,
	                     vector<MoleculeSet*> &bestNMoleculeSets, vector<MoleculeSet*> &bestIndividualMoleculeSets,
	                     time_t elapsedSeconds, bool resumeOrOptimizationFile);
	bool open(string &fileName, bool setMinDistances, bool bReadNodesFile, vector<MoleculeSet*> &moleculeSets,
	          vector<MoleculeSet*> &bestNMoleculeSets, vector<MoleculeSet*> &bestIndividualMoleculeSets);
	bool seedCompatible(Input &otherInput);
	static void checkDirectoryOrFileName(string &directoryName);
	static void trim(string& str);
	bool printBondInfo();
	bool printTestFileHeader(int iterationNumber, MoleculeSet &startingGeometry);
	bool printTestFileGeometry(int iterationNumber, MoleculeSet &geometry);
	bool printTestFileFooter();

	static string fileWithoutPath(const string &s);
	bool setupForIndependentRun(vector<string> &inputFiles, vector<MoleculeSet*> &seededMoleculeSets, bool &bSetupPreviouslyDone);
	static void sortMoleculeSets(vector<MoleculeSet*> &moleculeSets, int lo, int hi);
	static void saveBestN(vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestN, int n,
                     FLOAT fMinDistnaceBetweenSameMoleculeSets, int iNumEnergyFilesToSave, const char* sLogFilesDirectory);

	bool compileIndependentRunData(bool printOutput);
	
private:
	bool getStringParam(const char *fileLine, const string& parameterNameString, string &stringParam);
	bool getIntParam(const char *fileLine, const string& parameterNameString, int &myInt);
	bool getFloatParam(const char *fileLine, const string& parameterNameString, FLOAT &myFloat);
	bool readCartesianLine(const char *fileLine, const int maxLineLength, Point3D &cartesianPoint, int &atomicNumber);
	bool getYesNoParam(const char *fileLine, const string& parameterNameString, bool &yesNoParam);
	const char *printYesNoParam(bool yesNoParam);
	bool containsFileExtension(const char *fileName, const char *extension);
	bool readNodesFile();
	bool copyFileLines(const char* fromFileName, FILE* toFile);
};

#endif

