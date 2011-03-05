////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "unit.h"

bool memoryTest()
{
	MoleculeSet* pMoleculeSet;
	
	cout << endl;
	cout << "Testing memory deletion of MoleculeSet objects..." << endl;

	pMoleculeSet = new MoleculeSet();
	pMoleculeSet->setOutputEnergyFile(".", "best", 1, "com", 0, false);
	pMoleculeSet->setOutputEnergyFile(".", "best", 1, "com", 1, false);
	delete pMoleculeSet;

	pMoleculeSet = new MoleculeSet();
	pMoleculeSet->setOutputEnergyFile(".", "best", 1, "com", 1, false);
	pMoleculeSet->setOutputEnergyFile(".", "best", 1, "com", 0, false);
	delete pMoleculeSet;

	pMoleculeSet = new MoleculeSet();
	pMoleculeSet->setOutputEnergyFile(".", "shouldNeverExist", 1, "bogusExtension", 3, true);
	pMoleculeSet->setOutputEnergyFile(".", "shouldNeverExist", 1, "bogusExtension", 29, true);
	delete pMoleculeSet;
	
	cout << "Testing memory deletion of MoleculeSet objects passed!" << endl;
	
	return true;
}

bool ccLibReadTest()
{
	string unitTestFile = unitTestFolder + "/benzenesulfonamide.gamout";
	string unitTestFile2 = unitTestFolder + "/transitionState.log";
	FLOAT energy;
	MoleculeSet moleculeSet;
	bool cclibInstalled, openedFile, readEnergy, obtainedGeometry;
	
	cout << endl;
	cout << "Testing cclib..." << endl;

	cout << "Reading the energy only from file: " << unitTestFile.c_str() << endl;
	Energy::readOutputFileWithCCLib(unitTestFile.c_str(), energy, NULL, cclibInstalled, openedFile, readEnergy, obtainedGeometry);
	if (!cclibInstalled) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: cclib is not installed correctly!" << endl;
		return false;
	}
	if (!openedFile) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to open test file: " << unitTestFile << endl;
		return false;
	}
	if (!readEnergy) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the energy from: " << unitTestFile << endl;
		return false;
	}
	cout << "Energy: " << energy << " au " << endl;
	if (obtainedGeometry) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Read geometry when it wasn't supposed to have been read from: " << unitTestFile << endl;
		return false;
	}

	cout << "Reading energy and geometry from file: " << unitTestFile.c_str() << endl;
	Energy::readOutputFileWithCCLib(unitTestFile.c_str(), energy, &moleculeSet, cclibInstalled, openedFile, readEnergy, obtainedGeometry);
	if (!openedFile) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to open test file: " << unitTestFile << endl;
		return false;
	}
	if (!readEnergy) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the energy from: " << unitTestFile << endl;
		return false;
	}
	cout << "Energy: " << moleculeSet.getEnergy() << " au " << endl;
	if (!obtainedGeometry) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the geometry from: " << unitTestFile << endl;
		return false;
	}
	moleculeSet.printToScreen();
	if (moleculeSet.getIsTransitionState()) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: The structure in the output file is not a transition state: " << unitTestFile << endl;
		return false;
	}
	cout << "Is a transition state: " << moleculeSet.getIsTransitionState() << endl;

	cout << "Reading energy and geometry from file: " << unitTestFile2.c_str() << endl;
	Energy::readOutputFileWithCCLib(unitTestFile2.c_str(), energy, &moleculeSet, cclibInstalled, openedFile, readEnergy, obtainedGeometry);
	if (!openedFile) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to open test file: " << unitTestFile2 << endl;
		return false;
	}
	if (!readEnergy) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the energy from: " << unitTestFile2 << endl;
		return false;
	}
	cout << "Energy: " << moleculeSet.getEnergy() << " au " << endl;
	if (!obtainedGeometry) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the geometry from: " << unitTestFile2 << endl;
		return false;
	}
	moleculeSet.printToScreen();
	if (!moleculeSet.getIsTransitionState()) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: The structure in the output file is a transition state: " << unitTestFile2 << endl;
		return false;
	}
	cout << "Is a transition state: " << moleculeSet.getIsTransitionState() << endl;
	
	cout << "Testing of cclib passed!" << endl;
	return true;
}



int main(int argc, char *argv[])
{
	int totalTests = 0;
	int passedTests = 0;

	++totalTests;
	if (memoryTest())
		++passedTests;

	++totalTests;
	if (ccLibReadTest())
		++passedTests;

	cout << endl;
	if (totalTests == passedTests)
		cout << "All tests passed!" << endl;
	else
		cout << (totalTests-passedTests) << " of " << totalTests << " tests failed." << endl;
}

