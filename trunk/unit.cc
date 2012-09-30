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

FLOAT** getDistanceMatrix(MoleculeSet &moleculeSet, bool globalOrLocal) {
	unsigned int i, j;
	Point3D diff;
	FLOAT** matrix;
	matrix = new FLOAT*[moleculeSet.getNumberOfAtoms()];
	Atom const** atoms = moleculeSet.getAtoms();
	
	for (i = 0; i < (unsigned int)moleculeSet.getNumberOfAtoms(); ++i)
		matrix[i] = new FLOAT[moleculeSet.getNumberOfAtoms()];
	
	if (globalOrLocal) {
		for (i = 0; i < (unsigned int)moleculeSet.getNumberOfAtoms(); ++i)
			for (j = 0; j < (unsigned int)moleculeSet.getNumberOfAtoms(); ++j) {
				diff.x = atoms[i]->m_globalPoint.x - atoms[j]->m_globalPoint.x;
				diff.y = atoms[i]->m_globalPoint.y - atoms[j]->m_globalPoint.y;
				diff.z = atoms[i]->m_globalPoint.z - atoms[j]->m_globalPoint.z;
				matrix[i][j] = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
			}
	} else {
		for (i = 0; i < (unsigned int)moleculeSet.getNumberOfAtoms(); ++i)
			for (j = 0; j < (unsigned int)moleculeSet.getNumberOfAtoms(); ++j) {
				diff.x = atoms[i]->m_localPoint.x - atoms[j]->m_localPoint.x;
				diff.y = atoms[i]->m_localPoint.y - atoms[j]->m_localPoint.y;
				diff.z = atoms[i]->m_localPoint.z - atoms[j]->m_localPoint.z;
				matrix[i][j] = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
			}
	}
	
	return matrix;
}

void deleteDistanceMatrix(FLOAT** matrix, unsigned int size) {
	for (unsigned int i = 0; i < size; ++i)
		delete[] matrix[i];
	delete matrix;
}

bool distancesWithinTolerance(FLOAT distance1, FLOAT distance2, FLOAT tolerance) {
	FLOAT diff = distance1 - distance2;
	if (diff < 0)
		diff = -diff;
	return diff <= tolerance;
}

// This verifies that rotation and translation were performed correctly.
// The correctness is verified by examining distances between atoms within molecules.
// These should be the same before and after translation and rotation.
bool transformationCheck(MoleculeSet &moleculeSet) {
	const Molecule* molecules = moleculeSet.getMolecules();
	FLOAT** matrixBefore = getDistanceMatrix(moleculeSet, false);
	FLOAT** matrixAfter = getDistanceMatrix(moleculeSet, true);
	unsigned int moleculeI;
	unsigned int atomStart, atomEnd;
	unsigned int i, j;
	const FLOAT tolerance = 0.0001;
	
	atomStart = 0;
	for (moleculeI = 0; moleculeI < (unsigned int)moleculeSet.getNumberOfMolecules(); ++moleculeI) {
		atomEnd = atomStart + (unsigned int)molecules[moleculeI].getNumberOfAtoms();
		for (i = atomStart; i < atomEnd; ++i)
			for (j = atomStart; j < atomEnd; ++j)
				if (i != j && !distancesWithinTolerance(matrixBefore[i][j], matrixAfter[i][j], tolerance))
					return false;
		atomStart = atomEnd;
	}
	
	deleteDistanceMatrix(matrixBefore, moleculeSet.getNumberOfAtoms());
	deleteDistanceMatrix(matrixAfter, moleculeSet.getNumberOfAtoms());
	return true;
}

// This function verifies that minimum distance constraints have been enforced
bool minDistancesOK(MoleculeSet &moleculeSet, FLOAT generalMin, map<unsigned int, map<unsigned int,FLOAT> > &minAtomicDistances) {
	Atom const** atoms = moleculeSet.getAtoms();
	FLOAT** matrix = getDistanceMatrix(moleculeSet, true);
	unsigned int size = (unsigned int) moleculeSet.getNumberOfAtoms();
	unsigned int i, j;
	for (i = 0; i < size; ++i)
		for (j = 0; j < size; ++j)
			if (i != j) {
				if (matrix[i][j] < generalMin) {
					cout << "Atoms "
					     << Atom::s_rgAtomcSymbols[atoms[i]->m_iAtomicNumber] << (i+1) << " and "
					     << Atom::s_rgAtomcSymbols[atoms[j]->m_iAtomicNumber] << (j+1)
					     << " failed to meet the general minimum distance constraint of " << generalMin << endl;
					return false;
				}
				if (matrix[i][j] < minAtomicDistances[atoms[i]->m_iAtomicNumber][atoms[j]->m_iAtomicNumber]) {
					cout << "Atoms "
					     << Atom::s_rgAtomcSymbols[atoms[i]->m_iAtomicNumber] << (i+1) << " and "
					     << Atom::s_rgAtomcSymbols[atoms[j]->m_iAtomicNumber] << (j+1)
					     << " failed to meet the specific minimum distance constraint of " << minAtomicDistances[atoms[i]->m_iAtomicNumber][atoms[j]->m_iAtomicNumber] << endl;
					return false;
				}
			}
	deleteDistanceMatrix(matrix, moleculeSet.getNumberOfAtoms());
	return true;
}

bool moleculeSetInsideCube(MoleculeSet &moleculeSet, Point3D &boxDimensions) {
	Molecule* moleculeArray = moleculeSet.getMolecules();
	
	const Atom* atomArray;
	unsigned int i, j;
	for (i = 0; i < (unsigned int)moleculeSet.getNumberOfMolecules(); ++i) {
		atomArray = moleculeArray[i].getAtoms();
		for (j = 0; j < (unsigned int)moleculeArray[i].getNumberOfAtoms(); ++j) {
			if (atomArray[j].m_globalPoint.x < 0)
				return false;
			if (atomArray[j].m_globalPoint.x > boxDimensions.x)
				return false;
			if (atomArray[j].m_globalPoint.y < 0)
				return false;
			if (atomArray[j].m_globalPoint.y > boxDimensions.y)
				return false;
			if (atomArray[j].m_globalPoint.z < 0)
				return false;
			if (atomArray[j].m_globalPoint.z > boxDimensions.z)
				return false;
		}
	}
	return true;
}

void printPoint(const Point3D &point) {
	cout << "(x,y,z) = (" << point.x << "," << point.y << "," << point.z << ")";
}

void printDetailedInfo(MoleculeSet &moleculeSet, bool printBondLengths) {
	const Molecule* moleculeArray = moleculeSet.getMolecules();
	const Atom* atomArray;
	unsigned int i, j, k;
	Point3D diff;
	FLOAT lengthL, lengthG;
	for (i = 0; i < (unsigned int)moleculeSet.getNumberOfMolecules(); ++i) {
		cout << "center";
		printPoint(moleculeArray[i].getCenter());
		cout << ", angles in rad(x,y,z) = (" << moleculeArray[i].getAngleX() << "," << moleculeArray[i].getAngleY() << "," << moleculeArray[i].getAngleZ() << ")";
		cout << endl;
		atomArray = moleculeArray[i].getAtoms();
		for (j = 0; j < (unsigned int)moleculeArray[i].getNumberOfAtoms(); ++j) {
			cout << Atom::s_rgAtomcSymbols[atomArray[j].m_iAtomicNumber] << (atomArray[j].m_iMoleculeSetIndex+1) << ": local";
			printPoint(atomArray[j].m_localPoint);
			cout << ", global";
			printPoint(atomArray[j].m_globalPoint);
			cout << endl;
		}
		if (printBondLengths) {
			for (j = 0; j < (unsigned int)moleculeArray[i].getNumberOfAtoms(); ++j) {
				for (k = j+1; k < (unsigned int)moleculeArray[i].getNumberOfAtoms(); ++k) {
					diff.x = atomArray[j].m_localPoint.x - atomArray[k].m_localPoint.x;
					diff.y = atomArray[j].m_localPoint.y - atomArray[k].m_localPoint.y;
					diff.z = atomArray[j].m_localPoint.z - atomArray[k].m_localPoint.z;
					lengthL = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
					diff.x = atomArray[j].m_globalPoint.x - atomArray[k].m_globalPoint.x;
					diff.y = atomArray[j].m_globalPoint.y - atomArray[k].m_globalPoint.y;
					diff.z = atomArray[j].m_globalPoint.z - atomArray[k].m_globalPoint.z;
					lengthG = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
					cout << "bond lengths between " << j+1 << " and " << k+1 << ": local = " << lengthL << ", global = " << lengthG << endl;
				}
			}
		}
	}
}

void makeWater(Molecule &molecule)
{
	vector<Point3D> points;
	vector<int> atomicNumbers;
	Point3D o, h1, h2;
	o.x = 0;
	o.y = 0;
	o.z = 0;
	h1.x = 0.757;
	h1.y = 0.586;
	h1.z = 0;
	h2.x = -0.757;
	h2.y = 0.586;
	h2.z = 0;
	
	points.push_back(o);
	atomicNumbers.push_back(8);
	
	points.push_back(h1);
	atomicNumbers.push_back(1);
	
	points.push_back(h2);
	atomicNumbers.push_back(1);

	molecule.makeFromCartesian(points, atomicNumbers);
	molecule.initRotationMatrix();
	molecule.localToGlobal();
}

void makeWaterSet(MoleculeSet &moleculeSet, Molecule &water, unsigned int number) {
	moleculeSet.setNumberOfMolecules(number);
	Molecule* moleculeArray = moleculeSet.getMolecules();
	for (unsigned int i = 0; i < (unsigned int)moleculeSet.getNumberOfMolecules(); ++i)
		moleculeArray[i].copy(water);
	moleculeSet.init();
}

void setupMinDistances(FLOAT generalMin, map<unsigned int, map<unsigned int,FLOAT> > &minAtomicDistances) {
	Atom::initMinAtomicDistances(generalMin);
	for (map<unsigned int, map<unsigned int, FLOAT> >::iterator i = minAtomicDistances.begin(); i != minAtomicDistances.end(); i++)
		for (map<unsigned int, FLOAT>::iterator j = i->second.begin(); j != i->second.end(); j++)
			Atom::setMinAtomicDistance(i->first, j->first, j->second);
}

const char* testInitializationResults(MoleculeSet &moleculeSet, Point3D &box, FLOAT generalMin, map<unsigned int, map<unsigned int,FLOAT> > &minAtomicDistances) {
	if (!moleculeSetInsideCube(moleculeSet, box)) {
		return "\tReason: the molecule set is not inside the cube.";
	}
	
	if (!minDistancesOK(moleculeSet, generalMin, minAtomicDistances)) {
		return"\tReason: call to Init::init3DMoleculeSet did not place the molecule set observing the minimum distance constraints.";
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
	
	FLOAT generalMin = 0.5;
	map<unsigned int, map<unsigned int,FLOAT> > minAtomicDistances;
	minAtomicDistances[1][1] = 0.7;
	minAtomicDistances[1][8] = 0.9;
	minAtomicDistances[8][8] = 1.2;
	setupMinDistances(generalMin, minAtomicDistances);
	FLOAT maxAtomDist = 2;
	Point3D box;
	box.x = 10;
	box.y = 10;
	box.z = 10;
	
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
	
	reasonMessage = testInitializationResults(moleculeSet, linearBox, generalMin, minAtomicDistances);
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

	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances);
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
	
	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances);
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
	
	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances);
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
	
	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances);
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
