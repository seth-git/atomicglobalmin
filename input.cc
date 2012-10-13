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
	m_sOutputFileName = "output.out";
	m_sResumeFileName = "resume.res";
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
	m_bPerformNonFragSearch = false;
	m_bPerformBasinHopping = false;
	m_iIteration = 0;
	m_bDecreasingTemp = false;
	m_tElapsedSeconds = 0;
	m_iNumEnergyEvaluations = 0;
	m_iFreezeUntilIteration = 0;
	m_iAcceptedTransitionsIndex = -1;

	
	// Varables used in PSO
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

	// Variables used in optimization files
	m_iStructuresToOptimizeAtATime = 10;
	
	m_bRunComplete = false;
	
	m_messages = NULL;
	m_messagesDL = NULL;
	
	m_bTestMode = false;
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;
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
bool Input::getStringParam(const char *fileLine, const string& parameterNameString, string &stringParam)
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
bool Input::getIntParam(const char *fileLine, const string& parameterNameString, int &myInt)
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
bool Input::getFloatParam(const char *fileLine, const string& parameterNameString, FLOAT &myFloat)
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
				cout << m_messagesDL->m_sUnidentifiedElementSymbol << ": " << myString << endl;
				return false;
			}
		} else if (atomicNumber > MAX_ATOMIC_NUMBERS) {
			cout << "No there is no information available for the element with atomic number " << atomicNumber << "." << endl;
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
bool Input::getYesNoParam(const char *fileLine, const string& parameterNameString, bool &yesNoParam)
{
	string tempStr;
	if (!getStringParam(fileLine, parameterNameString, tempStr))
		return false;

	if (m_messages->m_sYes == tempStr)
		yesNoParam = true;
	else if (m_messages->m_sNo == tempStr)
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
	return m_messages->getYesNoParam(yesNoParam);
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
	string language = "Language";
	
	m_sInputFileName = "";
	m_messagesDL = Strings::instance();
	m_messages = m_messagesDL;
	
	// Read in each line of the file
	lineNumber = 1;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	if (getStringParam(fileLine, language, m_sLanguageCode)) {
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
	} else {
		m_sLanguageCode = "en";
	}
	m_messages = Strings::instance(m_sLanguageCode);

	if (strncmp(m_messages->m_sSimInputVersionLine.c_str(), fileLine, m_messages->m_sSimInputVersionLine.length()) == 0) {
		m_iAlgorithmToDo = SIMULATED_ANNEALING;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_messages->m_sSimResumeVersionLine.c_str(), fileLine, m_messages->m_sSimResumeVersionLine.length()) == 0) {
		m_iAlgorithmToDo = SIMULATED_ANNEALING;
		m_bResumeFileRead = true;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_messages->m_sSimOptimizationVersionLine.c_str(), fileLine, m_messages->m_sSimOptimizationVersionLine.length()) == 0) {
		m_iAlgorithmToDo = SIMULATED_ANNEALING;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = true;
	} else if (strncmp(m_messages->m_sPSOInputVersionLine.c_str(), fileLine, m_messages->m_sPSOInputVersionLine.length()) == 0) {
		m_iAlgorithmToDo = PARTICLE_SWARM_OPTIMIZATION;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_messages->m_sPSOResumeVersionLine.c_str(), fileLine, m_messages->m_sPSOResumeVersionLine.length()) == 0) {
		m_iAlgorithmToDo = PARTICLE_SWARM_OPTIMIZATION;
		m_bResumeFileRead = true;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_messages->m_sPSOOptimizationVersionLine.c_str(), fileLine, m_messages->m_sPSOOptimizationVersionLine.length()) == 0) {
		m_iAlgorithmToDo = PARTICLE_SWARM_OPTIMIZATION;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = true;
	} else if (strncmp(m_messages->m_sGAInputVersionLine.c_str(), fileLine, m_messages->m_sGAInputVersionLine.length()) == 0) {
		m_iAlgorithmToDo = GENETIC_ALGORITHM;
		m_bResumeFileRead = false;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_messages->m_sGAResumeVersionLine.c_str(), fileLine, m_messages->m_sGAResumeVersionLine.length()) == 0) {
		m_iAlgorithmToDo = GENETIC_ALGORITHM;
		m_bResumeFileRead = true;
		m_bOptimizationFileRead = false;
	} else if (strncmp(m_messages->m_sGAOptimizationVersionLine.c_str(), fileLine, m_messages->m_sGAOptimizationVersionLine.length()) == 0) {
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
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		printf(m_messagesDL->m_sLineShouldBeBlank.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_messages->m_sEnergyFunction, temp))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sEnergyFunction.c_str(), lineNumber);
		cout << endl;
		return false;
	}
    
 	m_pSelectedEnergyProgram = NULL;	
	for (i = 0; i < (int)EnergyProgram::s_energyPrograms.size(); ++i)
		if (temp == EnergyProgram::s_energyPrograms[i]->getName(m_messages))
			m_pSelectedEnergyProgram = EnergyProgram::s_energyPrograms[i];
	
	if (m_pSelectedEnergyProgram == NULL) {
		printf(m_messagesDL->m_snEnergyProgramNotRecognized.c_str(), lineNumber, temp.c_str());
		printf(m_messagesDL->m_sValidEnergyPrograms.c_str());
		cout << " ";
		for (i = 0; i < (int)EnergyProgram::s_energyPrograms.size(); ++i) {
			if (i >= 1)
				cout << ", ";
			cout << EnergyProgram::s_energyPrograms[i]->getName(m_messages);
		}
		cout << "." << endl;
		
		return false;
	}

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_messages->m_sPathToEnergyFiles, m_sPathToEnergyFiles))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sPathToEnergyFiles.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	checkDirectoryOrFileName(m_sPathToEnergyFiles);
	
	if ((m_pSelectedEnergyProgram->m_iNumOutputFileTypes == 0) || (m_sPathToEnergyFiles.length() == 0))
		m_sSaveLogFilesInDirectory = "";
	else
		m_sSaveLogFilesInDirectory = m_sPathToEnergyFiles + "/" + m_messages->m_sBestSavedStructures;
	
	m_sNodesFile = m_sPathToEnergyFiles + "/" + m_messages->m_sNodesFile;
	if (bReadNodesFile && (m_pSelectedEnergyProgram->m_sPathToExecutable.length() > 0))
		if (!readNodesFile())
			return false;

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_messages->m_sPathToScratch, m_sPathToScratch))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sPathToScratch.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	checkDirectoryOrFileName(m_sPathToScratch);
   	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getFloatParam(fileLine, m_messages->m_sBoxLength, m_boxDimensions.x))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sBoxLength.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	m_boxDimensions.y = m_boxDimensions.x;
	m_boxDimensions.z = m_boxDimensions.x;
	m_boxDimensions.w = 0;
 	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sLinearSructures, m_iLinearSructures))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sLinearSructures.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getFloatParam(fileLine, m_messages->m_sLinearBoxHeight, m_fLinearBoxHeight))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sLinearBoxHeight.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sPlanarStructures, m_iPlanarStructures))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sPlanarStructures.c_str(), lineNumber);
		cout << endl;
		return false;
	}

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_s3DStructures, m_i3DStructures))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_s3DStructures.c_str(), lineNumber);
		cout << endl;
		return false;
	}
    
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_s3DStructuresWithMaxDist, m_i3DStructuresWithMaxDist))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_s3DStructuresWithMaxDist.c_str(), lineNumber);
		cout << endl;
		return false;
	}
    
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_s3DNonFragStructuresWithMaxDist, m_i3DNonFragStructuresWithMaxDist))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_s3DNonFragStructuresWithMaxDist.c_str(), lineNumber);
		cout << endl;
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
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getFloatParam(fileLine, m_messages->m_sMinGeneralAtomDistance, m_fGeneralMinAtomDistance))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMinGeneralAtomDistance.c_str(), lineNumber);
		cout << endl;
		return false;
	}

	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (strncmp(fileLine,m_messages->m_sMinAtomDistance.c_str(),m_messages->m_sMinAtomDistance.length()) != 0)
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMinAtomDistance.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	if (setMinDistances)
		Atom::initMinAtomicDistances(m_fGeneralMinAtomDistance);
	while (true)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (strncmp(fileLine,m_messages->m_sMaxAtomDistance.c_str(),m_messages->m_sMaxAtomDistance.length()) == 0)
			break;
		sscanf(fileLine, "%d %d %lf", &i, &j, &tempFloat);
		if (setMinDistances)
			Atom::setMinAtomicDistance(i,j,tempFloat);
	}
	
	if (!getFloatParam(fileLine, m_messages->m_sMaxAtomDistance, m_fMaxAtomDistance))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMaxAtomDistance.c_str(), lineNumber);
		cout << endl;
		return false;
	}

	// Read the next line which should be blank
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		printf(m_messagesDL->m_sLineShouldBeBlank.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_messages->m_sSimulatedAnnealingParameters, temp))
		{
			cout << "Error reading '" << m_messages->m_sSimulatedAnnealingParameters << ":' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}

		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_messages->m_sPerformNonFragSearch, m_bPerformNonFragSearch))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sPerformNonFragSearch.c_str(), lineNumber);
			cout << endl;
			return false;
		}
	
		if (m_bPerformNonFragSearch)
			if ((m_iLinearSructures > 0) || (m_iPlanarStructures > 0) ||
			    (m_i3DStructures > 0) || (m_i3DStructuresWithMaxDist > 0) ||
			    (m_i3DNonFragStructuresWithMaxDist == 0)) {
				cout << m_messagesDL->m_sPopulationMustBeNonFragmented << endl;
				return false;
			}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_messages->m_sPerformBasinHopping, m_bPerformBasinHopping))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sPerformBasinHopping.c_str(), lineNumber);
			cout << endl;
			return false;
		}

		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_messages->m_sTransitionStateSearch, m_bTransitionStateSearch))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sTransitionStateSearch.c_str(), lineNumber);
			cout << endl;
			return false;
		}
	
		if (m_bPerformBasinHopping && m_bTransitionStateSearch) {
			cout << "You may not perform basin hopping and a transition state search at the same time." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_messages->m_sStartingTemperature, temp))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sStartingTemperature.c_str(), lineNumber);
			cout << endl;
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
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sBoltzmanConstant, m_fBoltzmanConstant))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sBoltzmanConstant.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (version == "1.0")
			m_fBoltzmanConstant *= HARTREE_TO_JOULES;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sNumIterationsBeforeDecreasingTemp, m_iNumIterationsBeforeDecreasingTemp))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sNumIterationsBeforeDecreasingTemp.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (m_iNumIterationsBeforeDecreasingTemp < 1)
		{
			cout << "The parameter '" << m_messages->m_sNumIterationsBeforeDecreasingTemp << "' on line "
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
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sAcceptanceRatio, m_fAcceptanceRatio))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sAcceptanceRatio.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		m_fAcceptanceRatio /= 100;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sQuenchingFactor, m_fQuenchingFactor))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sQuenchingFactor.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sMinTemperatureToStop, m_fMinTemperatureToStop))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMinTemperatureToStop.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sMinAcceptedTransitions, m_fMinAcceptedTransitions))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMinAcceptedTransitions.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		m_fMinAcceptedTransitions /= 100;
	
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sMaxIterations, m_iMaxIterations))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMaxIterations.c_str(), lineNumber);
			cout << endl;
			return false;
		}
	
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sNumPerterbations, m_fNumPerterbations))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sNumPerterbations.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!m_bResumeFileRead && !m_bOptimizationFileRead)
			m_fNumPerterbations += 0.5;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sStartCoordinatePerturbation, m_fStartCoordinatePerturbation))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sStartCoordinatePerturbation.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sMinCoordinatePerturbation, m_fMinCoordinatePerturbation))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMinCoordinatePerturbation.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sStartAnglePerturbation, m_fStartAnglePerturbation))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sStartAnglePerturbation.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		m_fStartAnglePerturbation *= DEG_TO_RAD;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sMinAnglePerturbation, m_fMinAnglePerturbation))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMinAnglePerturbation.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		m_fMinAnglePerturbation *= DEG_TO_RAD;
	} else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) {
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_messages->m_sParticleSwarmParameters, temp))
		{
			cout << "Error reading '" << m_messages->m_sParticleSwarmParameters << ":' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}

		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sStartCoordInertia, m_fStartCoordInertia))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sStartCoordInertia.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sEndCoordInertia, m_fEndCoordInertia))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sEndCoordInertia.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sReachEndInertiaAtIteration, m_iReachEndInertiaAtIteration))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sReachEndInertiaAtIteration.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sCoordIndividualMinimumAttraction, m_fCoordIndividualMinimumAttraction))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sCoordIndividualMinimumAttraction.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sCoordPopulationMinimumAttraction, m_fCoordPopulationMinimumAttraction))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sCoordPopulationMinimumAttraction.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sCoordMaximumVelocity, m_fCoordMaximumVelocity))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sCoordMaximumVelocity.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sStartAngleInertia, m_fStartAngleInertia))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sStartAngleInertia.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sEndAngleInertia, m_fEndAngleInertia))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sEndAngleInertia.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		if ((m_fEndCoordInertia != 0) || (m_fEndAngleInertia != 0) || (m_iReachEndInertiaAtIteration != 0))
			if ((m_fEndCoordInertia == 0) || (m_fEndAngleInertia == 0) || (m_iReachEndInertiaAtIteration == 0)) {
				cout << m_messagesDL->m_sPleaseSpecifyAllOrNoneOfThese << endl;
				cout << "\t" << m_messagesDL->m_sParameter << ": " << m_messages->m_sEndCoordInertia << endl;
				cout << "\t" << m_messagesDL->m_sParameter << ": " << m_messages->m_sReachEndInertiaAtIteration << endl;
				cout << "\t" << m_messagesDL->m_sParameter << ": " << m_messages->m_sEndAngleInertia << endl;
				return false;
			}
				
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sAngleIndividualMinimumAttraction, m_fAngleIndividualMinimumAttraction))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sAngleIndividualMinimumAttraction.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		m_fAngleIndividualMinimumAttraction *= DEG_TO_RAD;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sAnglePopulationMinimumAttraction, m_fAnglePopulationMinimumAttraction))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sAnglePopulationMinimumAttraction.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		m_fAnglePopulationMinimumAttraction *= DEG_TO_RAD;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sAngleMaximumVelocity, m_fAngleMaximumVelocity))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sAngleMaximumVelocity.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		m_fAngleMaximumVelocity *= DEG_TO_RAD;
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_messages->m_sStartVisibilityDistance, temp))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sStartVisibilityDistance.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		m_bStartingVisibilityAuto = (temp == m_messages->m_sAuto);
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
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sVisibilityDistanceIncrease, m_fVisibilityDistanceIncrease))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sVisibilityDistanceIncrease.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sSwitchToRepulsionWhenDiversityIs, m_fSwitchToRepulsionWhenDiversityIs))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sSwitchToRepulsionWhenDiversityIs.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sSwitchToRepulsionWhenNoProgress, m_iSwitchToRepulsionWhenNoProgress))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sSwitchToRepulsionWhenNoProgress.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sSwitchToAttractionWhenDiversityIs, m_fSwitchToAttractionWhenDiversityIs))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sSwitchToAttractionWhenDiversityIs.c_str(), lineNumber);
			cout << endl;
			return false;
		}

		if (!(((m_fSwitchToRepulsionWhenDiversityIs > 0) && (m_iSwitchToRepulsionWhenNoProgress > 0) &&
		       (m_fSwitchToAttractionWhenDiversityIs > 0)) ||
		      ((m_fSwitchToRepulsionWhenDiversityIs <= 0) && (m_iSwitchToRepulsionWhenNoProgress <= 0) &&
		       (m_fSwitchToAttractionWhenDiversityIs <= 0)))) {
			cout << m_messagesDL->m_sPleaseSpecifyAllOrNoneOfThese << endl;
			cout << "\t" << m_messagesDL->m_sParameter << ": " << m_messages->m_sSwitchToRepulsionWhenDiversityIs << endl;
			cout << "\t" << m_messagesDL->m_sParameter << ": " << m_messages->m_sSwitchToRepulsionWhenNoProgress << endl;
			cout << "\t" << m_messagesDL->m_sParameter << ": " << m_messages->m_sSwitchToAttractionWhenDiversityIs << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getFloatParam(fileLine, m_messages->m_sIndividualBestUpdateDist, m_fIndividualBestUpdateDist))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sIndividualBestUpdateDist.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sMaxIterations, m_iMaxIterations))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMaxIterations.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_messages->m_sEnforceMinDistOnCopy, m_bEnforceMinDistOnCopy))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sEnforceMinDistOnCopy.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_messages->m_sUseLocalOptimization, m_bUseLocalOptimization))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sUseLocalOptimization.c_str(), lineNumber);
			cout << endl;
			return false;
		}
	} else if (m_iAlgorithmToDo == GENETIC_ALGORITHM) {
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_messages->m_sGeneticAlgorithmParameters, temp))
		{
			cout << "Error reading '" << m_messages->m_sGeneticAlgorithmParameters << ":' on line "
			     << lineNumber << " in the input file." << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getYesNoParam(fileLine, m_messages->m_sUseLocalOptimization, m_bUseLocalOptimization))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sUseLocalOptimization.c_str(), lineNumber);
			cout << endl;
			return false;
		}
	
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sMaxIterations, m_iMaxIterations))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMaxIterations.c_str(), lineNumber);
			cout << endl;
			return false;
		}
	}
	
	// Read the next line which should be blank
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		printf(m_messagesDL->m_sLineShouldBeBlank.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sNumberOfBestStructuresToSave, m_iNumberOfBestStructuresToSave))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sNumberOfBestStructuresToSave.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getFloatParam(fileLine, m_messages->m_sMinDistnaceBetweenSameMoleculeSets, m_fMinDistnaceBetweenSameMoleculeSets))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMinDistnaceBetweenSameMoleculeSets.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sNumberOfLogFilesToSave, m_iNumberOfLogFilesToSave))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sNumberOfLogFilesToSave.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (m_pSelectedEnergyProgram->m_iNumOutputFileTypes == 0)
		m_iNumberOfLogFilesToSave = 0;
	if (m_iNumberOfLogFilesToSave > m_iNumberOfBestStructuresToSave)
		m_iNumberOfLogFilesToSave = m_iNumberOfBestStructuresToSave;
	
	if (m_fSwitchToRepulsionWhenDiversityIs > 0) {
		if (m_fSwitchToRepulsionWhenDiversityIs > m_fSwitchToAttractionWhenDiversityIs) {
			printf(m_messagesDL->m_sRepulsionAttractionError1.c_str(), m_messages->m_sSwitchToRepulsionWhenDiversityIs.c_str());
			cout << endl;
			printf(m_messagesDL->m_sRepulsionAttractionError2.c_str(), m_messages->m_sSwitchToAttractionWhenDiversityIs.c_str());
			cout << endl;
			return false;
		}
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_messages->m_sOutputFileName, m_sOutputFileName))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sOutputFileName.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_messages->m_sResumeFileName, m_sResumeFileName))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sResumeFileName.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sResumeFileNumIterations, m_iResumeFileNumIterations))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sResumeFileNumIterations.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (m_iResumeFileNumIterations <= 0)
		m_iResumeFileNumIterations = 1;
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sPrintSummaryInfoEveryNIterations, m_iPrintSummaryInfoEveryNIterations))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sPrintSummaryInfoEveryNIterations.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sCharge, m_iCharge))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sCharge.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sMultiplicity, m_iMultiplicity))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sMultiplicity.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_messages->m_sEnergyFileHeader, temp))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sEnergyFileHeader.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	m_sEnergyFileHeader = "";
	while (true)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		length = strlen(fileLine);
		if (length == 0)
			break;
		if ((m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN) && (length > 80)) {
			cout << m_messagesDL->m_sGaussianHeaderLineLongerThan80 << endl
			     << fileLine << endl;
			return false;
		}
		m_sEnergyFileHeader.append(fileLine);
		m_sEnergyFileHeader.append("\n");
	}
	
	++lineNumber;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getStringParam(fileLine, m_messages->m_sEnergyFileFooter, temp))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sEnergyFileFooter.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	m_sEnergyFileFooter = "";
	while (true)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		length = strlen(fileLine);
		if (length == 0)
			break;
		if ((m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN) && (length > 80)) {
			cout << m_messagesDL->m_sGaussianFooterLineLongerThan80 << endl
			     << fileLine << endl;
			return false;
		}
		m_sEnergyFileFooter.append(fileLine);
		m_sEnergyFileFooter.append("\n");
	}
	if (m_bUseLocalOptimization && (m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN)) {
		if (m_sEnergyFileHeader.find("opt") == string::npos) {
			cout << m_messagesDL->m_sSpecifyOptForGaussianOptimization << endl;
			return false;
		}
	}
	if (m_bPerformBasinHopping && (m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN)) {
		if (m_sEnergyFileHeader.find("opt") == string::npos) {
			cout << m_messagesDL->m_sSpecifyOptWithBasinHopping << endl;
			return false;
		}
	}
	if ((m_pSelectedEnergyProgram->m_iProgramID == GAUSSIAN) && (m_sEnergyFileFooter.length() > 0)) {
		if (m_sEnergyFileFooter.find("--link1--\n") != 0) {
			cout << m_messagesDL->m_sUseLink1InGaussianFooter << endl;
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
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sNumStructureTypes, m_iNumStructureTypes))
	{
		printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sNumStructureTypes.c_str(), lineNumber);
		cout << endl;
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
		printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		printf(m_messagesDL->m_sLineShouldBeBlank.c_str(), lineNumber);
		cout << endl;
		return false;
	}
	
	for (i = 0; i < m_iNumStructureTypes; ++i)
	{
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sNumStructuresOfEachType, m_iNumStructuresOfEachType[i]))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sNumStructuresOfEachType.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		++lineNumber;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			printf(m_messagesDL->m_sLineMissingInInputFile.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		if (!getStringParam(fileLine, m_messages->m_sStructureFormatOfThisType, m_sStructureFormats[i]))
		{
			printf(m_messagesDL->m_sErrorReadingParameter.c_str(), m_messages->m_sStructureFormatOfThisType.c_str(), lineNumber);
			cout << endl;
			return false;
		}
		
		if (m_sStructureFormats[i] == m_messages->m_sCartesian)
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
			cout << "Unknown input for parameter '" << m_messages->m_sStructureFormatOfThisType
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
			cout << "Please enter a number between 1 and " << m_tempelateMoleculeSet.getNumberOfMolecules() << " for the parameter '" << m_messages->m_sNumPerterbations << "' in the input file." << endl;
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
		printf(m_messagesDL->m_sUnableToReadFile.c_str(), m_sNodesFile.c_str());
		cout << endl;
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
		cout << m_messagesDL->m_sPleaseSpecifyOneNode << endl;
		return false;
	} else
		return true;
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
// Purpose: This function saves the input file.
// Parameters: moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets - the associated sets of chemical structures
// Returns: true if the file was saved successfully and false otherwise
bool Input::save(vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets, vector<MoleculeSet*> &bestIndividualMoleculeSets)
{
	string command;
	string* fileName;
	string tempFileName;
	
	if (!hasBeenInitialized())
		return false;
	if (!m_bResumeFileRead && !m_bOptimizationFileRead) {
		cout << m_messagesDL->m_sWritingFile << ": " << m_sInputFileName << endl;
		ofstream fout(m_sInputFileName.c_str(), ios::out);
		if (!fout.is_open()) {
			printf(m_messagesDL->m_sUnableToWriteTemporaryResume.c_str(), m_sInputFileName.c_str());
			cout << endl;
			return false;
		}
		printToFile(fout);
		fout.close();
		return true;
	} else if (m_bResumeFileRead) {
		fileName = &m_sResumeFileName;
	} else {
		fileName = &m_sInputFileName;
	}
	cout << m_messagesDL->m_sWritingFile << ": " << *fileName << endl;
	tempFileName.append(*fileName).append(".temp");
	writeResumeFile(tempFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets,
			m_tElapsedSeconds, m_bResumeFileRead);
	command.append("mv ").append(tempFileName).append(" ").append(*fileName);
	if (system(command.c_str())) {
		cout << m_messagesDL->m_sErrorUpdatingOptimization << ": " << command << endl;
		return false;
	}
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
	
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;
	m_messages = Strings::instance(m_sLanguageCode);
	m_messagesDL = m_messages;

	if (m_sLanguageCode != "en") {
		outFile << "Language: " << m_sLanguageCode << endl;
	}
	if (m_bResumeFileRead) {
		outFile << setiosflags(ios::fixed) << setprecision(resumeFilePrecision);
		if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
			outFile << m_messages->m_sSimResumeVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			outFile << m_messages->m_sPSOResumeVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == GENETIC_ALGORITHM)
			outFile << m_messages->m_sGAResumeVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
	} else if (m_bOptimizationFileRead) {
		outFile << setiosflags(ios::fixed) << setprecision(resumeFilePrecision);
		if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
			outFile << m_messages->m_sSimOptimizationVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			outFile << m_messages->m_sPSOOptimizationVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == GENETIC_ALGORITHM)
			outFile << m_messages->m_sGAOptimizationVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
	} else {
		outFile << setiosflags(ios::fixed) << setprecision(defaultPrecision);
		if (m_iAlgorithmToDo == SIMULATED_ANNEALING)
			outFile << m_messages->m_sSimInputVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			outFile << m_messages->m_sPSOInputVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
		else if (m_iAlgorithmToDo == GENETIC_ALGORITHM)
			outFile << m_messages->m_sGAInputVersionLine << " " << INPUT_FILE_VERSION << endl << endl;
	}
	
	outFile << m_messages->m_sEnergyFunction << ": " << m_pSelectedEnergyProgram->getName(m_messages) << endl;
	outFile << m_messages->m_sPathToEnergyFiles << ": " << m_sPathToEnergyFiles << endl;
	outFile << m_messages->m_sPathToScratch << ": " << m_sPathToScratch << endl;
	
	outFile << m_messages->m_sBoxLength << ": " << Atom::printFloat(m_boxDimensions.x) << endl;
	outFile << m_messages->m_sLinearSructures << ": " << m_iLinearSructures << endl;
	outFile << m_messages->m_sLinearBoxHeight << ": " << Atom::printFloat(m_fLinearBoxHeight) << endl;
	outFile << m_messages->m_sPlanarStructures << ": " << m_iPlanarStructures << endl;
	outFile << m_messages->m_s3DStructures << ": " << m_i3DStructures << endl;
	outFile << m_messages->m_s3DStructuresWithMaxDist << ": " << m_i3DStructuresWithMaxDist << endl;
	outFile << m_messages->m_s3DNonFragStructuresWithMaxDist << ": " << m_i3DNonFragStructuresWithMaxDist << endl;
	outFile << m_messages->m_sMinGeneralAtomDistance << ": " << Atom::printFloat(m_fGeneralMinAtomDistance) << endl;
	outFile << m_messages->m_sMinAtomDistance << ": " << endl;
	Atom::outputMinDistances(outFile);
	outFile << m_messages->m_sMaxAtomDistance << ": " << Atom::printFloat(m_fMaxAtomDistance) << endl;
	outFile << endl;
	
	if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		outFile << m_messages->m_sSimulatedAnnealingParameters << ":" << endl;
		outFile << m_messages->m_sPerformNonFragSearch << ": " << printYesNoParam(m_bPerformNonFragSearch) << endl;
		outFile << m_messages->m_sPerformBasinHopping << ": " << printYesNoParam(m_bPerformBasinHopping) << endl;
		outFile << m_messages->m_sTransitionStateSearch << ": " << printYesNoParam(m_bTransitionStateSearch) << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(1);
		if ((m_fDesiredAcceptedTransitions == 0) || (m_iIteration > NUM_ITERATIONS_TO_SET_SCALING_FACTOR))
			outFile << m_messages->m_sStartingTemperature << ": " << Atom::printFloat(m_fStartingTemperature) << endl;
		else
			outFile << m_messages->m_sStartingTemperature << ": " << Atom::printFloat(m_fDesiredAcceptedTransitions*100) << "%" << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_messages->m_sBoltzmanConstant << ": " << Atom::printFloat(m_fBoltzmanConstant) << endl;
		outFile << m_messages->m_sNumIterationsBeforeDecreasingTemp << ": " << m_iNumIterationsBeforeDecreasingTemp << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(1);
		outFile << m_messages->m_sAcceptanceRatio << ": " << Atom::printFloat(m_fAcceptanceRatio * 100) << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_messages->m_sQuenchingFactor << ": " << Atom::printFloat(m_fQuenchingFactor) << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(1);
		outFile << m_messages->m_sMinTemperatureToStop << ": " << Atom::printFloat(m_fMinTemperatureToStop) << endl;
		outFile << m_messages->m_sMinAcceptedTransitions << ": " << Atom::printFloat(m_fMinAcceptedTransitions * 100) << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_messages->m_sMaxIterations << ": " << m_iMaxIterations << endl;
		if (m_bResumeFileRead)
			outFile << m_messages->m_sNumPerterbations << ": " << Atom::printFloat(m_fNumPerterbations) << endl;
		else
			outFile << m_messages->m_sNumPerterbations << ": " << (int)m_fNumPerterbations << endl;
		outFile << m_messages->m_sStartCoordinatePerturbation << ": " << Atom::printFloat(m_fStartCoordinatePerturbation) << endl;
		outFile << m_messages->m_sMinCoordinatePerturbation << ": " << Atom::printFloat(m_fMinCoordinatePerturbation) << endl;
		outFile << m_messages->m_sStartAnglePerturbation << ": " << Atom::printFloat(m_fStartAnglePerturbation * RAD_TO_DEG) << endl;
		outFile << m_messages->m_sMinAnglePerturbation << ": " << Atom::printFloat(m_fMinAnglePerturbation * RAD_TO_DEG) << endl;
	} else if (m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION) {
		outFile << m_messages->m_sParticleSwarmParameters << ":" << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_messages->m_sStartCoordInertia << ": " << Atom::printFloat(m_fStartCoordInertia) << endl;
		outFile << m_messages->m_sEndCoordInertia << ": " << Atom::printFloat(m_fEndCoordInertia) << endl;
		outFile << m_messages->m_sReachEndInertiaAtIteration << ": " << m_iReachEndInertiaAtIteration << endl;
		outFile << m_messages->m_sCoordIndividualMinimumAttraction << ": " << Atom::printFloat(m_fCoordIndividualMinimumAttraction) << endl;
		outFile << m_messages->m_sCoordPopulationMinimumAttraction << ": " << Atom::printFloat(m_fCoordPopulationMinimumAttraction) << endl;
		if (m_fCoordMaximumVelocity != 0)
			outFile << m_messages->m_sCoordMaximumVelocity << ": " << Atom::printFloat(m_fCoordMaximumVelocity) << endl;
		else
			outFile << m_messages->m_sCoordMaximumVelocity << ": " << endl;
		outFile << m_messages->m_sStartAngleInertia << ": " << Atom::printFloat(m_fStartAngleInertia) << endl;
		outFile << m_messages->m_sEndAngleInertia << ": " << Atom::printFloat(m_fEndAngleInertia) << endl;
		outFile << m_messages->m_sAngleIndividualMinimumAttraction << ": " << Atom::printFloat(m_fAngleIndividualMinimumAttraction * RAD_TO_DEG) << endl;
		outFile << m_messages->m_sAnglePopulationMinimumAttraction << ": " << Atom::printFloat(m_fAnglePopulationMinimumAttraction * RAD_TO_DEG) << endl;
		if (m_fAngleMaximumVelocity != 0)
			outFile << m_messages->m_sAngleMaximumVelocity << ": " << Atom::printFloat(m_fAngleMaximumVelocity * RAD_TO_DEG) << endl;
		else
			outFile << m_messages->m_sAngleMaximumVelocity << ": " << endl;
		
		if (m_bStartingVisibilityAuto && (m_fStartVisibilityDistance == -1))
			outFile << m_messages->m_sStartVisibilityDistance << ": auto" << endl;
		else
			outFile << m_messages->m_sStartVisibilityDistance << ": " << Atom::printFloat(m_fStartVisibilityDistance) << endl;
		outFile << m_messages->m_sVisibilityDistanceIncrease << ": " << Atom::printFloat(m_fVisibilityDistanceIncrease) << endl;
		
		if (m_fSwitchToRepulsionWhenDiversityIs > 0) {
			outFile << m_messages->m_sSwitchToRepulsionWhenDiversityIs << ": " << Atom::printFloat(m_fSwitchToRepulsionWhenDiversityIs) << endl;
			outFile << m_messages->m_sSwitchToRepulsionWhenNoProgress << ": " << m_iSwitchToRepulsionWhenNoProgress << endl;
			outFile << m_messages->m_sSwitchToAttractionWhenDiversityIs << ": " << Atom::printFloat(m_fSwitchToAttractionWhenDiversityIs) << endl;
		} else {
			outFile << m_messages->m_sSwitchToRepulsionWhenDiversityIs << ": " << endl;
			outFile << m_messages->m_sSwitchToRepulsionWhenNoProgress << ": " << endl;
			outFile << m_messages->m_sSwitchToAttractionWhenDiversityIs << ": " << endl;
		}
		outFile << m_messages->m_sIndividualBestUpdateDist << ": " << Atom::printFloat(m_fIndividualBestUpdateDist) << endl;

		outFile << m_messages->m_sMaxIterations << ": " << m_iMaxIterations << endl;
		outFile << m_messages->m_sEnforceMinDistOnCopy << ": " << printYesNoParam(m_bEnforceMinDistOnCopy) << endl;
		outFile << m_messages->m_sUseLocalOptimization << ": " << printYesNoParam(m_bUseLocalOptimization) << endl;
	} else if (m_iAlgorithmToDo == GENETIC_ALGORITHM) {
		outFile << m_messages->m_sGeneticAlgorithmParameters << ":" << endl;
		if (!(m_bResumeFileRead || m_bOptimizationFileRead))
			outFile << setprecision(defaultPrecision);
		outFile << m_messages->m_sUseLocalOptimization << ": " << printYesNoParam(m_bUseLocalOptimization) << endl;
		outFile << m_messages->m_sMaxIterations << ": " << m_iMaxIterations << endl;
	}
	
	outFile << endl;
	if (!(m_bResumeFileRead || m_bOptimizationFileRead))
		outFile << setprecision(1);
	outFile << m_messages->m_sNumberOfBestStructuresToSave << ": " << m_iNumberOfBestStructuresToSave << endl;
	if (!(m_bResumeFileRead || m_bOptimizationFileRead))
		outFile << setprecision(defaultPrecision);
	outFile << m_messages->m_sMinDistnaceBetweenSameMoleculeSets << ": " << Atom::printFloat(m_fMinDistnaceBetweenSameMoleculeSets) << endl;
	outFile << m_messages->m_sNumberOfLogFilesToSave << ": " << m_iNumberOfLogFilesToSave << endl;
	outFile << m_messages->m_sOutputFileName << ": " << m_sOutputFileName << endl;
	outFile << m_messages->m_sResumeFileName << ": " << m_sResumeFileName << endl;
	outFile << m_messages->m_sResumeFileNumIterations << ": " << m_iResumeFileNumIterations << endl;
	outFile << m_messages->m_sPrintSummaryInfoEveryNIterations << ": " << m_iPrintSummaryInfoEveryNIterations << endl;
	
	outFile << m_messages->m_sCharge << ": " << m_iCharge << endl;
	outFile << m_messages->m_sMultiplicity << ": " << m_iMultiplicity << endl;
	outFile << m_messages->m_sEnergyFileHeader << ":" << endl;
	outFile << m_sEnergyFileHeader << endl;
	outFile << m_messages->m_sEnergyFileFooter << ":" << endl;
	outFile << m_sEnergyFileFooter << endl;
	
	outFile << m_messages->m_sNumStructureTypes << ": " << m_iNumStructureTypes << endl;
	
	if (!(m_bResumeFileRead || m_bOptimizationFileRead))
		outFile << setprecision(defaultPrecision);
	for (i = 0; i < m_iNumStructureTypes; ++i)
	{
		outFile << endl << m_messages->m_sNumStructuresOfEachType << ": " << m_iNumStructuresOfEachType[i] << endl;
		outFile << m_messages->m_sStructureFormatOfThisType << ": " << m_messages->m_sCartesian << endl;
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
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;
	Input outputFile;
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	vector<MoleculeSet*> bestIndividualMoleculeSets;
	bool success;

	printf(m_messagesDL->m_sReadingParamsFormInput.c_str(), m_sOutputFileName.c_str());
	cout << endl;
	success = outputFile.open(m_sOutputFileName, false, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets);
	outputFile.m_bResumeFileRead = false;
	outputFile.m_bOptimizationFileRead = m_bOptimizationFileRead;
	
	if (success)
		outputFile.printToFile(outFile);
	else
	{
		printf(m_messagesDL->m_sUnableToReadFile.c_str(), m_sOutputFileName.c_str());
		cout << endl;
		printToFile(outFile);
	}
}

void Input::writeResumeFile(string &fileName, vector<MoleculeSet*> &moleculeSets,
                            vector<MoleculeSet*> &bestNMoleculeSets, vector<MoleculeSet*> &bestIndividualMoleculeSets,
                            time_t elapsedSeconds, bool resumeOrOptimizationFile)
{
	int i;
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;
	m_messages = Strings::instance(m_sLanguageCode);
	m_messagesDL = m_messages;
	
	m_iTotalPopulationSize = (int)moleculeSets.size();
	// Create the resume file just in case we need to stop the program
	ofstream resumeFile(fileName.c_str(), ios::out);
	if (!resumeFile.is_open()) {
		printf(m_messagesDL->m_sUnableToWriteTemporaryResume.c_str(), fileName.c_str());
		cout << endl;
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
		resumeFile << m_messages->m_sStructuresToOptimizeAtATime << ": " << m_iStructuresToOptimizeAtATime << endl;
	} else if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		resumeFile << m_messages->m_sDecreasingTemp << ": " << m_bDecreasingTemp << endl;
		
		resumeFile << m_messages->m_sAcceptedTransitions << ":" << endl;
		resumeFile << m_prgAcceptedTransitions[0];
		for (i = 1; i < m_iNumIterationsBeforeDecreasingTemp; ++i)
			resumeFile << "," << m_prgAcceptedTransitions[i];
		resumeFile << endl;
		
		resumeFile << m_messages->m_sAcceptedTransitionsIndex << ": "
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
	
	resumeFile << m_messages->m_sIteration << ": " << m_iIteration << endl;
	resumeFile << m_messages->m_sFreezeUntilIteration << ": " << m_iFreezeUntilIteration << endl;
	
	resumeFile << "Run Complete: " << printYesNoParam(m_bRunComplete) << endl;
	resumeFile << m_messages->m_sEnergyCalculations << ": " << m_iNumEnergyEvaluations << endl;
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
		m_messagesDL = Strings::instance();
		m_messages = m_messagesDL;
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
			cout << "The line that's supposed to have this parameter is missing: " << m_messages->m_sStructuresToOptimizeAtATime << endl;
			infile.close();
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sStructuresToOptimizeAtATime, m_iStructuresToOptimizeAtATime))
		{
			cout << "Error reading the parameter '" << m_messages->m_sStructuresToOptimizeAtATime << "' in the input file." << endl;
			infile.close();
			return false;
		}
	} else if (m_iAlgorithmToDo == SIMULATED_ANNEALING) {
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: " << m_messages->m_sDecreasingTemp << endl;
			infile.close();
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sDecreasingTemp, temp))
		{
			cout << "Error reading the parameter '" << m_messages->m_sDecreasingTemp << "' in the input file." << endl;
			infile.close();
			return false;
		}
		m_bDecreasingTemp = temp;
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << m_messages->m_sAcceptedTransitions << endl;
			infile.close();
			return false;
		}
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line after this parameter is missing: "
			     << m_messages->m_sAcceptedTransitions << endl;
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
			     << " but it's of length " << i << ": " << m_messages->m_sAcceptedTransitions << endl;
			return false;
		}
		
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line that's supposed to have this parameter is missing: "
			     << m_messages->m_sAcceptedTransitionsIndex << endl;
			infile.close();
			return false;
		}
		if (!getIntParam(fileLine, m_messages->m_sAcceptedTransitionsIndex,
		                 m_iAcceptedTransitionsIndex))
		{
			cout << "Error reading the parameter '" << m_messages->m_sAcceptedTransitionsIndex
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
		cout << "The line that's supposed to have this parameter is missing: " << m_messages->m_sIteration << endl;
		infile.close();
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sIteration, m_iIteration))
	{
		cout << "Error reading the parameter '" << m_messages->m_sIteration << "' in the input file." << endl;
		infile.close();
		return false;
	}
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line that's supposed to have this parameter is missing: " << m_messages->m_sFreezeUntilIteration << endl;
		infile.close();
		return false;
	}
	if (!getIntParam(fileLine, m_messages->m_sFreezeUntilIteration, m_iFreezeUntilIteration))
	{
		cout << "Error reading the parameter '" << m_messages->m_sFreezeUntilIteration << "' in the input file." << endl;
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
		     << m_messages->m_sEnergyCalculations << endl;
		infile.close();
		return false;
	}
	if (sscanf(fileLine, "Number of energy value calculations: %d", &m_iNumEnergyEvaluations) != 1)
	{
		cout << "Error reading the parameter '" << "Number of energy value calculations"
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
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;
	int i;
	int j;
	Molecule* molecules = m_tempelateMoleculeSet.getMolecules();
	
	j = 0;
	for (i = 0; i < m_iNumStructureTypes; ++i) {
		printf(m_messagesDL->m_snMoleculeNumber.c_str(), (i+1));
		if (!molecules[j].findBonds())
			return false;
		if (!molecules[j].checkConnectivity())
			return false;
		molecules[j].printBondInfo();
		j += m_iNumStructuresOfEachType[i];
	}
	printf(m_messagesDL->m_snCheckBonding1.c_str());
	printf(m_messagesDL->m_snCheckBonding2.c_str());
	printf(m_messagesDL->m_snBondInfoFormat.c_str());
	return true;
}

bool Input::printTestFileHeader(int iterationNumber, MoleculeSet &startingGeometry)
{
	string logFileName;
	string header = Input::s_program_directory+"/unitTests/testfiles/header.txt";
	string spacer1_1 = Input::s_program_directory+"/unitTests/testfiles/spacer1.1.txt";
	string spacer1_2 = Input::s_program_directory+"/unitTests/testfiles/spacer1.2.txt";
	char temp[500];
	
	logFileName = Input::s_program_directory+"/unitTests/testfiles/test";
	if (iterationNumber == 0) {
		snprintf(temp,sizeof(temp),"rm %s/unitTests/testfiles/test*.log", Input::s_program_directory.c_str());
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
		string spacer2_1 = Input::s_program_directory+"/unitTests/testfiles/spacer2.1.txt";
		string spacer2_2 = Input::s_program_directory+"/unitTests/testfiles/spacer2.2.txt";
		
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
	string footer = Input::s_program_directory+"/unitTests/testfiles/footer.txt";
	
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
				printf(m_messagesDL->m_snUnableToCreateDirectory.c_str(), inputFileDirectory.c_str());
				throw m_messagesDL->m_snUnableToCreateDirectory.c_str();
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
						printf(m_messagesDL->m_snUnableToCreateDirectory.c_str(), m_sPathToEnergyFiles.c_str());
						throw "";
					}
				}
				if (m_pSelectedEnergyProgram->m_sPathToExecutable.length() > 0) {
					m_sNodesFile = m_sPathToEnergyFiles + "/" + m_messages->m_sNodesFile;
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
	m_sNodesFile = m_sPathToEnergyFiles + "/" + m_messages->m_sNodesFile;
	
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
	string frozenString;
	frozenString.append(m_messages->m_sFreezingSeededMoleculesFor).append(" %d ").append(m_messages->m_sIterations);
	
	
	// Read the individual output files
	last_iteration = 1073741824; // a big number
	for (i = 0; i < m_iTotalPopulationSize; ++i) {
		snprintf(outputFileName, sizeof(outputFileName), "%s/%s_%d.out", outputFileDirectory.c_str(), outputFileWithoutPath.c_str(), (i+1));
		if (printOutput)
			cout << m_messagesDL->m_sReadingFile << ": " << outputFileName << endl;
		fin.open(outputFileName, ifstream::in);
		if (!fin.is_open())
		{
			cout << m_messagesDL->m_sCantWriteToFile << ": " << outputFileName << endl;
			return false;
		}
//		bestEnergies.push_back(vector<FLOAT>);
		while (fin.getline(fileLine, sizeof(fileLine)))
		{
			if (strncmp(fileLine, m_messages->m_sSeedingPopulationFromFile.c_str(), m_messages->m_sSeedingPopulationFromFile.length()+1) == 0) {
				seedFiles = (fileLine+29);
				seedFiles.replace(seedFiles.length()-3,3,""); // remove the "..."
			}
			if (sscanf(fileLine, frozenString.c_str(), &m_iFreezeUntilIteration) == 1) {
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
		fout << m_messages->m_sPerformingBasinHopping << "..." << endl;
	} else if (m_bTransitionStateSearch) {
		fout << "Searching for Transition States..." << endl;
	} else {
		fout << m_messages->m_sPerformingSimulatingAnnealing << "..." << endl;
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

