////////////////////////////////////////////////////////////////////////////////
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "main.h"

/*
bool testDistanceInitialization(string inputFileName)
{
	Input input;
	Energy *pEnergy;
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	bool returnValue = true;
	
	cout << "Testing Distance Initialization" << endl;
	
	cout << "Reading Input File: " << inputFileName << endl;
	if (!input.open(inputFileName, moleculeSets, bestNMoleculeSets))
	{
		return false;
	}
	Molecule::initRandoms();
	
	MoleculeSet *newMoleculeSet = new MoleculeSet();
	newMoleculeSet->copy(input.m_tempelateMoleculeSet);
	
	pEnergy = new Energy(&input,0, "LOCAL", true);
	do {
//		newMoleculeSet->initPositionsAndAngles(input.m_boxDimensions, 10);
		newMoleculeSet->initPositionsAndAnglesWithMaxDist(input.m_boxDimensions, input.m_fMaxAtomDistance, 10);
		pEnergy->calculateEnergy(*newMoleculeSet);
	} while (!pEnergy->getConverged());
	newMoleculeSet->setEnergy(pEnergy->getEnergy());
	newMoleculeSet->printRankInfo();
	
	if (!newMoleculeSet->checkMaxDistConstraints(input.m_fMaxAtomDistance)) {
		cout << "Error: Maximum distance constraint violated!" << endl;
		returnValue = false;
	} else
		cout << "Good: Maximum distance constraint not violated!" << endl;
	
	
	delete newMoleculeSet;
	delete pEnergy;
	cout << "Finished!!" << endl;
	return returnValue;
} */

/*
bool testComparisons(string inputFileName)
{
	Input input;
	Energy *pEnergy;
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	bool returnValue = true;
	
	cout << "Testing Distance Initialization" << endl;
	
	cout << "Reading Input File: " << inputFileName << endl;
	if (!input.open(inputFileName, moleculeSets, bestNMoleculeSets))
	{
		return false;
	}
	initRandoms();
	
	MoleculeSet *newMoleculeSet = new MoleculeSet();
	newMoleculeSet->copy(input.m_tempelateMoleculeSet);

	pEnergy = new Energy(&input,0, "LOCAL", false);
	do {
		newMoleculeSet->initPositionsAndAngles(input.m_boxDimensions, 500);
		pEnergy->calculateEnergy(*newMoleculeSet);
	} while (!pEnergy->getConverged());
	newMoleculeSet->setEnergy(pEnergy->getEnergy());
	
	MoleculeSet *newMoleculeSet2 = new MoleculeSet();
	newMoleculeSet2->copy(*newMoleculeSet);
	
	if (!newMoleculeSet->moleculeSetsTheSame(*newMoleculeSet2,0.000001)) {
		cout << "Error: Molecule set coppies souldn't be different but they are!" << endl;
		returnValue = false;
	} else
		cout << "Good: Molecule set coppies sould not be different and they are not." << endl;
	
	cout << "Perturbing a copy..." << endl;
	
	newMoleculeSet2->performTransformation(input.m_boxDimensions, input.m_fStartCoordinatePerturbation,
		input.m_fStartAnglePerturbation);
	
	if (newMoleculeSet->moleculeSetsTheSame(*newMoleculeSet2,0.000001)) {
		cout << "Error: Different molecule sets sould be different but they aren't!" << endl;
		returnValue = false;
	} else
		cout << "Good: Different molecule sets sould be different and they are." << endl;
	
	if (newMoleculeSet->moleculeSetsTheSame(*newMoleculeSet2,1)) {
		cout << "Good: Slightly different molecule sets shouldn't be that different and they aren't." << endl;
	} else {
		cout << "Error: Slightly different molecule sets shouldn't be that different but they are!" << endl;
		returnValue = false;
	}
	
	delete newMoleculeSet;
	delete newMoleculeSet2;
	delete pEnergy;
	cout << "Finished!!" << endl;
	return returnValue;
}

bool testWriteAndReadResumeFile(string inputFileName)
{
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	vector<MoleculeSet*> readMoleculeSets;
	vector<MoleculeSet*> readBestNMoleculeSets;
	MoleculeSet* newMoleculeSet;
	Input input;
	Input readInput;
	Energy *pEnergy;
	int i;
	bool returnValue = true; // Success
	
	cout << "Testing write of resume file..." << endl;
	
	cout << "Reading Input File: " << inputFileName << endl;
	if (!input.open(inputFileName, moleculeSets, bestNMoleculeSets))
	{
		cout << "Error reading input file, stopping test...";
		return false;
	}
	initRandoms();
	
	// Create the initial population based on the template molecule set
	cout << "Creating Initial Population..." << endl;
	pEnergy = new Energy(&input,0, "LOCAL", false);
	for (i = 0; i < input.m_i3DStructures; ++i)
	{
		newMoleculeSet = new MoleculeSet();
		newMoleculeSet->copy(input.m_tempelateMoleculeSet);
		do {
			newMoleculeSet->initPositionsAndAngles(input.m_boxDimensions, 500);
			pEnergy->calculateEnergy(*newMoleculeSet);
		} while (!pEnergy->getConverged());
		newMoleculeSet->setEnergy(pEnergy->getEnergy());
		moleculeSets.push_back(newMoleculeSet);
	}
	
	cout << "Finding Best N..." << endl;
	saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
                         input.m_fMinDistnaceBetweenSameMoleculeSets);
   
	cout << "Writing resume file..." << endl;
	string resumeFileName = "unitTestFiles/resume.res";
	input.m_iIteration = 101;
	readInput.m_iIteration = -1;
	input.writeResumeFile(resumeFileName, moleculeSets, bestNMoleculeSets, 0, 50);
	
	cout << "Reading resume file..." << endl;
	if (!readInput.open(resumeFileName, readMoleculeSets, readBestNMoleculeSets)) {
		cout << "There was an error when reading the resume file, stopping test..." << endl;
		returnValue = false;
	}
	if (returnValue == true)
		if (input.m_iIteration != readInput.m_iIteration) {
			cout << "Error: Iteration number in resume file different that original iteration number!" << endl;
			returnValue = false;
		}
	if (returnValue == true)
		if ((signed int)moleculeSets.size() != (signed int)readMoleculeSets.size()) {
			cout << "Error: Different number of molecule sets in the resume file than in the original set!" << endl;
			returnValue = false;
		}
	if (returnValue == true)
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			if (!moleculeSets[i]->moleculeSetsTheSame(*readMoleculeSets[i],1.0e-12)) {
				cout << "Error: Molecule sets loaded from resume file are different than original molecule sets!" << endl;
				returnValue = false;
				break;
			}
	if (returnValue == true)
		if ((signed int)bestNMoleculeSets.size() != (signed int)readBestNMoleculeSets.size()) {
			cout << "Error: Different number of best molecule sets in the resume file than in the original best set!" << endl;
			returnValue = false;
		}
	if (returnValue == true)
		for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			if (!bestNMoleculeSets[i]->moleculeSetsTheSame(*readBestNMoleculeSets[i],1.0e-12)) {
				cout << "Error: Best molecule sets loaded from resume file are different than original best molecule sets!" << endl;
				returnValue = false;
				break;
			}
	
	if (returnValue == true)
	{
		cout << "Good: All molecule sets read from the resume file seem to be identical to the original molecule sets." << endl;
		cout << "Writing second resume file..." << endl;
		string resumeFileName2 = "unitTestFiles/resume2.res";
		input.writeResumeFile(resumeFileName2, readMoleculeSets, readBestNMoleculeSets, 0, 50);
		ifstream infile(resumeFileName.c_str());
		if (!infile)
		{
			cout << "Can't open the file: " << resumeFileName << endl;
			returnValue = false;
		}
		ifstream infile2(resumeFileName2.c_str());
		if (!infile2)
		{
			cout << "Can't open the file: " << resumeFileName2 << endl;
			returnValue = false;
		}
		if (returnValue == true) {
			const int MAX_LINE_LENGTH = 500;
			char fileLine[MAX_LINE_LENGTH];
			char fileLine2[MAX_LINE_LENGTH];
			bool readLineOK, readLine2OK;
			int lineNumber = 0;
			while (true) {
				readLineOK = infile.getline(fileLine, MAX_LINE_LENGTH);
				readLine2OK = infile2.getline(fileLine2, MAX_LINE_LENGTH);
				if ((readLineOK && !readLine2OK) || (!readLineOK && readLine2OK)) {
					cout << "There are a different number of lines in the two resume files." << endl;
					returnValue = false;
					break;
				}
				if (!readLineOK && !readLine2OK)
					break;
				++lineNumber;
				if (strncmp(fileLine,fileLine2,MAX_LINE_LENGTH) != 0)
				{
					cout << "Line " << lineNumber << " is different in the two resume files." << endl;
					returnValue = false;
					break;
				}
			}
		}
		if (returnValue == true)
			cout << "Good: The two resume files are exactly the same!" << endl;
		infile.close();
		infile2.close();
	}
	
		
	// Clean up
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		delete moleculeSets[i];
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	moleculeSets.clear();
	bestNMoleculeSets.clear();
	for (i = 0; i < (signed int)readMoleculeSets.size(); ++i)
		delete readMoleculeSets[i];
	for (i = 0; i < (signed int)readBestNMoleculeSets.size(); ++i)
		delete readBestNMoleculeSets[i];
	readMoleculeSets.clear();
	readBestNMoleculeSets.clear();
	delete pEnergy;
	cout << "Test Finished!!" << endl << endl;
	return returnValue;
}*/

int main(int argc, char *argv[])
{
	string inputFileName = "/home/boldyrev/seth/runs/something.txt";
	Input input;
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	vector<MoleculeSet*> bestIndividualMoleculeSets; // The best solution found for each individual (used in particle swarm optimization)

	Molecule::initRandoms();
	cout << "Hello world!" << endl;

	
//	string inputFileName = "unitTestFiles/testInput.txt";
//	if (!testDistanceInitialization(inputFileName))
//		return 0;
//	if (!testComparisons(inputFileName))
//		return 0;
//	if (!testWriteAndReadResumeFile(inputFileName))
//		return 0;
	return 0;
}

