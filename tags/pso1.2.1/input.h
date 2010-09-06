////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing a molecule or a set of adams that
//    belong in a group.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __INPUT_H__
#define __INPUT_H__

#include "typedef.h"
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
using std::ofstream;
using std::ifstream;
using std::ostream;
using namespace std;
using std::setw;
using std::time;

#include "molecule.h"
#include "moleculeSet.h"

#define SIMULATED_ANNEALING 1
#define PARTICLE_SWARM_OPTIMIZATION 2
#define GENETIC_ALGORITHM 3
#define VERSION "1.2.1"

#define GAUSSIAN        1
#define LENNARD_JONES   2

#define ACCEPTANCE_RATIO_NUM_ITERATIONS 100
#define NUM_ITERATIONS_TO_SET_SCALING_FACTOR 200
#define SET_SCALING_FACTOR_EVERY 20

#define HARTREE_TO_JOULES       2622950

// This class reads user input from a file in a specific format
// and can also copy input parameters to a new file.
class Input
{
public:
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
	
	// General variables
	string m_sInputFileName;
	string m_sPathToEnergyProgram;
	string m_sPathToEnergyFiles;
	string m_sPathToScratch;
	int m_iAlgorithmToDo;
	bool m_bResumeFileRead;
	bool m_bOptimizationFileRead;
	int m_iEnergyFunction;
	string m_sOutputFileName;
	string m_sResumeFileName;
	int m_iResumeFileNumIterations;
	string m_sAtomicMassFileName;
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

private:
	// This is a list of string parameters that have the text descriptons of the above variables
	
	// Strings specific to simulated annealing
	string m_sSimulatedAnnealingParametersDisplayed;
	string m_sPerformNonFragSearchDisplayed;
	string m_sPerformBasinHoppingDisplayed;
	string m_sTransitionStateSearchDisplayed;
	string m_sStartingTemperatureDisplayed;
	string m_sBoltzmanConstantDisplayed;
	string m_sNumIterationsBeforeDecreasingTempDisplayed;
	string m_sAcceptanceRatioDisplayed;
	string m_sQuenchingFactorDisplayed;
	string m_sMinTemperatureToStopDisplayed;
	string m_sMinAcceptedTransitionsDisplayed;
	string m_sNumPerterbationsDisplayed;
	string m_sStartCoordinatePerturbationDisplayed;
	string m_sMinCoordinatePerturbationDisplayed;
	string m_sStartAnglePerturbationDisplayed;
	string m_sMinAnglePerturbationDisplayed;
	
	// Strings specific to PSO
	string m_sParticleSwarmParametersDisplayed;
	string m_sStartCoordInertiaDisplayed;
	string m_sEndCoordInertiaDisplayed;
	string m_sCoordIndividualMinimumAttractionDisplayed;
	string m_sCoordPopulationMinimumAttractionDisplayed;
	string m_sCoordMaximumVelocityDisplayed;
	string m_sStartAngleInertiaDisplayed;
	string m_sEndAngleInertiaDisplayed;
	string m_sAngleIndividualMinimumAttractionDisplayed;
	string m_sAnglePopulationMinimumAttractionDisplayed;
	string m_sAngleMaximumVelocityDisplayed;
	string m_sReachEndInertiaAtIterationDisplayed;
	string m_sSwitchToRepulsionWhenDiversityIsDisplayed;
	string m_sSwitchToRepulsionWhenNoProgressDisplayed;
	string m_sSwitchToAttractionWhenDiversityIsDisplayed;
	string m_sSwitchToAttractionReplaceBestDisplayed;
	string m_sIndividualBestUpdateDistDisplayed;
	string m_sEnforceMinDistOnCopyDisplayed;
	string m_sStartVisibilityDistanceDisplayed;
	string m_sVisibilityDistanceIncreaseDisplayed;
	string m_sUseLocalOptimizationDisplayed;
	
	// Strings specific to the genetic algorithm
	string m_sGeneticAlgorithmParametersDisplayed;
	
	// Strings used in all algorithms
	string m_sSimInputVersionLineDisplayed;
	string m_sSimResumeVersionLineDisplayed;
	string m_sSimOptimizationVersionLineDisplayed;
	string m_sPSOInputVersionLineDisplayed;
	string m_sPSOResumeVersionLineDisplayed;
	string m_sPSOOptimizationVersionLineDisplayed;
	string m_sGAInputVersionLineDisplayed;
	string m_sGAResumeVersionLineDisplayed;
	string m_sGAOptimizationVersionLineDisplayed;
	string m_sEnergyFunctionDisplayed;
	string m_sPathToEnergyProgramDisplayed;
	string m_sPathToEnergyFilesDisplayed;
	string m_sPathToScratchDisplayed;
	string m_sOutputFileNameDisplayed;
	string m_sResumeFileNameDisplayed;
	string m_sResumeFileNumIterationsDisplayed;
	string m_sChargeDisplayed;
	string m_sMultiplicityDisplayed;
	string m_sLinearSructuresDisplayed;
	string m_sPlanarStructuresDisplayed;
	string m_s3DStructuresDisplayed;
	string m_s3DStructuresWithMaxDistDisplayed;
	string m_s3DNonFragStructuresWithMaxDistDisplayed;
	string m_sMinDistnaceBetweenSameMoleculeSetsDisplayed;
	string m_sNumberOfBestStructuresToSaveDisplayed;
	string m_sNumberOfLogFilesToSaveDisplayed;
	string m_sMaxAtomDistanceDisplayed;
	string m_sMinGeneralAtomDistanceDisplayed;
	string m_sMinAtomDistanceDisplayed;
	string m_sBoxLengthDisplayed;
	string m_sLinearBoxHeightDisplayed;
	string m_sMaxIterationsDisplayed;
	string m_sNumStructureTypesDisplayed;
	string m_sNumStructuresOfEachTypeDisplayed;
	string m_sStructureFormatOfThisTypeDisplayed;
	string m_sPrintSummaryInfoEveryNIterationsDisplayed;
	string m_sJobQueueTemplateDisplayed;
	string m_sNumJobQueueJobsDisplayed;
	string m_sEnergyFileHeaderDisplayed;
	string m_sEnergyFileFooterDisplayed;
	string m_sUsePrevWaveFunctionDisplayed;

	string m_sIterationDisplayed;
	string m_sFreezeUntilIterationDisplayed;
	string m_sDecreasingTempDisplayed;
	string m_sAverageTransitionsDisplayDisplayed;
	string m_sAcceptedTransitionsDisplayed;
	string m_sAcceptedTransitionsIndexDisplayed;

	string m_sStructuresToOptimizeAtATimeDisplayed;

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
	void static checkDirectoryOrFileName(string &directoryName);
	void static trim(string& str);
	void printBondInfo();
	void printTestFileHeader(int iterationNumber, MoleculeSet &startingGeometry);
	void printTestFileGeometry(int iterationNumber, MoleculeSet &geometry);
	void printTestFileFooter();
	
private:
	bool getStringParam(const char *fileLine, string parameterNameString, string &stringParam);
	bool getIntParam(const char *fileLine, string parameterNameString, int &myInt);
	bool getFloatParam(const char *fileLine, string parameterNameString, FLOAT &myFloat);
	bool readCartesianLine(const char *fileLine, const int maxLineLength, Point3D &cartesianPoint, int &atomicNumber);
	bool getYesNoParam(const char *fileLine, string parameterNameString, bool &yesNoParam);
	const char *printYesNoParam(bool yesNoParam);
	bool containsFileExtension(const char *fileName, const char *extension);
	bool containsOnlyNumbers(const char *string);
	static char *trueFalseToYesNoStr(int aBool);
	bool readNodesFile();
	void copyFileLines(const char* fromFileName, FILE* toFile);
};

#endif

