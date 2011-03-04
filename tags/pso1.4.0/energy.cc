////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "energy.h"

string Energy::s_header = "";
string Energy::s_footer = "";
string Energy::s_checkPointFileName = "";
bool Energy::s_bGetStandardOrientation = true;
bool Energy::s_bSemiEmpirical = false;
string Energy::s_pathToEnergyFiles = "";
int Energy::s_iCharge = 0;
int Energy::s_iMultiplicity = 0;
EnergyProgram Energy::s_energyProgram;
string Energy::s_scratchDirectory = "";
string Energy::s_fullScratchDirectory = "";
string Energy::s_scratchCommand = "";

bool Energy::init(const Input &input, int rank)
{
	bool success = true;
	s_energyProgram.copy(*input.m_pSelectedEnergyProgram);
	
	if (input.m_sPathToScratch.length() > 0) {
		s_scratchDirectory = input.m_sPathToScratch + "/" + Input::fileWithoutPath(input.m_sInputFileName.c_str());
		s_fullScratchDirectory = s_scratchDirectory.c_str() + ToString(rank);
	} else {
		s_scratchDirectory = "";
		s_fullScratchDirectory = "";
	}

	s_pathToEnergyFiles = input.m_sPathToEnergyFiles;
	s_header = input.m_sEnergyFileHeader;
	s_footer = input.m_sEnergyFileFooter;
	s_iCharge = input.m_iCharge;
	s_iMultiplicity = input.m_iMultiplicity;

	
	switch (s_energyProgram.m_iProgramID) {
	case GAUSSIAN:
	case GAUSSIAN_WITH_CCLIB:
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
	switch (s_energyProgram.m_iProgramID) {
	case GAUSSIAN:
	case GAUSSIAN_WITH_CCLIB:
		snprintf(mpiInitMessage, maxSize, "%s|%s|%s|%s", s_energyProgram.toString().c_str(), s_pathToEnergyFiles.c_str(),
		         s_scratchDirectory.c_str(), s_checkPointFileName.c_str());
		break;
	default:
		snprintf(mpiInitMessage, maxSize, "%s|%s|%s", s_energyProgram.toString().c_str(), s_pathToEnergyFiles.c_str(),
		         s_scratchDirectory.c_str());
		break;
	}
}

bool Energy::createScratchDir(void)
{
	char commandLine[500];
		
	try {
		if (!deleteScratchDir())
			throw "Couldn't delete scratch directory";
		
		if (s_fullScratchDirectory.length() > 0) {
			if (!MoleculeSet::fileExists(s_fullScratchDirectory.c_str())) {
				snprintf(commandLine, sizeof(commandLine), "mkdir %s", s_fullScratchDirectory.c_str());
				if (system(commandLine) == -1)
					throw "Couldn't make scratch directory";
			}
			if (chdir(s_fullScratchDirectory.c_str()) == -1)
				throw "Couldn't cd to scratch directory";
		}
	} catch (const char *message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
		return false;
	}

	return true;
}

bool Energy::deleteScratchDir(void)
{
	char commandLine[500];

	if ((s_fullScratchDirectory.length() > 0) && MoleculeSet::fileExists(s_fullScratchDirectory.c_str())) {
		snprintf(commandLine, sizeof(commandLine), "rm -rf %s", s_fullScratchDirectory.c_str());
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
	int i;
	
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
	if (!s_energyProgram.set(messages))
		return false;
	
	switch (s_energyProgram.m_iProgramID) {
	case GAUSSIAN:
	case GAUSSIAN_WITH_CCLIB:
		i = s_energyProgram.getNumParameters();
		if (i+3 != (signed int)messages.size())
			return false;
		s_pathToEnergyFiles = messages[i++];
		s_scratchDirectory = messages[i++];
		s_checkPointFileName = messages[i++];
		if (s_scratchDirectory.length() > 0) {
			s_fullScratchDirectory = s_scratchDirectory.c_str() + ToString(rank);
			s_scratchCommand = "export GAUSS_SCRDIR=" + s_fullScratchDirectory + " && ";
//			s_scratchCommand = "env SCRDIR=" + s_fullScratchDirectory + " ";
		} else {
			s_fullScratchDirectory = "";
		}

		break;
	default:
		i = s_energyProgram.getNumParameters();
		if (i+2 != (signed int)messages.size())
			return false;
		s_pathToEnergyFiles = messages[i++];
		s_scratchDirectory = messages[i++];
		if (s_scratchDirectory.length() > 0) {
			s_fullScratchDirectory = s_scratchDirectory.c_str() + ToString(rank);
			s_scratchCommand = "";
		} else {
			s_fullScratchDirectory = "";
		}

		break;
	}
	for (int i = 0; i < (int)messages.size(); ++i)
		delete[] messages[i];
	messages.clear();
	return success;
}

void Energy::createInputFiles(vector<MoleculeSet*> &population)
{
	for (int i = 0; i < (signed int)population.size(); ++i)
		createInputFile(*population[i], i+1, true, false);
}

bool Energy::createInputFile(MoleculeSet &moleculeSet, int populationMemberNumber, bool resetInputFileName, bool writeMetaData)
{
	string fileName;

	if (resetInputFileName)
		moleculeSet.setInputEnergyFile(s_pathToEnergyFiles.c_str(), ENERGY_TEMP_FILE, populationMemberNumber, s_energyProgram.m_sInputFileExtension.c_str());
	setOutputEnergyFiles(populationMemberNumber, moleculeSet, false); // We expect these output files
	
	// Create the input files
	switch (s_energyProgram.m_iProgramID) {
	case GAUSSIAN:
	case GAUSSIAN_WITH_CCLIB:
		Energy::createGaussianInputFile(moleculeSet, populationMemberNumber, writeMetaData);
		break;
	default:
		cout << "Please modify the createInputFile function in energy.cc for your energy program: " << s_energyProgram.m_sName << endl;
		exit(0);
		break;
	}
	moleculeSet.deleteOutputEnergyFiles(false); // Delete any old output files
	return true;
}

bool Energy::createGaussianInputFile(MoleculeSet &moleculeSet, int populationMemberNumber, bool writeEnergyValueInHeader)
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
	
	ofstream fout(moleculeSet.getInputEnergyFile(), ios::out);
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
	return true;
}

void Energy::readOutputFile(const char* outputFileName, FLOAT &energy, MoleculeSet* pMoleculeSet,
                            bool &openedFile, bool &readEnergy, bool &obtainedGeometry)
{
	bool normalTerminationOfGaussian, cclibInstalled;

	if (s_energyProgram.m_bUsesCclib) {
		readOutputFileWithCCLib(outputFileName, energy, pMoleculeSet, cclibInstalled, openedFile, readEnergy, obtainedGeometry);
		if (!cclibInstalled) {
			cout << "Please install cclib!" << endl;
			exit(0);
		}
	} else {
		switch (s_energyProgram.m_iProgramID) {
			case GAUSSIAN:
				readGaussianOutputFile(outputFileName, energy, pMoleculeSet, openedFile, readEnergy, obtainedGeometry, normalTerminationOfGaussian);
				break;
			default:
				cout << "Please modify the readOutputFile function in energy.cc for your energy program: " << s_energyProgram.m_sName << endl;
				exit(0);
				break;
		}
	}
}

/************************************************************************
 * Function looking for the SCF-energy and extracting it from the log-file
 * *********************************************************************/
void Energy::readGaussianOutputFile(const char* logFile, FLOAT &energy, MoleculeSet* pMoleculeSet,
                                    bool &openedFile, bool &readEnergy, bool &obtainedGeometry, bool &normalTermination)
{
	ifstream fin(logFile);
	char line[300];
	string stringLine;
	string nextLine;
	string pieceLine;
	int dummy;
	Point3D *cartesianPoints = NULL;
	int *atomicNumbers = NULL;
	int i;
	int startingIndex, endIndex;

	openedFile = false;
	readEnergy = false;
	obtainedGeometry = false;
	normalTermination = false;
	
	if(!fin)
		return;
	openedFile = true;
	
	if (pMoleculeSet != NULL) {
		cartesianPoints = new Point3D[(unsigned int)pMoleculeSet->getNumberOfAtoms()];
		atomicNumbers = new int[(unsigned int)pMoleculeSet->getNumberOfAtoms()];
	}

	energy = 0;
	try {
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
					readEnergy=true;
					continue;
				}
				endIndex=stringLine.find(" ",startingIndex);
			
				pieceLine=stringLine.substr(startingIndex, endIndex-startingIndex);
				energy=atof(pieceLine.c_str());
				if (pMoleculeSet != NULL)
					pMoleculeSet->setEnergy(energy);
				readEnergy=true;
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
					obtainedGeometry = true;
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
		if (obtainedGeometry && (pMoleculeSet != NULL))
			pMoleculeSet->assignReadCoordinates(cartesianPoints, atomicNumbers);
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: '" << message << "'" << endl;
	}
	
	if (atomicNumbers != NULL)
		delete[] atomicNumbers;
	if (cartesianPoints != NULL)
		delete[] cartesianPoints;
}

void Energy::readOutputFileWithCCLib(const char* fileName, FLOAT &energy, MoleculeSet* pMoleculeSet,
                                     bool &cclibInstalled, bool &openedFile, bool &readEnergy, bool &obtainedGeometry)
{
	char cmd[500];
	char line[500];
	int numAtoms;
	int lineLength;
	Point3D *cartesianPoints = NULL;
	int *atomicNumbers = NULL;
	int i;
	char* myString;
	const char* infoNotFoundFlag = "not found\n";
	bool isTransitionState;

	cclibInstalled = false;
	openedFile = false;
	readEnergy = false;
	obtainedGeometry = false;
	energy = 0;
	isTransitionState = false;

	if (pMoleculeSet != NULL)
		snprintf(cmd, sizeof(cmd), "python %s %s scfenergies_last_au isTransitionState natom atomnos atomcoords_last", EnergyProgram::cclibPythonScript, fileName);
	else
		snprintf(cmd, sizeof(cmd), "python %s %s scfenergies_last_au", EnergyProgram::cclibPythonScript, fileName);
//	cout << "executing: " << cmd << endl;
	FILE* pipe;
	try {
		pipe = popen(cmd, "r");
		if (pipe == NULL)
			throw "Unable to call cclib!";
		if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
			throw "Unable get output from cclib.";
		cclibInstalled = true;
		
		if ((strncmp("I/O error 2 (", line, 13) == 0) && (strstr(line, fileName) != 0)) {
			cout << line;
			throw "Could not upen the output file in readOutputFileWithCCLib.";
		}
		openedFile = true;

		if (strcmp(line,infoNotFoundFlag) == 0)
			throw "The energy was not found in the output file.";
		energy=atof(line);
		readEnergy = true;

		if (pMoleculeSet != NULL) {
			pMoleculeSet->setEnergy(energy);
			
			if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
				throw "Error reading transition state information from cclib.";
			if (strcmp(line,infoNotFoundFlag) == 0)
				isTransitionState = false;
			else
				isTransitionState = (bool)atoi(line);
			pMoleculeSet->setIsTransitionState(isTransitionState);
			
			if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
				throw "Error reading the number of atoms from from cclib.";
			if (strcmp(line,infoNotFoundFlag) == 0)
				throw "The number of atoms was not found in the output file.";
			numAtoms = atoi(line);
			if (numAtoms <= 0)
				throw "The number of atoms returned from cclib must be greater than zero.";
		
			atomicNumbers = new int[(unsigned int)numAtoms];
			i = 0;
			while (i < numAtoms) {
				if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
					throw "Error reading atomic number line from cclib.";
				if (strcmp(line,infoNotFoundFlag) == 0)
					throw "The atomic numbers were not found in the output file.";
				lineLength = strlen(line);
				if (lineLength < 2)
					throw "cclib atomic number line length is less than the minimum.";
			
				// get rid of brakets
				line[0] = ' ';
				if (line[lineLength-2] == ']')
					line[lineLength-2] = ' ';
			
				myString = strtok(line, " ");
				while ((myString != NULL) && (i < numAtoms)) {
					atomicNumbers[i] = atoi(myString);
					++i;
					myString = strtok(NULL, " ");
				}
			}
			if (i != numAtoms)
				throw "Failed to read the atomic numbers in Energy::readOutputFileWithCCLib.";
		
			cartesianPoints = new Point3D[(unsigned int)numAtoms];
			for (i = 0; i < numAtoms; ++i) {
				if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL))
					throw "Error reading geometry line from cclib.";
				if (strcmp(line,infoNotFoundFlag) == 0)
					throw "The geometry was not found in the output file.";
				lineLength = strlen(line);
				if (lineLength < 8)
					throw "cclib geometry line length is less than the minimum.";

				// get rid of brakets
				line[0] = ' ';
				line[1] = ' ';
				line[lineLength-2] = ' ';
				if (line[lineLength-3] == ']')
					line[lineLength-3] = ' ';
				if (sscanf(line, "%lf %lf %lf", &cartesianPoints[i].x, &cartesianPoints[i].y, &cartesianPoints[i].z) != 3)
					throw "Error reading cartesian coordinates in Energy::readOutputFileWithCCLib";
			}
			obtainedGeometry = true;
			if (numAtoms == pMoleculeSet->getNumberOfAtoms())
				pMoleculeSet->assignReadCoordinates(cartesianPoints, atomicNumbers);
			else {
				vector<Point3D> cartesianPointVector;
				vector<int> atomicNumberVector;
				for (i = 0; i < numAtoms; ++i) {
					cartesianPointVector.push_back(cartesianPoints[i]);
					atomicNumberVector.push_back(atomicNumbers[i]);
				}
				
				Molecule tempMolecule;
				tempMolecule.makeFromCartesian(cartesianPointVector, atomicNumberVector);
				tempMolecule.initRotationMatrix();
				tempMolecule.localToGlobal();

				pMoleculeSet->setNumberOfMolecules(1);
				Molecule* moleculeArray = pMoleculeSet->getMolecules();
				moleculeArray[0].copy(tempMolecule);
				pMoleculeSet->setEnergy(energy); // do this again, since we recreated the moleculeSet
				pMoleculeSet->setIsTransitionState(isTransitionState); // do this again, since we recreated the moleculeSet
			}
		}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: '" << message << "'" << endl;
	}
	if (atomicNumbers != NULL)
		delete[] atomicNumbers;
	if (cartesianPoints != NULL)
		delete[] cartesianPoints;
	if (pipe != NULL)
		pclose(pipe);
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

void Energy::setOutputEnergyFiles(int populationMemberNumber, MoleculeSet &moleculeSet, bool checkExistence)
{
	switch (s_energyProgram.m_iProgramID) {
	case GAUSSIAN:
	case GAUSSIAN_WITH_CCLIB:
		moleculeSet.setOutputEnergyFile(s_pathToEnergyFiles.c_str(), ENERGY_TEMP_FILE, populationMemberNumber, s_energyProgram.m_sOutputFileTypeExtensions[0].c_str(), 0, checkExistence);
		if (s_checkPointFileName.length() > 0)
			moleculeSet.setOutputEnergyFile(s_pathToEnergyFiles.c_str(), s_checkPointFileName.c_str(), populationMemberNumber, s_energyProgram.m_sOutputFileTypeExtensions[1].c_str(), 1, checkExistence);
		break;
	default:
		for (int i = 0; i < s_energyProgram.m_iNumOutputFileTypes; ++i)
     		moleculeSet.setOutputEnergyFile(s_pathToEnergyFiles.c_str(), ENERGY_TEMP_FILE, populationMemberNumber, s_energyProgram.m_sOutputFileTypeExtensions[i].c_str(), i, checkExistence);
		break;
	}
}

bool Energy::doEnergyCalculation(int populationMemberNumber)
{
	char commandLine[1000];
	MoleculeSet moleculeSet;

	try {
		moleculeSet.setInputEnergyFile(s_pathToEnergyFiles.c_str(), ENERGY_TEMP_FILE, populationMemberNumber, s_energyProgram.m_sInputFileExtension.c_str());
		if (s_fullScratchDirectory.length() > 0)
			if (!moleculeSet.moveOrCopyInputEnergyFile("", false))
			    throw "Unable to copy input enery file to the scratch directory.";
		                                           
		switch (s_energyProgram.m_iProgramID) {
		case GAUSSIAN:
		case GAUSSIAN_WITH_CCLIB:
			if (s_fullScratchDirectory.length() > 0)
				moleculeSet.setOutputEnergyFile("", ENERGY_TEMP_FILE, populationMemberNumber, s_energyProgram.m_sOutputFileTypeExtensions[0].c_str(), 0, false);
			else
				moleculeSet.setOutputEnergyFile(s_pathToEnergyFiles.c_str(), ENERGY_TEMP_FILE, populationMemberNumber, s_energyProgram.m_sOutputFileTypeExtensions[0].c_str(), 0, false);

			snprintf(commandLine, sizeof(commandLine), "%s%s < %s > %s", s_scratchCommand.c_str(), s_energyProgram.m_sPathToExecutable.c_str(),
			         moleculeSet.getInputEnergyFile(), moleculeSet.getOutputEnergyFile(0));
			if (system(commandLine) == -1)
				throw "Unable to run Gaussian.";
			
			if (s_checkPointFileName.length() > 0) {
				if (s_fullScratchDirectory.length() > 0)
					moleculeSet.setOutputEnergyFile("", s_checkPointFileName.c_str(), populationMemberNumber, s_energyProgram.m_sOutputFileTypeExtensions[1].c_str(), 1, true);
				else
					moleculeSet.setOutputEnergyFile(s_pathToEnergyFiles.c_str(), s_checkPointFileName.c_str(), populationMemberNumber, s_energyProgram.m_sOutputFileTypeExtensions[1].c_str(), 1, true);
			}
			break;
		default:
			cout << "Please modify the doEnergyCalculation function and indicate how '" << s_energyProgram.m_sName << "' should be called." << endl;
			exit(0);
			break;
		}
		if (s_fullScratchDirectory.length() > 0) {
			if (!moleculeSet.moveOrCopyOutputEnergyFiles(s_pathToEnergyFiles.c_str(), false))
				throw "Unable to copy output energy files from the scratch directory.";
			snprintf(commandLine, sizeof(commandLine), "rm %s/*", s_fullScratchDirectory.c_str());
			if (system(commandLine) == -1)
				throw "Unable to delete files in the scratch directory.";
		}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	return true;
}

