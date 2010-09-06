////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "energy.h"

int Energy::s_energyFunction = -1;
string Energy::s_header = "";
string Energy::s_footer = "";
string Energy::s_checkPointFileName = "";
bool Energy::s_bGetStandardOrientation = true;
bool Energy::s_bSemiEmpirical = false;
string Energy::s_pathToEnergyFiles = "";
int Energy::s_iCharge = 0;
int Energy::s_iMultiplicity = 0;
string Energy::s_energyProgramWithPath = "";
string Energy::s_scratchDirectory = "";
string Energy::s_fullScratchDirectory = "";
string Energy::s_scratchCommand = "";

bool Energy::init(const Input &input, int rank)
{
	bool success = true;
	s_energyFunction = input.m_iEnergyFunction;

	if (input.m_sPathToScratch.length() > 0) {
		s_scratchDirectory = input.m_sPathToScratch + "/" + Input::fileWithoutPath(input.m_sInputFileName.c_str());
		s_fullScratchDirectory = s_scratchDirectory.c_str() + ToString(rank);
	} else {
		s_scratchDirectory = "";
		s_fullScratchDirectory = "";
	}

	s_energyProgramWithPath = input.m_sPathToEnergyProgram;
	s_pathToEnergyFiles = input.m_sPathToEnergyFiles;

	switch (s_energyFunction) {
	case GAUSSIAN:
		s_header = input.m_sEnergyFileHeader;
		s_footer = input.m_sEnergyFileFooter;
		s_iCharge = input.m_iCharge;
		s_iMultiplicity = input.m_iMultiplicity;

		if (s_fullScratchDirectory.length() > 0) {
			s_scratchCommand = "export GAUSS_SCRDIR=" + s_fullScratchDirectory + " && ";
//			s_scratchCommand = "env SCRDIR=" + s_fullScratchDirectory + " ";
		}
		
		string lowerHeader, lowerFooter;
		string::size_type pos;
		
		lowerHeader = input.m_sEnergyFileHeader;
		lower(lowerHeader);
		pos = lowerHeader.find("nosymm");
		s_bGetStandardOrientation = (pos == string::npos); // true if we didn't find it
		s_bSemiEmpirical = ((lowerHeader.find("#p") != string::npos) || (lowerHeader.find("# p") != string::npos));
		
		//////////// Get the name of the checkpoint file  //////////////////
		try {
			s_checkPointFileName = getGaussianCheckpointFile(input.m_sEnergyFileHeader.c_str());
			if ((s_checkPointFileName.length() == 0) && input.m_bTransitionStateSearch) {
				cout << "Unable to locate checkpoint file name.  Quitting..." << endl;
				throw "Unable to locate checkpoint file name.  Quitting...";
			}
			
			if ((s_checkPointFileName.length() > 0) && (s_footer.length() > 0)) {
				pos = s_footer.find(s_checkPointFileName);
				if (pos == string::npos) { // If we didn't find it
					cout << "The checkpoint file that is specified in the header is not in the footer.  Quitting..." << endl;
					throw "The checkpoint file that is specified in the header is not in the footer.  Quitting...";
				}
			}
		} catch (const char* message) {
			success = false;
			if (PRINT_CATCH_MESSAGES)
				cerr << "Caught message: " << message << endl;
		}
		//////////// End get the name of the checkpoint file  //////////////////
		break;
	}
	return success;
}

string Energy::getGaussianCheckpointFile(const char* gaussianHeader)
{
	string lowerHeader = gaussianHeader;
	string checkPointFileName = "";
	lower(lowerHeader);

	string::size_type pos;
	const char *header;
	char fileName[500];
	int fileNameLength;
	
	//////////// Get the name of the checkpoint file  //////////////////
	checkPointFileName = "";
	pos = lowerHeader.find("% chk");
	if (pos == string::npos) // if we didn't find it
		pos = lowerHeader.find("%chk");
	if (pos != string::npos) { // if we found it
		// find the beginning of the name
		header = gaussianHeader;
		header = &header[pos];
		while (*header != '=')
			++header;
		while ((*header != '\0') && !isFileCharacter(*header))
			++header;
		if (*header == '\0')
			return "";
		fileNameLength = 1;
		while (isFileCharacter(header[fileNameLength]))
			++fileNameLength;
		
		strncpy(fileName,header,fileNameLength);
		fileName[fileNameLength] = '\0';
		checkPointFileName = fileName;
	}
	return checkPointFileName;
}

void Energy::createMPIInitMessage(char* mpiInitMessage, int maxSize)
{
	switch (s_energyFunction) {
	case LENNARD_JONES:
		// not implemented with mpi
		break;
	case GAUSSIAN:
		snprintf(mpiInitMessage, maxSize, "%d|%s|%s|%s|%s", s_energyFunction, s_energyProgramWithPath.c_str(), s_pathToEnergyFiles.c_str(),
		         s_scratchDirectory.c_str(), s_checkPointFileName.c_str());
		break;
	}
}

bool Energy::createScratchDir(void)
{
	char commandLine[500];
	
	if (s_energyFunction == LENNARD_JONES)
		return true;
	
	try {
		if (!deleteScratchDir())
			throw "";
		
		if (s_fullScratchDirectory.length() > 0)
			if (!MoleculeSet::fileExists(s_fullScratchDirectory.c_str())) {
				snprintf(commandLine, sizeof(commandLine), "mkdir %s", s_fullScratchDirectory.c_str());
				if (system(commandLine) == -1)
					throw "";
			}
			if (chdir(s_fullScratchDirectory.c_str()) == -1)
				throw "";
		}
	catch (const char *message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
		return false;
	}

	return true;
}

bool Energy::deleteScratchDir(void)
{
	char commandLine[500];
	
	if (s_energyFunction == LENNARD_JONES)
		return true;

	if ((s_fullScratchDirectory.length() > 0) && MoleculeSet::fileExists(s_fullScratchDirectory.c_str())) {
		snprintf(commandLine, sizeof(commandLine), "rm -rf %s", s_fullScratchDirectory.c_str());
//		cout << "Executing command '" << commandLine << "'" << endl;
		return system(commandLine) != -1;
	}
	return true;
}

bool Energy::init(const char* mpiInitMessage, int rank)
{
	bool success = true;
	const char* mpiMessagePtr;
	int messageLength;
	char* message;
	vector<char*> messages;
	
	mpiMessagePtr = mpiInitMessage;
	while (true) {
		messageLength = strcspn(mpiMessagePtr, "|\0");
		message = new char[messageLength+1];
		memcpy(message, mpiMessagePtr, messageLength);
		message[messageLength] = '\0';
		messages.push_back(message);
		if (mpiMessagePtr[messageLength] == '\0')
			break;
		mpiMessagePtr += messageLength+1;
	}
	
	if (messages.size() <= 1)
		return false;
	s_energyFunction = atoi(messages[0]);
	switch (s_energyFunction) {
	case LENNARD_JONES:
		// not implemented with mpi
		break;
	case GAUSSIAN:
		if (messages.size() != 5)
			return false;
		s_energyProgramWithPath = messages[1];
		s_pathToEnergyFiles = messages[2];
		s_scratchDirectory = messages[3];
		s_checkPointFileName = messages[4];
		if (s_scratchDirectory.length() > 0) {
			s_fullScratchDirectory = s_scratchDirectory.c_str() + ToString(rank);
			s_scratchCommand = "export GAUSS_SCRDIR=" + s_fullScratchDirectory + " && ";
//			s_scratchCommand = "env SCRDIR=" + s_fullScratchDirectory + " ";
		} else {
			s_fullScratchDirectory = "";
		}
		if (s_fullScratchDirectory.length() > 0) {
		}

		break;
	}
	for (int i = 0; i < (int)messages.size(); ++i)
		delete[] messages[i];
	messages.clear();
	return success;
}

string Energy::getInputFileName(int populationMemberNumber)
{
	char fileName[500];
	snprintf(fileName, sizeof(fileName), "%s/%s%d.com", s_pathToEnergyFiles.c_str(), ENERGY_TEMP_FILE, populationMemberNumber);
	return fileName;
}

string Energy::getScratchInputFileName(int populationMemberNumber)
{
	char fileName[500];
	snprintf(fileName, sizeof(fileName), "%s%d.com", ENERGY_TEMP_FILE, populationMemberNumber);
//	snprintf(fileName, sizeof(fileName), "%s/%s%d.com", s_fullScratchDirectory.c_str(), ENERGY_TEMP_FILE, populationMemberNumber);
	return fileName;
}

string Energy::getOutputFileName(int populationMemberNumber)
{
	char fileName[500];
	snprintf(fileName, sizeof(fileName), "%s/%s%d.log", s_pathToEnergyFiles.c_str(), ENERGY_TEMP_FILE, populationMemberNumber);
	return fileName;
}

string Energy::getScratchOutputFileName(int populationMemberNumber)
{
	char fileName[500];
	snprintf(fileName, sizeof(fileName), "%s%d.log", ENERGY_TEMP_FILE, populationMemberNumber);
//	snprintf(fileName, sizeof(fileName), "%s/%s%d.log", s_fullScratchDirectory.c_str(), ENERGY_TEMP_FILE, populationMemberNumber);
	return fileName;
}

string Energy::getCheckPointFileName(int populationMemberNumber)
{
	return s_pathToEnergyFiles+"/"+s_checkPointFileName+ToString(populationMemberNumber) + ".chk";
}

string Energy::getScratchCheckPointFileName(int populationMemberNumber)
{
	return s_checkPointFileName+ToString(populationMemberNumber) + ".chk";
//	return s_fullScratchDirectory+"/"+s_checkPointFileName+ToString(populationMemberNumber) + ".chk";
}

void Energy::createInputFiles(vector<MoleculeSet*> &population)
{
	for (int i = 0; i < (signed int)population.size(); ++i)
		createInputFile(*population[i], i+1);
}

void Energy::createInputFile(MoleculeSet &moleculeSet, int populationMemberNumber)
{
	char commandLine[500];
	string fileName;
	
	// Create the input files
	switch (s_energyFunction) {
	case GAUSSIAN:
		Energy::createGaussianInputFile(getInputFileName(populationMemberNumber).c_str(), populationMemberNumber, moleculeSet, false);
			
		// Delete the old log file if it exists
		fileName = getOutputFileName(populationMemberNumber);
		if (MoleculeSet::fileExists(fileName.c_str())) {
			snprintf(commandLine, sizeof(commandLine), "rm %s", fileName.c_str());
			system(commandLine);
		}
		moleculeSet.setEnergyFile(fileName.c_str());
		
		// Delete the old checkpoint file if there is one
		if (s_checkPointFileName.length() > 0) {
			fileName = getCheckPointFileName(populationMemberNumber);
			if (MoleculeSet::fileExists(fileName.c_str())) {
				snprintf(commandLine, sizeof(commandLine), "rm %s", fileName.c_str());
				system(commandLine);
			}
			moleculeSet.setCheckPointFile(fileName.c_str());
		}
		break;
	}
}

void Energy::createGaussianInputFile(const char* inputFileName, int populationMemberNumber, MoleculeSet &moleculeSet, bool writeEnergyValueInHeader)
{
	string header = s_header;
	string footer = s_footer;
	string::size_type pos;

	string fullCheckPointFileName = s_checkPointFileName+ToString(populationMemberNumber);
	
	// Rename the checkpoint file if there is one
	if (s_checkPointFileName.length() > 0) {
		pos = header.find(s_checkPointFileName);
		header.replace(pos,s_checkPointFileName.length(), fullCheckPointFileName);
		if (footer.length() > 0) {
			pos = footer.find(s_checkPointFileName);
			if (pos != string::npos) // If we found it
				footer.replace(pos,s_checkPointFileName.length(), fullCheckPointFileName);
		}
	}
	
	ofstream fout(inputFileName, ios::out);
	fout << header << endl;
	if (writeEnergyValueInHeader)
		fout << "This is a computer generated structure with energy: " << Atom::printFloat(moleculeSet.getEnergy()) << endl << endl;
	else
		fout << "This is a computer generated structure." << endl << endl;
	fout << s_iCharge << " " << s_iMultiplicity << endl;

	moleculeSet.writeToGausianComFile(fout);

	fout << endl;

	if (footer.length() > 0) {
		fout << footer << endl
		     << "Title" << endl << endl
		     << s_iCharge << " " << s_iMultiplicity << endl;
	}
	fout.close();
}

/************************************************************************
 * Function looking for the SCF-energy and extracting it from the log-file
 * *********************************************************************/
int Energy::readGaussianLogFile(const char* logFile, FLOAT &energy, MoleculeSet* pMoleculeSet)
{
	ifstream fin(logFile);
	char line[300];
	string stringLine;
	bool foundEnergy = false;
	bool foundCoordinates = false;
	string nextLine;
	string pieceLine;
	int dummy;
	Point3D *cartesianPoints = NULL;
	int *atomicNumbers = NULL;
	int i;
	int startingIndex, endIndex;
	bool normalTermination = false;
	int returnValue = 0;
	
	if (pMoleculeSet != NULL) {
		cartesianPoints = new Point3D[(unsigned int)pMoleculeSet->getNumberOfAtoms()];
		atomicNumbers = new int[(unsigned int)pMoleculeSet->getNumberOfAtoms()];
	}

	energy = 0;
	if(!fin)
		return 0;
	else
		returnValue |= OPENED_FILE;
	while(fin.getline(line, 300))
	{
		stringLine=line;
		if (s_bSemiEmpirical)
			startingIndex=stringLine.find("Energy=   ");
		else
			startingIndex=stringLine.find("SCF Done:");
		
		if (startingIndex>=0) // If we found it
		{
			startingIndex=stringLine.find("=");
			startingIndex=stringLine.find("-",startingIndex); //energy should be negative.
			if (startingIndex < 0) { // Sometimes the energy isn't negative with semi-emperical methods
				energy = 0;
				foundEnergy=true;
				continue;
			}
			endIndex=stringLine.find(" ",startingIndex);
			
			pieceLine=stringLine.substr(startingIndex, endIndex-startingIndex);
			energy=atof(pieceLine.c_str());
			if (pMoleculeSet != NULL)
				pMoleculeSet->setEnergy(energy);
			foundEnergy=true;
		}
		if (pMoleculeSet != NULL) {
			startingIndex=stringLine.find(" 1 imaginary frequencies (negative Signs) ");
			if (startingIndex>=0) // if we found it
				pMoleculeSet->setIsTransitionState(true);

			if (s_bGetStandardOrientation)
				startingIndex=stringLine.find("Standard orientation");
			else
				startingIndex=stringLine.find("Input orientation");
			
			if (startingIndex>=0)
			{
				foundCoordinates = true;
				fin.getline(line, 300); //get ----
				fin.getline(line, 300); //get first line of text
				fin.getline(line, 300); //get second line of text
				fin.getline(line, 300); //get ----
				
				for (i = 0; i < pMoleculeSet->getNumberOfAtoms(); ++i)
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
		startingIndex=stringLine.find("Normal termination");
		if (startingIndex>=0) // if we found it
			normalTermination = true;
	}
	fin.close();
	if (pMoleculeSet != NULL) {
		if (foundCoordinates)
			pMoleculeSet->assignReadCoordinates(cartesianPoints, atomicNumbers);
		delete[] cartesianPoints;
		delete[] atomicNumbers;
	}
	
	if (foundEnergy)
		returnValue |= READ_ENERGY;
	if (foundCoordinates || (pMoleculeSet == NULL))
		returnValue |= OBTAINED_GEOMETRY;
	if (normalTermination)
		returnValue |= NORMAL_TERMINATION;

	return returnValue;
}


void Energy::lower(string &s)
{
	const char *schars = s.c_str();
	char *c = new char[s.length()+1];
	for (int i = 0; i < (signed int)s.length(); ++i)
		c[i] = tolower(schars[i]);
	c[s.length()] = '\0';
	s = c;
	
	delete[] c;
}

bool Energy::isFileCharacter(char character)
{
	return (((character >= 'a') && (character <= 'z')) ||
		    ((character >= 'A') && (character <= 'Z')) ||
		    ((character >= '0') && (character <= '9')) ||
		    (character == '.') || (character == ',') || (character == '_'));
}

bool Energy::doEnergyCalculation(int populationMemberNumber)
{
	char commandLine[500];
	try {
		switch (s_energyFunction) {
		case GAUSSIAN:
			snprintf(commandLine, sizeof(commandLine), "cp %s %s", getInputFileName(populationMemberNumber).c_str(),
			                                                getScratchInputFileName(populationMemberNumber).c_str());
			if (system(commandLine) == -1)
				throw "";
			snprintf(commandLine, sizeof(commandLine), "%s%s < %s > %s", s_scratchCommand.c_str(), s_energyProgramWithPath.c_str(),
			         getScratchInputFileName(populationMemberNumber).c_str(), getScratchOutputFileName(populationMemberNumber).c_str());
			if (system(commandLine) == -1)
				throw "";
			snprintf(commandLine, sizeof(commandLine), "cp %s %s", getScratchOutputFileName(populationMemberNumber).c_str(),
			                                                              getOutputFileName(populationMemberNumber).c_str());
			if (system(commandLine) == -1)
				throw "";
			if (s_checkPointFileName.length() > 0) {
				snprintf(commandLine, sizeof(commandLine), "cp %s %s", getScratchCheckPointFileName(populationMemberNumber).c_str(),
				                                                              getCheckPointFileName(populationMemberNumber).c_str());
				if (system(commandLine) == -1)
					throw "";
			}
			snprintf(commandLine, sizeof(commandLine), "rm %s/*", s_fullScratchDirectory.c_str());
			if (system(commandLine) == -1)
				throw "";
			break;
		}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	return true;
}

