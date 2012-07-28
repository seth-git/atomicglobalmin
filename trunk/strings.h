////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing messages in different languages.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <string.h>
#include <iostream>
#include <map>
#include <utility>

#include <stdio.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using std::ifstream;
using namespace std;

class Strings
{
public:
	static bool init();
	static Strings* instance();
	static Strings* instance(string languageCode);
	static const string s_sDefaultLanguageCode;
	
	Strings () { m_bLoaded = false; }
	bool init(const char* languageCode);

	void printOptions();
	void printHelperOptions();

	string m_sLanguageCode;

	string m_sSimulatedAnnealingParameters;
	string m_sPerformNonFragSearch;
	string m_sPerformBasinHopping;
	string m_sTransitionStateSearch;
	string m_sStartingTemperature;
	string m_sBoltzmanConstant;
	string m_sNumIterationsBeforeDecreasingTemp;
	string m_sAcceptanceRatio;
	string m_sNumPerterbations;
	string m_sStartCoordinatePerturbation;
	string m_sMinCoordinatePerturbation;
	string m_sStartAnglePerturbation;
	string m_sMinAnglePerturbation;
	string m_sMinTemperatureToStop;
	string m_sMinAcceptedTransitions;
	string m_sQuenchingFactor;
	string m_sDecreasingTemp;
	string m_sAverageTransitionsDisplay;
	string m_sAcceptedTransitions;
	string m_sAcceptedTransitionsIndex;
	string m_sParticleSwarmParameters;
	string m_sStartCoordInertia;
	string m_sEndCoordInertia;
	string m_sReachEndInertiaAtIteration;
	string m_sCoordIndividualMinimumAttraction;
	string m_sCoordPopulationMinimumAttraction;
	string m_sCoordMaximumVelocity;
	string m_sStartAngleInertia;
	string m_sEndAngleInertia;
	string m_sAngleIndividualMinimumAttraction;
	string m_sAnglePopulationMinimumAttraction;
	string m_sAngleMaximumVelocity;
	string m_sSwitchToRepulsionWhenDiversityIs;
	string m_sSwitchToRepulsionWhenNoProgress;
	string m_sSwitchToAttractionWhenDiversityIs;
	string m_sIndividualBestUpdateDist;
	string m_sEnforceMinDistOnCopy;
	string m_sStartVisibilityDistance;
	string m_sAuto;
	string m_sVisibilityDistanceIncrease;
	string m_sUseLocalOptimization;
	string m_sSimInputVersionLine;
	string m_sSimResumeVersionLine;
	string m_sSimOptimizationVersionLine;
	string m_sPSOInputVersionLine;
	string m_sPSOResumeVersionLine;
	string m_sPSOOptimizationVersionLine;
	string m_sGAInputVersionLine;
	string m_sGAResumeVersionLine;
	string m_sGAOptimizationVersionLine;
	string m_sEnergyFunction;
	string m_sPathToEnergyFiles;
	string m_sPathToScratch;
	string m_sOutputFileName;
	string m_sResumeFileName;
	string m_sResumeFileNumIterations;
	string m_sCharge;
	string m_sMultiplicity;
	string m_sLinearSructures;
	string m_sPlanarStructures;
	string m_s3DStructures;
	string m_s3DStructuresWithMaxDist;
	string m_s3DNonFragStructuresWithMaxDist;
	string m_sMinDistnaceBetweenSameMoleculeSets;
	string m_sNumberOfBestStructuresToSave;
	string m_sNumberOfLogFilesToSave;
	string m_sMaxAtomDistance;
	string m_sMinGeneralAtomDistance;
	string m_sMinAtomDistance;
	string m_sBoxLength;
	string m_sLinearBoxHeight;
	string m_sMaxIterations;
	string m_sNumStructureTypes;
	string m_sNumStructuresOfEachType;
	string m_sStructureFormatOfThisType;
	string m_sPrintSummaryInfoEveryNIterations;
	string m_sEnergyFileHeader;
	string m_sEnergyFileFooter;
	string m_sGeneticAlgorithmParameters;
	string m_sIteration;
	string m_sFreezeUntilIteration;
	string m_sStructuresToOptimizeAtATime;

	string m_sReadingFile;
	string m_sInitializingPopulation;
	string m_sRunningSimulatingAnnealing;
	string m_sRunningParticleSwarmOptimization;
	string m_sRunningGeneticAlgorithm;
	string m_sSettingTheTemperature;
	string m_sAbbreviationIteration;
	string m_sAbbreviationNoTransitionStates;
	string m_sAbbreviationBestEnergy;
	string m_sAbbreviationTemperature;
	string m_sAbbreviationNumberOfPerterbations;
	string m_sAbbreviationCoordinateAnglePerterbations;
	string m_sAbbreviationAcceptedPerterbationPercentage;
	string m_sEndTemperatureAndIteration1;
	string m_sEndTemperatureAndIteration2;
	string m_sOptimizingBestStructure;
	string m_sBestStructure;
	string m_sEnergy;
	string m_sEnergyCalculations;
	string m_sFinishTime1;
	string m_sFinishTime2;
	string m_sFinishTime3;
	string m_sFinishTime4;
	string m_sFinishTime5;
	
	string m_sOptionH;
	string m_sOptionH2;
	string m_sOptionI;
	string m_sOptionM;
	string m_sOptionS;
	string m_sOptionSMessage;
	string m_sOptionTest;
	string m_sOptionWalltime;
	string m_sOptionWalltimeMessage;

	string m_sOptionHelpH;
	string m_sOptionHelpH2;
	string m_sOptionHelpA;
	string m_sOptionHelpAMessage;
	string m_sOptionHelpB;
	string m_sOptionHelpC;
	string m_sOptionHelpCMessage;
	string m_sOptionHelpM;
	string m_sOptionHelpMMessage;
	string m_sOptionHelpO;
	string m_sOptionHelpOMessage;
	string m_sOptionHelpOR;
	string m_sOptionHelpORMessage;
	string m_sOptionHelpRO;
	string m_sOptionHelpROMessage;
	string m_sOptionHelpP;
	string m_sOptionHelpPMessage;
	string m_sOptionHelpT;
	string m_sOptionHelpTMessage;
	string m_sOptionHelpU;
	string m_sOptionHelpUMessage;

	string m_sUsage;
	string m_sDesc;
	string m_sHOptionI;
	string m_sHOptionM;
	string m_sHOptionS;
	string m_sHOptionTest;
	string m_sHOptionWalltime;

	string m_sHelpUsage;
	string m_sHelpDesc;
	string m_sHOptionHelpA;
	string m_sHOptionHelpB;
	string m_sHOptionHelpC;
	string m_sHOptionHelpM;
	string m_sHOptionHelpO;
	string m_sHOptionHelpOR;
	string m_sHOptionHelpP;
	string m_sHOptionHelpT;
	string m_sHOptionHelpU;

	string m_sProgramOutput;
	string m_sSeedingPopulationFromFile;
	string m_sFreezingSeededMoleculesFor;
	string m_sIterations;
	string m_sUnFreezingSeededMolecules;
	string m_sPerformingBasinHopping;
	string m_sSearchingForTransitionStates;
	string m_sPerformingSimulatingAnnealing;
	string m_sResumingProgram;
	string m_sSettingTemperature;
	
	string m_sSwitchingToRepulsion;
	string m_sResettingVisibility;
	string m_sSwitchingToAttraction;
	string m_sAbbreviationCoordinateVelocity;
	string m_sAbbreviationAngleVelocity;
	string m_sAbbreviationDiversity;
	string m_sAbbreviationVisibility;
	string m_sAbbreviationBestStructures;
	string m_sSettingRMSVisibilityDist;
	string m_sPerformingCalculations;
	string m_sStructuresRemaining;
	
	string m_sCantWriteToFile;
	string m_sCantAppendToFile;
	string m_sCantOpenOutputFile;
	string m_sCantInitializePopulation;
	string m_sNotAllCalcFinished;
	string m_sTimeToFinishAndCleanUp;
	string m_sNonResumableFile;
	string m_sUseInpFileInstead;
	string m_sErrorUpdatingResume;
	string m_sErrorUpdatingOptimization;
	string m_sErrorPrintingTestFileGeometry;
	string m_sCaughtMessage;
	string m_sFreezingNotImplementedWGA;
	string m_sNoStructuresRemaining;
	string m_sHelpMenu;
	
	string m_sReadingSeedFile;
	string m_sYes;
	string m_sNo;
	string m_sBestNDirectoryNotEmpty1;
	string m_sBestNDirectoryNotEmpty2;
	string m_sDeletingDirectory;
	string m_sCouldntDeleteFilesInDir;
	string m_sDirectoryNotDeleted;
	string m_sExiting;
	string m_sCreatingDirectory;
	string m_sCouldntCreateDirectory;
	string m_sRunComplete;
	string m_sLastArgMustBeInput;
	string m_sCantUseIMOptionsTogether;
	string m_sIOptOnlyForSimulatedAnnealing;
	string m_sIOptOnlyWInput;
	string m_sIOptPopSizeEqualNoMPIProcs1;
	string m_sIOptPopSizeEqualNoMPIProcs2;
	string m_sIOptPopSizeEqualNoMPIProcs;
	string m_sFreezingIterationsCantBeNegative;
	string m_sCantReadFile;
	string m_sSeedFileNotInputFile;
	string m_sSeedFileIncompatible1;
	string m_sSeedFileIncompatible2;
	string m_sSeedFileIncompatible;
	string m_sSeedFileContainsFragmented;
	string m_sStructureIsFragmented;
	string m_sFailedToInit3dNonFragStructure1;
	string m_sFailedToInit3dNonFragStructure2;
	string m_sFailedToInit3dNonFragStructure;
	string m_sFreezingRequiresOneLessMolecule;
	string m_sNoTransSearchWLennardJones;
	string m_sNoTransFoundContinueAnyway;
	string m_sErrorOpeningInputWIOpt;
	string m_sFailedToInitEnergyCalc;
	string m_sNoTestWithOptFiles;
	string m_sUnrecognizedArgumentOrOption;
	string m_sCouldntCreateScratchDirectory;
	string m_sTimeBeforeWallTime1;
	string m_sTimeBeforeWallTime2;
	string m_sUnidentifiedElementSymbol;
	
	string m_sMultipleOptionsError;
	string m_sNotResumeOrOptFile;
	string m_sBest;
	string m_sStructuresFromIteration;
	string m_sPopulationOf;
	string m_sStructureNumber;
	string m_sOutputWrittenTo;
	string m_sEnterNumStructuresToOptimizeAt1Time;
	string m_sCreatingOptimizationFile;
	string m_sDirectoryDoesntExistCreateIt;
	string m_sCreatedDirectory;
	string m_sErrorCreatingDirectory;
	string m_sDirectoryNotCreated;
	string m_sAnswerYesOrNo;
	string m_sNumStructuresInFile;
	string m_sEnterNumInputFiles;
	string m_sAll;
	string m_sFilesNotCreated;
	string m_sCreateingNInputFiles;
	string m_sDone;
	string m_sEnterNumStructuresToTransfer;
	string m_sChangingNumSavedOptimizedStructures;
	string m_sCreatedOptFile;
	string m_sPleaseCleanDirectory;
	string m_sPleaseNoResOrOptFile;
	string m_sFileIsNotOptFile;
	string m_sAddFromThisResFile;
	string m_sEnterResumeFile;
	string m_sNotAResumeFile;
	string m_sNoStructuresLeftToTransfer;
	string m_sTransferedNStructuresToOptFile;
	string m_sUnrecognizedArgOrOption;
	
	string m_sBestSavedStructures;
	string m_sNodesFile;
	string m_sPopulationMustBeNonFragmented;
	string m_sPleaseSpecifyAllOrNoneOfThese;
	string m_sParameter;
	string m_sRepulsionAttractionError1;
	string m_sRepulsionAttractionError2;
	string m_sGaussianHeaderLineLongerThan80;
	string m_sGaussianFooterLineLongerThan80;
	string m_sSpecifyOptForGaussianOptimization;
	string m_sSpecifyOptWithBasinHopping;
	string m_sUseLink1InGaussianFooter;
	string m_sUnableToReadFile;
	string m_sPleaseSpecifyOneNode;
	string m_sReadingParamsFormInput;
	string m_sUnableToWriteTemporaryResume;

private:
	static map<string,Strings> s_instances;

	bool m_bLoaded;
};

#endif

