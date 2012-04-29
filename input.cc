////////////////////////////////////////////////////////////////////////////////
// Purpose: This file reads parameters from the input file.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "input.h"
#include "energy.h"

string Input::s_program_directory = "";

/////////////////////////////////////////////////////////////////////
// Purpose: This constructor initializes important variables.
// Parameters: none
// Returns: nothing
Input::Input (void)
{
	m_bTransitionStateSearch = false;
	m_pSelectedEnergyProgram = NULL;	
	m_sInputFileName = "";
	m_bResumeFileRead = false;
	m_bOptimizationFileRead = false;
	
	m_iCharge = 0;
	m_iMultiplicity = 1;
	m_fStartingTemperature = 1000;
	m_fBoltzmanConstant = 3.0e-7; // this is the default constant when converting from hartree to jewels
	m_fDesiredAcceptedTransitions = 0.9; // this is a percentage between 0 and 1
	m_iLinearSructures = 10;
	m_iPlanarStructures = 10;
	m_i3DStructures = 10;
	m_i3DStructuresWithMaxDist = 10;
	m_i3DNonFragStructuresWithMaxDist = 10;
	m_iTotalPopulationSize = m_iLinearSructures + m_iPlanarStructures + m_i3DStructures +
	                         m_i3DStructuresWithMaxDist + m_i3DNonFragStructuresWithMaxDist;
	m_fMinDistnaceBetweenSameMoleculeSets = 5;
	m_iNumberOfBestStructuresToSave = 1;
	m_iNumberOfLogFilesToSave = 0;
	
	m_fNumPerterbations = 1;
	m_fStartCoordinatePerturbation = 0.01;
	m_fMinCoordinatePerturbation = 0.001;
	m_fStartAnglePerturbation = PIE / 6;
	m_fMinAnglePerturbation = PIE / 60;
	m_fGeneralMinAtomDistance = 0.7;
	m_fMinAcceptedTransitions = 0.05; // stop if the number of accepted transitions / the number of transitions
	                                  // is below this percentage for 3 or > generations/temperatures
	m_iNumIterationsBeforeDecreasingTemp = 50;
	m_prgAcceptedTransitions = new int[m_iNumIterationsBeforeDecreasingTemp];
	for (int i = 0; i < m_iNumIterationsBeforeDecreasingTemp; ++i)
		m_prgAcceptedTransitions[i] = 0;
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
	m_iPrintSummaryInfoEveryNIterations = 20;
	
	m_sNodesFile = "";
	m_srgNodeNames.clear();
	m_sEnergyFileHeader = "";
	
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
	m_tElapsedSeconds = 0;
	m_iNumEnergyEvaluations = 0;
	m_iFreezeUntilIteration = 0;
	m_iAcceptedTransitionsIndex = -1;

	
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
	m_fStartVisibilityDistance = -1;
	m_bStartingVisibilityAuto = true;
	m_fVisibilityDistanceIncrease = 0.003;
	m_bUseLocalOptimization = false;
	m_fSwitchToRepulsionWhenDiversityIs = 0;
	m_iSwitchToRepulsionWhenNoProgress = 0;
	m_fSwitchToAttractionWhenDiversityIs = 0;
	m_fIndividualBestUpdateDist = 0.1;
	
	// Variables used in PSO resume file
	m_fVisibilityDistance = 0; // depends on the number of atoms and the size of the search box
	m_fAttractionRepulsion = 1;
	m_iNumIterationsBestEnergyHasntChanged = 0;

	// Varables used in Genetic Algorithm
	m_sGeneticAlgorithmParametersDisplayed = "Genetic Algorithm Parameters";
	
	// Variables used in optimization files
	m_iStructuresToOptimizeAtATime = 10;
	
	// Simulated Annealing Strings
	m_sPerformNonFragSearchDisplayed = "Search only for non-fragmented structures";
	m_sPerformBasinHoppingDisplayed = "Use energy value from local optimization(basin hopping)";
	m_sTransitionStateSearchDisplayed = "Search for transition states (random search with every perturbation accepted)";
	m_sStartingTemperatureDisplayed = "Starting temperature or desired starting percentage of accepted transitions(use %)";
	m_sBoltzmanConstantDisplayed = "Boltzmann constant";
	m_sNumIterationsBeforeDecreasingTempDisplayed = "Minimum number of iterations before decreasing the temperature (N)";
	m_sAcceptanceRatioDisplayed = "Decrease the temperature when the percentage of accepted transitions for the past N iterations is below";
	m_sNumPerterbationsDisplayed = "Starting number of perturbations per iteration";
	m_sStartCoordinatePerturbationDisplayed = "Starting coordinate perturbation (angstroms)";
	m_sMinCoordinatePerturbationDisplayed = "Minimum coordinate perturbation (angstroms)";
	m_sStartAnglePerturbationDisplayed = "Starting angle perturbation (deg)";
	m_sMinAnglePerturbationDisplayed = "Minimum angle perturbation (deg)";
	m_sMinTemperatureToStopDisplayed = "Don't stop while the temperature is above this value";
	m_sMinAcceptedTransitionsDisplayed = "Stop if the percentage of accepted transitions for the past N iterations is below";
	m_sQuenchingFactorDisplayed = "Quenching factor(use 1 for basin hopping)";
	m_sDecreasingTempDisplayed = "Decreasing temperature";
	m_sAverageTransitionsDisplayDisplayed = "Transition count";
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
	m_sIndividualBestUpdateDistDisplayed = "Don't update individual best solutions within this RMS distance of the best seen by the population";
	m_sEnforceMinDistOnCopyDisplayed = "Enforce minimum distance constraints on a copy of each structure rather than on the original";
	m_sStartVisibilityDistanceDisplayed = "Starting RMS visibility distance (use 'auto' for automatic)";
	m_sVisibilityDistanceIncreaseDisplayed = "Increase the RMS visibility distance by this amount each iteration";
	m_sUseLocalOptimizationDisplayed = "Use energy value from local optimization";
	
	m_sSimInputVersionLineDisplayed = "Monte Carlo Simulated Annealing Input File Version";
	m_sSimResumeVersionLineDisplayed = "Monte Carlo Simulated Annealing Resume File Version";
	m_sSimOptimizationVersionLineDisplayed = "Monte Carlo Simulated Annealing Optimization File Version";
	m_sPSOInputVersionLineDisplayed = "Particle Swarm Optimization Input File Version ";
	m_sPSOResumeVersionLineDisplayed = "Particle Swarm Optimization Resume File Version";
	m_sPSOOptimizationVersionLineDisplayed = "Particle Swarm Optimization Optimization File Version";
	m_sGAInputVersionLineDisplayed = "Genetic Algorithm Input File Version ";
	m_sGAResumeVersionLineDisplayed = "Genetic Algorithm Resume File Version";
	m_sGAOptimizationVersionLineDisplayed = "Genetic Algorithm Optimization File Version";
	m_sEnergyFunctionDisplayed = "Energy function to use";
	m_sPathToEnergyFilesDisplayed = "Path to energy files";
	m_sPathToScratchDisplayed = "Path to scratch directory (optional)";
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
	m_sNumberOfLogFilesToSaveDisplayed = "Save this many quantum output files from the list of best structures";
	
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
	m_sEnergyFileHeaderDisplayed = "Energy file header";
	m_sEnergyFileFooterDisplayed = "Energy file footer (optional)";
	
	m_sIterationDisplayed = "Iteration number";
	m_sFreezeUntilIterationDisplayed = "Freeze seeded molecules/units for this many iterations";
	
	m_sStructuresToOptimizeAtATimeDisplayed = "Number of structures to optimize at a time";

	m_bRunComplete = false;
	
	m_bTestMode = false;
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
	if ((*fileLine != ':') || (*parameterName != '\0'))
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
	if ((*fileLine != ':') || (*parameterName != '\0'))
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
	if ((*fileLine != ':') || (*parameterName != '\0'))
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
	int i;

	strncpy(copyOfFileLine, fileLine, maxLineLength);

	myString = strtok(copyOfFileLine, " ");
	if (myString == NULL)
		return false;
	else {
		atomicNumber = atoi(myString);
		if (atomicNumber < 1) {
			for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i)
				if (myString == Atom::s_rgAtomcSymbols[i]) {
					atomicNumber = i;
					break;
				}
			if (atomicNumber < 1) {
				cout << "Unidentified element symbol: " << myString << endl;
				return false;
			}
		} else if (atomicNumber > MAX_ATOMIC_NUMBERS) {
			cout << "No there is no information available the for element with atomic number " << atomicNumber << "." << endl;
			cout << "Please update the periodic table file." << endl;
			return false;
		}
	}

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
// Purpose: This is a helper function that prints a boolean parameter.
// Parameters: yesNoParam - the boolean parameter
// Returns: yes or no
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

void Input::trim(string& str)
{
	string::size_type pos = str.find_last_not_of(' ');
	if(pos != string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != string::npos) str.erase(0, pos);
	} else
		str.erase(str.begin(), str.end());
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function reads input from the input file.
// Parameters: fileName - the input file name
// Returns: true if there were no errors
//     Note: If an error does occur, descriptive messages about
//     the error are printed before returning.
bool Input::readFile(ifstream &infile, bool setMinDistances, bool bReadNodesFile)
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
	int length;
	string::size_type versionPos;
	string version;
	
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
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_sSimResumeVersionLineDisplayed.c_str(), fileLine, m_sSimResumeVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = SIMULATED_ANNEALING;
		m_bResumeFileRead = true;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_sSimOptimizationVersionLineDisplayed.c_str(), fileLine, m_sSimOptimizationVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = SIMULATED_ANNEALING;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = true;
	} else if (strncmp(m_sPSOInputVersionLineDisplayed.c_str(), fileLine, m_sPSOInputVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = PARTICLE_SWARM_OPTIMIZATION;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_sPSOResumeVersionLineDisplayed.c_str(), fileLine, m_sPSOResumeVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = PARTICLE_SWARM_OPTIMIZATION;
		m_bResumeFileRead = true;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_sPSOOptimizationVersionLineDisplayed.c_str(), fileLine, m_sPSOOptimizationVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = PARTICLE_SWARM_OPTIMIZATION;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = true;
	} else if (strncmp(m_sGAInputVersionLineDisplayed.c_str(), fileLine, m_sGAInputVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = GENETIC_ALGORITHM;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_sGAResumeVersionLineDisplayed.c_str(), fileLine, m_sGAResumeVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = GENETIC_ALGORITHM;
		m_bResumeFileRead = true;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_sGAOptimizationVersionLineDisplayed.c_str(), fileLine, m_sGAOptimizationVersionLineDisplayed.length()) == 0) {
		m_iAlgorithmToDo = GENETIC_ALGORITHM;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = true;
	} else {
		cout << "Unrecognized input file header on line " << lineNumber << "." << endl;
		return false;
	}
	
	temp = fileLine;
	trim(temp);
	versionPos = temp.rfind(" ") + 1;
	version = temp.substr(versionPos);
	if (strncmp(version.c_str(),INPUT_FILE_VERSION, strlen(INPUT_FILE_VERSION)) != 0)
		cout << "Warning: this input file is not version " << INPUT_FILE_VERSION << "." << endl;
	
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
	if (!getStringParam(fileLine, m_sEnergyFunctionDisplayed, temp))
	{
		cout << "Error reading the parameter '" << m_sEnergyFunctionDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
    
 	m_pSelectedEnergyProgram = NULL;	
	for (i = 0; i < (int)EnergyProgram::s_energyPrograms.size(); ++i)
		if (temp == EnergyProgram::s_energyPrograms[i]->m_sName)
			m_pSelectedEnergyProgram = EnergyProgram::s_energyPrograms[i];
	if (m_pSelectedEnergyProgram == NULL) {
		cout << "Energy function not recognized on line " << lineNumber << ": '" << temp << "'" << endl;
		cout << "Valid energy functions are: ";
		for (i = 0; i < (int)EnergyProgram::s_energyPrograms.size(); ++i) {
			if (i >= 1)
				cout << ", ";
			cout << EnergyProgram::s_energyPrograms[i]->m_sName;
		}
		cout << endl;
		
		return false;
	}

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
	
	if ((m_pSelectedEnergyProgram->m_iNumOutputFileTypes == 0) || (m_sPathToEnergyFiles.length() == 0))
		m_sSaveLogFilesInDirectory = "";
	else
		m_sSaveLogFilesInDirectory = m_sPathToEnergyFiles + "/bestSavedStructures";
	
	m_sNodesFile = m_sPathToEnergyFiles + "/nodes.txt";
	if (bReadNodesFile && (m_pSelectedEnergyProgram->m_sPathToExecutable.length() > 0))
		if (!readNodesFile())
			return false;

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sPathToScratchDisplayed, m_sPathToScratch))
	{
		cout << "Error reading the parameter '" << m_sPathToScratchDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	checkDirectoryOrFileName(m_sPathToScratch);
   	
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
	
	if (setMinDistances)
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
		if (setMinDistances)
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
	
		if (m_bPerformNonFragSearch)
			if ((m_iLinearSructures > 0) || (m_iPlanarStructures > 0) ||
			    (m_i3DStructures > 0) || (m_i3DStructuresWithMaxDist > 0) ||
			    (m_i3DNonFragStructuresWithMaxDist == 0)) {
				cout << "When searching for non-fragmented structures, the entire population must be initialized as non-fragmented." << endl;
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

		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_sTransitionStateSearchDisplayed, m_bTransitionStateSearch))
		{
			cout << "Error reading the parameter '" << m_sTransitionStateSearchDisplayed << "' on line " << lineNumber << 
				" in the input file." << endl;
			return false;
		}
	
		if (m_bPerformBasinHopping && m_bTransitionStateSearch) {
			cout << "You may not perform basin hopping and a transition state search at the same time." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_sStartingTemperatureDisplayed, temp))
		{
			cout << "Error reading the parameter '" << m_sStartingTemperatureDisplayed << "' on line " << lineNumber << 
				" in the input file." << endl;
			return false;
		}
		if (temp.find("%") != string::npos) { // if we found a % sign
			m_fDesiredAcceptedTransitions = atof(temp.c_str())*0.01;
			m_fStartingTemperature = 0; // will be set in main
		} else {
			m_fDesiredAcceptedTransitions = 0;
			m_fStartingTemperature = atof(temp.c_str());
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
		if (version == "1.0")
			m_fBoltzmanConstant *= HARTREE_TO_JOULES;
		
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
		for (i = 0; i < m_iNumIterationsBeforeDecreasingTemp; ++i)
			m_prgAcceptedTransitions[i] = 0;
		
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
		if (!getFloatParam(fileLine, m_sNumPerterbationsDisplayed, m_fNumPerterbations))
		{
			cout << "Error reading the parameter '" << m_sNumPerterbationsDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		if (!m_bResumeFileRead && !m_bOptimizationFileRead)
			m_fNumPerterbations += 0.5;
		
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
		m_fAngleIndividualMinimumAttraction *= DEG_TO_RAD;
		
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
		m_fAnglePopulationMinimumAttraction *= DEG_TO_RAD;
		
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
		m_fAngleMaximumVelocity *= DEG_TO_RAD;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_sStartVisibilityDistanceDisplayed, temp))
		{
			cout << "Error reading the parameter '" << m_sStartVisibilityDistanceDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
                m_bStartingVisibilityAuto = (strncmp(temp.c_str(),"auto",4) == 0);
		if (m_bStartingVisibilityAuto) {
			m_fStartVisibilityDistance = -1;
			m_fVisibilityDistance = 0;
		} else {
			m_fStartVisibilityDistance = atof(temp.c_str());
			m_fVisibilityDistance = m_fStartVisibilityDistance;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_sVisibilityDistanceIncreaseDisplayed, m_fVisibilityDistanceIncrease))
		{
			cout << "Error reading the parameter '" << m_sVisibilityDistanceIncreaseDisplayed << "' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
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
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_sNumberOfLogFilesToSaveDisplayed, m_iNumberOfLogFilesToSave))
	{
		cout << "Error reading the parameter '" << m_sNumberOfLogFilesToSaveDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	if (m_pSelectedEnergyProgram->m_iNumOutputFileTypes == 0)
		m_iNumberOfLogFilesToSave = 0;
	if (m_iNumberOfLogFilesToSave > m_iNumberOfBestStructuresToSave)
		m_iNumberOfLogFilesToSave = m_iNumberOfBestStructuresToSave;
	
	if (m_fSwitchToRepulsionWhenDiversityIs > 0) {
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
		length = strlen(fileLine);
		if (length == 0)
			break;
		if ((m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN) && (length > 80)) {
			cout << "A line in the Gaussian energy file header is longer than 80 characters, please fix this:" << endl
			     << fileLine << endl;
			return false;
		}
		m_sEnergyFileHeader.append(fileLine);
		m_sEnergyFileHeader.append("\n");
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "Line " << lineNumber << " is missing in the input file." << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_sEnergyFileFooterDisplayed, temp))
	{
		cout << "Error reading the parameter '" << m_sEnergyFileFooterDisplayed << "' on line "
		     << lineNumber << " in the input file." << endl;
		return false;
	}
	
	m_sEnergyFileFooter = "";
	while (true)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "Line " << lineNumber << " is missing in the input file." << endl;
			return false;
		}
		length = strlen(fileLine);
		if (length == 0)
			break;
		if ((m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN) && (length > 80)) {
			cout << "A line in the Gaussian energy file footer is longer than 80 characters, please fix this:" << endl
			     << fileLine << endl;
			return false;
		}
		m_sEnergyFileFooter.append(fileLine);
		m_sEnergyFileFooter.append("\n");
	}
	if (m_bUseLocalOptimization && (m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN)) {
		if (m_sEnergyFileHeader.find("opt") == string::npos) {
			cout << "You have local optimization turned on, but you haven't specified 'opt' in the energy file header.  Please do this." << endl;
			return false;
		}
	}
	if (m_bPerformBasinHopping && (m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN)) {
		if (m_sEnergyFileHeader.find("opt") == string::npos) {
			cout << "When performing basin hopping with Gaussian, please specify 'opt' in the energy file header." << endl;
			return false;
		}
	}
	if ((m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN) && (m_sEnergyFileFooter.length() > 0)) {
		if (m_sEnergyFileFooter.find("--link1--\n") != 0) {
			cout << "When specifying an energy file footer with Gaussian, make sure that '--link1--' is on the first line." << endl;
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
	m_tempelateMoleculeSet.setNumberOfMolecules((int)moleculeVector.size());
	moleculeArray = m_tempelateMoleculeSet.getMolecules();
	for (i = 0; i < (int)moleculeVector.size(); ++i)
	{
		moleculeArray[i].copy(*moleculeVector[i]);
		delete moleculeVector[i];
	}
	moleculeVector.clear();
	if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
		if ((int)m_fNumPerterbations > m_tempelateMoleculeSet.getNumberOfMolecules()) {
			cout << "Please enter a number between 1 and " << m_tempelateMoleculeSet.getNumberOfMolecules() << " for the parameter '" << m_sNumPerterbationsDisplayed << "' in the input file." << endl;
			return false;
		}
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
	if (m_srgNodeNames.size() < 1) {
		cout << "Please specify at least one node in the nodes file." << endl;
		return false;
	} else
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
	static char yesStr[4];
	static char noStr[3];
	
	strcpy(yesStr,"yes");
	strcpy(noStr,"no");

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
	
	if (m_bResumeFileRead) {
		outFile << setiosflags(ios::fixed) << setprecision(resumeFilePrecision);
		if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
			outFile << m_sSimResumeVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			outFile << m_sPSOResumeVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == GENETIC_ALGORITHM)
			outFile << m_sGAResumeVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
	} else if (m_bOptimizationFileRead) {
		outFile << setiosflags(ios::fixed) << setprecision(resumeFilePrecision);
		if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
			outFile << m_sSimOptimizationVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			outFile << m_sPSOOptimizationVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == GENETIC_ALGORITHM)
			outFile << m_sGAOptimizationVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
	} else {
		outFile << setiosflags(ios::fixed) << setprecision(defaultPrecision);
		if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
			outFile << m_sSimInputVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			outFile << m_sPSOInputVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == GENETIC_ALGORITHM)
			outFile << m_sGAInputVersionLineDisplayed << " " << INPUT_FILE_VERSION << endl << endl;
	}
	
	outFile << m_sEnergyFunctionDisplayed << ": " << m_pSelectedEnergyProgram->m_sName << endl;
	outFile << m_sPathToEnergyFilesDisplayed << ": " << m_sPathToEnergyFiles << endl;
	outFile << m_sPathToScratchDisplayed << ": " << m_sPathToScratch << endl;
	
	outFile << m_sBoxLengthDisplayed << ": " << Atom::printFloat(m_boxDimensions.x) << endl;
	outFile << m_sLinearSructuresDisplayed << ": " << m_iLinearSructures << endl;
	outFile << m_sLinearBoxHeightDisplayed << ": " << Atom::printFloat(m_fLinearBoxHeight) << endl;
	outFile << m_sPlanarStructuresDisplayed << ": " << m_iPlanarStructures << endl;
	outFile << m_s3DStructuresDisplayed << ": " << m_i3DStructures << endl;
	outFile << m_s3DStructuresWithMaxDistDisplayed << ": " << m_i3DStructuresWithMaxDist << endl;
	outFile << m_s3DNonFragStructuresWithMaxDistDisplayed << ": " << m_i3DNonFragStructuresWithMaxDist << endl;
	outFile << m_sMinGeneralAtomDistanceDisplayed << ": " << Atom::printFloat(m_fGeneralMinAtomDistance) << endl;
	outFile << m_sMinAtomDistanceDisplayed << ": " << endl;
	Atom::outputMinDistances(outFile);
	outFile << m_sMaxAtomDistanceDisplayed << ": " << Atom::printFloat(m_fMaxAtomDistance) << endl;
	outFile << endl;
	
	if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		outFile << m_sSimulatedAnnealingParametersDisplayed << ":" << endl;
		outFile << m_sPerformNonFragSearchDisplayed << ": " << printYesNoParam(m_bPerformNonFragSearch) << endl;
		outFile << m_sPerformBasinHoppingDisplayed << ": " << printYesNoParam(m_bPerformBasinHopping) << endl;
		outFile << m_sTransitionStateSearchDisplayed << ": " << printYesNoParam(m_bTransitionStateSearch) << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(1);
		if ((m_fDesiredAcceptedTransitions == 0) || (m_iIteration > NUM_ITERATIONS_TO_SET_SCALING_FACTOR))
			outFile << m_sStartingTemperatureDisplayed << ": " << Atom::printFloat(m_fStartingTemperature) << endl;
		else
			outFile << m_sStartingTemperatureDisplayed << ": " << Atom::printFloat(m_fDesiredAcceptedTransitions*100) << "%" << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_sBoltzmanConstantDisplayed << ": " << Atom::printFloat(m_fBoltzmanConstant) << endl;
		outFile << m_sNumIterationsBeforeDecreasingTempDisplayed << ": " << m_iNumIterationsBeforeDecreasingTemp << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(1);
		outFile << m_sAcceptanceRatioDisplayed << ": " << Atom::printFloat(m_fAcceptanceRatio * 100) << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_sQuenchingFactorDisplayed << ": " << Atom::printFloat(m_fQuenchingFactor) << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(1);
		outFile << m_sMinTemperatureToStopDisplayed << ": " << Atom::printFloat(m_fMinTemperatureToStop) << endl;
		outFile << m_sMinAcceptedTransitionsDisplayed << ": " << Atom::printFloat(m_fMinAcceptedTransitions * 100) << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_sMaxIterationsDisplayed << ": " << m_iMaxIterations << endl;
		if (m_bResumeFileRead)
			outFile << m_sNumPerterbationsDisplayed << ": " << Atom::printFloat(m_fNumPerterbations) << endl;
		else
			outFile << m_sNumPerterbationsDisplayed << ": " << (int)m_fNumPerterbations << endl;
		outFile << m_sStartCoordinatePerturbationDisplayed << ": " << Atom::printFloat(m_fStartCoordinatePerturbation) << endl;
		outFile << m_sMinCoordinatePerturbationDisplayed << ": " << Atom::printFloat(m_fMinCoordinatePerturbation) << endl;
		outFile << m_sStartAnglePerturbationDisplayed << ": " << Atom::printFloat(m_fStartAnglePerturbation * RAD_TO_DEG) << endl;
		outFile << m_sMinAnglePerturbationDisplayed << ": " << Atom::printFloat(m_fMinAnglePerturbation * RAD_TO_DEG) << endl;
	} else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) {
		outFile << m_sParticleSwarmParametersDisplayed << ":" << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_sStartCoordInertiaDisplayed << ": " << Atom::printFloat(m_fStartCoordInertia) << endl;
		outFile << m_sEndCoordInertiaDisplayed << ": " << Atom::printFloat(m_fEndCoordInertia) << endl;
		outFile << m_sReachEndInertiaAtIterationDisplayed << ": " << m_iReachEndInertiaAtIteration << endl;
		outFile << m_sCoordIndividualMinimumAttractionDisplayed << ": " << Atom::printFloat(m_fCoordIndividualMinimumAttraction) << endl;
		outFile << m_sCoordPopulationMinimumAttractionDisplayed << ": " << Atom::printFloat(m_fCoordPopulationMinimumAttraction) << endl;
		if (m_fCoordMaximumVelocity != 0)
			outFile << m_sCoordMaximumVelocityDisplayed << ": " << Atom::printFloat(m_fCoordMaximumVelocity) << endl;
		else
			outFile << m_sCoordMaximumVelocityDisplayed << ": " << endl;
		outFile << m_sStartAngleInertiaDisplayed << ": " << Atom::printFloat(m_fStartAngleInertia) << endl;
		outFile << m_sEndAngleInertiaDisplayed << ": " << Atom::printFloat(m_fEndAngleInertia) << endl;
		outFile << m_sAngleIndividualMinimumAttractionDisplayed << ": " << Atom::printFloat(m_fAngleIndividualMinimumAttraction * RAD_TO_DEG) << endl;
		outFile << m_sAnglePopulationMinimumAttractionDisplayed << ": " << Atom::printFloat(m_fAnglePopulationMinimumAttraction * RAD_TO_DEG) << endl;
		if (m_fAngleMaximumVelocity != 0)
			outFile << m_sAngleMaximumVelocityDisplayed << ": " << Atom::printFloat(m_fAngleMaximumVelocity * RAD_TO_DEG) << endl;
		else
			outFile << m_sAngleMaximumVelocityDisplayed << ": " << endl;
		
		if (m_bStartingVisibilityAuto && (m_fStartVisibilityDistance == -1))
			outFile << m_sStartVisibilityDistanceDisplayed << ": auto" << endl;
		else
			outFile << m_sStartVisibilityDistanceDisplayed << ": " << Atom::printFloat(m_fStartVisibilityDistance) << endl;
		outFile << m_sVisibilityDistanceIncreaseDisplayed << ": " << Atom::printFloat(m_fVisibilityDistanceIncrease) << endl;
		
		if (m_fSwitchToRepulsionWhenDiversityIs > 0) {
			outFile << m_sSwitchToRepulsionWhenDiversityIsDisplayed << ": " << Atom::printFloat(m_fSwitchToRepulsionWhenDiversityIs) << endl;
			outFile << m_sSwitchToRepulsionWhenNoProgressDisplayed << ": " << m_iSwitchToRepulsionWhenNoProgress << endl;
			outFile << m_sSwitchToAttractionWhenDiversityIsDisplayed << ": " << Atom::printFloat(m_fSwitchToAttractionWhenDiversityIs) << endl;
		} else {
			outFile << m_sSwitchToRepulsionWhenDiversityIsDisplayed << ": " << endl;
			outFile << m_sSwitchToRepulsionWhenNoProgressDisplayed << ": " << endl;
			outFile << m_sSwitchToAttractionWhenDiversityIsDisplayed << ": " << endl;
		}
		outFile << m_sIndividualBestUpdateDistDisplayed << ": " << Atom::printFloat(m_fIndividualBestUpdateDist) << endl;

		outFile << m_sMaxIterationsDisplayed << ": " << m_iMaxIterations << endl;
		outFile << m_sEnforceMinDistOnCopyDisplayed << ": " << printYesNoParam(m_bEnforceMinDistOnCopy) << endl;
		outFile << m_sUseLocalOptimizationDisplayed << ": " << printYesNoParam(m_bUseLocalOptimization) << endl;
	} else if (m_iAlgorithmToDo == GENETIC_ALGORITHM) {
		outFile << m_sGeneticAlgorithmParametersDisplayed << ":" << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_sUseLocalOptimizationDisplayed << ": " << printYesNoParam(m_bUseLocalOptimization) << endl;
		outFile << m_sMaxIterationsDisplayed << ": " << m_iMaxIterations << endl;
	}
	
	outFile << endl;
	if (!(m_bResumeFileRead || m_bOptimizationFileRead))
		outFile << setprecision(1);
	outFile << m_sNumberOfBestStructuresToSaveDisplayed << ": " << m_iNumberOfBestStructuresToSave << endl;
	if (!(m_bResumeFileRead || m_bOptimizationFileRead))
		outFile << setprecision(defaultPrecision);
	outFile << m_sMinDistnaceBetweenSameMoleculeSetsDisplayed << ": " << Atom::printFloat(m_fMinDistnaceBetweenSameMoleculeSets) << endl;
	outFile << m_sNumberOfLogFilesToSaveDisplayed << ": " << m_iNumberOfLogFilesToSave << endl;
	outFile << m_sOutputFileNameDisplayed << ": " << m_sOutputFileName << endl;
	outFile << m_sResumeFileNameDisplayed << ": " << m_sResumeFileName << endl;
	outFile << m_sResumeFileNumIterationsDisplayed << ": " << m_iResumeFileNumIterations << endl;
	outFile << m_sPrintSummaryInfoEveryNIterationsDisplayed << ": " << m_iPrintSummaryInfoEveryNIterations << endl;
	
	outFile << m_sChargeDisplayed << ": " << m_iCharge << endl;
	outFile << m_sMultiplicityDisplayed << ": " << m_iMultiplicity << endl;
	outFile << m_sEnergyFileHeaderDisplayed << ":" << endl;
	outFile << m_sEnergyFileHeader << endl;
	outFile << m_sEnergyFileFooterDisplayed << ":" << endl;
	outFile << m_sEnergyFileFooter << endl;
	
	outFile << m_sNumStructureTypesDisplayed << ": " << m_iNumStructureTypes << endl;
	
	if (!(m_bResumeFileRead || m_bOptimizationFileRead))
		outFile << setprecision(defaultPrecision);
	for (i = 0; i < m_iNumStructureTypes; ++i)
	{
		outFile << endl << m_sNumStructuresOfEachTypeDisplayed << ": " << m_iNumStructuresOfEachType[i] << endl;
		outFile << m_sStructureFormatOfThisTypeDisplayed << ": " << m_sStructureFormats[i] << endl;
		for (j = 0; j < (int)m_cartesianPoints[i].size(); ++j)
		{
			outFile << m_atomicNumbers[i][j] << " ";
			outFile << Atom::printFloat(m_cartesianPoints[i][j].x) << " ";
			outFile << Atom::printFloat(m_cartesianPoints[i][j].y) << " ";
			outFile << Atom::printFloat(m_cartesianPoints[i][j].z) << endl;
		}
	}
}

void Input::printInputParamsToFile(ofstream &outFile)
{
	Input outputFile;
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	vector<MoleculeSet*> bestIndividualMoleculeSets;
	bool success;

	cout << "Reading input parameters from file: " << m_sOutputFileName << endl;
	success = outputFile.open(m_sOutputFileName, false, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets);
	outputFile.m_bResumeFileRead = false;
	outputFile.m_bOptimizationFileRead = m_bOptimizationFileRead;
	
	if (success)
		outputFile.printToFile(outFile);
	else
	{
		cout << "Unable to read file: " << m_sOutputFileName << endl;
		printToFile(outFile);
	}
}

void Input::writeResumeFile(string &fileName, vector<MoleculeSet*> &moleculeSets,
                            vector<MoleculeSet*> &bestNMoleculeSets, vector<MoleculeSet*> &bestIndividualMoleculeSets,
                            time_t elapsedSeconds, bool resumeOrOptimizationFile)
{
	int i;
	m_iTotalPopulationSize = (int)moleculeSets.size();
	// Create the resume file just in case we need to stop the program
	ofstream resumeFile(fileName.c_str(), ios::out);
	if (!resumeFile.is_open()) {
		cout << "Unable to write the temporary resume file: " << fileName << endl;
		return;
	}
	if (resumeOrOptimizationFile) {
		m_bResumeFileRead = true;
		m_bOptimizationFileRead = false;
	} else {
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = true;
	}
	printToFile(resumeFile);
	resumeFile << "\n\n" << moleculeSets.size() << " structures in the current population:" << endl;
	for (i = 0; i < (int)moleculeSets.size(); ++i)
	{
		resumeFile << "Structure #: " << (i+1) << endl;
		moleculeSets[i]->printToResumeFile(resumeFile, (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) && !m_bOptimizationFileRead);
	}
	
	resumeFile << "\nBest " << bestNMoleculeSets.size() << " structures: " << endl;
	for (i = 0; i < (int)bestNMoleculeSets.size(); ++i)
	{
		resumeFile << "Structure #: " << (i+1) << endl;
		bestNMoleculeSets[i]->printToResumeFile(resumeFile, false);
	}

	if (m_bOptimizationFileRead) {
		resumeFile << m_sStructuresToOptimizeAtATimeDisplayed << ": " << m_iStructuresToOptimizeAtATime << endl;
	} else if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		resumeFile << m_sDecreasingTempDisplayed << ": " << m_bDecreasingTemp << endl;
		
		resumeFile << m_sAcceptedTransitionsDisplayed << ":" << endl;
		resumeFile << m_prgAcceptedTransitions[0];
		for (i = 1; i < m_iNumIterationsBeforeDecreasingTemp; ++i)
			resumeFile << "," << m_prgAcceptedTransitions[i];
		resumeFile << endl;
		
		resumeFile << m_sAcceptedTransitionsIndexDisplayed << ": "
		           << m_iAcceptedTransitionsIndex << endl;
	} else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) {
		resumeFile << "\nIndividual best structures:" << endl;
		for (i = 0; i < (int)bestIndividualMoleculeSets.size(); ++i)
		{
			resumeFile << "Structure #: " << (i+1) << endl;
			bestIndividualMoleculeSets[i]->printToResumeFile(resumeFile, false);
		}
		resumeFile << "Visibility distance: " << m_fVisibilityDistance << endl;
		resumeFile << "Attraction or repulsion(1 or -1): " << m_fAttractionRepulsion << endl;
		resumeFile << "Number of iterations in which the best energy hasn't changed: "
		           << m_iNumIterationsBestEnergyHasntChanged << endl;
	} else if (m_iAlgorithmToDo == GENETIC_ALGORITHM) {
	}
	
	resumeFile << m_sIterationDisplayed << ": " << m_iIteration << endl;
	resumeFile << m_sFreezeUntilIterationDisplayed << ": " << m_iFreezeUntilIteration << endl;
	
	resumeFile << "Run Complete: " << printYesNoParam(m_bRunComplete) << endl;
	resumeFile << "Number of times the energy was calculated: " << m_iNumEnergyEvaluations << endl;
	m_tElapsedSeconds = elapsedSeconds;
	resumeFile << "Elapsed seconds: " << m_tElapsedSeconds << endl;
	
	resumeFile.close();
}

bool Input::open(string &fileName, bool setMinDistances, bool bReadNodesFile, vector<MoleculeSet*> &moleculeSets,
                 vector<MoleculeSet*> &bestNMoleculeSets, vector<MoleculeSet*> &bestIndividualMoleculeSets)
{
	const int MAX_LINE_LENGTH = 50000;
	char fileLine[MAX_LINE_LENGTH];
	char copyOfFileLine[MAX_LINE_LENGTH];
	ifstream infile(fileName.c_str());
	int i;
	int temp;
	MoleculeSet* newMoleculeSet;
	int numBestStructures;
	char* myString;
	unsigned long int myLong;

	for (i = 0; i < (int)moleculeSets.size(); ++i)
		delete moleculeSets[i];
	moleculeSets.clear();
	for (i = 0; i < (int)bestIndividualMoleculeSets.size(); ++i)
		delete bestIndividualMoleculeSets[i];
	bestIndividualMoleculeSets.clear();
	for (i = 0; i < (int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();
	
	if (!infile)
	{
		cout << "Can't open the input file: " << fileName << endl;
		return false;
	}
	if (!readFile(infile, setMinDistances, bReadNodesFile)) {
		infile.close();
		return false;
	}
	
	if (!(m_bResumeFileRead || m_bOptimizationFileRead)) {
		m_sInputFileName = fileName;
		for (i = 0; i < (int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		moleculeSets.clear();
		for (i = 0; i < (int)bestNMoleculeSets.size(); ++i)
			delete bestNMoleculeSets[i];
		bestNMoleculeSets.clear();
		for (i = 0; i < (int)bestIndividualMoleculeSets.size(); ++i)
			delete bestIndividualMoleculeSets[i];
		bestIndividualMoleculeSets.clear();
		m_iIteration = 0;
		m_bDecreasingTemp = false;
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
		cout << "The line containing the text '# structures in the current population' is missing in the input file." << endl;
		infile.close();
		return false;
	}
	if (sscanf(fileLine, "%d structures in the current population", &m_iTotalPopulationSize) != 1)
	{
		cout << "Failed to read the number of structures in the current population in the input file." << endl;
		cout << "Line with the error: " << fileLine << endl;
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
		if (!newMoleculeSet->readFromResumeFile(infile, fileLine, MAX_LINE_LENGTH, (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) && !m_bOptimizationFileRead)) {
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

		for (int j = 0; j < (int)m_pSelectedEnergyProgram->m_iNumOutputFileTypes; ++j)
			newMoleculeSet->setOutputEnergyFile(m_sSaveLogFilesInDirectory.c_str(), "best",
			      i+1, m_pSelectedEnergyProgram->m_sOutputFileTypeExtensions[j].c_str(), j, true);
		bestNMoleculeSets.push_back(newMoleculeSet);
	}
	
	if (m_bOptimizationFileRead) {
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: " << m_sStructuresToOptimizeAtATimeDisplayed << endl;
			infile.close();
			return false;
		}
		if (!getIntParam(fileLine, m_sStructuresToOptimizeAtATimeDisplayed, m_iStructuresToOptimizeAtATime))
		{
			cout << "Error reading the parameter '" << m_sStructuresToOptimizeAtATimeDisplayed << "' in the input file." << endl;
			infile.close();
			return false;
		}
	} else if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
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
			return false;
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
		if (sscanf(fileLine, "Visibility distance: %lf", &m_fVisibilityDistance) != 1)
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
		cout << "The line that's supposed to have this parameter is missing: " << m_sFreezeUntilIterationDisplayed << endl;
		infile.close();
		return false;
	}
	if (!getIntParam(fileLine, m_sFreezeUntilIterationDisplayed, m_iFreezeUntilIteration))
	{
		cout << "Error reading the parameter '" << m_sFreezeUntilIterationDisplayed << "' in the input file." << endl;
		infile.close();
		return false;
	}

	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line that's supposed to have this parameter is missing: "
		     << "Run Complete" << endl;
		infile.close();
		return false;
	}
	if (!getYesNoParam(fileLine, "Run Complete", m_bRunComplete))
	{
		cout << "Error reading the parameter '" << "Run Complete"
		     << "' in the input file." << endl;
		cout << "Line with the error: " << fileLine << endl;
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
		     << "Elapsed seconds" << endl;
		infile.close();
		return false;
	}
	if (sscanf(fileLine, "Elapsed seconds: %lu", &myLong) != 1)
	{
		cout << "Error reading the parameter '" << "Elapsed seconds"
		     << "' in the input file." << endl;
		cout << "Line with the error: " << fileLine << endl;
		infile.close();
		return false;
	}
	m_tElapsedSeconds = myLong;
	
	infile.close();
	m_sInputFileName = fileName;
	return true;
}

bool Input::seedCompatible(Input &seedInput)
{
	int i, j;

        if (m_iNumStructureTypes != seedInput.m_iNumStructureTypes) {
		cout << "Different number of structure types found in the seed file: " << seedInput.m_sInputFileName << endl;
                return false;
	}
        for (i = 0; i < m_iNumStructureTypes; ++i) {
                if (m_atomicNumbers[i].size() != seedInput.m_atomicNumbers[i].size()) {
			cout << "Different number of atoms found for structure type #" << (i+1) << " in the seed file: " << seedInput.m_sInputFileName << endl;
                        return false;
		}
                for (j = 0; j < (int)m_atomicNumbers[i].size(); ++j) {
                        if (m_atomicNumbers[i][j] != seedInput.m_atomicNumbers[i][j]) {
				cout << "Different atomic number found for atom #" << (j+1) << " in structure type #" << (i+1) << " in the seed file: " << seedInput.m_sInputFileName << endl;
                                return false;
			}
			if (m_cartesianPoints[i][j].x != seedInput.m_cartesianPoints[i][j].x) {
				cout << "Different x coordinate found for atom #" << (j+1) << " in structure type #" << (i+1) << " in the seed file: " << seedInput.m_sInputFileName << endl;
				return false;
			}
			if (m_cartesianPoints[i][j].y != seedInput.m_cartesianPoints[i][j].y) {
				cout << "Different y coordinate found for atom #" << (j+1) << " in structure type #" << (i+1) << " in the seed file: " << seedInput.m_sInputFileName << endl;
				return false;
			}
			if (m_cartesianPoints[i][j].z != seedInput.m_cartesianPoints[i][j].z) {
				cout << "Different z coordinate found for atom #" << (j+1) << " in structure type #" << (i+1) << " in the seed file: " << seedInput.m_sInputFileName << endl;
				return false;
			}
		}
		if (m_iNumStructuresOfEachType[i] < seedInput.m_iNumStructuresOfEachType[i]) {
			cout << "There are too many structures of type #" << (i+1) << " in the seed file: " << seedInput.m_sInputFileName << endl;
			return false;
		}
	}
	return true;
}

bool Input::printBondInfo()
{
	int i;
	int j;
	Molecule* molecules = m_tempelateMoleculeSet.getMolecules();
	
	j = 0;
	for (i = 0; i < m_iNumStructureTypes; ++i) {
		cout << "Molecule No.: " << (i+1) << endl;
		if (!molecules[j].findBonds())
			return false;
		if (!molecules[j].checkConnectivity())
			return false;
		molecules[j].printBondInfo();
		j += m_iNumStructuresOfEachType[i];
	}
	cout << "Please check that bonds are in the correct locations." << endl;
	cout << "If they are not, update the bond length criteria in the file: bondLengths.txt." << endl;
	cout << "The format is: atomic symbol,atomic symbol,bond type (s for single, d for double, t for triple),minimum distance-maximum distance in angstroms." << endl;
	return true;
}

bool Input::printTestFileHeader(int iterationNumber, MoleculeSet &startingGeometry)
{
	string logFileName;
	string header = Input::s_program_directory+"/testfiles/header.txt";
	string spacer1_1 = Input::s_program_directory+"/testfiles/spacer1.1.txt";
	string spacer1_2 = Input::s_program_directory+"/testfiles/spacer1.2.txt";
	char temp[500];
	
	logFileName = Input::s_program_directory+"/testfiles/test";
	if (iterationNumber == 0) {
		snprintf(temp,sizeof(temp),"rm %s/testfiles/test*.log", Input::s_program_directory.c_str());
		system(temp);
	} else {
		sprintf(temp, "%d", (iterationNumber / 2000) + 1);
		logFileName.append(temp);
	}
	logFileName.append(".log");
	
	m_testFile = fopen(logFileName.c_str(), "w");

	if (m_testFile == NULL) {
		cout << "Unable to write to file: " << logFileName << endl;
		return false;
	}
	if (!copyFileLines(header.c_str(), m_testFile))
		return false;
	startingGeometry.writeToGausianLogFile(m_testFile);
	if (!copyFileLines(spacer1_1.c_str(), m_testFile))
		return false;
	fprintf(m_testFile, "%0.9f", startingGeometry.getEnergy());
	if (!copyFileLines(spacer1_2.c_str(), m_testFile))
		return false;
	return true;
}

bool Input::printTestFileGeometry(int iterationNumber, MoleculeSet &geometry)
{
	if (iterationNumber % 2000 == 0) {
		printTestFileFooter();
		return printTestFileHeader(iterationNumber, geometry);
	} else {
		string spacer2_1 = Input::s_program_directory+"/testfiles/spacer2.1.txt";
		string spacer2_2 = Input::s_program_directory+"/testfiles/spacer2.2.txt";
		
		if (m_testFile == NULL)
			return false;
		
		geometry.writeToGausianLogFile(m_testFile);
		if (!copyFileLines(spacer2_1.c_str(), m_testFile))
			return false;
		fprintf(m_testFile, "%0.9f", geometry.getEnergy());
		if (!copyFileLines(spacer2_2.c_str(), m_testFile))
			return false;
	}
	return true;
}

bool Input::printTestFileFooter()
{
	string footer = Input::s_program_directory+"/testfiles/footer.txt";
	
	if (m_testFile == NULL)
		return false;
	
	if (!copyFileLines(footer.c_str(), m_testFile)) {
		fclose(m_testFile);
		return false;
	} else {
		fclose(m_testFile);
		return true;
	}
}

bool Input::copyFileLines(const char* fromFileName, FILE* toFile)
{
	FILE* ifp;
	size_t MAX_LINE_LENGTH = 5000;
	char* fileLine;
	
	ifp = fopen(fromFileName, "r");
	if (ifp == NULL)
	{
		cout << "Can't open the file: " << fromFileName << endl;
		return false;
	}
	
	fileLine = (char *) malloc (MAX_LINE_LENGTH);
	while (getline(&fileLine, &MAX_LINE_LENGTH, ifp) != -1) {
		if (strstr(fileLine,"Done") != NULL)
			fileLine[strlen(fileLine)-1] = '\0'; // change the last character to 'end of string' (remove new line)
		fprintf(toFile, "%s", fileLine);
	}
	delete[] fileLine;
	fclose(ifp);
	return true;
}

string Input::fileWithoutPath(const string &s)
{
	string::size_type pos;
	string answer;
	
	pos = s.rfind("/");
	if (pos == string::npos) // if we didn't find it
		return s;
	answer = s.c_str() + pos + 1;
	return answer;
}

bool Input::setupForIndependentRun(vector<string> &inputFiles, vector<MoleculeSet*> &seededMoleculeSets, bool &bSetupPreviouslyDone)
{
	string savedInputFileName = m_sInputFileName;
	string savedOutputFileName = m_sOutputFileName;
	string savedResumeFileName = m_sResumeFileName;
	string savedPathToEnergyFiles = m_sPathToEnergyFiles;
	string inputFileDirectory;
	string inputFileWithoutPath;
	string tempName;
	char inputFileName[500];
	char tempFileName[500];
	string::size_type pos;
	int i;
	struct stat fileStatistics;
	bool success = true;
	char commandLine[500];

	int savediLinearSructures = m_iLinearSructures;
	int savediPlanarStructures = m_iPlanarStructures;
	int savedi3DStructures = m_i3DStructures;
	int savedi3DStructuresWithMaxDist = m_i3DStructuresWithMaxDist;
	int savedi3DNonFragStructuresWithMaxDist = m_i3DNonFragStructuresWithMaxDist;
	int savediTotalPopulationSize = m_iTotalPopulationSize;
	int iLinearSructures = m_iLinearSructures;
	int iPlanarStructures = m_iPlanarStructures;
	int i3DStructures = m_i3DStructures;
	int i3DStructuresWithMaxDist = m_i3DStructuresWithMaxDist;
	int i3DNonFragStructuresWithMaxDist = m_i3DNonFragStructuresWithMaxDist;
	vector<MoleculeSet*> moleculeSetVectorOfOne;
	vector<MoleculeSet*> emptyMoleculeSets;
	
	bSetupPreviouslyDone = false;
	try {
		inputFileDirectory = m_sInputFileName;
		pos = inputFileDirectory.find(".inp");
		if (pos != string::npos) // If we found it
			inputFileDirectory.replace(pos, 4, "Runs");
		else
			inputFileDirectory = inputFileDirectory + "Runs";
		if (stat(inputFileDirectory.c_str(), &fileStatistics) != 0) { // Errors occurred in getting stats, the directory doesn't exist
			snprintf(commandLine, sizeof(commandLine), "mkdir %s", inputFileDirectory.c_str());
			success = system(commandLine) != -1;
			if (!success) {
				cout << "Unable to create directory: " << inputFileDirectory << endl;
				throw "";
			}
			bSetupPreviouslyDone = false;
		} else {
			bSetupPreviouslyDone = true;
		}
		
		inputFileWithoutPath = fileWithoutPath(m_sInputFileName);
		pos = inputFileWithoutPath.find(".inp");
		if (pos != string::npos) // If we found it
			inputFileWithoutPath.replace(pos, 4, "");
		
		m_iTotalPopulationSize = 1;
		for (i = 0; i < savediTotalPopulationSize; ++i) {
			m_iLinearSructures = 0;
			m_iPlanarStructures = 0;
			m_i3DStructures = 0;
			m_i3DStructuresWithMaxDist = 0;
			m_i3DNonFragStructuresWithMaxDist = 0;
			if (iLinearSructures > 0) {
				m_iLinearSructures = 1;
				--iLinearSructures;
			} else if (iPlanarStructures > 0) {
				m_iPlanarStructures = 1;
				--iPlanarStructures;
			} else if (i3DStructures > 0) {
				m_i3DStructures = 1;
				--i3DStructures;
			} else if (i3DStructuresWithMaxDist > 0) {
				m_i3DStructuresWithMaxDist = 1;
				--i3DStructuresWithMaxDist;
			} else if (i3DNonFragStructuresWithMaxDist > 0) {
				m_i3DNonFragStructuresWithMaxDist = 1;
				--i3DNonFragStructuresWithMaxDist;
			}
			if (bSetupPreviouslyDone && (seededMoleculeSets.size() > 0)) {
				cout << "You requested seeding, but it appears the run has already been started." << endl;
				cout << "Either turn seeding off or restart the run with seeding (first delete " << inputFileDirectory << " and files in " << m_sPathToEnergyFiles << ")." << endl;
				throw "You requested seeding, but it appears the population has already been seeded.";
			}
			if (bSetupPreviouslyDone || (seededMoleculeSets.size() > 0)) {
				snprintf(inputFileName, sizeof(inputFileName), "%s/%s_%d.res", inputFileDirectory.c_str(), inputFileWithoutPath.c_str(), (i+1));
				if ((seededMoleculeSets.size() == 0) && stat(inputFileName, &fileStatistics) != 0) { // if we aren't seeding and the file doesn't exist
					snprintf(inputFileName, sizeof(inputFileName), "%s/%s_%d.inp", inputFileDirectory.c_str(), inputFileWithoutPath.c_str(), (i+1));
				}
			} else {
				snprintf(inputFileName, sizeof(inputFileName), "%s/%s_%d.inp", inputFileDirectory.c_str(), inputFileWithoutPath.c_str(), (i+1));
			}
			
			if (savedPathToEnergyFiles.length() > 0) {
				snprintf(tempFileName, sizeof(tempFileName), "%s/Run%d", savedPathToEnergyFiles.c_str(), (i+1));
				m_sPathToEnergyFiles = tempFileName;
				if (stat(m_sPathToEnergyFiles.c_str(), &fileStatistics) != 0) { // Errors occurred in getting stats, the file doesn't exist
					snprintf(commandLine, sizeof(commandLine), "mkdir %s", m_sPathToEnergyFiles.c_str());
					success = system(commandLine) != -1;
					if (!success) {
						cout << "Unable to create directory: " << m_sPathToEnergyFiles << endl;
						throw "";
					}
				}
				if (m_pSelectedEnergyProgram->m_sPathToExecutable.length() > 0) {
					m_sNodesFile = m_sPathToEnergyFiles + "/nodes.txt";
					ofstream nodesFile(m_sNodesFile.c_str(), ios::out);
					nodesFile << m_srgNodeNames[i];
					nodesFile.close();
				}
			}

			if (stat(inputFileName, &fileStatistics) != 0) { // Errors occurred in getting stats, the file doesn't exist
				// create the file
				m_sInputFileName = inputFileName;

				snprintf(tempFileName, sizeof(tempFileName), "%s/%s_%d.out", inputFileDirectory.c_str(), inputFileWithoutPath.c_str(), (i+1));
				m_sOutputFileName = tempFileName;

				snprintf(tempFileName, sizeof(tempFileName), "%s/%s_%d.res", inputFileDirectory.c_str(), inputFileWithoutPath.c_str(), (i+1));
				m_sResumeFileName = tempFileName;
				
				if (seededMoleculeSets.size() == 0) {
					ofstream inputFile(m_sInputFileName.c_str(), ios::out);
					if (!inputFile.is_open())
					{
						cout << "Unable to write to the file: " << m_sInputFileName << endl;
						throw "Unable to create a file.";
					}
					printToFile(inputFile);
					inputFile.close();
				} else {
					moleculeSetVectorOfOne.push_back(seededMoleculeSets[i]);
					writeResumeFile(m_sInputFileName, moleculeSetVectorOfOne, emptyMoleculeSets, emptyMoleculeSets, 0, true);
					moleculeSetVectorOfOne.clear();
				}
			}
			
			inputFiles.push_back(inputFileName);
		}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
		success = false;
	}
	
	m_sInputFileName = savedInputFileName;
	m_sOutputFileName = savedOutputFileName;
	m_sResumeFileName = savedResumeFileName;
	m_iLinearSructures = savediLinearSructures;
	m_iPlanarStructures = savediPlanarStructures;
	m_i3DStructures = savedi3DStructures;
	m_i3DStructuresWithMaxDist = savedi3DStructuresWithMaxDist;
	m_i3DNonFragStructuresWithMaxDist = savedi3DNonFragStructuresWithMaxDist;
	m_iTotalPopulationSize = savediTotalPopulationSize;
	m_sPathToEnergyFiles = savedPathToEnergyFiles;
	m_sNodesFile = m_sPathToEnergyFiles + "/nodes.txt";
	
	return success;
}

void Input::sortMoleculeSets(vector<MoleculeSet*> &moleculeSets, int lo, int hi)
{
	int left, right;
	FLOAT median;
	MoleculeSet* temp;
	
	if( hi > lo ) // if at least 2 elements, then
	{
		left=lo; right=hi;
		median= moleculeSets[(lo+hi)/2]->getEnergy();  // just an estimate!
		
		while(right >= left) // partition moleculeSets[lo..hi]
		// moleculeSets[lo..left-1] <= median and moleculeSets[right+1..hi] >= median
		{
			while(moleculeSets[left]->getEnergy() < median)
				left++;
			
			while(moleculeSets[right]->getEnergy() > median)
				right--;
			
			if(left > right)
				break;
			//swap
			temp=moleculeSets[left];
			moleculeSets[left]=moleculeSets[right];
			moleculeSets[right]=temp;
			left++;
			right--;
		}
		sortMoleculeSets(moleculeSets, lo, right);
		sortMoleculeSets(moleculeSets, left,  hi);
	}
}

void Input::saveBestN(vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestN, int n,
                     FLOAT fMinDistnaceBetweenSameMoleculeSets, int iNumEnergyFilesToSave, const char* sLogFilesDirectory)
{
	int i, ii, iii, indexToInsert;
	MoleculeSet* temp;
	vector<MoleculeSet*> sortedMoleculeSets;
	const char* energyFilePrefix = "best";
	
	for (i = 0; i < (int)moleculeSets.size(); ++i)
		sortedMoleculeSets.push_back(moleculeSets[i]);
	
	sortMoleculeSets(sortedMoleculeSets, 0, sortedMoleculeSets.size()-1);
	
	for (i = 0; i < (int)sortedMoleculeSets.size(); ++i)
	{
		// Find if/where to insert sortedMoleculeSets[i] in bestN
		indexToInsert = (int)bestN.size();
		while ((indexToInsert >= 1) && (sortedMoleculeSets[i]->getEnergy() < bestN[indexToInsert-1]->getEnergy()))
			--indexToInsert;
		if ((indexToInsert < (int)bestN.size()) || ((int)bestN.size() < n))
		{
			// Make sure there aren't any stuctures with lower energy that are "the same" as sortedMoleculeSets[i]
			for (ii = indexToInsert-1; ii >= 0; --ii)
				if (bestN[ii]->withinDistance(*sortedMoleculeSets[i],fMinDistnaceBetweenSameMoleculeSets))
				{
					indexToInsert = -1;
					break;
				}
			if (indexToInsert == -1)
				continue; // don't insert 
			
			// Insert
			// First shift the MoleculeSets at indexToInsert to the right
			bestN.push_back(NULL);
			for (ii = (int)bestN.size()-1; ii > indexToInsert; --ii) {
				bestN[ii] = bestN[ii-1];
				
				if ((ii+1) <= iNumEnergyFilesToSave)
					bestN[ii]->moveOrCopyOutputEnergyFiles(ii+1, true);
			}

			// Insert a copy of sortedMoleculeSets[i]
			temp = new MoleculeSet();
			temp->copy(*sortedMoleculeSets[i]);
			bestN[indexToInsert] = temp;
				
			if (indexToInsert < iNumEnergyFilesToSave)
				bestN[indexToInsert]->moveOrCopyOutputEnergyFiles(sLogFilesDirectory, energyFilePrefix, indexToInsert+1, false);
			
			// Remove any structures higher in energy than sortedMoleculeSets[i] that are the same as sortedMoleculeSets[i]
			for (ii = indexToInsert+1; ii < (int)bestN.size(); ++ii)
				while (bestN[ii]->withinDistance(*bestN[indexToInsert], fMinDistnaceBetweenSameMoleculeSets))
				{
					bestN[ii]->deleteOutputEnergyFiles(true);
					delete bestN[ii];
					for (iii = ii; iii < (int)bestN.size()-1; ++iii) {
						bestN[iii] = bestN[iii+1];
				
						if ((iii+1) <= iNumEnergyFilesToSave)
							bestN[iii]->moveOrCopyOutputEnergyFiles(iii+1, true);
					}
					bestN.pop_back();
					if (ii >= (int)bestN.size())
						break;
				}
			
			// Make sure that bestN isn't bigger than n
			// This could happen if bestN == n and then we added one to the list
			if ((int)bestN.size() > n)
			{
				if ((int)bestN.size() <= iNumEnergyFilesToSave)
					bestN[bestN.size()-1]->deleteOutputEnergyFiles(true);
				delete bestN[bestN.size()-1];
				bestN.pop_back();
			}
		}
	}
}

bool Input::compileIndependentRunData(bool printOutput)
{
	string outputFileDirectory;
	string outputFileWithoutPath;
	string::size_type pos;
	int i, j;
	struct stat fileStatistics;
	ofstream fout;
	ifstream fin;
	char outputFileName[500];
	char resumeFileName[500];
	string resumeFileNameString;
	char fileLine[5000];
	int last_iteration;
	string seedFiles;
	Input input;

	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> moleculeSetsTemp;
	vector<MoleculeSet*> bestNMoleculeSets;
	vector<MoleculeSet*> bestNMoleculeSetsTemp;
	vector<MoleculeSet*> bestIndividualMoleculeSets; // The best solution found for each individual (used in particle swarm optimization)
	vector<MoleculeSet*> emptyMoleculeSets; // This is only created here so we have something to pass in for
	                                        // bestIndividualMoleculeSets which is used in PSO, not simulated annealing.

	outputFileDirectory = m_sInputFileName;
	pos = outputFileDirectory.find(".inp");
	if (pos != string::npos) // If we found it
		outputFileDirectory.replace(pos, 4, "Runs");
	else
		outputFileDirectory = outputFileDirectory + "Runs";
	if (stat(outputFileDirectory.c_str(), &fileStatistics) != 0) // Errors occurred in getting stats, the directory doesn't exist
		return false;
		
	outputFileWithoutPath = fileWithoutPath(m_sOutputFileName);
	pos = outputFileWithoutPath.find(".out");
	if (pos != string::npos) // If we found it
		outputFileWithoutPath.replace(pos, 4, "");

	int iterationTemp;
	int transStatesFoundTemp;
	FLOAT bestEnergyTemp;
	char tempString[100];
	FLOAT temperatureTemp;
	int numPertTemp;
	FLOAT coordinatePertTemp;
	FLOAT anglePertTemp;
	FLOAT acceptedPerterbationsTemp;
	
	vector<int> iteration[m_iTotalPopulationSize];
	vector<FLOAT> bestEnergies[m_iTotalPopulationSize];
	vector<int> transStatesFound[m_iTotalPopulationSize];
	vector<FLOAT> temperatures[m_iTotalPopulationSize];
	vector<int> numPert[m_iTotalPopulationSize];
	vector<FLOAT> coordinatePert[m_iTotalPopulationSize];
	vector<FLOAT> anglePert[m_iTotalPopulationSize];
	vector<FLOAT> acceptedPerterbations[m_iTotalPopulationSize];
	
	// Read the individual output files
	last_iteration = 1073741824; // a big number
	for (i = 0; i < m_iTotalPopulationSize; ++i) {
		snprintf(outputFileName, sizeof(outputFileName), "%s/%s_%d.out", outputFileDirectory.c_str(), outputFileWithoutPath.c_str(), (i+1));
		if (printOutput)
			cout << "Reading: " << outputFileName << endl;
		fin.open(outputFileName, ifstream::in);
		if (!fin.is_open())
		{
			cout << "Unable to read the output file: " << outputFileName << endl;
			return false;
		}
//		bestEnergies.push_back(vector<FLOAT>);
		while (fin.getline(fileLine, sizeof(fileLine)))
		{
			if (strncmp(fileLine, "Using seeded population from", 28) == 0) {
				seedFiles = (fileLine+29);
				seedFiles.replace(seedFiles.length()-3,3,""); // remove the "..."
			}
			if (sscanf(fileLine, "Assigning frozen status to the coordinates of seeded atoms for %d iterations...", &m_iFreezeUntilIteration) == 1) {
			}

			if (m_bTransitionStateSearch) {
				if (sscanf(fileLine, "It: %d Number of Transition States Found: %d Best Energy: %s",
				           &iterationTemp, &transStatesFoundTemp, tempString) == 3) {
					iteration[i].push_back(iterationTemp);
					bestEnergies[i].push_back(bestEnergyTemp);
					transStatesFound[i].push_back(transStatesFoundTemp);
					if (transStatesFoundTemp > 0)
						bestEnergies[i].push_back(atof(tempString));
					else
						bestEnergies[i].push_back(0);
				}
			} else {
				if (sscanf(fileLine, "It: %d Best Energy: %lf Temp: %lf Num Pert: %d Coord, Angle Pert: %lf, %lf Accepted Pert: %lf%%",
				           &iterationTemp, &bestEnergyTemp, &temperatureTemp, &numPertTemp, &coordinatePertTemp, &anglePertTemp, &acceptedPerterbationsTemp) == 7) {
					iteration[i].push_back(iterationTemp);
					bestEnergies[i].push_back(bestEnergyTemp);
					temperatures[i].push_back(temperatureTemp);
					numPert[i].push_back(numPertTemp);
					coordinatePert[i].push_back(coordinatePertTemp);
					anglePert[i].push_back(anglePertTemp);
					acceptedPerterbations[i].push_back(acceptedPerterbationsTemp);
				}
			}
		}
		fin.close();
		if (last_iteration > (int)bestEnergies[i].size())
			last_iteration = bestEnergies[i].size();
	}
	
	if (printOutput)
		cout << "Writing: " << m_sOutputFileName << endl;
	fout.open(m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
	if (!fout.is_open())
	{
		cout << "Unable to open the output file: " << m_sOutputFileName << endl;
		return false;
	}
	printToFile(fout);
	fout << endl << endl;
	fout << "Output from program:" << endl;
	
	if (seedFiles.length() > 0) {
		fout << "Using seeded population from " << seedFiles << "..." << endl;
		if (m_iFreezeUntilIteration > 0) {
			fout << "Assigning frozen status to the coordinates of seeded atoms for " << m_iFreezeUntilIteration << " iterations..." << endl;
		}
	} else {
		fout << "Initializing the population..." << endl;
	}
	if (m_bPerformBasinHopping && (m_fQuenchingFactor == 1.0)) {
		fout << "Performing Basin Hopping..." << endl;
	} else if (m_bTransitionStateSearch) {
		fout << "Searching for Transition States..." << endl;
	} else {
		fout << "Simulating Annealing..." << endl;
	}
	bool foundFreezIteration = false;
	for (i = 0; i < last_iteration; ++i) {
		iterationTemp = 0;
		for (j = 0; j < m_iTotalPopulationSize; ++j)
			iterationTemp += iteration[j][i];
		iterationTemp /= m_iTotalPopulationSize;
		fout << "It: " << iterationTemp;

		if (m_bTransitionStateSearch) {
			transStatesFoundTemp = 0;
			for (j = 0; j < m_iTotalPopulationSize; ++j)
				transStatesFoundTemp += transStatesFound[j][i];
			fout << " Number of Transition States Found: " << transStatesFoundTemp;
		}
		fout << setiosflags(ios::fixed) << setprecision(8);
		if (!m_bTransitionStateSearch || (transStatesFoundTemp > 0)) {
			bestEnergyTemp = 0;
			for (j = 0; j < m_iTotalPopulationSize; ++j)
				if (bestEnergyTemp > bestEnergies[j][i])
					bestEnergyTemp = bestEnergies[j][i];
			fout << " Best Energy: " << bestEnergyTemp;
		} else
			fout << " Best Energy: -";
		if (!m_bTransitionStateSearch) {
			temperatureTemp = 0;
			for (j = 0; j < m_iTotalPopulationSize; ++j)
				temperatureTemp += temperatures[j][i];
			temperatureTemp /= m_iTotalPopulationSize;
			fout << setiosflags(ios::fixed) << setprecision(1)
				<< " Temp: " << temperatureTemp;

			numPertTemp = 0;
			for (j = 0; j < m_iTotalPopulationSize; ++j)
				numPertTemp += numPert[j][i];
			numPertTemp /= m_iTotalPopulationSize;
			fout << " Num Pert: " << numPertTemp;
			
			coordinatePertTemp = 0;
			for (j = 0; j < m_iTotalPopulationSize; ++j)
				coordinatePertTemp += coordinatePert[j][i];
			coordinatePertTemp /= m_iTotalPopulationSize;
			anglePertTemp = 0;
			for (j = 0; j < m_iTotalPopulationSize; ++j)
				anglePertTemp += anglePert[j][i];
			anglePertTemp /= m_iTotalPopulationSize;
			
			fout << setiosflags(ios::fixed) << setprecision(4)
				<< " Coord, Angle Pert: " << coordinatePertTemp << ", "
				<< anglePertTemp;
			
			acceptedPerterbationsTemp = 0;
			for (j = 0; j < m_iTotalPopulationSize; ++j)
				acceptedPerterbationsTemp += acceptedPerterbations[j][i];
			acceptedPerterbationsTemp /= m_iTotalPopulationSize;
			fout << setiosflags(ios::fixed) << setprecision(1)
				<< " Accepted Pert: " << acceptedPerterbationsTemp << "%";
		}
		fout << endl;
		if ((m_iFreezeUntilIteration > 0) && (iterationTemp >= m_iFreezeUntilIteration) && !foundFreezIteration) {
			foundFreezIteration = true;
			fout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
		}
	}
	fout.close();

	if (m_sSaveLogFilesInDirectory.length() > 0) {
		char commandLine[500];
		if (stat(m_sSaveLogFilesInDirectory.c_str(), &fileStatistics) == 0) { // If no errors occurred in getting stats, the file exists
			if (printOutput)
				cout << "Deleting directory: " << m_sSaveLogFilesInDirectory << endl;
			snprintf(commandLine, sizeof(commandLine), "rm -rf %s", m_sSaveLogFilesInDirectory.c_str());
			if (system(commandLine) != 0)
				return false;
		}
		snprintf(commandLine, sizeof(commandLine), "mkdir %s", m_sSaveLogFilesInDirectory.c_str());
		if (printOutput)
			cout << "Creating directory: " << m_sSaveLogFilesInDirectory << endl;
		if (system(commandLine) != 0)
			return false;
	}

	// Open the resume files
	bool success = true;
	for (i = 0; i < m_iTotalPopulationSize; ++i) {
		snprintf(resumeFileName, sizeof(resumeFileName), "%s/%s_%d.res", outputFileDirectory.c_str(), outputFileWithoutPath.c_str(), (i+1));
		resumeFileNameString = resumeFileName;
		if (printOutput)
			cout << "Reading: " << resumeFileName << endl;
		if (!input.open(resumeFileNameString, false, false, moleculeSetsTemp, bestNMoleculeSetsTemp, bestIndividualMoleculeSets)) {
			success = false;
			break;
		}
		moleculeSets.push_back(moleculeSetsTemp[0]);
		moleculeSetsTemp.pop_back();
		saveBestN(bestNMoleculeSetsTemp, bestNMoleculeSets, m_iNumberOfBestStructuresToSave,
				m_fMinDistnaceBetweenSameMoleculeSets, m_iNumberOfLogFilesToSave, m_sSaveLogFilesInDirectory.c_str());
		for (j = 0; j < (int)moleculeSetsTemp.size(); ++j)
			delete moleculeSetsTemp[j];
		moleculeSetsTemp.clear();
		for (j = 0; j < (int)bestNMoleculeSetsTemp.size(); ++j)
			delete bestNMoleculeSetsTemp[j];
		bestNMoleculeSetsTemp.clear();
		for (j = 0; j < (int)bestIndividualMoleculeSets.size(); ++j)
			delete bestIndividualMoleculeSets[j];
		bestIndividualMoleculeSets.clear();
	}
	for (j = 0; j < (int)moleculeSetsTemp.size(); ++j)
		delete moleculeSetsTemp[j];
	moleculeSetsTemp.clear();
	for (i = 0; i < (int)bestNMoleculeSetsTemp.size(); ++i)
		delete bestNMoleculeSetsTemp[i];
	bestNMoleculeSetsTemp.clear();
	for (i = 0; i < (int)bestIndividualMoleculeSets.size(); ++i)
		delete bestIndividualMoleculeSets[i];
	bestIndividualMoleculeSets.clear();
	if (!success)
		return false;
	
	// Set the temperature to be negative, as a flag so we know this resume file can't be resumed (it's only there so we can create an optimization file from the resume file).	
	m_fDesiredAcceptedTransitions = -m_fDesiredAcceptedTransitions;	
	m_fStartingTemperature = -m_fStartingTemperature;	
	if (printOutput)
		cout << "Writing: " << m_sResumeFileName << " (contains the list of best structures, but is not resumable)" << endl;
	writeResumeFile(m_sResumeFileName, moleculeSets, bestNMoleculeSets, emptyMoleculeSets, 0, true);
	m_fStartingTemperature = -m_fStartingTemperature;	
	m_fDesiredAcceptedTransitions = -m_fDesiredAcceptedTransitions;	
	
	for (i = 0; i < (int)moleculeSets.size(); ++i)
		delete moleculeSets[i];
	moleculeSets.clear();
	for (i = 0; i < (int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();
	
	return true;
}

