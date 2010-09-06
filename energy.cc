////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "energy.h"

Energy::Energy(const Input *pInput, int energyObjectId, const char *nodeName, string energyFunction)
{
	const char *header;
	int fileNameLength;
	char fileName[500];
	char numberString[25];
	string::size_type pos;
	
	m_pInput = pInput;
	m_fEnergy = 0;
	m_bConverged = false;
	m_iEnergyObjectId = energyObjectId;
	sprintf(numberString,"%d",m_iEnergyObjectId);
	m_sEnergyObjectId = numberString;
	
	// Set some constants
	if (energyFunction.compare("Gaussian") == 0)
		m_iEnergyFunctionToUse = GAUSSIAN;
	else if (energyFunction.compare("Lennard Jones") == 0)
		m_iEnergyFunctionToUse = LENNARD_JONES;
	else {
		cout << "Unrecognized energy function: " << energyFunction << endl;
		cout << "Please enter a name from the list and remember that names are case sensitive." << endl;
		exit(0);
	}
	
	if (m_iEnergyFunctionToUse == GAUSSIAN) {
		m_sGeneralEnergyFileHeader = m_pInput->m_sEnergyFileHeader;
		
		//////////// Get the name of the checkpoint file  //////////////////
		m_sGeneralCheckPointFileName = "";
		pos = m_sGeneralEnergyFileHeader.find("% chk");
		if (pos == string::npos) {
			if (pInput->m_bUsePrevWaveFunction) {
				cout << "Unable to locate checkpoint file name.  Quitting..." << endl;
				exit(0);
			}
		} else {
			header = m_sGeneralEnergyFileHeader.c_str();
			header = &header[pos+5];
			while ((*header != '\0') && !isFileCharacter(*header))
				++header;
			if ((*header == '\0') && pInput->m_bUsePrevWaveFunction) {
				cout << "Unable to locate checkpoint file name where it should be.  Quitting..." << endl;
				exit(0);
			}
			fileNameLength = 1;
			while (isFileCharacter(header[fileNameLength]))
				++fileNameLength;
			
			strncpy(fileName,header,fileNameLength);
			fileName[fileNameLength] = '\0';
			m_sGeneralCheckPointFileName = fileName;
		}
		//////////// End get the name of the checkpoint file  //////////////////
		
		if (!pInput->m_bUsePrevWaveFunction && (m_sGeneralCheckPointFileName.length() > 0)) {
			// Rename the checkpoint file if there is one
			pos = m_sGeneralEnergyFileHeader.find(m_sGeneralCheckPointFileName);
			m_sGeneralEnergyFileHeader.replace(pos,m_sGeneralCheckPointFileName.length(),
					m_pInput->m_sPathToEnergyFiles+"/"+m_sGeneralCheckPointFileName+m_sEnergyObjectId);
		}
		m_sRun = "abinitio";
	}
	m_nodeName = nodeName;
	m_iTimesCalculatedEnergy = 0;
	m_iTimesConverged = 0;
}

Energy::~Energy ()
{
	
}

bool Energy::isFileCharacter(char character)
{
	return (((character >= 'a') && (character <= 'z')) ||
		    ((character >= 'A') && (character <= 'Z')) ||
		    ((character >= '0') && (character <= '9')) ||
		    (character == '.') || (character == ',') || (character == '_'));
}

/************************************
 * Function generating the com-file 
 * for single-point energy calculation
 ***********************************/
void Energy::writeOne(const string &fileToMake, int config, const string &sEnergyFileHeader,
              int charge, int multiplicity, MoleculeSet &moleculeSet)
{
	ofstream fout(fileToMake.c_str(), ios::out);
	fout << sEnergyFileHeader << endl
	     << "This is a computer generated structure." << endl << endl
	     << charge << " " << multiplicity << endl;
	
	moleculeSet.writeToGausianFile(fout);
	
	fout << endl;
	fout.close();
}

		
/***********************************************************
 * a nice little function to change an integer into a string
 * author   : Alex Birch. date     : May 27, 2003 
 * ********************************************************/
string Energy::int2string(long long number)
{
	string temp;
	string rv;
	if (number < 0) {
		rv = "-";
		number = -number;
	}
	else if (!number)
		return "0";
	for (; number; number /= 10)
		temp += char(number % 10 + '0');
	for (int i = temp.size() - 1; i >= 0; i--)
		rv += temp[i];
	return rv;
}

/******************************************************
 * Another function extracting energy from the log-file 
 * (for s.p.) for semiempyrical runs only
 *****************************************************/
bool Energy::getEsp(const string &logFile, FLOAT &answer)
{
	ifstream fin(logFile.c_str());
	char line[300];
	string stringLine;
	bool found=false;
	string pieceLine;
	string nextLine;
	const char *result;
	answer = 0;
	if(!fin){
		cerr<<"could not open file  "<<logFile<<" exiting "<<endl;
		exit(1);
	}
	while((fin.getline(line, 300))&&(found == false))
	{
		stringLine=line;
		int startingIndex=stringLine.find("Energy=   ");
		if((startingIndex>=0)&&(startingIndex<(signed int)stringLine.size()))
		{
			fin.getline(line, 300);
			nextLine=line;
			stringLine=stringLine+nextLine;
			int numChar=28;
			pieceLine=stringLine.substr(startingIndex, numChar);
			const int endIndex=pieceLine.rfind(" ");//finish taking the figure when space is found.
			const int beginingIndex=startingIndex + 10;//energy is always negative.
			numChar=endIndex-beginingIndex;//number if chrs to take then.
			pieceLine=pieceLine.substr(beginingIndex, numChar);
			result=pieceLine.c_str();
			answer=atof(result);
			found=true;
		}

		startingIndex=stringLine.find("Normal termination");
		if (startingIndex>=0) // If we found it
			return found;
	}
	fin.close();
	return false;
}

/************************************************************************
 * Function looking for the B3LYP-energy and extracting it from the log-file
 * *********************************************************************/
bool Energy::extractE(const string &logFile, FLOAT &answer)
{
	ifstream fin(logFile.c_str());
	char line[300];
	string stringLine;
	bool found=false;
	string nextLine;
	string pieceLine;
	answer = 0;
	if(!fin){
	         cerr<<"could not open file  "<<logFile<<" exiting "<<endl;
	         exit(1);
	         }
	if( m_sRun == "abinitio" )
	{
		while(fin.getline(line, 300))
		{
			stringLine=line;
			
			int startingIndex=stringLine.find("LYP) =  ");//looking for the place E(RB+HF+LYP) in log-file
								      //we believe here that the B3LYP method is in use	
								      //otherwise - need modification.
			if (startingIndex>=0) // If we found it
			{
				fin.getline(line, 300);
			        nextLine=line;
		        	stringLine=stringLine+nextLine;

				int numChar=28;
				pieceLine=stringLine.substr(startingIndex, numChar);
				const int endIndex=pieceLine.rfind(" ");//finish taking the figure when space is found.
				const int beginingIndex=pieceLine.find("-");//energy is always negative.
				numChar=endIndex-beginingIndex;//number if chrs to take then.
				pieceLine=pieceLine.substr(beginingIndex, numChar);
				answer=atof(pieceLine.c_str());
				found=true;
			}
		}
	}
	fin.close();

	return found;
}

// Function getting Standard Orientation from a Gaussian log-file
void Energy::getStandardOrientation(const string &logFile, MoleculeSet &moleculeSet)
{
	ifstream fin(logFile.c_str());
	char line[300];
	int i;
	int dummy;
	Point3D *cartesianPoints = new Point3D[(unsigned int)moleculeSet.getNumberOfAtoms()];
	int *atomicNumbers = new int[(unsigned int)moleculeSet.getNumberOfAtoms()];
	
	while(fin.getline(line, 300))
	{
		string stringLine=line;
		int startingIndex=stringLine.find("Standard orientation");

		if (startingIndex>=0)
		{
			fin.getline(line, 300); //get ----
			fin.getline(line, 300); //get first line of text
			fin.getline(line, 300); //get second line of text
			fin.getline(line, 300); //get ----
			
			for (i = 0; i < moleculeSet.getNumberOfAtoms(); ++i)
			{
				fin >> dummy;//center number
				fin >> atomicNumbers[i];
				fin >> dummy;//atomic type
				fin >> cartesianPoints[i].x;
				fin >> cartesianPoints[i].y;
				fin >> cartesianPoints[i].z;
			}
		}
	}
	fin.close();
	moleculeSet.assignReadCoordinates(cartesianPoints,atomicNumbers);
	delete[] cartesianPoints;
	delete[] atomicNumbers;
}

void Energy::backupWaveFunctionFile(MoleculeSet &moleculeSet)
{
	char commandString[500];
	string checkPointFileName;
	string completeCheckpointFileName, backupCheckpointFileName;
	struct stat fileStatistics;
	char numberString[25];

	sprintf(numberString,"%d",moleculeSet.getId());
	checkPointFileName = m_sGeneralCheckPointFileName+numberString;
	completeCheckpointFileName = m_pInput->m_sPathToEnergyFiles + "/" + checkPointFileName + ".chk";
	backupCheckpointFileName = m_pInput->m_sPathToEnergyFiles + "/" + checkPointFileName + ".bak";
	if (stat(completeCheckpointFileName.c_str(), &fileStatistics) == 0) { // If the file exists
		sprintf(commandString,"cp %s %s", completeCheckpointFileName.c_str(), backupCheckpointFileName.c_str());
		if (system(commandString))
			cout << "Error backing up the wave function file: " << checkPointFileName << ".chk" << endl;
	}
}

void Energy::restoreWaveFunctionFile(MoleculeSet &moleculeSet)
{
	char commandString[500];
	string checkPointFileName;
	string completeCheckpointFileName, backupCheckpointFileName;
	struct stat fileStatistics;
	char numberString[25];
	
	sprintf(numberString,"%d",moleculeSet.getId());
	checkPointFileName = m_sGeneralCheckPointFileName+numberString;
	completeCheckpointFileName = m_pInput->m_sPathToEnergyFiles + "/" + checkPointFileName + ".chk";
	backupCheckpointFileName = m_pInput->m_sPathToEnergyFiles + "/" + checkPointFileName + ".bak";
	if (stat(backupCheckpointFileName.c_str(), &fileStatistics) == 0) { // If the file exists
		sprintf(commandString,"mv %s %s", backupCheckpointFileName.c_str(), completeCheckpointFileName.c_str());
		if (system(commandString))
			cout << "Error restoring the backup wave function file: " << checkPointFileName << ".bak" << endl;
	}
}

void Energy::deleteEnergyFiles(MoleculeSet *moleculeSet)
{
	char commandLine[500];
	struct stat fileStatistics;
	string fileName, logFileName;
	char moleculeSetId[25];
	
	if (m_iEnergyFunctionToUse == GAUSSIAN) {
		if (m_pInput->m_bUsePrevWaveFunction) {
			if (moleculeSet == NULL) {
				cout << "moleculeSet == null in Energy.deleteEnergyFiles.  This should not happen." << endl;
				return;
			}
			sprintf(moleculeSetId, "%d", moleculeSet->getId());
			string tempString = moleculeSetId;
			fileName = ENERGY_TEMP_FILE + tempString;
		} else
			fileName = ENERGY_TEMP_FILE + m_sEnergyObjectId;
		
		// Delete the old com file if it exists
		logFileName = m_pInput->m_sPathToEnergyFiles + "/" + fileName + ".com";
		if (stat(logFileName.c_str(), &fileStatistics) == 0) { // If no errors occurred in getting stats, the file exists
			snprintf(commandLine, 500, "rm %s", logFileName.c_str());
			system(commandLine);
		}
		
		// Delete the old log file if it exists
		logFileName = m_pInput->m_sPathToEnergyFiles + "/" + fileName + ".log";
		if (stat(logFileName.c_str(), &fileStatistics) == 0) { // If no errors occurred in getting stats, the file exists
			snprintf(commandLine, 500, "rm %s", logFileName.c_str());
			system(commandLine);
		}
		
		// Delete the checkpoint file if we're using previous wave functions
		if (m_pInput->m_bUsePrevWaveFunction) {
			string completeCheckpointFileName;
			completeCheckpointFileName = m_pInput->m_sPathToEnergyFiles+"/"+m_sGeneralCheckPointFileName+moleculeSetId+".chk";
			if (stat(completeCheckpointFileName.c_str(), &fileStatistics) == 0) { // If the file exists
				snprintf(commandLine, 500, "rm %s", completeCheckpointFileName.c_str());
				system(commandLine);
			}
			
			// Delete the backup checkpoint file
			completeCheckpointFileName = m_pInput->m_sPathToEnergyFiles+"/"+m_sGeneralCheckPointFileName+moleculeSetId+".bak";
			if (stat(completeCheckpointFileName.c_str(), &fileStatistics) == 0) { // If the file exists
				snprintf(commandLine, 500, "rm %s", completeCheckpointFileName.c_str());
				system(commandLine);
			}
		}
	}
}

void Energy::calculateEnergy(MoleculeSet &moleculeSet, bool usePreviousWaveFunctionThisTime, bool performLocalOptimization, bool readOptimizedCoordinates)
{
	string checkPointFileName;
	++m_iTimesCalculatedEnergy;
	m_fEnergy = 0;
	m_bConverged = false;
	string energyFileHeader = m_sGeneralEnergyFileHeader;
	string fileName;
	string jobQueueFileName;
	string jobQueueFileContents;
	char commandLine[500];
	
	if (m_iEnergyFunctionToUse == LENNARD_JONES)
	{
		if (performLocalOptimization) {
			if (readOptimizedCoordinates) {
				moleculeSet.performLennardJonesOptimization(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
				m_fEnergy = moleculeSet.computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
			} else {
				MoleculeSet copy;
				copy.copy(moleculeSet);
				copy.performLennardJonesOptimization(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
				m_fEnergy = copy.computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
			}
		} else {
			m_fEnergy = moleculeSet.computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
		}
		
		m_bConverged = true;
		++m_iTimesConverged;
		return;
	} else if (m_iEnergyFunctionToUse == GAUSSIAN) {
		char moleculeSetId[25];
		sprintf(moleculeSetId,"%d",moleculeSet.getId());
		
		if (m_pInput->m_bUsePrevWaveFunction) {
			string::size_type pos;
			struct stat fileStatistics;
			string completeCheckpointFileName;
			
			// Rename the checkpoint file
			checkPointFileName = m_sGeneralCheckPointFileName+moleculeSetId;
			pos = energyFileHeader.find(m_sGeneralCheckPointFileName);
			energyFileHeader.replace(pos,m_sGeneralCheckPointFileName.length(),
					m_pInput->m_sPathToEnergyFiles+"/"+checkPointFileName);
			
			// Make sure guess(read) is in the file if it needs to be there
			completeCheckpointFileName = m_pInput->m_sPathToEnergyFiles + "/" + checkPointFileName + ".chk";
			if ((stat(completeCheckpointFileName.c_str(), &fileStatistics) == 0) // If a previous checkpoint file exists
			    && (usePreviousWaveFunctionThisTime)) { // And if we're using the previous wave function this time
				pos = energyFileHeader.find("guess");
				if (pos == string::npos) {
					energyFileHeader.insert(energyFileHeader.length()-1, " guess(read)");
				} else {
					// Replace the existing guess with read
					const char *filePtr = energyFileHeader.c_str();
					int guessLength;
					
					pos += 5;
					filePtr = &filePtr[pos];
					while ((*filePtr != '\0') && !isFileCharacter(*filePtr)) {
						++filePtr;
						++pos;
					}
					if (*filePtr == '\0') {
						cout << "Problem putting guess(read) in the energy file header..." << endl;
						exit(0);
					}
					guessLength = 1;
					while (isFileCharacter(filePtr[guessLength]))
						++guessLength;
					energyFileHeader.replace(pos,guessLength,"read");
				}
			}
			
			
			string tempString = moleculeSetId;
			fileName = ENERGY_TEMP_FILE + tempString;
		} else
			fileName = ENERGY_TEMP_FILE + m_sEnergyObjectId;
		
		// Create the .com file
		string  inputFileName = m_pInput->m_sPathToEnergyFiles + "/" + fileName + ".com";
		writeOne(inputFileName, m_iEnergyObjectId, energyFileHeader, m_pInput->m_iCharge, m_pInput->m_iMultiplicity, moleculeSet);
		
		// Delete the old log file if it exists
		struct stat fileStatistics;
		string logFileName = m_pInput->m_sPathToEnergyFiles + "/" + fileName + ".log";
		if (stat(logFileName.c_str(), &fileStatistics) == 0) { // If no errors occurred in getting stats, the file exists
			snprintf(commandLine, 500, "rm %s", logFileName.c_str());
			system(commandLine);
		}
		
		if ((m_pInput->m_sJobQueueTemplate.length() > 0)) {
			string::size_type pos;
			jobQueueFileName = m_pInput->m_sPathToEnergyFiles + "/" + fileName;
			jobQueueFileContents = m_pInput->m_sJobQueueTemplateFileContents;
			pos = jobQueueFileContents.find("$ENERGY_PROGRAM_GOES_HERE");
			jobQueueFileContents.replace(pos,25,m_pInput->m_sPathToEnergyProgram);
			pos = jobQueueFileContents.find("$INPUT_FILE_NAME_GOES_HERE");
			jobQueueFileContents.replace(pos,26,inputFileName);
			
			// Create the job queue file
			ofstream jobQueueFile(jobQueueFileName.c_str(), ios::out);
			if (!jobQueueFile.is_open()) {
				cout << "Unable to write to the file: " << jobQueueFileName << endl;
				return;
        		}
        		jobQueueFile << jobQueueFileContents;
			jobQueueFile.close();
			
			// Run the job
			string temporaryFile = jobQueueFileName+".out";
			chdir(m_pInput->m_sPathToEnergyFiles.c_str());
			snprintf(commandLine, 500, "qsub %s > %s", jobQueueFileName.c_str(), temporaryFile.c_str());
			int command = system(commandLine);
			m_bConverged = (command == 0);
			if (!m_bConverged) {
				cout << "Unable to submit job to queue: " << jobQueueFileName << endl;
				return;
			}
			
			// Find the job number
			ifstream jobNumberFile(temporaryFile.c_str());
			int MAX_LINE_LENGTH = 500;
			char fileLine[MAX_LINE_LENGTH];
			if (!jobNumberFile) {
				cout << "Error opening the file: " << temporaryFile << endl;
				m_bConverged = false;
				return;
			}
			if (!jobNumberFile.getline(fileLine, MAX_LINE_LENGTH)) {
				cout << "Error reading the file: " << temporaryFile << endl;
				m_bConverged = false;
				return;
			}
			int jobNumber = atoi(fileLine);
			jobNumberFile.close();
			
			// Check to see if the job is done
			snprintf(commandLine, 500, "qstat > %s", temporaryFile.c_str());
			bool jobDone = false;
			int tempSuccess;
			while (!jobDone) {
				sleep(20); // sleep for some number of seconds
				tempSuccess = (system(commandLine) == 0);
				if (!tempSuccess) {
					cout << "Unable to execute qstat for job " << jobNumber << endl;
					continue;
				}
				jobDone = true;
				ifstream tempFile(temporaryFile.c_str());
				while (tempFile.getline(fileLine, MAX_LINE_LENGTH)) {
					if (jobNumber == atoi(fileLine)) {
						jobDone = false;
						break;
					}
				}
				tempFile.close();
			}
		} else {
			// -o 'NumberOfPasswordPrompts 0'         --> this is code for entering a password (not used)
			if (strcmp(m_nodeName, "LOCAL") == 0)
				snprintf(commandLine, 500, "%s %s/%s", m_pInput->m_sPathToEnergyProgram.c_str(),
				        m_pInput->m_sPathToEnergyFiles.c_str(), fileName.c_str());
			else
				snprintf(commandLine, 500, "%s cd %s \\&\\& %s %s", m_nodeName,
			            m_pInput->m_sPathToEnergyFiles.c_str(), m_pInput->m_sPathToEnergyProgram.c_str(), fileName.c_str());
			system(commandLine);
			// Note, if performing local optimization, system(commandLine) may not return 1 if the structure
			// does not converge which is why we don't check to see what system(commandLine) returned
			m_bConverged = true;
		}
		if (m_bConverged) {
			if (m_sRun == "abinitio")
				m_bConverged = extractE(logFileName, m_fEnergy);
			else if (m_sRun == "semi" )
				m_bConverged = getEsp(logFileName, m_fEnergy);
			if (readOptimizedCoordinates)
				getStandardOrientation(logFileName, moleculeSet);
		}
		if(m_bConverged)
			++m_iTimesConverged;
	}
}

