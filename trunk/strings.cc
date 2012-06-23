////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing messages in different languages.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "strings.h"

const string Strings::s_sDefaultLanguageCode = "en";
map<string,Strings> Strings::s_instances;

bool Strings::init()
{
	return s_instances[s_sDefaultLanguageCode].init(s_sDefaultLanguageCode.c_str());
}

Strings* Strings::instance()
{
	return &s_instances[s_sDefaultLanguageCode];
}

Strings* Strings::instance(string languageCode)
{
	if (!s_instances[languageCode].m_bLoaded)
		if (!s_instances[languageCode].init(languageCode.c_str()))
			return NULL;
	return &s_instances[languageCode];
}

bool Strings::init (const char* languageCode)
{
	char fileName[100];
	sprintf(fileName, "translation/input_%s.properties", languageCode);
	ifstream infile(fileName);
	const int MAX_LINE_LENGTH = 2000;
	char fileLine[MAX_LINE_LENGTH];
	char name[MAX_LINE_LENGTH];
	char str[MAX_LINE_LENGTH];
	map<string,string> stringMap;
	bool duplicates = false;

	m_bLoaded = false;

	if (!infile)
	{
		cout << "Can't open the file: " << fileName << endl;
		return false;
	}

	while (infile.getline(fileLine, MAX_LINE_LENGTH)) {
		if (sscanf(fileLine, "input.%s = %[^\t\n]", name, str) == 2) {
			if (stringMap[name].length() > 0) {
				cout << "Error: Found two strings with the same key: '" << name << "'." << endl;
				duplicates = true;
			}
			stringMap[name] = str;
		}
	}
	infile.close();

	map<string,string> valueMap;
	map<string,string>::iterator iter;   
	for (iter = stringMap.begin(); iter != stringMap.end(); iter++ ) {
		if (valueMap[iter->second].length() > 0 && iter->first.find("Option") == string::npos) {
			cout << "Error: Found two strings with the same value: '" << iter->second << "'." << endl;
			duplicates = true;
		}
		valueMap[iter->second] = iter->first;
	}
	if (duplicates)
		return false;

	m_sSimulatedAnnealingParameters = stringMap["SimulatedAnnealingParameters"];
	m_sPerformNonFragSearch = stringMap["PerformNonFragSearch"];
	m_sPerformBasinHopping = stringMap["PerformBasinHopping"];
	m_sTransitionStateSearch = stringMap["TransitionStateSearch"];
	m_sStartingTemperature = stringMap["StartingTemperature"];
	m_sBoltzmanConstant = stringMap["BoltzmanConstant"];
	m_sNumIterationsBeforeDecreasingTemp = stringMap["NumIterationsBeforeDecreasingTemp"];
	m_sAcceptanceRatio = stringMap["AcceptanceRatio"];
	m_sNumPerterbations = stringMap["NumPerterbations"];
	m_sStartCoordinatePerturbation = stringMap["StartCoordinatePerturbation"];
	m_sMinCoordinatePerturbation = stringMap["MinCoordinatePerturbation"];
	m_sStartAnglePerturbation = stringMap["StartAnglePerturbation"];
	m_sMinAnglePerturbation = stringMap["MinAnglePerturbation"];
	m_sMinTemperatureToStop = stringMap["MinTemperatureToStop"];
	m_sMinAcceptedTransitions = stringMap["MinAcceptedTransitions"];
	m_sQuenchingFactor = stringMap["QuenchingFactor"];
	m_sDecreasingTemp = stringMap["DecreasingTemp"];
	m_sAverageTransitionsDisplay = stringMap["AverageTransitionsDisplay"];
	m_sAcceptedTransitions = stringMap["AcceptedTransitions"];
	m_sAcceptedTransitionsIndex = stringMap["AcceptedTransitionsIndex"];
	m_sParticleSwarmParameters = stringMap["ParticleSwarmParameters"];
	m_sStartCoordInertia = stringMap["StartCoordInertia"];
	m_sEndCoordInertia = stringMap["EndCoordInertia"];
	m_sReachEndInertiaAtIteration = stringMap["ReachEndInertiaAtIteration"];
	m_sCoordIndividualMinimumAttraction = stringMap["CoordIndividualMinimumAttraction"];
	m_sCoordPopulationMinimumAttraction = stringMap["CoordPopulationMinimumAttraction"];
	m_sCoordMaximumVelocity = stringMap["CoordMaximumVelocity"];
	m_sStartAngleInertia = stringMap["StartAngleInertia"];
	m_sEndAngleInertia = stringMap["EndAngleInertia"];
	m_sAngleIndividualMinimumAttraction = stringMap["AngleIndividualMinimumAttraction"];
	m_sAnglePopulationMinimumAttraction = stringMap["AnglePopulationMinimumAttraction"];
	m_sAngleMaximumVelocity = stringMap["AngleMaximumVelocity"];
	m_sSwitchToRepulsionWhenDiversityIs = stringMap["SwitchToRepulsionWhenDiversityIs"];
	m_sSwitchToRepulsionWhenNoProgress = stringMap["SwitchToRepulsionWhenNoProgress"];
	m_sSwitchToAttractionWhenDiversityIs = stringMap["SwitchToAttractionWhenDiversityIs"];
	m_sIndividualBestUpdateDist = stringMap["IndividualBestUpdateDist"];
	m_sEnforceMinDistOnCopy = stringMap["EnforceMinDistOnCopy"];
	m_sStartVisibilityDistance = stringMap["StartVisibilityDistance"];
	m_sVisibilityDistanceIncrease = stringMap["VisibilityDistanceIncrease"];
	m_sUseLocalOptimization = stringMap["UseLocalOptimization"];
	m_sSimInputVersionLine = stringMap["SimInputVersionLine"];
	m_sSimResumeVersionLine = stringMap["SimResumeVersionLine"];
	m_sSimOptimizationVersionLine = stringMap["SimOptimizationVersionLine"];
	m_sPSOInputVersionLine = stringMap["PSOInputVersionLine"];
	m_sPSOResumeVersionLine = stringMap["PSOResumeVersionLine"];
	m_sPSOOptimizationVersionLine = stringMap["PSOOptimizationVersionLine"];
	m_sGAInputVersionLine = stringMap["GAInputVersionLine"];
	m_sGAResumeVersionLine = stringMap["GAResumeVersionLine"];
	m_sGAOptimizationVersionLine = stringMap["GAOptimizationVersionLine"];
	m_sEnergyFunction = stringMap["EnergyFunction"];
	m_sPathToEnergyFiles = stringMap["PathToEnergyFiles"];
	m_sPathToScratch = stringMap["PathToScratch"];
	m_sOutputFileName = stringMap["OutputFileName"];
	m_sResumeFileName = stringMap["ResumeFileName"];
	m_sResumeFileNumIterations = stringMap["ResumeFileNumIterations"];
	m_sCharge = stringMap["Charge"];
	m_sMultiplicity = stringMap["Multiplicity"];
	m_sLinearSructures = stringMap["LinearSructures"];
	m_sPlanarStructures = stringMap["PlanarStructures"];
	m_s3DStructures = stringMap["3DStructures"];
	m_s3DStructuresWithMaxDist = stringMap["3DStructuresWithMaxDist"];
	m_s3DNonFragStructuresWithMaxDist = stringMap["3DNonFragStructuresWithMaxDist"];
	m_sMinDistnaceBetweenSameMoleculeSets = stringMap["MinDistnaceBetweenSameMoleculeSets"];
	m_sNumberOfBestStructuresToSave = stringMap["NumberOfBestStructuresToSave"];
	m_sNumberOfLogFilesToSave = stringMap["NumberOfLogFilesToSave"];
	m_sMaxAtomDistance = stringMap["MaxAtomDistance"];
	m_sMinGeneralAtomDistance = stringMap["MinGeneralAtomDistance"];
	m_sMinAtomDistance = stringMap["MinAtomDistance"];
	m_sBoxLength = stringMap["BoxLength"];
	m_sLinearBoxHeight = stringMap["LinearBoxHeight"];
	m_sMaxIterations = stringMap["MaxIterations"];
	m_sNumStructureTypes = stringMap["NumStructureTypes"];
	m_sNumStructuresOfEachType = stringMap["NumStructuresOfEachType"];
	m_sStructureFormatOfThisType = stringMap["StructureFormatOfThisType"];
	m_sPrintSummaryInfoEveryNIterations = stringMap["PrintSummaryInfoEveryNIterations"];
	m_sEnergyFileHeader = stringMap["EnergyFileHeader"];
	m_sEnergyFileFooter = stringMap["EnergyFileFooter"];
	m_sGeneticAlgorithmParameters = stringMap["GeneticAlgorithmParameters"];
	m_sIteration = stringMap["Iteration"];
	m_sFreezeUntilIteration = stringMap["FreezeUntilIteration"];
	m_sStructuresToOptimizeAtATime = stringMap["StructuresToOptimizeAtATime"];

	m_sReadingFile = stringMap["ReadingFile"];
	m_sInitializingPopulation = stringMap["InitializingPopulation"];
	m_sRunningSimulatingAnnealing = stringMap["RunningSimulatingAnnealing"];
	m_sRunningParticleSwarmOptimization = stringMap["RunningParticleSwarmOptimization"];
	m_sRunningGeneticAlgorithm = stringMap["RunningGeneticAlgorithm"];
	m_sSettingTheTemperature = stringMap["SettingTheTemperature"];
	m_sAbbreviationIteration = stringMap["AbbreviationIteration"];
	m_sAbbreviationNoTransitionStates = stringMap["AbbreviationNoTransitionStates"];
	m_sAbbreviationBestEnergy = stringMap["AbbreviationBestEnergy"];
	m_sAbbreviationTemperature = stringMap["AbbreviationTemperature"];
	m_sAbbreviationNumberOfPerterbations = stringMap["AbbreviationNumberOfPerterbations"];
	m_sAbbreviationCoordinateAnglePerterbations = stringMap["AbbreviationCoordinateAnglePerterbations"];
	m_sAbbreviationAcceptedPerterbationPercentage = stringMap["AbbreviationAcceptedPerterbationPercentage"];
	m_sEndTemperatureAndIteration1 = stringMap["EndTemperatureAndIteration1"];
	m_sEndTemperatureAndIteration2 = stringMap["EndTemperatureAndIteration2"];
	m_sOptimizingBestStructure = stringMap["OptimizingBestStructure"];
	m_sBestStructure = stringMap["BestStructure"];
	m_sEnergy = stringMap["Energy"];
	m_sEnergyCalculations = stringMap["EnergyCalculations"];
	m_sFinishTime1 = stringMap["FinishTime1"];
	m_sFinishTime2 = stringMap["FinishTime2"];
	m_sFinishTime3 = stringMap["FinishTime3"];
	m_sFinishTime4 = stringMap["FinishTime4"];
	m_sFinishTime5 = stringMap["FinishTime5"];
	
	m_sOptionH = stringMap["OptionH"];
	m_sOptionH2 = stringMap["OptionH2"];
	m_sOptionI = stringMap["OptionI"];
	m_sOptionM = stringMap["OptionM"];
	m_sOptionS = stringMap["OptionS"];
	m_sOptionSMessage = stringMap["OptionSMessage"];
	m_sOptionTest = stringMap["OptionTest"];
	m_sOptionWalltime = stringMap["OptionWalltime"];
	m_sOptionWalltimeMessage = stringMap["OptionWalltimeMessage"];

	m_sOptionHelpH = stringMap["OptionHelpH"];
	m_sOptionHelpH2 = stringMap["OptionHelpH2"];
	m_sOptionHelpA = stringMap["OptionHelpA"];
	m_sOptionHelpAMessage = stringMap["OptionHelpAMessage"];
	m_sOptionHelpB = stringMap["OptionHelpB"];
	m_sOptionHelpC = stringMap["OptionHelpC"];
	m_sOptionHelpCMessage = stringMap["OptionHelpCMessage"];
	m_sOptionHelpM = stringMap["OptionHelpM"];
	m_sOptionHelpMMessage = stringMap["OptionHelpMMessage"];
	m_sOptionHelpO = stringMap["OptionHelpO"];
	m_sOptionHelpOMessage = stringMap["OptionHelpOMessage"];
	m_sOptionHelpOR = stringMap["OptionHelpOR"];
	m_sOptionHelpORMessage = stringMap["OptionHelpORMessage"];
	m_sOptionHelpRO = stringMap["OptionHelpRO"];
	m_sOptionHelpROMessage = stringMap["OptionHelpROMessage"];
	m_sOptionHelpP = stringMap["OptionHelpP"];
	m_sOptionHelpPMessage = stringMap["OptionHelpPMessage"];
	m_sOptionHelpT = stringMap["OptionHelpT"];
	m_sOptionHelpTMessage = stringMap["OptionHelpTMessage"];
	m_sOptionHelpU = stringMap["OptionHelpU"];
	m_sOptionHelpUMessage = stringMap["OptionHelpUMessage"];

	m_sUsage = stringMap["Usage"];
	m_sDesc = stringMap["Desc"];
	m_sHOptionI = stringMap["HOptionI"];
	m_sHOptionM = stringMap["HOptionM"];
	m_sHOptionS = stringMap["HOptionS"];
	m_sHOptionTest = stringMap["HOptionTest"];
	m_sHOptionWalltime = stringMap["HOptionWalltime"];

	m_sHelpUsage = stringMap["HelpUsage"];
	m_sHelpDesc = stringMap["HelpDesc"];
	m_sHOptionHelpA = stringMap["HOptionHelpA"];
	m_sHOptionHelpB = stringMap["HOptionHelpB"];
	m_sHOptionHelpC = stringMap["HOptionHelpC"];
	m_sHOptionHelpM = stringMap["HOptionHelpM"];
	m_sHOptionHelpO = stringMap["HOptionHelpO"];
	m_sHOptionHelpOR = stringMap["HOptionHelpOR"];
	m_sHOptionHelpP = stringMap["HOptionHelpP"];
	m_sHOptionHelpT = stringMap["HOptionHelpT"];
	m_sHOptionHelpU = stringMap["HOptionHelpU"];

	m_sProgramOutput = stringMap["ProgramOutput"];
	m_sSeedingPopulationFromFile = stringMap["SeedingPopulationFromFile"];
	m_sFreezingSeededMoleculesFor = stringMap["FreezingSeededMoleculesFor"];
	m_sIterations = stringMap["Iterations"];
	m_sUnFreezingSeededMolecules = stringMap["UnFreezingSeededMolecules"];
	m_sPerformingBasinHopping = stringMap["PerformingBasinHopping"];
	m_sSearchingForTransitionStates = stringMap["SearchingForTransitionStates"];
	m_sPerformingSimulatingAnnealing = stringMap["PerformingSimulatingAnnealing"];
	m_sResumingProgram = stringMap["ResumingProgram"];
	m_sSettingTemperature = stringMap["SettingTemperature"];

	m_sSwitchingToRepulsion = stringMap["SwitchingToRepulsion"];
	m_sResettingVisibility = stringMap["ResettingVisibility"];
	m_sSwitchingToAttraction = stringMap["SwitchingToAttraction"];
	m_sAbbreviationCoordinateVelocity = stringMap["AbbreviationCoordinateVelocity"];
	m_sAbbreviationAngleVelocity = stringMap["AbbreviationAngleVelocity"];
	m_sAbbreviationDiversity = stringMap["AbbreviationDiversity"];
	m_sAbbreviationVisibility = stringMap["AbbreviationVisibility"];
	m_sAbbreviationBestStructures = stringMap["AbbreviationBestStructures"];
	m_sSettingRMSVisibilityDist = stringMap["SettingRMSVisibilityDist"];
	m_sPerformingCalculations = stringMap["PerformingCalculations"];
	m_sStructuresRemaining = stringMap["StructuresRemaining"];
	
	m_sCantWriteToFile = stringMap["CantWriteToFile"];
	m_sCantAppendToFile = stringMap["CantAppendToFile"];
	m_sCantOpenOutputFile = stringMap["CantOpenOutputFile"];
	m_sCantInitializePopulation = stringMap["CantInitializePopulation"];
	m_sNotAllCalcFinished = stringMap["NotAllCalcFinished"];
	m_sTimeToFinishAndCleanUp = stringMap["TimeToFinishAndCleanUp"];
	m_sNonResumableFile = stringMap["NonResumableFile"];
	m_sUseInpFileInstead = stringMap["UseInpFileInstead"];
	m_sErrorUpdatingResume = stringMap["ErrorUpdatingResume"];
	m_sErrorUpdatingOptimization = stringMap["ErrorUpdatingOptimization"];
	m_sErrorPrintingTestFileGeometry = stringMap["ErrorPrintingTestFileGeometry"];
	m_sCaughtMessage = stringMap["CaughtMessage"];
	m_sFreezingNotImplementedWGA = stringMap["FreezingNotImplementedWGA"];
	m_sNoStructuresRemaining = stringMap["NoStructuresRemaining"];
	m_sHelpMenu = stringMap["HelpMenu"];
	
	m_sReadingSeedFile = stringMap["ReadingSeedFile"];
	m_sYes = stringMap["Yes"];
	m_sNo = stringMap["No"];
	m_sBestNDirectoryNotEmpty1 = stringMap["BestNDirectoryNotEmpty1"];
	m_sBestNDirectoryNotEmpty2 = stringMap["BestNDirectoryNotEmpty2"];
	m_sDeletingDirectory = stringMap["DeletingDirectory"];
	m_sCouldntDeleteFilesInDir = stringMap["CouldntDeleteFilesInDir"];
	m_sDirectoryNotDeleted = stringMap["DirectoryNotDeleted"];
	m_sExiting = stringMap["Exiting"];
	m_sCreatingDirectory = stringMap["CreatingDirectory"];
	m_sCouldntCreateDirectory = stringMap["CouldntCreateDirectory"];
	m_sRunComplete = stringMap["RunComplete"];
	m_sLastArgMustBeInput = stringMap["LastArgMustBeInput"];
	m_sCantUseIMOptionsTogether = stringMap["CantUseIMOptionsTogether"];
	m_sIOptOnlyForSimulatedAnnealing = stringMap["IOptOnlyForSimulatedAnnealing"];
	m_sIOptOnlyWInput = stringMap["IOptOnlyWInput"];
	m_sIOptPopSizeEqualNoMPIProcs1 = stringMap["IOptPopSizeEqualNoMPIProcs1"];
	m_sIOptPopSizeEqualNoMPIProcs2 = stringMap["IOptPopSizeEqualNoMPIProcs2"];
	m_sIOptPopSizeEqualNoMPIProcs = stringMap["IOptPopSizeEqualNoMPIProcs"];
	m_sFreezingIterationsCantBeNegative = stringMap["FreezingIterationsCantBeNegative"];
	m_sCantReadFile = stringMap["CantReadFile"];
	m_sSeedFileNotInputFile = stringMap["SeedFileNotInputFile"];
	m_sSeedFileIncompatible1 = stringMap["SeedFileIncompatible1"];
	m_sSeedFileIncompatible2 = stringMap["SeedFileIncompatible2"];
	m_sSeedFileIncompatible = stringMap["SeedFileIncompatible"];
	m_sSeedFileContainsFragmented = stringMap["SeedFileContainsFragmented"];
	m_sStructureIsFragmented1 = stringMap["StructureIsFragmented1"];
	m_sStructureIsFragmented2 = stringMap["StructureIsFragmented2"];
	m_sStructureIsFragmented3 = stringMap["StructureIsFragmented3"];
	m_sFailedToInit3dNonFragStructure1 = stringMap["FailedToInit3dNonFragStructure1"];
	m_sFailedToInit3dNonFragStructure2 = stringMap["FailedToInit3dNonFragStructure2"];
	m_sFailedToInit3dNonFragStructure = stringMap["FailedToInit3dNonFragStructure"];
	m_sFreezingRequiresOneLessMolecule = stringMap["FreezingRequiresOneLessMolecule"];
	m_sNoTransSearchWLennardJones = stringMap["NoTransSearchWLennardJones"];
	m_sNoTransFoundContinueAnyway = stringMap["NoTransFoundContinueAnyway"];
	m_sErrorOpeningInputWIOpt = stringMap["ErrorOpeningInputWIOpt"];
	m_sFailedToInitEnergyCalc = stringMap["FailedToInitEnergyCalc"];
	m_sNoTestWithOptFiles = stringMap["NoTestWithOptFiles"];
	m_sUnrecognizedArgumentOrOption = stringMap["UnrecognizedArgumentOrOption"];
	m_sCouldntCreateScratchDirectory = stringMap["CouldntCreateScratchDirectory"];
	m_sTimeBeforeWallTime1 = stringMap["TimeBeforeWallTime1"];
	m_sTimeBeforeWallTime2 = stringMap["TimeBeforeWallTime2"];
	m_sUnidentifiedElementSymbol = stringMap["UnidentifiedElementSymbol"];
	
	m_bLoaded = true;
	return true;
}


void printPadding(unsigned int characters) {
	for (unsigned int i = 0; i < characters; ++i) {
		printf(" ");
	}
}

void printOption(const char* option, unsigned int optionLength, const char* description) {
	const unsigned int lineLength = 80;	
	const unsigned int len = strlen(description);
	char descCopy[len+1];
	strncpy(descCopy,description, len+1);
	
	unsigned int length, count;
	char* word;
	
	count = 0;
	printf(" -%s", option);
	count += 2 + strlen(option);
	if (count+1 > optionLength) {
		optionLength = count + 1;
	}
	printPadding(optionLength - count);
	count = optionLength;
	
	word = strtok (descCopy," ");
	while (word != NULL)
	{
		length = strlen(word);
		if (length + count > lineLength) {
			printf("\n");
			printPadding(optionLength);
			count = optionLength;
		}
		if (count != optionLength) {
			printf(" ");
			++count;
		}
		printf ("%s",word);
		count += length;
		word = strtok (NULL, " ");
	}
	printf("\n");
}

void Strings::printOptions()
{
	const unsigned int optionLength = 12;
	cout << m_sUsage << endl;
	cout << m_sDesc << endl << endl;
	printOption("i", optionLength, m_sHOptionI.c_str());
	printOption("m", optionLength, m_sHOptionM.c_str());
	printOption("s", optionLength, m_sHOptionS.c_str());
	printOption("test", optionLength, m_sHOptionTest.c_str());
	printOption("walltime", optionLength, m_sHOptionWalltime.c_str());
	cout << endl;
}

void Strings::printHelperOptions()
{
	const unsigned int optionLength = 10;
	cout << m_sHelpUsage << endl;
	cout << m_sHelpDesc << endl << endl;
	printOption("a", optionLength, m_sHOptionHelpA.c_str());
	printOption("b", optionLength, m_sHOptionHelpB.c_str());
	printOption("c", optionLength, m_sHOptionHelpC.c_str());
	printOption("m", optionLength, m_sHOptionHelpM.c_str());
	printOption("o", optionLength, m_sHOptionHelpO.c_str());
	printOption("or", optionLength, m_sHOptionHelpOR.c_str());
	printOption("p", optionLength, m_sHOptionHelpP.c_str());
	printOption("t", optionLength, m_sHOptionHelpT.c_str());
	printOption("u", optionLength, m_sHOptionHelpU.c_str());
	cout << endl;
}

