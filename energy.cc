////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "energy.h"

Energy::Energy()
{
}

Energy::~Energy ()
{
}  

FLOAT Energy::calculateEnergies (int energyCalculationType, const Input &input, vector<MoleculeSet*> &population, vector<MoleculeSet*> &optimizedPopulation)
{
	int i;
	MoleculeSet *pMoleculeSet = NULL;
	for (i = 0; i < (signed int)optimizedPopulation.size(); ++i)
		delete optimizedPopulation[i];
	optimizedPopulation.clear();
	if (input.m_iEnergyFunction == LENNARD_JONES) {
		switch (energyCalculationType) {
			case OPTIMIZE_BUT_DONT_READ:
				for (i = 0; i < (signed int)population.size(); ++i) {
					pMoleculeSet = new MoleculeSet();
					pMoleculeSet->copy(*population[i]);
					pMoleculeSet->performLennardJonesOptimization(LENNARD_JONES_EPSILON, LENNARD_JONES_SIGMA);
					pMoleculeSet->computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
					population[i]->setEnergy(pMoleculeSet->getEnergy());
					delete pMoleculeSet;
				}
			break;
			case OPTIMIZE_AND_READ:
				for (i = 0; i < (signed int)population.size(); ++i) {
					pMoleculeSet = new MoleculeSet();
					pMoleculeSet->copy(*population[i]);
					pMoleculeSet->performLennardJonesOptimization(LENNARD_JONES_EPSILON, LENNARD_JONES_SIGMA);
					pMoleculeSet->computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
					population[i]->setEnergy(pMoleculeSet->getEnergy());
					optimizedPopulation.push_back(pMoleculeSet);
				}
			break;
			case SINGLE_POINT_ENERGY_CALCULATION:
			case TRANSITION_STATE_SEARCH:
			default:
				for (i = 0; i < (signed int)population.size(); ++i)
					population[i]->computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
			break;
		}
		return 1.0;
	} else {
		char numberString[50];
		string sId;
		string *sInputFileNames;
		string *sOutputFileNames;
		string fileNamePrefix;
		string::size_type pos;
		string lowerHeader, lowerFooter;
		bool getStandardOrientation;
		string sCheckPointFileName;
		const char *header;
		int fileNameLength;
		char fileName[500];
		struct stat fileStatistics;
		char commandLine[500];
		FLOAT energy;
		string inputFileWithoutPath;
		bool semiEmpirical;
		int gaussianReturned;
		int converged;
		
		if (input.m_sPathToScratch.length() > 0)
			inputFileWithoutPath = fileWithoutPath(input.m_sInputFileName);
		else
			inputFileWithoutPath = "";
		
		fileNamePrefix = input.m_sPathToEnergyFiles + "/" + ENERGY_TEMP_FILE;

		lowerHeader = input.m_sEnergyFileHeader;
		lower(lowerHeader);
		pos = lowerHeader.find("nosymm");
		getStandardOrientation = (pos == string::npos); // true if we didn't find it
		semiEmpirical = ((lowerHeader.find("#p") != string::npos) || (lowerHeader.find("# p") != string::npos));
		
		sInputFileNames = new string[population.size()];
		sOutputFileNames = new string[population.size()];

		//////////// Get the name of the checkpoint file  //////////////////
		sCheckPointFileName = "";
		pos = lowerHeader.find("% chk");
		if (pos == string::npos) // if we didn't find it
			pos = lowerHeader.find("%chk");
		if (pos == string::npos) { // if we didn't find it
			if (input.m_bTransitionStateSearch) {
				cout << "Unable to locate checkpoint file name.  Quitting..." << endl;
				exit(0);
			}
		} else {
			// find the beginning of the name
			header = input.m_sEnergyFileHeader.c_str();
			header = &header[pos];
			while (*header != '=')
				++header;
			while ((*header != '\0') && !isFileCharacter(*header))
				++header;
			if ((*header == '\0') && input.m_bTransitionStateSearch) {
				cout << "Unable to locate checkpoint file name where it should be.  Quitting..." << endl;
				exit(0);
			}
			fileNameLength = 1;
			while (isFileCharacter(header[fileNameLength]))
				++fileNameLength;
			
			strncpy(fileName,header,fileNameLength);
			fileName[fileNameLength] = '\0';
			sCheckPointFileName = fileName;

			if (input.m_sEnergyFileFooter.length() > 0) {
				pos = input.m_sEnergyFileFooter.find(sCheckPointFileName);
				if (pos == string::npos) { // If we didn't find it
					lowerFooter = input.m_sEnergyFileFooter;
					lower(lowerFooter);
					pos = lowerFooter.find("chk");
					if (pos == string::npos) { // If we didn't find it
						cout << "The checkpoint file that is specified in the header is not in the footer.  Quitting..." << endl;
						exit(0);
					}
				}
			}
		}
		//////////// End get the name of the checkpoint file  //////////////////
		
		// Create the input files
		for (i = 0; i < (signed int)population.size(); ++i) {
			sprintf(numberString, "%d", (i+1));
			sId = numberString;
			sInputFileNames[i] = fileNamePrefix + sId + ".com";
			sOutputFileNames[i] = fileNamePrefix + sId + ".log";
			createGaussianInputFile(sInputFileNames[i], sCheckPointFileName, sId, input, *population[i], false);
			
			// Delete the old log file if it exists
			if (stat(sOutputFileNames[i].c_str(), &fileStatistics) == 0) { // If no errors occurred in getting stats, the file exists
				snprintf(commandLine, 500, "rm %s", sOutputFileNames[i].c_str());
				system(commandLine);
			}
		}
		
		// Do the energy calculations
		
		// This line uses mpirun to start the mpi program that runs Gaussian.
		// Modify the options according to your system setup.
		//snprintf(commandLine, 500, "mpirun -nolocal -v -np %d -machinefile %s ./mpi %s %s %d %s %s", (signed int)input.m_srgNodeNames.size(), input.m_sNodesFile.c_str(), input.m_sPathToEnergyProgram.c_str(), fileNamePrefix.c_str(), (signed int)population.size(), input.m_sPathToScratch.c_str(), inputFileWithoutPath.c_str());
		
		// This line uses mpiexec to start the mpi program that runs Gaussian.
		// Only use this line or the one above, but not both.
		snprintf(commandLine, 500, "mpiexec ./mpi %s %s %d %s %s", input.m_sPathToEnergyProgram.c_str(), fileNamePrefix.c_str(), (signed int)population.size(), input.m_sPathToScratch.c_str(), inputFileWithoutPath.c_str());
		
		system(commandLine);
		
		// Read the output files
		converged = 0;
		for (i = 0; i < (signed int)population.size(); ++i) {
			population[i]->setEnergyFile(sOutputFileNames[i].c_str());
			if ((energyCalculationType == TRANSITION_STATE_SEARCH) || (energyCalculationType == OPTIMIZE_AND_READ)) {
				pMoleculeSet = new MoleculeSet();
				pMoleculeSet->copy(*population[i]);
			}
			gaussianReturned = readGaussianLogFile(sOutputFileNames[i], energy, pMoleculeSet, getStandardOrientation, semiEmpirical);
			if (!(gaussianReturned & OPENED_FILE)) {
				cerr << "Could not open file  '" << sOutputFileNames[i] << "'.  Exiting... " <<endl;
				exit(0);
			}
			if ((gaussianReturned & READ_ENERGY) && (gaussianReturned & OBTAINED_GEOMETRY))
				++converged;
			else
				cerr << "Opened this file, but could not read it: '" << sOutputFileNames[i] << "'." <<endl;
			if (((energyCalculationType == TRANSITION_STATE_SEARCH) && pMoleculeSet->getIsTransitionState()) ||
			    (energyCalculationType == OPTIMIZE_AND_READ))
				optimizedPopulation.push_back(pMoleculeSet);
			if ((energyCalculationType == TRANSITION_STATE_SEARCH) && pMoleculeSet->getIsTransitionState()) {
				cout << "Found transition state:" << endl;
				population[i]->printToScreen();
			}
			population[i]->setEnergy(energy);
		}
		delete[] sInputFileNames;
		delete[] sOutputFileNames;
		return (FLOAT)converged / (FLOAT)population.size();
	}
	return 1.0;
}

void Energy::createGaussianInputFile(string &inputFileName, string &sCheckPointFileName, string &sId, const Input &input, MoleculeSet &moleculeSet, bool writeEnergyValueInHeader)
{
	string header = input.m_sEnergyFileHeader;
	string footer = input.m_sEnergyFileFooter;
	string::size_type pos;
	
	// Rename the checkpoint file if there is one
	if (sCheckPointFileName.length() > 0) {
		pos = header.find(sCheckPointFileName);
		header.replace(pos,sCheckPointFileName.length(),
		               input.m_sPathToEnergyFiles+"/"+sCheckPointFileName+sId);
		if (footer.length() > 0) {
			pos = footer.find(sCheckPointFileName);
			if (pos != string::npos) // If we found it
				footer.replace(pos,sCheckPointFileName.length(),
					input.m_sPathToEnergyFiles+"/"+sCheckPointFileName+sId);
		}
	}
	
	ofstream fout(inputFileName.c_str(), ios::out);
	fout << header << endl;
	if (writeEnergyValueInHeader)
		fout << "This is a computer generated structure with energy: " << Atom::printFloat(moleculeSet.getEnergy()) << endl << endl;
	else
		fout << "This is a computer generated structure." << endl << endl;
	fout << input.m_iCharge << " " << input.m_iMultiplicity << endl;

	moleculeSet.writeToGausianComFile(fout);

	fout << endl;

	if (footer.length() > 0) {
		fout << footer << endl
		     << "Title" << endl << endl
		     << input.m_iCharge << " " << input.m_iMultiplicity << endl;
	}
	fout.close();
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

string Energy::fileWithoutPath(const string &s)
{
	string::size_type pos;
	string answer;
	
	pos = s.rfind("/");
	if (pos == string::npos) // if we didn't find it
		return s;
	answer = s.c_str() + pos + 1;
	return answer;
}

/************************************************************************
 * Function looking for the SCF-energy and extracting it from the log-file
 * *********************************************************************/
int Energy::readGaussianLogFile(const string &logFile, FLOAT &energy, MoleculeSet* pMoleculeSet, bool getStandardOrientation, bool semiEmpirical)
{
	ifstream fin(logFile.c_str());
	char line[300];
	string stringLine;
	bool foundEnergy = false;
	bool foundCoordinates = false;
	string nextLine;
	string pieceLine;
	int dummy;
	Point3D *cartesianPoints;
	int *atomicNumbers;
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
		if (semiEmpirical)
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

			if (getStandardOrientation)
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

void Energy::removeFirstNode(const Input &input)
{
	fstream fout(input.m_sNodesFile.c_str(), ios::out);
	
	if (!fout) {
		cout << "Can't write to file: " << input.m_sNodesFile << endl;
		exit(0);
	}
	for (unsigned int i = 1; i < input.m_srgNodeNames.size(); ++i)
		fout << input.m_srgNodeNames[i] << endl;
	fout << endl;

        fout.close();
}

