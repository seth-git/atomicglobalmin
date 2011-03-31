////////////////////////////////////////////////////////////////////////////////
// Purpose: This file reads parameters from the input file.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "input.h"

/////////////////////////////////////////////////////////////////////
// Purpose: This constructor initializes important variables.
// Parameters: none
// Returns: nothing
Input::Input (void)
{
	m_sEnergyFunction = "Lennard Jones";
	m_sInputFileName = "";
	m_bResumeFileRead = false;
	
	m_iCharge = 0;
	m_iMultiplicity = 1;
	m_fStartingTemperature = 1000;
	m_fBoltzmanConstant = 3.0e-7; // this is the default constant when converting from hartree to jewels
	m_iLinearSructures = 10;
	m_iPlanarStructures = 10;
	m_i3DStructures = 10;
	m_i3DStructuresWithMaxDist = 10;
	m_i3DNonFragStructuresWithMaxDist = 10;
	m_iTotalPopulationSize = m_iLinearSructures + m_iPlanarStructures + m_i3DStructures +
	                         m_i3DStructuresWithMaxDist + m_i3DNonFragStructuresWithMaxDist;
	m_fMinDistnaceBetweenSameMoleculeSets = 5;
	m_iNumberOfBestStructuresToSave = 5;
	
	m_fStartCoordinatePerturbation = 0.01;
	m_fMinCoordinatePerturbation = 0.001;
	m_fStartAnglePerturbation = 3;
	m_fMinAnglePerturbation = 0.3;
	m_fGeneralMinAtomDistance = 0.7;
	m_fMinAcceptedTransitions = 0.05; // stop if the number of accepted transitions / the number of transitions
	                                  // is below this percentage for 3 or > generations/temperatures
	m_iNumIterationsBeforeDecreasingTemp = 50;
	m_prgAcceptedTransitions = new int[m_iNumIterationsBeforeDecreasingTemp];
	m_iMaxIterations = 50000;
	m_fAcceptanceRatio = 0.3;
	m_fQuenchingFactor = 0.999; // between 0.950 and 0.999
	
	m_boxDimensions.x = 5;
	m_boxDimensions.y = 5;
	m_boxDimensions.z = 5;
	m_fLinearBoxHeight = 1.5;
	m_fMinTemperatureToStop = 400;
	m_sOutputFileName = "output.txt";
	m_sResumeFileName = "resume.txt";
	m_iResumeFileNumIterations = 1;
	m_sAtomicMassFileName = "periodic_table.csv";
	m_iPrintSummaryInfoEveryNIterations = 20;
	
	m_sNodesFile = "";
	m_srgNodeNames.clear();
	m_sEnergyFileHeader = "";
	m_bUsePrevWaveFunction = false;
	
	m_iNumStructureTypes = 0;
	m_iNumStructuresOfEachType = NULL;
	m_sStructureFormats = NULL;
	m_cartesianPoints = NULL;
	m_atomicNumbers = NULL;

	// Variables used in Simulated Annealing Simulation
	m_sSimulatedAnnealingParametersDisplayed = "Simulated Annealing Parameters";
	m_bPerformNonFragSearch = false;
	m_bPerformBasinHopping = false;
	m_iIteration = 0;
	m_bDecreasingTemp = false;
	m_iConsecutiveGenerationsOfMeetingAcceptanceRatio = 0;
	m_iGenerationsSinceMinAcceptedTransitionsReached = 0;
	m_tTimeStampStart = 0;
	m_tTimeStampEnd = 0;
	m_iNumEnergyEvaluations = 0;
	
	// Varables used in PSO
	m_sParticleSwarmParametersDisplayed = "Particle Swarm Optimization Parameters";
	m_fStartCoordInertia = 0.99;
	m_fEndCoordInertia = 0.4;
	m_iReachEndInertiaAtIteration = 150;
	m_fCoordIndividualMinimumAttraction = 0.005;
	m_fCoordPopulationMinimumAttraction = 0.005;
	m_fCoordMaximumVelocity = 0;
	m_fStartAngleInertia = 0.99;
	m_fEndAngleInertia = 0.4;
	m_fAngleIndividualMinimumAttraction = 0.005;
	m_fAnglePopulationMinimumAttraction = 0.005;
	m_fAngleMaximumVelocity = 0;
	m_bEnforceMinDistOnCopy = false;
	m_fLocalMinDist = 0; // depends on the number of atoms and the size of the search box
	m_fLocalMinDistIncrease = 0;
	m_bInitInPairs = false;
	m_bUseLocalOptimization = false;
	m_fSwitchToRepulsionWhenDiversityIs = 0;
	m_iSwitchToRepulsionWhenNoProgress = 0;
	m_fSwitchToAttractionWhenDiversityIs = 0;
	m_iSwitchToAttractionReplaceBest = 0;
	m_fIndividualBestUpdateDist = 0.1;
	
	// Variables used in PSO resume file
	m_fAttractionRepulsion = 1;
	m_fStartVisibilityDistance = 0;
	m_iNumIterationsBestEnergyHasntChanged = 0;

	// Varables used in Genetic Algorithm
	m_sGeneticAlgorithmParametersDisplayed = "Genetic Algorithm Parameters";

	
	// Simulated Annealing Strings
	m_sPerformNonFragSearchDisplayed = "Search only for non-fragmented structures";
	m_sPerformBasinHoppingDisplayed = "Perform basin hopping search";
	m_sStartingTemperatureDisplayed = "Starting temperature";
	m_sBoltzmanConstantDisplayed = "Scaling factor (substituent for Boltzmann constant)";
	m_sNumIterationsBeforeDecreasingTempDisplayed = "Minimum number of iterations before decreasing the temperature (N)";
	m_sAcceptanceRatioDisplayed = "Decrease the temperature when the percentage of accepted transitions for the past N iterations is below";
	m_sStartCoordinatePerturbationDisplayed = "Starting coordinate perturbation (angstroms)";
	m_sMinCoordinatePerturbationDisplayed = "Minimum coordinate perturbation (angstroms)";
	m_sStartAnglePerturbationDisplayed = "Starting angle perturbation (deg)";
	m_sMinAnglePerturbationDisplayed = "Minimum angle perturbation (deg)";
	m_sMinTemperatureToStopDisplayed = "Don't stop while the temperature is above this value";
	m_sMinAcceptedTransitionsDisplayed = "Stop if the accepted transitions is below this percentage for each of five consecutive iterations";
	m_sQuenchingFactorDisplayed = "Quenching factor";
	m_sDecreasingTempDisplayed = "Decreasing temperature";
	m_sAverageTransitionsDisplayDisplayed = "Transition count";
	m_sConsecutiveGenerationsOfMeetingAcceptanceRatioDisplayed = "Consecutive iterations below acceptance ratio";
	m_sGenerationsSinceMinAcceptedTransitionsReachedDisplayed = "Consecutive iterations below minimum accepted transitions";
	m_sAcceptedTransitionsDisplayed = "Number of accepted transitions in the past several iterations";
	m_sAcceptedTransitionsIndexDisplayed = "Index to the above parameter";
	
	// PSO strings
	m_sStartCoordInertiaDisplayed = "Start coordinate inertia (w)";
	m_sEndCoordInertiaDisplayed = "End coordinate inertia (optional)";
	m_sReachEndInertiaAtIterationDisplayed = "Reach end coordinate and angle inertias at iteration (optional)";
	m_sCoordIndividualMinimumAttractionDisplayed = "Coordinate individual minimum attraction (c2)";
	m_sCoordPopulationMinimumAttractionDisplayed = "Coordinate population minimum attraction (c1)";
	m_sCoordMaximumVelocityDisplayed = "Maximum Coordinate Velocity (Vmax, optional)";
	m_sStartAngleInertiaDisplayed = "Start angle inertia (w)";
	m_sEndAngleInertiaDisplayed = "End angle inertia (optional)";
	m_sAngleIndividualMinimumAttractionDisplayed = "Angle individual minimum attraction (c2, deg)";
	m_sAnglePopulationMinimumAttractionDisplayed = "Angle population minimum attraction (c1, deg)";
	m_sAngleMaximumVelocityDisplayed = "Maximum Angle Velocity (Vmax, optional)";
	m_sSwitchToRepulsionWhenDiversityIsDisplayed = "Switch to the repulsion phase when (1) diversity is below (optional)";
	m_sSwitchToRepulsionWhenNoProgressDisplayed = "And when (2) progress hasn't been made for this number of iterations (optional)";
	m_sSwitchToAttractionWhenDiversityIsDisplayed = "Switch to the attraction phase when diversity is above (optional)";
	m_sSwitchToAttractionReplaceBestDisplayed = "When switching to the attraction phase, replace this number of individual best solutions with population best solutions";
	m_sIndividualBestUpdateDistDisplayed = "Don't update individual best solutions within this RMS distance of the best seen by the population";
	m_sEnforceMinDistOnCopyDisplayed = "Enforce minimum distance constraints on a copy of each structure rather than on the original";
	m_sLocalMinDistDisplayed = "Starting visibility RMS distance (optional)";
	m_sLocalMinDistIncreaseDisplayed = "Increase the visibility RMS distance by this amount each iteration (optional)";
	m_sInitInPairsDisplayed = "Initialize structures in pairs where each is a near copy of the other (gives non-zero initial visibility)";
	m_sUseLocalOptimizationDisplayed = "Use local optimization (energy value only)";
	
	m_sSimInputVersionLineDisplayed = "Monte Carlo Simulated Annealing Input File Version";
	m_sSimResumeVersionLineDisplayed = "Monte Carlo Simulated Annealing Resume File Version";
	m_sPSOInputVersionLineDisplayed = "Particle Swarm Optimization Input File Version ";
	m_sPSOResumeVersionLineDisplayed = "Particle Swarm Optimization Resume File Version";
	m_sGAInputVersionLineDisplayed = "Genetic Algorithm Input File Version ";
	m_sGAResumeVersionLineDisplayed = "Genetic Algorithm Resume File Version";
	m_sEnergyFunctionDisplayed = "Energy function to use (Gaussian or Lennard Jones)";
	m_sPathToEnergyProgramDisplayed = "Path to energy program";
	m_sPathToEnergyFilesDisplayed = "Path to energy files";
	m_sOutputFileNameDisplayed = "Output file name";
	m_sResumeFileNameDisplayed = "Resume file name (optional)";
	m_sResumeFileNumIterationsDisplayed = "Write resume file after every set of this number of iterations (optional)";
	m_sChargeDisplayed = "Charge";
	m_sMultiplicityDisplayed = "Multiplicity";
	m_sLinearSructuresDisplayed = "Number of linear structures";
	m_sPlanarStructuresDisplayed = "Number of planar structures";
	m_s3DStructuresDisplayed = "Number of fragmented 3D structures";
	m_s3DStructuresWithMaxDistDisplayed = "Number of partially non-fragmented 3D structures";
	m_s3DNonFragStructuresWithMaxDistDisplayed = "Number of completely non-fragmented 3D structures";
	
	m_sMinDistnaceBetweenSameMoleculeSetsDisplayed = "Consider 2 structures \"different\" if their RMS distance is greater or equal to (angstroms)";
	m_sNumberOfBestStructuresToSaveDisplayed = "Save this many of the best \"different\" structures";
	
	m_sMaxAtomDistanceDisplayed = "Maximum inter-atomic distance (angstroms)";
	m_sMinGeneralAtomDistanceDisplayed = "General minimum atom distance (used if no value specified below)";
	m_sMinAtomDistanceDisplayed = "Specific minimum atom distances (angstroms)";
	m_sBoxLengthDisplayed = "Search cube length, width, and height";
	m_sLinearBoxHeightDisplayed = "Initialize linear structures in a box with the above cube length and a height and width of";
	m_sMaxIterationsDisplayed = "Maximum number of allowed iterations";
	m_sNumStructureTypesDisplayed = "Number of unit types";
	m_sNumStructuresOfEachTypeDisplayed = "Number of units of this type";
	m_sStructureFormatOfThisTypeDisplayed = "Format of this unit type";
	m_sPrintSummaryInfoEveryNIterationsDisplayed = "Print summary information after each set of this many iterations";
	m_sNodesFileDisplayed = "Cluster node names file (put LOCAL for local nodes)";
	m_sJobQueueTemplateDisplayed = "Job queue template file (used instead of the node names file, optional)";
	m_sNumJobQueueJobsDisplayed = "Number of jobs submitted to the job queue at a time";
	m_sEnergyFileHeaderDisplayed = "Energy file header";
	m_sUsePrevWaveFunctionDisplayed = "Every other iteration, use the wave function from the previous iteration";
	
	m_sIterationDisplayed = "Iteration number";
}

/////////////////////////////////////////////////////////////////////
// Purpose: This distructor cleans up.
// Parameters: none
// Returns: nothing
Input::~Input (void)
{
	int i;
	unsigned int j;
	delete[] m_iNumStructuresOfEachType;
	delete[] m_sStructureFormats;
	for (i = 0; i < m_iNumStructureTypes; ++i)
	{
		m_cartesianPoints[i].clear();
		m_atomicNumbers[i].clear();
	}
	delete[] m_cartesianPoints;
	delete[] m_atomicNumbers;
	for (j = 0; j < m_srgNodeNames.size(); ++j)
		delete[] m_srgNodeNames[j];
	m_srgNodeNames.clear();
	delete[] m_prgAcceptedTransitions;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This is a helper function that reads a string parameter.
// Parameters: fileLine - the line of text read from the input file.
//             parameterNameString - the descriptive text that should be
//                        in the input file on this line.
//             stringPtr - where the string read from the input file
//                         will be stored.  This is a pointer to a pointer
//                         because this function allocates memory for
//                         the string, and we need to change the string
//                         pointer in the calling function.
// Returns: True if parameterName appeared in the file with a colon (:)
//          after it and the string was read correctly.
//          Note: no error will be reported if the string is empty
bool Input::getStringParam(const char *fileLine, string parameterNameString, string &stringParam)
{
	int counter;
	const char* parameterName = parameterNameString.c_str();

	// Make sure we're reading the right parameter
	while (tolower(*fileLine) == tolower(*parameterName))
	{
		++fileLine;
		++parameterName;
	}

	// Check for the ':' character
	if (*fileLine != ':')
		return false;
	++fileLine;

	// Skip spaces
	counter = 0;
	while ((*fileLine == ' ') && (counter < 10))
	{
		++fileLine;
		++counter;
	}
	
	stringParam = (string)fileLine;
	
	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This is a helper function that reads an integer parameter.
// Parameters: fileLine - the line of text read from the input file.
//             parameterNameString - the descriptive text that should be
//                        in the input file on this line.
//             myInt - where the integer parameter will be stored.
// Returns: True if parameterName appeared in the file with a colon (:)
//          after it and the integer was read correctly.
//          Note: no error will be reported if there is no integer present.
//          In this case, the integer is assigned to be zero.
bool Input::getIntParam(const char *fileLine, string parameterNameString, int &myInt)
{
	int counter;
	const char* parameterName = parameterNameString.c_str();
	
	// Make sure we're reading the right parameter
	while (tolower(*fileLine) == tolower(*parameterName))
	{
		++fileLine;
		++parameterName;
	}

	// Check for the ':' character
	if (*fileLine != ':')
		return false;
	++fileLine;

	// Skip spaces
	counter = 0;
	while ((*fileLine == ' ') && (counter < 10))
	{
		++fileLine;
		++counter;
	}

	myInt = atoi(fileLine);

	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This is a helper function that reads an FLOAT parameter.
// Parameters: fileLine - the line of text read from the input file.
//             parameterNameString - the descriptive text that should be
//                        in the input file on this line.
//             myFloat - where the FLOAT parameter will be stored.
// Returns: True if parameterName appeared in the file with a colon (:)
//          after it and the float was read correctly.
//          Note: no error will be reported if there is no float present.
//          In this case, the float is assigned to be zero.
bool Input::getFloatParam(const char *fileLine, string parameterNameString, FLOAT &myFloat)
{
	int counter;
	const char* parameterName = parameterNameString.c_str();
	
	// Make sure we're reading the right parameter
	while (tolower(*fileLine) == tolower(*parameterName))
	{
		++fileLine;
		++parameterName;
	}

	// Check for the ':' character
	if (*fileLine != ':')
		return false;
	++fileLine;

	// Skip spaces
	counter = 0;
	while ((*fileLine == ' ') && (counter < 10))
	{
		++fileLine;
		++counter;
	}

	myFloat = atof(fileLine);

	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This is a helper function that reads a line containing
//    an atomic number and three coordinates for an atom.
// Parameters: fileLine - the line of text read from the input file.
//             cartesianPoint - the location where the atom will be stored
//             atomicNumber - where the atomic number will be stored
// Returns: True if the line was read correctly,
//          False otherwise
bool Input::readCartesianLine(const char *fileLine, const int maxLineLength, Point3D &cartesianPoint, int &atomicNumber)
{
	char copyOfFileLine[maxLineLength];
	char* myString;

	strncpy(copyOfFileLine, fileLine, maxLineLength);

	myString = strtok(copyOfFileLine, " ");
	if (myString == NULL)
		return false;
	else
		atomicNumber = atoi(myString);

	myString = strtok(NULL, " ");
	if (myString == NULL)
		return false;
	else
		cartesianPoint.x = atof(myString);

	myString = strtok(NULL, " ");
	if (myString == NULL)
		return false;
	else
		cartesianPoint.y = atof(myString);

	myString = strtok(NULL, " ");
	if (myString == NULL)
		return false;
	else
		cartesianPoint.z = atof(myString);
	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This is a helper function that reads an boolean parameter.
// Parameters: fileLine - the line of text read from the input file.
//             parameterName - the descriptive text that should be
//                        in the input file on this line.
//             yesNoParam - where the boolean parameter will be stored.
// Returns: True if parameterName appeared in the file with a colon (:)
//          after it and the boolean parameter was read correctly.
//          Note: this function expects a yes or no to be read from
//          the input file and returns false if it's not there.
bool Input::getYesNoParam(const char *fileLine, string parameterNameString, bool &yesNoParam)
{
	string tempStr;
	if (!getStringParam(fileLine, parameterNameString, tempStr))
		return false;

	// Do a case insensitive comparison.
	if (strncmp(tempStr.c_str(),"yes",3) == 0)
		yesNoParam = true;
	else if (strncmp(tempStr.c_str(),"no",2) == 0)
		yesNoParam = false;
	else
		return false;
	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This is a helper function that reads an boolean parameter.
// Parameters: outFile - the file
//             yesNoParam - the boolean parameter
// Returns: Nothing
const char *Input::printYesNoParam(bool yesNoParam)
{
	if (yesNoParam)
		return "yes";
	else
		return "no";
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function ensures that directory names have only forward
//     slashes and no back slashes.  It also ensures that there
//     is no trailing forward slash.
// Parameters: dir - the directory (or file name)
// Returns: nothing
void Input::checkDirectoryOrFileName(string &directoryName)
{
	char *dir;
	char *dirIndex;
	int dirLength;
	
	dir = new char[directoryName.length()+1];
	strncpy(dir,directoryName.c_str(),directoryName.length()+1);
	
	dirIndex = dir;
	dirLength = 0;
	
	// Change forward slashes to back slashes
	while (*dirIndex != '\0')
	{
		++dirLength;
		if (*dirIndex == '\\')
			*dirIndex = '/';
		++dirIndex;
	}

	// Check for the trailing back slash
	if (dirLength > 0)
		if (dir[dirLength-1] == '/')
			dir[dirLength-1] = '\0';
	
	directoryName = dir;
	delete[] dir;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function reads input from the input file.
// Parameters: fileName - the input file name
// Returns: true if there were no errors
//     Note: If an error does occur, descriptive messages about
//     the error are printed before returning.
bool Input::readFile(ifstream &infile)
{
	const int MAX_LINE_LENGTH = 5000;
	char fileLine[MAX_LINE_LENGTH];
	int lineNumber;
	vector<Molecule*> moleculeVector;
	Molecule* pMoleculeTemplate;
	Molecule* pMoleculeTemplate2;
	Molecule* moleculeArray;
	Point3D cartesianPoint;
	int atomicNumber;
	int i, j;
	FLOAT tempFloat;
	string temp;
	
	m_sInputFileName = "";
	
	// Read in each line of the file
	lineNumber = 1;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}

	if (strncmp(m_sSimInputVersionLineDisplayed.c_str(), fileLine, m_sSimInputVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = SIMULATED_ANNEALING;
		m_bResumeFileRead = false;
	} else if (strncmp(m_sSimResumeVersionLineDisplayed.c_str(), fileLine, m_sSimResumeVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = SIMULATED_ANNEALING;
		m_bResumeFileRead = true;
	} else if (strncmp(m_sPSOInputVersionLineDisplayed.c_str(), fileLine, m_sPSOInputVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = PARTICLE_SWARM_OPTIMIZATION;
		m_bResumeFileRead = false;
	} else if (strncmp(m_sPSOResumeVersionLineDisplayed.c_str(), fileLine, m_sPSOResumeVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = PARTICLE_SWARM_OPTIMIZATION;
		m_bResumeFileRead = true;
	} else if (strncmp(m_sGAInputVersionLineDisplayed.c_str(), fileLine, m_sGAInputVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = GENETIC_ALGORITHM;
		m_bResumeFileRead = false;
	} else if (strncmp(m_sGAResumeVersionLineDisplayed.c_str(), fileLine, m_sGAResumeVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = GENETIC_ALGORITHM;
		m_bResumeFileRead = true;
	} else {
		cout << "Unrecognized input file header on line " << lineNumber << "." << endl;
		return false;
	}
	
	temp = fileLine;
	if (temp.find(VERSION) == string::npos)
		cout << "Warning: this input file is not version " << VERSION << " which may cause errors." << endl;
	
	// Read the next line which should be blank
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		cout << "Line " << lineNumber << " should be blank in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sEnergyFunctionDisplayed, m_sEnergyFunction))
	{
		cout << "Error reading the parameter '" << m_sEnergyFunctionDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sPathToEnergyProgramDisplayed, m_sPathToEnergyProgram))
	{
		cout << "Error reading the parameter '" << m_sPathToEnergyProgramDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	checkDirectoryOrFileName(m_sPathToEnergyProgram);

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sPathToEnergyFilesDisplayed, m_sPathToEnergyFiles))
	{
		cout << "Error reading the parameter '" << m_sPathToEnergyFilesDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	checkDirectoryOrFileName(m_sPathToEnergyFiles);
   	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getFloatParam(fileLine, m_sBoxLengthDisplayed, m_boxDimensions.x))
	{
		cout << "Error reading the parameter '" << m_sBoxLengthDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	m_boxDimensions.y = m_boxDimensions.x;
	m_boxDimensions.z = m_boxDimensions.x;
	m_boxDimensions.w = 0;
 	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sLinearSructuresDisplayed, m_iLinearSructures))
	{
		cout << "Error reading the parameter '" << m_sLinearSructuresDisplayed << "' on line " << lineNumber << 
			" in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getFloatParam(fileLine, m_sLinearBoxHeightDisplayed, m_fLinearBoxHeight))
	{
		cout << "Error reading the parameter '" << m_sLinearBoxHeightDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sPlanarStructuresDisplayed, m_iPlanarStructures))
	{
		cout << "Error reading the parameter '" << m_sPlanarStructuresDisplayed << "' on line " << lineNumber << 
			" in the input file." << endl;
		return false;
	}

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_s3DStructuresDisplayed, m_i3DStructures))
	{
		cout << "Error reading the parameter '" << m_s3DStructuresDisplayed << "' on line " << lineNumber << 
			" in the input file." << endl;
		return false;
	}
    
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_s3DStructuresWithMaxDistDisplayed, m_i3DStructuresWithMaxDist))
	{
		cout << "Error reading the parameter '" << m_s3DStructuresWithMaxDistDisplayed << "' on line " << lineNumber << 
			" in the input file." << endl;
		return false;
	}
    
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_s3DNonFragStructuresWithMaxDistDisplayed, m_i3DNonFragStructuresWithMaxDist))
	{
		cout << "Error reading the parameter '" << m_s3DNonFragStructuresWithMaxDistDisplayed << "' on line " << lineNumber << 
			" in the input file." << endl;
		return false;
	}
	
	m_iTotalPopulationSize = m_iLinearSructures + m_iPlanarStructures + m_i3DStructures
	                       + m_i3DStructuresWithMaxDist + m_i3DNonFragStructuresWithMaxDist;
	if (m_iTotalPopulationSize <= 0)
	{
		cout << "Please specify a population size of one or greater." << endl;
		return false;
	}

	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getFloatParam(fileLine, m_sMinGeneralAtomDistanceDisplayed, m_fGeneralMinAtomDistance))
	{
		cout << "Error reading the parameter '" << m_sMinGeneralAtomDistanceDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (strncmp(fileLine,m_sMinAtomDistanceDisplayed.c_str(),m_sMinAtomDistanceDisplayed.length()) != 0)
	{
		cout << "Error reading the parameter '" << m_sMinAtomDistanceDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	Atom::initMinAtomicDistances(m_fGeneralMinAtomDistance);
	while (true)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (strncmp(fileLine,m_sMaxAtomDistanceDisplayed.c_str(),m_sMaxAtomDistanceDisplayed.length()) == 0)
			break;
		sscanf(fileLine, "%d %d %lf", &i, &j, &tempFloat);
		Atom::setMinAtomicDistance(i,j,tempFloat);
	}
	
	if (!getFloatParam(fileLine, m_sMaxAtomDistanceDisplayed, m_fMaxAtomDistance))
	{
		cout << "Error reading the parameter '" << m_sMaxAtomDistanceDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}

	// Read the next line which should be blank
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		cout << "Line " << lineNumber << " should be blank in the input file." << endl;
		return false;
	}
	
	if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_sSimulatedAnnealingParametersDisplayed, temp))
		{
			cout << "Error reading '" << m_sParticleSwarmParametersDisplayed << ":' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}

		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_sPerformNonFragSearchDisplayed, m_bPerformNonFragSearch))
		{
			cout << "Error reading the parameter '" << m_sPerformNonFragSearchDisplayed << "' on line " << lineNumber << 
				" in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_sPerformBasinHoppingDisplayed, m_bPerformBasinHopping))
		{
			cout << "Error reading the parameter '" << m_sPerformBasinHoppingDisplayed << "' on line " << lineNumber << 
				" in the input file." << endl;
			return false;
		}
		
		if (m_bPerformNonFragSearch)
			if ((m_iLinearSructures > 0) || (m_iPlanarStructures > 0) ||
			    (m_i3DStructures > 0) || (m_i3DStructuresWithMaxDist > 0) || (m_i3DNonFragStructuresWithMaxDist == 0)) {
				cout << "When searching for non-fragmented structures, the entire population must be initialized as non-fragmented." << endl;
				exit(0);
			}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sStartingTemperatureDisplayed, m_fStartingTemperature))
		{
			cout << "Error reading the parameter '" << m_sStartingTemperatureDisplayed << "' on line " << lineNumber << 
				" in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sBoltzmanConstantDisplayed, m_fBoltzmanConstant))
		{
			cout << "Error reading the parameter '" << m_sBoltzmanConstantDisplayed << "' on line " << lineNumber << 
				" in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_sNumIterationsBeforeDecreasingTempDisplayed, m_iNumIterationsBeforeDecreasingTemp))
		{
			cout << "Error reading the parameter '" << m_sNumIterationsBeforeDecreasingTempDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		if (m_iNumIterationsBeforeDecreasingTemp < 1)
		{
			cout << "The parameter '" << m_sNumIterationsBeforeDecreasingTempDisplayed << "' on line "
			     << lineNumber << " in the input file cannot be less than one." << endl;
			return false;
		}
		delete[] m_prgAcceptedTransitions;
		m_prgAcceptedTransitions = new int[m_iNumIterationsBeforeDecreasingTemp];
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sAcceptanceRatioDisplayed, m_fAcceptanceRatio))
		{
			cout << "Error reading the parameter '" << m_sAcceptanceRatioDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		m_fAcceptanceRatio /= 100;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sQuenchingFactorDisplayed, m_fQuenchingFactor))
		{
			cout << "Error reading the parameter '" << m_sQuenchingFactorDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sMinTemperatureToStopDisplayed, m_fMinTemperatureToStop))
		{
			cout << "Error reading the parameter '" << m_sMinTemperatureToStopDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sMinAcceptedTransitionsDisplayed, m_fMinAcceptedTransitions))
		{
			cout << "Error reading the parameter '" << m_sMinAcceptedTransitionsDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		m_fMinAcceptedTransitions /= 100;
	
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_sMaxIterationsDisplayed, m_iMaxIterations))
		{
			cout << "Error reading the parameter '" << m_sMaxIterationsDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sStartCoordinatePerturbationDisplayed, m_fStartCoordinatePerturbation))
		{
			cout << "Error reading the parameter '" << m_sStartCoordinatePerturbationDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sMinCoordinatePerturbationDisplayed, m_fMinCoordinatePerturbation))
		{
			cout << "Error reading the parameter '" << m_sMinCoordinatePerturbationDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sStartAnglePerturbationDisplayed, m_fStartAnglePerturbation))
		{
			cout << "Error reading the parameter '" << m_sStartAnglePerturbationDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		m_fStartAnglePerturbation *= DEG_TO_RAD;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sMinAnglePerturbationDisplayed, m_fMinAnglePerturbation))
		{
			cout << "Error reading the parameter '" << m_sMinAnglePerturbationDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		m_fMinAnglePerturbation *= DEG_TO_RAD;
	} else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) {
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_sParticleSwarmParametersDisplayed, temp))
		{
			cout << "Error reading '" << m_sParticleSwarmParametersDisplayed << ":' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}

		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sStartCoordInertiaDisplayed, m_fStartCoordInertia))
		{
			cout << "Error reading the parameter '" << m_sStartCoordInertiaDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sEndCoordInertiaDisplayed, m_fEndCoordInertia))
		{
			cout << "Error reading the parameter '" << m_sEndCoordInertiaDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_sReachEndInertiaAtIterationDisplayed, m_iReachEndInertiaAtIteration))
		{
			cout << "Error reading the parameter '" << m_sReachEndInertiaAtIterationDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sCoordIndividualMinimumAttractionDisplayed, m_fCoordIndividualMinimumAttraction))
		{
			cout << "Error reading the parameter '" << m_sCoordIndividualMinimumAttractionDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sCoordPopulationMinimumAttractionDisplayed, m_fCoordPopulationMinimumAttraction))
		{
			cout << "Error reading the parameter '" << m_sCoordPopulationMinimumAttractionDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sCoordMaximumVelocityDisplayed, m_fCoordMaximumVelocity))
		{
			cout << "Error reading the parameter '" << m_sCoordMaximumVelocityDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sStartAngleInertiaDisplayed, m_fStartAngleInertia))
		{
			cout << "Error reading the parameter '" << m_sStartAngleInertiaDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sEndAngleInertiaDisplayed, m_fEndAngleInertia))
		{
			cout << "Error reading the parameter '" << m_sEndAngleInertiaDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}

		if ((m_fEndCoordInertia != 0) || (m_fEndAngleInertia != 0) || (m_iReachEndInertiaAtIteration != 0))
			if ((m_fEndCoordInertia == 0) || (m_fEndAngleInertia == 0) || (m_iReachEndInertiaAtIteration == 0)) {
				cout << "Please either specify values for all these parameters or none of them:" << endl;
				cout << "\tParameter: " << m_sEndCoordInertiaDisplayed << endl;
				cout << "\tParameter: " << m_sReachEndInertiaAtIterationDisplayed << endl;
				cout << "\tParameter: " << m_sEndAngleInertiaDisplayed << endl;
				return false;
			}
				
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sAngleIndividualMinimumAttractionDisplayed, m_fAngleIndividualMinimumAttraction))
		{
			cout << "Error reading the parameter '" << m_sAngleIndividualMinimumAttractionDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sAnglePopulationMinimumAttractionDisplayed, m_fAnglePopulationMinimumAttraction))
		{
			cout << "Error reading the parameter '" << m_sAnglePopulationMinimumAttractionDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sAngleMaximumVelocityDisplayed, m_fAngleMaximumVelocity))
		{
			cout << "Error reading the parameter '" << m_sAngleMaximumVelocityDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sLocalMinDistDisplayed, m_fLocalMinDist))
		{
			cout << "Error reading the parameter '" << m_sLocalMinDistDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sLocalMinDistIncreaseDisplayed, m_fLocalMinDistIncrease))
		{
			cout << "Error reading the parameter '" << m_sLocalMinDistIncreaseDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_sInitInPairsDisplayed, m_bInitInPairs))
		{
			cout << "Error reading the parameter '" << m_sInitInPairsDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		if (m_bInitInPairs) {
			if ((m_iLinearSructures % 2 != 0) || (m_iPlanarStructures % 2 != 0) || (m_i3DStructures % 2 != 0) ||
			    (m_i3DStructuresWithMaxDist % 2 != 0) || (m_i3DNonFragStructuresWithMaxDist % 2 != 0)) {
				cout << "Error on line " << lineNumber << ".  When intializing structures in pairs," << endl;
				cout << "there must be an even number of each type of structure." << endl;
				return false;
			}
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sSwitchToRepulsionWhenDiversityIsDisplayed, m_fSwitchToRepulsionWhenDiversityIs))
		{
			cout << "Error reading the parameter '" << m_sSwitchToRepulsionWhenDiversityIsDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_sSwitchToRepulsionWhenNoProgressDisplayed, m_iSwitchToRepulsionWhenNoProgress))
		{
			cout << "Error reading the parameter '" << m_sSwitchToRepulsionWhenNoProgressDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sSwitchToAttractionWhenDiversityIsDisplayed, m_fSwitchToAttractionWhenDiversityIs))
		{
			cout << "Error reading the parameter '" << m_sSwitchToAttractionWhenDiversityIsDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_sSwitchToAttractionReplaceBestDisplayed, m_iSwitchToAttractionReplaceBest))
		{
			cout << "Error reading the parameter '" << m_sSwitchToAttractionReplaceBestDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}

		if (!(((m_fSwitchToRepulsionWhenDiversityIs > 0) && (m_iSwitchToRepulsionWhenNoProgress > 0) &&
		       (m_fSwitchToAttractionWhenDiversityIs > 0)) ||
		      ((m_fSwitchToRepulsionWhenDiversityIs <= 0) && (m_iSwitchToRepulsionWhenNoProgress <= 0) &&
		       (m_fSwitchToAttractionWhenDiversityIs <= 0)))) {
			cout << "Please either specify values for all these parameters or none of them:" << endl;
			cout << "\tParameter: " << m_sSwitchToRepulsionWhenDiversityIsDisplayed << endl;
			cout << "\tParameter: " << m_sSwitchToRepulsionWhenNoProgressDisplayed << endl;
			cout << "\tParameter: " << m_sSwitchToAttractionWhenDiversityIsDisplayed << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sIndividualBestUpdateDistDisplayed, m_fIndividualBestUpdateDist))
		{
			cout << "Error reading the parameter '" << m_sIndividualBestUpdateDistDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_sMaxIterationsDisplayed, m_iMaxIterations))
		{
			cout << "Error reading the parameter '" << m_sMaxIterationsDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_sEnforceMinDistOnCopyDisplayed, m_bEnforceMinDistOnCopy))
		{
			cout << "Error reading the parameter '" << m_sEnforceMinDistOnCopyDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_sUseLocalOptimizationDisplayed, m_bUseLocalOptimization))
		{
			cout << "Error reading the parameter '" << m_sUseLocalOptimizationDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
	} else if (m_iAlgorithmToDo == GENETIC_ALGORITHM) {
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_sGeneticAlgorithmParametersDisplayed, temp))
		{
			cout << "Error reading '" << m_sParticleSwarmParametersDisplayed << ":' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
	}
	
	// Read the next line which should be blank
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		cout << "Line " << lineNumber << " should be blank in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sNumberOfBestStructuresToSaveDisplayed, m_iNumberOfBestStructuresToSave))
	{
		cout << "Error reading the parameter '" << m_sNumberOfBestStructuresToSaveDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getFloatParam(fileLine, m_sMinDistnaceBetweenSameMoleculeSetsDisplayed, m_fMinDistnaceBetweenSameMoleculeSets))
	{
		cout << "Error reading the parameter '" << m_sMinDistnaceBetweenSameMoleculeSetsDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	if (m_fSwitchToRepulsionWhenDiversityIs > 0) {
		if (m_iTotalPopulationSize < m_iSwitchToAttractionReplaceBest) {
			cout << "The parameter: '" << m_sSwitchToAttractionReplaceBestDisplayed << "'";
			cout << " can not be bigger than the population size." << endl;
			return false;
		}
		if (m_iSwitchToAttractionReplaceBest > m_iNumberOfBestStructuresToSave) {
			cout << "The parameter: '" << m_sSwitchToAttractionReplaceBestDisplayed << "'" << endl;
			cout << "can not be bigger than the parameter: '" << m_sNumberOfBestStructuresToSaveDisplayed << "'" << endl;
			return false;
		}
		if (m_fSwitchToRepulsionWhenDiversityIs > m_fSwitchToAttractionWhenDiversityIs) {
			cout << "The parameter: '" << m_sSwitchToRepulsionWhenDiversityIsDisplayed << "'" << endl;
			cout << "can not be bigger than the parameter: '" << m_sSwitchToAttractionWhenDiversityIsDisplayed << "'" << endl;
			return false;
		}
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sOutputFileNameDisplayed, m_sOutputFileName))
	{
		cout << "Error reading the parameter '" << m_sOutputFileNameDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sResumeFileNameDisplayed, m_sResumeFileName))
	{
		cout << "Error reading the parameter '" << m_sResumeFileNameDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sResumeFileNumIterationsDisplayed, m_iResumeFileNumIterations))
	{
		cout << "Error reading the parameter '" << m_sResumeFileNumIterationsDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	if (m_iResumeFileNumIterations <= 0)
		m_iResumeFileNumIterations = 1;
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sPrintSummaryInfoEveryNIterationsDisplayed, m_iPrintSummaryInfoEveryNIterations))
	{
		cout << "Error reading the parameter '" << m_sPrintSummaryInfoEveryNIterationsDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	// We used to read the m_sAtomicMassFileName, but now we just use the default name for simplicity
	Atom::initAtomicMasses(m_sAtomicMassFileName); // Read the file containing the atomic masses of atoms
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sNodesFileDisplayed, m_sNodesFile))
	{
		cout << "Error reading the parameter '" << m_sNodesFileDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sJobQueueTemplateDisplayed, m_sJobQueueTemplate))
	{
		cout << "Error reading the parameter '" << m_sJobQueueTemplateDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sNumJobQueueJobsDisplayed, m_iNumJobQueueJobs))
	{
		cout << "Error reading the parameter '" << m_sNumJobQueueJobsDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}

	if (m_sJobQueueTemplate.length() == 0) {
		if (!readNodesFile())
			return false;
	} else {
		if (m_iNumJobQueueJobs <= 0) {
			cout << "Please specify a value for this parameter: '" << m_sNumJobQueueJobsDisplayed << endl;
		}
		if (!readTemplateFile())
			return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getYesNoParam(fileLine, m_sUsePrevWaveFunctionDisplayed, m_bUsePrevWaveFunction))
	{
		cout << "Error reading the parameter '" << m_sUsePrevWaveFunctionDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	if (m_bUsePrevWaveFunction && (m_sEnergyFunction.compare("Lennard Jones") == 0)) {
		cout << "Turning off option to use previous wave function for Lennard Jones potential." << endl;
		m_bUsePrevWaveFunction = false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sChargeDisplayed, m_iCharge))
	{
		cout << "Error reading the parameter '" << m_sChargeDisplayed << "' on line " << lineNumber << 
			" in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sMultiplicityDisplayed, m_iMultiplicity))
	{
		cout << "Error reading the parameter '" << m_sMultiplicityDisplayed << "' on line " << lineNumber << 
			" in the input file." << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sEnergyFileHeaderDisplayed, temp))
	{
		cout << "Error reading the parameter '" << m_sEnergyFileHeaderDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	m_sEnergyFileHeader = "";
	while (true)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (strlen(fileLine) == 0)
			break;
		m_sEnergyFileHeader.append(fileLine);
		m_sEnergyFileHeader.append("\n");
	}
	if (m_bUsePrevWaveFunction) {
		string::size_type position;
		string::size_type position2;
		if (m_sEnergyFileHeader.find("% chk") == string::npos) {
			cout << "Please specify a checkpoint file name in the energy file header." << endl;
			return false;
		}
		if ((position = m_sEnergyFileHeader.find("guess")) != string::npos) {
			if (((position2 = m_sEnergyFileHeader.find("read", position)) != string::npos) &&
			    (position2 - position <= 10)) {
				cout << "Please do not specify the guess as read in the energy file header." << endl;
				return false;
			}
		}
	}
	if (m_bUseLocalOptimization && (m_sEnergyFunction == "Gaussian")) {
		if (m_sEnergyFileHeader.find("opt") == string::npos) {
			cout << "You have local optimization turned on, but you haven't specified 'opt' in the energy file header.  Please do this." << endl;
			return false;
		}
	}
	if (m_bPerformBasinHopping && (m_sEnergyFunction == "Gaussian")) {
		if (m_sEnergyFileHeader.find("opt") == string::npos) {
			cout << "When performing basin hopping with Gaussian, please specify 'opt' in the energy file header." << endl;
			return false;
		}
	}
	
	// Do some clean up
	delete[] m_iNumStructuresOfEachType;
	delete[] m_sStructureFormats;
	for (i = 0; i < m_iNumStructureTypes; ++i)
	{
		m_cartesianPoints[i].clear();
		m_atomicNumbers[i].clear();
	}
	delete[] m_cartesianPoints;
	delete[] m_atomicNumbers;

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sNumStructureTypesDisplayed, m_iNumStructureTypes))
	{
		cout << "Error reading the parameter '" << m_sNumStructureTypesDisplayed << "' on line " << lineNumber << 
			" in the input file." << endl;
		return false;
	}
	
	// Do some initialization
	m_iNumStructuresOfEachType = new int[m_iNumStructureTypes];
	m_sStructureFormats = new string[m_iNumStructureTypes];
	m_cartesianPoints = new vector<Point3D>[m_iNumStructureTypes];
	m_atomicNumbers = new vector<int>[m_iNumStructureTypes];
	
	// Read the next line which should be blank
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		cout << "Line " << lineNumber << " should be blank in the input file." << endl;
		return false;
	}
	
	for (i = 0; i < m_iNumStructureTypes; ++i)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_sNumStructuresOfEachTypeDisplayed, m_iNumStructuresOfEachType[i]))
		{
			cout << "Error reading the parameter '" << m_sNumStructuresOfEachTypeDisplayed << "' on line "
	                     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_sStructureFormatOfThisTypeDisplayed, m_sStructureFormats[i]))
		{
			cout << "Error reading the parameter '" << m_sStructureFormatOfThisTypeDisplayed << "' on line "
	                     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		if (m_sStructureFormats[i] == "Cartesian")
		{
			while (1)
			{
				++lineNumber;
				if (!infile.getline(fileLine, MAX_LINE_LENGTH))
					break;
				if (strlen(fileLine) == 0)
					break;
				else if (!readCartesianLine(fileLine, MAX_LINE_LENGTH, cartesianPoint, atomicNumber))
				{
					cout << "Error reading cartesian coordinates on line " << lineNumber
					     << " in the input file." << endl;
					return false;
				}
				m_cartesianPoints[i].push_back(cartesianPoint);
				m_atomicNumbers[i].push_back(atomicNumber);
			}
			if (m_cartesianPoints[i].size() < 1)
			{
				cout << "No structure specified for structure type " << (i + 1)
				     << " in the input file." << endl;
				return false;
			}
			else
			{
				pMoleculeTemplate = new Molecule();
				pMoleculeTemplate->makeFromCartesian(m_cartesianPoints[i], m_atomicNumbers[i]);
				pMoleculeTemplate->initRotationMatrix();
				pMoleculeTemplate->localToGlobal();
				for (j = 0; j < m_iNumStructuresOfEachType[i]; ++j)
				{
					pMoleculeTemplate2 = new Molecule();
					pMoleculeTemplate2->copy(*pMoleculeTemplate);
					moleculeVector.push_back(pMoleculeTemplate2);
				}
				delete pMoleculeTemplate;
			}
		}
		else
		{
			cout << "Unknown input for parameter '" << m_sStructureFormatOfThisTypeDisplayed
			     << "' on line " << lineNumber << " in the input file." << endl;
			return false;
		}
	}
	// The createChild function in MoleculeSet relies on molecules being inserted in this order.
	m_tempelateMoleculeSet.setNumberOfMolecules((signed int)moleculeVector.size());
	moleculeArray = m_tempelateMoleculeSet.getMolecules();
	for (i = 0; i < (signed int)moleculeVector.size(); ++i)
	{
		moleculeArray[i].copy(*moleculeVector[i]);
		delete moleculeVector[i];
	}
	moleculeVector.clear();
	m_tempelateMoleculeSet.initAtomIndexes();
	return true;
}

bool Input::readNodesFile()
{
	ifstream infile(m_sNodesFile.c_str());
	const int MAX_LINE_LENGTH = 500;
	char fileLine[MAX_LINE_LENGTH];
	char *nodeName;
	int length;
	
	if (!infile)
	{
		cout << "Can't open the file: " << m_sNodesFile << endl;
		return false;
	}
	
	for (unsigned int i = 0; i < m_srgNodeNames.size(); ++i)
		delete[] m_srgNodeNames[i];
	m_srgNodeNames.clear();
	
	while (infile.getline(fileLine, MAX_LINE_LENGTH)) {
		length = strlen(fileLine);
		if (length == 0)
			break;
		nodeName = new char[length+1];
		strncpy(nodeName,fileLine,length+1);
		m_srgNodeNames.push_back(nodeName);
	}
	infile.close();
	return true;
}

bool Input::readTemplateFile()
{
	ifstream infile(m_sJobQueueTemplate.c_str());
	const int MAX_LINE_LENGTH = 500;
	char fileLine[MAX_LINE_LENGTH];
	unsigned int i;
	
	if (!infile)
	{
		cout << "Can't open the file: " << m_sJobQueueTemplate << endl;
		return false;
	}
	
	m_sJobQueueTemplateFileContents = "";
	while (infile.getline(fileLine, MAX_LINE_LENGTH)) {
		m_sJobQueueTemplateFileContents += fileLine;
		m_sJobQueueTemplateFileContents += "\n";
	}
	infile.close();
	
	for (i = 0; i < m_srgNodeNames.size(); ++i)
		delete[] m_srgNodeNames[i];
	m_srgNodeNames.clear();
	
	for (i = 0; i < (unsigned int)m_iNumJobQueueJobs; ++i)
		m_srgNodeNames.push_back("");
	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: See returns below.
// Parameters: string - a character array
// Returns: true if the string contains one or more numbers and if
//          it contains no non-numeric characters.
bool Input::containsOnlyNumbers(const char *string)
{
	const char *stringPtr;

	if (string == NULL)
		return false;

	stringPtr = string;
	while (true)
	{
		switch (*stringPtr)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			++stringPtr;
			continue; // go to the start of the next loop
		case '\0':
			if (stringPtr == string)
				return false;
			else
				return true;
		default:
			return false;
		}
	}

}

/////////////////////////////////////////////////////////////////////
// Purpose: This function determines if the give file name has the
//    given extension.
// Parameters: fileName - the file name
//             extension - 4 character file extension (example: ".bmp")
// Returns: NULL if the input is valid
//          and a pointer to an error message if the input is invalid
bool Input::containsFileExtension(const char *fileName, const char *extension)
{
	int fileNameLength;

	fileNameLength = strlen(fileName);
	if (fileNameLength < 4)
		return false;
	return (strncmp(fileName+fileNameLength-4,extension,3) == 0);
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function converts a boolean value to a string.
// Parameters: aBool - input boolean value
// Returns: a pointer to "yes" if aBool is true
//          a pointer to "no" if aBool is false
char *Input::trueFalseToYesNoStr(int aBool)
{
	static char *yesStr = "yes";
	static char *noStr = "no";

	if (aBool)
		return yesStr;
	else
		return noStr;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function saves the SIP file.
// Parameters: fileName - the file name
// Returns: true if the file was saved successfully and false otherwise
bool Input::save(const char *fileName)
{
/*	if (strcmp(fileName,m_sInputFileName) != 0)
		AssignStringParam(&m_pszInputFileName,fileName);
	return Save();*/
	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function saves the SIP file.
// Parameters: none
// Returns: true if the file was saved successfully and false otherwise
bool Input::save(void)
{
/*	ofstream fout("output.txt", ios::out);

	PrintToFile(fout);

	fout.close();*/
	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function prints the input parameters to a new file.
// Parameters: outFile - pointer to the new file (assumed to be open).
// Returns: nothing
void Input::printToFile(ofstream &outFile)
{
	const int defaultPrecision = 6;
	const int resumeFilePrecision = 15;
	int i, j;
	
	if (m_bResumeFileRead)
	{
		outFile << setiosflags(ios::fixed) << setprecision(resumeFilePrecision);
		if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
			outFile << m_sSimResumeVersionLineDisplayed << " " << VERSION << endl << endl;
		else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			outFile << m_sPSOResumeVersionLineDisplayed << " " << VERSION << endl << endl;
		else if (m_iAlgorithmToDo == GENETIC_ALGORITHM)
			outFile << m_sGAResumeVersionLineDisplayed << " " << VERSION << endl << endl;
	} else {
		outFile << setiosflags(ios::fixed) << setprecision(defaultPrecision);
		if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
			outFile << m_sSimInputVersionLineDisplayed << " " << VERSION << endl << endl;
		else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			outFile << m_sPSOInputVersionLineDisplayed << " " << VERSION << endl << endl;
		else if (m_iAlgorithmToDo == GENETIC_ALGORITHM)
			outFile << m_sGAInputVersionLineDisplayed << " " << VERSION << endl << endl;
	}
	
	outFile << m_sEnergyFunctionDisplayed << ": " << m_sEnergyFunction << endl;
	outFile << m_sPathToEnergyProgramDisplayed << ": " << m_sPathToEnergyProgram << endl;
	outFile << m_sPathToEnergyFilesDisplayed << ": " << m_sPathToEnergyFiles << endl;
	
	outFile << m_sBoxLengthDisplayed << ": " << m_boxDimensions.x << endl;
	outFile << m_sLinearSructuresDisplayed << ": " << m_iLinearSructures << endl;
	outFile << m_sLinearBoxHeightDisplayed << ": " << m_fLinearBoxHeight << endl;
	outFile << m_sPlanarStructuresDisplayed << ": " << m_iPlanarStructures << endl;
	outFile << m_s3DStructuresDisplayed << ": " << m_i3DStructures << endl;
	outFile << m_s3DStructuresWithMaxDistDisplayed << ": " << m_i3DStructuresWithMaxDist << endl;
	outFile << m_s3DNonFragStructuresWithMaxDistDisplayed << ": " << m_i3DNonFragStructuresWithMaxDist << endl;
	outFile << m_sMinGeneralAtomDistanceDisplayed << ": " << m_fGeneralMinAtomDistance << endl;
	outFile << m_sMinAtomDistanceDisplayed << ": " << endl;
	Atom::outputMinDistances(outFile);
	outFile << m_sMaxAtomDistanceDisplayed << ": " << m_fMaxAtomDistance << endl;
	outFile << endl;
	
	if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		outFile << m_sSimulatedAnnealingParametersDisplayed << ":" << endl;
		outFile << m_sPerformNonFragSearchDisplayed << ": " << printYesNoParam(m_bPerformNonFragSearch) << endl;
		outFile << m_sPerformBasinHoppingDisplayed << ": " << printYesNoParam(m_bPerformBasinHopping) << endl;
		if (!m_bResumeFileRead)
			outFile << setprecision(1);
		outFile << m_sStartingTemperatureDisplayed << ": " << m_fStartingTemperature << endl;
		if (!m_bResumeFileRead)
			outFile << setprecision(defaultPrecision);
		outFile << m_sBoltzmanConstantDisplayed << ": " << m_fBoltzmanConstant << endl;
		outFile << m_sNumIterationsBeforeDecreasingTempDisplayed << ": " << m_iNumIterationsBeforeDecreasingTemp << endl;
		if (!m_bResumeFileRead)
			outFile << setprecision(1);
		outFile << m_sAcceptanceRatioDisplayed << ": " << (m_fAcceptanceRatio * 100) << endl;
		if (!m_bResumeFileRead)
			outFile << setprecision(defaultPrecision);
		outFile << m_sQuenchingFactorDisplayed << ": " << m_fQuenchingFactor << endl;
		if (!m_bResumeFileRead)
			outFile << setprecision(1);
		outFile << m_sMinTemperatureToStopDisplayed << ": " << m_fMinTemperatureToStop << endl;
		outFile << m_sMinAcceptedTransitionsDisplayed << ": " << (m_fMinAcceptedTransitions * 100) << endl;
		if (!m_bResumeFileRead)
			outFile << setprecision(defaultPrecision);
		outFile << m_sMaxIterationsDisplayed << ": " << m_iMaxIterations << endl;
		outFile << m_sStartCoordinatePerturbationDisplayed << ": " << m_fStartCoordinatePerturbation << endl;
		outFile << m_sMinCoordinatePerturbationDisplayed << ": " << m_fMinCoordinatePerturbation << endl;
		outFile << m_sStartAnglePerturbationDisplayed << ": " << (m_fStartAnglePerturbation * RAD_TO_DEG) << endl;
		outFile << m_sMinAnglePerturbationDisplayed << ": " << (m_fMinAnglePerturbation * RAD_TO_DEG) << endl;
	} else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) {
		outFile << m_sParticleSwarmParametersDisplayed << ":" << endl;
		if (!m_bResumeFileRead)
			outFile << setprecision(defaultPrecision);
		outFile << m_sStartCoordInertiaDisplayed << ": " << m_fStartCoordInertia << endl;
		outFile << m_sEndCoordInertiaDisplayed << ": " << m_fEndCoordInertia << endl;
		outFile << m_sReachEndInertiaAtIterationDisplayed << ": " << m_iReachEndInertiaAtIteration << endl;
		outFile << m_sCoordIndividualMinimumAttractionDisplayed << ": " << m_fCoordIndividualMinimumAttraction << endl;
		outFile << m_sCoordPopulationMinimumAttractionDisplayed << ": " << m_fCoordPopulationMinimumAttraction << endl;
		if (m_fCoordMaximumVelocity != 0)
			outFile << m_sCoordMaximumVelocityDisplayed << ": " << m_fCoordMaximumVelocity << endl;
		else
			outFile << m_sCoordMaximumVelocityDisplayed << ": " << endl;
		outFile << m_sStartAngleInertiaDisplayed << ": " << m_fStartAngleInertia << endl;
		outFile << m_sEndAngleInertiaDisplayed << ": " << m_fEndAngleInertia << endl;
		outFile << m_sAngleIndividualMinimumAttractionDisplayed << ": " << m_fAngleIndividualMinimumAttraction << endl;
		outFile << m_sAnglePopulationMinimumAttractionDisplayed << ": " << m_fAnglePopulationMinimumAttraction << endl;
		if (m_fAngleMaximumVelocity != 0)
			outFile << m_sAngleMaximumVelocityDisplayed << ": " << m_fAngleMaximumVelocity << endl;
		else
			outFile << m_sAngleMaximumVelocityDisplayed << ": " << endl;
		
		outFile << m_sLocalMinDistDisplayed << ": ";
		if (m_fLocalMinDist != 0)
			outFile << m_fLocalMinDist << endl;
		else
			outFile << endl;
		outFile << m_sLocalMinDistIncreaseDisplayed << ": ";
		if (m_fLocalMinDistIncrease != 0)
			outFile << m_fLocalMinDistIncrease << endl;
		else
			outFile << endl;
		
		outFile << m_sInitInPairsDisplayed << ": " << printYesNoParam(m_bInitInPairs) << endl;
		
		if (m_fSwitchToRepulsionWhenDiversityIs > 0) {
			outFile << m_sSwitchToRepulsionWhenDiversityIsDisplayed << ": " << m_fSwitchToRepulsionWhenDiversityIs << endl;
			outFile << m_sSwitchToRepulsionWhenNoProgressDisplayed << ": " << m_iSwitchToRepulsionWhenNoProgress << endl;
			outFile << m_sSwitchToAttractionWhenDiversityIsDisplayed << ": " << m_fSwitchToAttractionWhenDiversityIs << endl;
			outFile << m_sSwitchToAttractionReplaceBestDisplayed << ": " << m_iSwitchToAttractionReplaceBest << endl;
		} else {
			outFile << m_sSwitchToRepulsionWhenDiversityIsDisplayed << ": " << endl;
			outFile << m_sSwitchToRepulsionWhenNoProgressDisplayed << ": " << endl;
			outFile << m_sSwitchToAttractionWhenDiversityIsDisplayed << ": " << endl;
			outFile << m_sSwitchToAttractionReplaceBestDisplayed << ": " << endl;
		}
		outFile << m_sIndividualBestUpdateDistDisplayed << ": " << m_fIndividualBestUpdateDist << endl;

		outFile << m_sMaxIterationsDisplayed << ": " << m_iMaxIterations << endl;
		outFile << m_sEnforceMinDistOnCopyDisplayed << ": " << printYesNoParam(m_bEnforceMinDistOnCopy) << endl;
		outFile << m_sUseLocalOptimizationDisplayed << ": " << printYesNoParam(m_bUseLocalOptimization) << endl;
	} else if (m_iAlgorithmToDo == GENETIC_ALGORITHM) {
		outFile << m_sGeneticAlgorithmParametersDisplayed << ":" << endl;
		if (!m_bResumeFileRead)
			outFile << setprecision(defaultPrecision);
	}
	
	outFile << endl;
	if (!m_bResumeFileRead)
		outFile << setprecision(1);
	outFile << m_sNumberOfBestStructuresToSaveDisplayed << ": " << m_iNumberOfBestStructuresToSave << endl;
	if (!m_bResumeFileRead)
		outFile << setprecision(defaultPrecision);
	outFile << m_sMinDistnaceBetweenSameMoleculeSetsDisplayed << ": " << m_fMinDistnaceBetweenSameMoleculeSets << endl;
	outFile << m_sOutputFileNameDisplayed << ": " << m_sOutputFileName << endl;
	outFile << m_sResumeFileNameDisplayed << ": " << m_sResumeFileName << endl;
	outFile << m_sResumeFileNumIterationsDisplayed << ": " << m_iResumeFileNumIterations << endl;
	outFile << m_sPrintSummaryInfoEveryNIterationsDisplayed << ": " << m_iPrintSummaryInfoEveryNIterations << endl;
	outFile << m_sNodesFileDisplayed << ": " << m_sNodesFile << endl;
	outFile << m_sJobQueueTemplateDisplayed << ": " << m_sJobQueueTemplate << endl;
	if (m_sJobQueueTemplate.length() > 0)
		outFile << m_sNumJobQueueJobsDisplayed << ": " << m_iNumJobQueueJobs << endl;
	else
		outFile << m_sNumJobQueueJobsDisplayed << ": " << endl;
	outFile << m_sUsePrevWaveFunctionDisplayed << ": " << printYesNoParam(m_bUsePrevWaveFunction) << endl;
	
	outFile << m_sChargeDisplayed << ": " << m_iCharge << endl;
	outFile << m_sMultiplicityDisplayed << ": " << m_iMultiplicity << endl;
	outFile << m_sEnergyFileHeaderDisplayed << ":" << endl;
	outFile << m_sEnergyFileHeader << endl;
	
	outFile << m_sNumStructureTypesDisplayed << ": " << m_iNumStructureTypes << endl;
	
	if (!m_bResumeFileRead)
		outFile << setprecision(defaultPrecision);
	for (i = 0; i < m_iNumStructureTypes; ++i)
	{
		outFile << endl << m_sNumStructuresOfEachTypeDisplayed << ": " << m_iNumStructuresOfEachType[i] << endl;
		outFile << m_sStructureFormatOfThisTypeDisplayed << ": " << m_sStructureFormats[i] << endl;
		for (j = 0; j < (signed int)m_cartesianPoints[i].size(); ++j)
		{
			outFile << m_atomicNumbers[i][j] << " ";
			outFile << m_cartesianPoints[i][j].x << " ";
			outFile << m_cartesianPoints[i][j].y << " ";
			outFile << m_cartesianPoints[i][j].z << endl;
		}
	}
}

void Input::writeResumeFile(string &fileName, vector<MoleculeSet*> &moleculeSets,
                            vector<MoleculeSet*> &bestNMoleculeSets, vector<MoleculeSet*> &bestIndividualMoleculeSets,
                            time_t startTime, time_t endTime)
{
	int i;
	// Create the resume file just in case we need to stop the program
	ofstream resumeFile(fileName.c_str(), ios::out);
	if (!resumeFile.is_open()) {
		cout << "Unable to write the temporary resume file: " << fileName << endl;
		return;
	}
	m_bResumeFileRead = true;
	printToFile(resumeFile);
	resumeFile << "\n\nStructures in the current population:" << endl;
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
	{
		resumeFile << "Structure #: " << (i+1) << endl;
		moleculeSets[i]->printToResumeFile(resumeFile, (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION));
	}
	
	resumeFile << "\nBest " << bestNMoleculeSets.size() << " structures: " << endl;
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
	{
		resumeFile << "Structure #: " << (i+1) << endl;
		bestNMoleculeSets[i]->printToResumeFile(resumeFile, false);
	}
	
	if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		resumeFile << m_sDecreasingTempDisplayed << ": " << m_bDecreasingTemp << endl;
		resumeFile << m_sConsecutiveGenerationsOfMeetingAcceptanceRatioDisplayed << ": "
	               << m_iConsecutiveGenerationsOfMeetingAcceptanceRatio << endl;
		resumeFile << m_sGenerationsSinceMinAcceptedTransitionsReachedDisplayed << ": "
		           << m_iGenerationsSinceMinAcceptedTransitionsReached << endl;
		
		resumeFile << m_sAcceptedTransitionsDisplayed << ":" << endl;
		resumeFile << m_prgAcceptedTransitions[0];
		for (i = 1; i < m_iNumIterationsBeforeDecreasingTemp; ++i)
			resumeFile << "," << m_prgAcceptedTransitions[i];
		resumeFile << endl;
		
		resumeFile << m_sAcceptedTransitionsIndexDisplayed << ": "
		           << m_iAcceptedTransitionsIndex << endl;
	} else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) {
		resumeFile << "\nIndividual best structures:" << endl;
		for (i = 0; i < (signed int)bestIndividualMoleculeSets.size(); ++i)
		{
			resumeFile << "Structure #: " << (i+1) << endl;
			bestIndividualMoleculeSets[i]->printToResumeFile(resumeFile, false);
		}
		resumeFile << "Visibility distance: " << m_fLocalMinDist << endl;
		resumeFile << "Starting visibility distance: " << m_fStartVisibilityDistance << endl;
		resumeFile << "Attraction or repulsion(1 or -1): " << m_fAttractionRepulsion << endl;
		resumeFile << "Number of iterations in which the best energy hasn't changed: "
		           << m_iNumIterationsBestEnergyHasntChanged << endl;
	} else if (m_iAlgorithmToDo == GENETIC_ALGORITHM) {
	}

	
	resumeFile << m_sIterationDisplayed << ": " << m_iIteration << endl;
	
	resumeFile << "Number of times the energy was calculated: " << m_iNumEnergyEvaluations << endl;
	m_tTimeStampStart = startTime;
	m_tTimeStampEnd = endTime;
	resumeFile << "Time when program started execution: " << m_tTimeStampStart << endl;
	resumeFile << "Time when program stopped execution: " << m_tTimeStampEnd << endl;
	
	resumeFile.close();
}

bool Input::open(string &fileName, vector<MoleculeSet*> &moleculeSets,
                 vector<MoleculeSet*> &bestNMoleculeSets, vector<MoleculeSet*> &bestIndividualMoleculeSets)
{
	const int MAX_LINE_LENGTH = 5000;
	char fileLine[MAX_LINE_LENGTH];
	char copyOfFileLine[MAX_LINE_LENGTH];
	ifstream infile(fileName.c_str());
	int i;
	int temp;
	MoleculeSet* newMoleculeSet;
	int numBestStructures;
	char* myString;
	
	if (!infile)
	{
		cout << "Can't open the input file: " << fileName << endl;
		return false;
	}
	if (!readFile(infile)) {
		infile.close();
		return false;
	}
	
	if (!m_bResumeFileRead) {
		m_sInputFileName = fileName;
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		moleculeSets.clear();
		for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			delete bestNMoleculeSets[i];
		bestNMoleculeSets.clear();
		for (i = 0; i < (signed int)bestIndividualMoleculeSets.size(); ++i)
			delete bestIndividualMoleculeSets[i];
		bestIndividualMoleculeSets.clear();
		m_iIteration = 0;
		m_bDecreasingTemp = false;
		m_iConsecutiveGenerationsOfMeetingAcceptanceRatio = 0;
		m_iGenerationsSinceMinAcceptedTransitionsReached = 0;
		infile.close();
		return true;
	}
	
	
	// Read the rest of the file which is the "resume" part of the resume file
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "There should be two blank lines before the structures in the input file." << endl;
		infile.close();
		return false;
	}
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line containing the text 'Structures in the current population' is missing in the input file." << endl;
		infile.close();
		return false;
	}
	
	int moleculeSetId = 0;
	for (i = 0; i < m_iTotalPopulationSize; ++i)
	{
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line containing the structure number is missing in the input file." << endl;
			infile.close();
			return false;
		}
		if (sscanf(fileLine, "Structure #: %d", &temp) != 1)
		{
			cout << "Failed to read a structure number in the input file." << endl;
			cout << "Line with the error: " << fileLine << endl;
			infile.close();
			return false;
		}
		if ((i + 1) != temp) {
			cout << "Structure number mismatch in the input file.  Expecting: " << (i + 1)
			                                                 << ", Found: " << temp << endl;
			infile.close();
			return false;
		}
		newMoleculeSet = new MoleculeSet();
		newMoleculeSet->copy(m_tempelateMoleculeSet);
		newMoleculeSet->setId(++moleculeSetId);
		if (!newMoleculeSet->readFromResumeFile(infile, fileLine, MAX_LINE_LENGTH, (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION))) {
			delete newMoleculeSet;
			infile.close();
			return false;
		}
		moleculeSets.push_back(newMoleculeSet);
	}
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "There should be two blank lines between the population structures and the best structures in the input file." << endl;
		infile.close();
		return false;
	}
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line containing the number of best structures is missing in the input file." << endl;
		infile.close();
		return false;
	}
	if (sscanf(fileLine, "Best %d structures:", &numBestStructures) != 1)
	{
		cout << "Failed to read the number of best structures in the input file." << endl;
		cout << "Line with the error: " << fileLine << endl;
		infile.close();
		return false;
	}
	for (i = 0; i < numBestStructures; ++i)
	{
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line containing the best structure number is missing in the input file." << endl;
			infile.close();
			return false;
		}
		if (sscanf(fileLine, "Structure #: %d", &temp) != 1)
		{
			cout << "Failed to read a best structure number in the input file." << endl;
			cout << "Line with the error: " << fileLine << endl;
			infile.close();
			return false;
		}
		if ((i + 1) != temp) {
			cout << "Best structure number mismatch in the input file.  Expecting: " << (i + 1)
			                                                      << ", Found: " << temp << endl;
			infile.close();
			return false;
		}
		newMoleculeSet = new MoleculeSet();
		newMoleculeSet->copy(m_tempelateMoleculeSet);
		if (!newMoleculeSet->readFromResumeFile(infile, fileLine, MAX_LINE_LENGTH, false)) {
			delete newMoleculeSet;
			infile.close();
			return false;
		}
		bestNMoleculeSets.push_back(newMoleculeSet);
	}
	
	if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: " << m_sDecreasingTempDisplayed << endl;
			infile.close();
			return false;
		}
		if (!getIntParam(fileLine, m_sDecreasingTempDisplayed, temp))
		{
			cout << "Error reading the parameter '" << m_sDecreasingTempDisplayed << "' in the input file." << endl;
			infile.close();
			return false;
		}
		m_bDecreasingTemp = temp;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << m_sConsecutiveGenerationsOfMeetingAcceptanceRatioDisplayed << endl;
			infile.close();
			return false;
		}
		if (!getIntParam(fileLine, m_sConsecutiveGenerationsOfMeetingAcceptanceRatioDisplayed,
		                 m_iConsecutiveGenerationsOfMeetingAcceptanceRatio))
		{
			cout << "Error reading the parameter '" << m_sConsecutiveGenerationsOfMeetingAcceptanceRatioDisplayed
			     << "' in the input file." << endl;
			infile.close();
			return false;
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << m_sGenerationsSinceMinAcceptedTransitionsReachedDisplayed << endl;
			infile.close();
			return false;
		}
		if (!getIntParam(fileLine, m_sGenerationsSinceMinAcceptedTransitionsReachedDisplayed,
		                 m_iGenerationsSinceMinAcceptedTransitionsReached))
		{
			cout << "Error reading the parameter '" << m_sGenerationsSinceMinAcceptedTransitionsReachedDisplayed
			     << "' in the input file." << endl;
			infile.close();
			return false;
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << m_sAcceptedTransitionsDisplayed << endl;
			infile.close();
			return false;
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line after this parameter is missing: "
			     << m_sAcceptedTransitionsDisplayed << endl;
			infile.close();
			return false;
		}
		// Get the comma separated list of integers in fileLine
		strncpy(copyOfFileLine, fileLine, MAX_LINE_LENGTH);
		myString = strtok(copyOfFileLine, ",");
		i = 0;
		if (myString == NULL)
			return false;
		else
			m_prgAcceptedTransitions[i] = atoi(myString);
		++i;
		while (i < m_iNumIterationsBeforeDecreasingTemp) {
			myString = strtok(NULL, ",");
			if (myString == NULL) {
				--i;
				break;
			} else
				m_prgAcceptedTransitions[i] = atoi(myString);
			++i;
		}
		if (i != m_iNumIterationsBeforeDecreasingTemp) {
			cout << "The list of numbers after this parameter should be of length " << m_iNumIterationsBeforeDecreasingTemp
			     << " but it's of length " << i << ": " << m_sAcceptedTransitionsDisplayed << endl;
			exit(0);
		}
		
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << m_sAcceptedTransitionsIndexDisplayed << endl;
			infile.close();
			return false;
		}
		if (!getIntParam(fileLine, m_sAcceptedTransitionsIndexDisplayed,
		                 m_iAcceptedTransitionsIndex))
		{
			cout << "Error reading the parameter '" << m_sAcceptedTransitionsIndexDisplayed
			     << "' in the input file." << endl;
			infile.close();
			return false;
		}
	} else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) {
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "There should be a blank line before the best individual structures in the input file." << endl;
			infile.close();
			return false;
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << "Individual best structures:" << endl;
			infile.close();
			return false;
		}
		if (strncmp(fileLine, "Individual best structures:", MAX_LINE_LENGTH) != 0)
		{
			cout << "Failed to read the line that should say 'Individual best structures:' in the input file." << endl;
			cout << "Line with the error: " << fileLine << endl;
			infile.close();
			return false;
		}
		for (i = 0; i < m_iTotalPopulationSize; ++i)
		{
			if (!infile.getline(fileLine, MAX_LINE_LENGTH))
			{
				cout << "The line containing the individual best structure number is missing in the input file." << endl;
				infile.close();
				return false;
			}
			if (sscanf(fileLine, "Structure #: %d", &temp) != 1)
			{
				cout << "Failed to read an individual best structure number in the input file." << endl;
				cout << "Line with the error: " << fileLine << endl;
				infile.close();
				return false;
			}
			if ((i + 1) != temp) {
				cout << "Individual best structure number mismatch in the input file.  Expecting: " << (i + 1)
				                                                      << ", Found: " << temp << endl;
				infile.close();
				return false;
			}
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(m_tempelateMoleculeSet);
			if (!newMoleculeSet->readFromResumeFile(infile, fileLine, MAX_LINE_LENGTH, false)) {
				delete newMoleculeSet;
				infile.close();
				return false;
			}
			bestIndividualMoleculeSets.push_back(newMoleculeSet);
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << "Visibility distance:" << endl;
			infile.close();
			return false;
		}
		if (sscanf(fileLine, "Visibility distance: %lf", &m_fLocalMinDist) != 1)
		{
			cout << "Error reading the parameter '" << "Visibility distance"
			     << "' in the input file." << endl;
			cout << "Line with the error: " << fileLine << endl;
			infile.close();
			return false;
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << "Starting visibility distance:" << endl;
			infile.close();
			return false;
		}
		if (sscanf(fileLine, "Starting visibility distance: %lf", &m_fStartVisibilityDistance) != 1)
		{
			cout << "Error reading the parameter '" << "Starting visibility distance"
			     << "' in the input file." << endl;
			cout << "Line with the error: " << fileLine << endl;
			infile.close();
			return false;
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << "Attraction or repulsion(1 or -1):" << endl;
			infile.close();
			return false;
		}
		if (sscanf(fileLine, "Attraction or repulsion(1 or -1): %lf", &m_fAttractionRepulsion) != 1)
		{
			cout << "Error reading the parameter '" << "Attraction or repulsion(1 or -1)"
			     << "' in the input file." << endl;
			cout << "Line with the error: " << fileLine << endl;
			infile.close();
			return false;
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << "Number of iterations in which the best energy hasn't changed:" << endl;
			infile.close();
			return false;
		}
		if (sscanf(fileLine, "Number of iterations in which the best energy hasn't changed: %d",
		                    &m_iNumIterationsBestEnergyHasntChanged) != 1)
		{
			cout << "Error reading the parameter '" << "Number of iterations in which the best energy hasn't changed"
			     << "' in the input file." << endl;
			cout << "Line with the error: " << fileLine << endl;
			infile.close();
			return false;
		}
	} else if (m_iAlgorithmToDo == GENETIC_ALGORITHM) {
	}
	
	// Read some more parameters
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line that's supposed to have this parameter is missing: " << m_sIterationDisplayed << endl;
		infile.close();
		return false;
	}
	if (!getIntParam(fileLine, m_sIterationDisplayed, m_iIteration))
	{
		cout << "Error reading the parameter '" << m_sIterationDisplayed << "' in the input file." << endl;
		infile.close();
		return false;
	}



	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line that's supposed to have this parameter is missing: "
		     << "Number of times the energy was calculated" << endl;
		infile.close();
		return false;
	}
	if (sscanf(fileLine, "Number of times the energy was calculated: %d", &m_iNumEnergyEvaluations) != 1)
	{
		cout << "Error reading the parameter '" << "Number of times the energy was calculated"
		     << "' in the input file." << endl;
		cout << "Line with the error: " << fileLine << endl;
		infile.close();
		return false;
	}

	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line that's supposed to have this parameter is missing: "
		     << "Time when program started execution" << endl;
		infile.close();
		return false;
	}
	if (sscanf(fileLine, "Time when program started execution: %d", &temp) != 1)
	{
		cout << "Error reading the parameter '" << "Time when program started execution"
		     << "' in the input file." << endl;
		cout << "Line with the error: " << fileLine << endl;
		infile.close();
		return false;
	}
	m_tTimeStampStart = temp;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line that's supposed to have this parameter is missing: "
		     << "Time when program stopped execution" << endl;
		infile.close();
		return false;
	}
	if (sscanf(fileLine, "Time when program stopped execution: %d", &temp) != 1)
	{
		cout << "Error reading the parameter '" << "Time when program stopped execution"
		     << "' in the input file." << endl;
		cout << "Line with the error: " << fileLine << endl;
		infile.close();
		return false;
	}
	m_tTimeStampEnd = temp;
	
	infile.close();
	m_sInputFileName = fileName;
	return true;
}
