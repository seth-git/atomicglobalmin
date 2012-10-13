////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "unit.h"

const char* memoryTest()
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
	
	return NULL;
}

const char* ccLibReadTest()
{
	const char* testName = "ccLibReadTest"; 
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
		return testName;
	}
	if (!openedFile) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to open test file: " << unitTestFile << endl;
		return testName;
	}
	if (!readEnergy) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the energy from: " << unitTestFile << endl;
		return testName;
	}
	cout << "Energy: " << energy << " au " << endl;
	if (obtainedGeometry) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Read geometry when it wasn't supposed to have been read from: " << unitTestFile << endl;
		return testName;
	}

	cout << "Reading energy and geometry from file: " << unitTestFile.c_str() << endl;
	Energy::readOutputFileWithCCLib(unitTestFile.c_str(), energy, &moleculeSet, cclibInstalled, openedFile, readEnergy, obtainedGeometry);
	if (!openedFile) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to open test file: " << unitTestFile << endl;
		return testName;
	}
	if (!readEnergy) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the energy from: " << unitTestFile << endl;
		return testName;
	}
	cout << "Energy: " << moleculeSet.getEnergy() << " au " << endl;
	if (!obtainedGeometry) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the geometry from: " << unitTestFile << endl;
		return testName;
	}
	moleculeSet.printToScreen();
	if (moleculeSet.getIsTransitionState()) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: The structure in the output file is not a transition state: " << unitTestFile << endl;
		return testName;
	}
	cout << "Is a transition state: " << moleculeSet.getIsTransitionState() << endl;

	cout << "Reading energy and geometry from file: " << unitTestFile2.c_str() << endl;
	Energy::readOutputFileWithCCLib(unitTestFile2.c_str(), energy, &moleculeSet, cclibInstalled, openedFile, readEnergy, obtainedGeometry);
	if (!openedFile) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to open test file: " << unitTestFile2 << endl;
		return testName;
	}
	if (!readEnergy) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the energy from: " << unitTestFile2 << endl;
		return testName;
	}
	cout << "Energy: " << moleculeSet.getEnergy() << " au " << endl;
	if (!obtainedGeometry) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: Unable to read the geometry from: " << unitTestFile2 << endl;
		return testName;
	}
	moleculeSet.printToScreen();
	if (!moleculeSet.getIsTransitionState()) {
		cout << "Testing of cclib failed!" << endl;
		cout << "\tReason: The structure in the output file is a transition state: " << unitTestFile2 << endl;
		return testName;
	}
	cout << "Is a transition state: " << moleculeSet.getIsTransitionState() << endl;
	
	cout << "Testing of cclib passed!" << endl;
	return NULL;
}

const char* testRandomSeeding(void) {
	static const char* testName = "testRandomSeeding";
	char commandLine[500];
	char line[500];
	string simulatedAnnealingFile = "LJ7_Sim.inp";
	const char* failMessage = "Testing of random number seeding failed!";
	ofstream fout;
	Input input;
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	vector<MoleculeSet*> bestIndividualMoleculeSets; // The best solution found for each individual (used in particle swarm optimization)
	string bufferOutputFile = unitTestFolder + "/temp.txt";
	FILE* pipe;
	
	cout << endl;
	cout << "Testing random number seeding..." << endl;

	if (!input.open(simulatedAnnealingFile, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets)) {
		cout << failMessage << endl;
		cout << "\tReason: unable to upen file: " << simulatedAnnealingFile << endl;
		return testName;
	}

	input.m_sInputFileName = unitTestFolder+"/"+input.m_sInputFileName;
	input.m_sOutputFileName = unitTestFolder+"/"+input.m_sOutputFileName;
	input.m_sResumeFileName = unitTestFolder+"/"+input.m_sResumeFileName;
	input.m_iResumeFileNumIterations = 1;
	input.m_iMaxIterations = 5;
	input.m_iPrintSummaryInfoEveryNIterations = 1;
	cout << "Writing file: " << input.m_sInputFileName << endl;
	fout.open(input.m_sInputFileName.c_str(), ofstream::out);
	if (!fout.is_open())
	{
		cout << failMessage << endl;
		cout << "\tReason: unable to write to file: " << input.m_sInputFileName << endl;
		return testName;
	}
	input.printToFile(fout);
	fout.close();
	
	snprintf(commandLine, sizeof(commandLine), "./pso %s > %s", input.m_sInputFileName.c_str(), bufferOutputFile.c_str());
	cout << "Executing: " << commandLine << endl;
	if (system(commandLine) == -1) {
		cout << failMessage << endl;
		cout << "\tReason: unable to execute command: " << commandLine << endl;
		return testName;
	}
	
	snprintf(commandLine, sizeof(commandLine), "mv %s %s.saved", input.m_sOutputFileName.c_str(), input.m_sOutputFileName.c_str());
	cout << "Executing: " << commandLine << endl;
	if (system(commandLine) == -1) {
		cout << failMessage << endl;
		cout << "\tReason: unable to execute command: " << commandLine << endl;
		return testName;
	}

	useconds_t sleepTime = 100000;
	cout << "Sleeping for " << sleepTime << " microseconds." << endl;
	usleep(sleepTime); // microseconds
	
	snprintf(commandLine, sizeof(commandLine), "./pso %s > %s", input.m_sInputFileName.c_str(), bufferOutputFile.c_str());
	cout << "Executing: " << commandLine << endl;
	if (system(commandLine) == -1) {
		cout << failMessage << endl;
		cout << "\tReason: unable to execute command: " << commandLine << endl;
		return testName;
	}
	
	snprintf(commandLine, sizeof(commandLine), "diff %s %s.saved", input.m_sOutputFileName.c_str(), input.m_sOutputFileName.c_str());
	cout << "Executing: " << commandLine << endl;
	pipe = popen(commandLine, "r");
	if (pipe == NULL) {
		cout << failMessage << endl;
		cout << "\tReason: unable to execute command: " << commandLine << endl;
		return testName;
	}
	if (feof(pipe) || (fgets(line, sizeof(line), pipe) == NULL)) {
		cout << failMessage << endl;
		cout << "\tReason: these files are identical when they shouldn't be: " << input.m_sOutputFileName.c_str()
			 << " and " << input.m_sOutputFileName.c_str() << ".saved" << endl;
		return testName;
	}
	pclose(pipe);

	snprintf(commandLine, sizeof(commandLine), "rm %s %s %s.saved %s %s", input.m_sInputFileName.c_str(),
	         input.m_sOutputFileName.c_str(), input.m_sOutputFileName.c_str(), input.m_sResumeFileName.c_str(), bufferOutputFile.c_str());
	cout << "Executing: " << commandLine << endl;
	if (system(commandLine) == -1) {
		cout << failMessage << endl;
		cout << "\tReason: unable to execute command: " << commandLine << endl;
		return testName;
	}
	
	cout << "Testing of random number seeding succeeded!" << endl;
	return NULL;
}

const char* testInitializationResults(MoleculeSet &moleculeSet, Point3D &box, FLOAT generalMin,
		map<unsigned int, map<unsigned int,FLOAT> > &minAtomicDistances, FLOAT *pMaxDist, unsigned int fragmentation) {
	if (!moleculeSetInsideCube(moleculeSet, box)) {
		return "\tReason: the molecule set is not inside the cube.";
	}
	
	if (!distanceConstraintsOK(moleculeSet, &generalMin, &minAtomicDistances, pMaxDist, fragmentation)) {
		return"\tReason: the molecule set does not observe minimum or maximum distance constraints.";
	}
	
	if (!transformationCheck(moleculeSet)) {
		return "\tReason: Rotation and translation of molecules performed in Init::init3DMoleculeSet produced differences in inter-molecular atomic distances.";
	}
	return NULL;
}

const char* testInitialization(void) {
	static const char* testName = "testInitialization";
	const char* failMessage = "Testing of initialization failed!";
	const char* reasonMessage;
	
	cout << endl;
	cout << "Testing initialization..." << endl;
	cout << "Testing minimum and maximum distance constraints, fragmentation contraints, cube constraints, rotation, and translation." << endl;
	
	FLOAT generalMin = 0.5;
	map<unsigned int, map<unsigned int,FLOAT> > minAtomicDistances;
	minAtomicDistances[1][1] = 0.7;
	minAtomicDistances[1][8] = 0.9;
	minAtomicDistances[8][8] = 1.2;
	setupMinDistances(generalMin, minAtomicDistances);
	FLOAT maxAtomDist = 2;
	Point3D box;
	box.x = box.y = box.z = 10;
	
	Molecule water;
	makeWater(water);
	
	MoleculeSet moleculeSet;
	
	cout << "Initializing 5 linear water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	FLOAT linearLengthAndWidth = 2.5;
	Point3D linearBox = box;
	linearBox.y = linearLengthAndWidth;
	linearBox.z = linearLengthAndWidth;
	if (!Init::initLinearMoleculeSet(moleculeSet, linearBox)) {
		cout << failMessage << endl;
		cout << "\tReason: call to Init::initLinearMoleculeSet returned false." << endl;
		return testName;
	}
	
	// Init::initLinearMoleculeSet centers everything inside box
	// Center everything inside linearBox, so we can test that it fits inside linearBox.
	moleculeSet.centerInBox(linearBox);
	
	reasonMessage = testInitializationResults(moleculeSet, linearBox, generalMin, minAtomicDistances, NULL, FRAGMENTED);
	if (reasonMessage != NULL) {
		cout << failMessage << endl;
		cout << reasonMessage << endl;
		return testName;
	}
	cout << "  Passed!" << endl;
	
	cout << "Initializing 5 planar water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	Point3D planarBox = box;
	planarBox.z = 0;
	if (!Init::initPlanarMoleculeSet(moleculeSet, planarBox)) {
		cout << failMessage << endl;
		cout << "\tReason: call to Init::initPlanarMoleculeSet returned false." << endl;
		return testName;
	}

	FLOAT boxCenter = box.z / 2;
	FLOAT tolerance = 0.0001;
	Point3D centerOfMass;
	const Molecule* molecules = moleculeSet.getMolecules();
	for (unsigned int i = 0; i < (unsigned int)moleculeSet.getNumberOfMolecules(); ++i) {
		centerOfMass = molecules[i].getCenter();
		if (!distancesWithinTolerance(centerOfMass.z, boxCenter, tolerance)) {
			cout << failMessage << endl;
			cout << "\tReason: call to Init::initPlanarMoleculeSet returned did not place the center of mass z coordinate close enough to the expected value of " << boxCenter
			     << " (actual value: " << centerOfMass.z << ")." << endl;
			return testName;
		}
	}

	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances, NULL, FRAGMENTED);
	if (reasonMessage != NULL) {
		cout << failMessage << endl;
		cout << reasonMessage << endl;
		return testName;
	}
	cout << "  Passed!" << endl;
	
	cout << "Initializing 5 3D water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	if (!Init::init3DMoleculeSet(moleculeSet, box)) {
		cout << failMessage << endl;
		cout << "\tReason: call to Init::init3DMoleculeSet returned false." << endl;
		return testName;
	}
	
	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances, NULL, FRAGMENTED);
	if (reasonMessage != NULL) {
		cout << reasonMessage << endl;
		cout << failMessage << endl;
		return testName;
	}
	cout << "  Passed!" << endl;
	
	cout << "Initializing 5 3D partially non-fragmented water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	if (!Init::init3DMoleculeSetWithMaxDist(moleculeSet, box, maxAtomDist)) {
		cout << failMessage << endl;
		cout << "\tReason: call to Init::init3DMoleculeSetWithMaxDist returned false." << endl;
		return testName;
	}
	
	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances, &maxAtomDist, PARTIALLY_FRAGMENTED);
	if (reasonMessage != NULL) {
		cout << reasonMessage << endl;
		cout << failMessage << endl;
		return testName;
	}
	cout << "  Passed!" << endl;
	
	cout << "Initializing 5 3D non-fragmented water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	if (!Init::init3DNonFragMoleculeSetWithMaxDist(moleculeSet, box, maxAtomDist)) {
		cout << failMessage << endl;
		cout << "\tReason: call to Init::init3DNonFragMoleculeSetWithMaxDist returned false." << endl;
		return testName;
	}
	
	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances, &maxAtomDist, NOT_FRAGMENTED);
	if (reasonMessage != NULL) {
		cout << reasonMessage << endl;
		cout << failMessage << endl;
		return testName;
	}
	cout << "  Passed!" << endl;
	
	cout << "Initialization test succeeded!" << endl;
	return NULL;
}

void addFailedTestName(string &failedUnitTests, const char* failedTestName)
{
	if (failedUnitTests.length() > 0)
		failedUnitTests.append(", ");
	failedUnitTests.append(failedTestName);
}

int main(int argc, char *argv[])
{
	const char* (*tests[])() = {&memoryTest, &testRandomSeeding, &testInitialization, &ccLibReadTest};
	string failedUnitTests;
	unsigned int passedTests = 0;
	const char* failedTestName;
	const unsigned int totalTests = sizeof(tests)/sizeof(tests[0]);
	
	if (!Init::initProgram(0)) {
		cout << "Failed to initialize program." << endl;
		return 0;
	}
	
	for (unsigned int i = 0; i < totalTests; ++i) {
		failedTestName = (*tests[i]) ();
		if (failedTestName == NULL)
			++passedTests;
		else
			addFailedTestName(failedUnitTests, failedTestName);
	}
	
	cout << endl;
	cout << "Tests run: " << totalTests << "  Tests passed: " << passedTests << "  Tests failed: " << (totalTests - passedTests) << endl;
	cout << endl;
	if (totalTests == passedTests)
		cout << "All tests passed!" << endl;
	else {
		cout << "Failed tests: " << failedUnitTests << endl;
	}
	cout << endl;
}
