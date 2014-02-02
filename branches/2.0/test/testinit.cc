
#include "testinit.h"
#include <list>

const char* testMatrixMultiplication() {
	static const char* testName = "testMatrixMultlipication";
	const char* failMessage = "Testing of matrix multiplication failed!";
	static const FLOAT matrix1[MATRIX_WIDTH][MATRIX_WIDTH] = {
			{1, 2, 3, 4},
			{5, 6, 7, 8},
			{9, 1, 2, 3},
			{4, 5, 6, 7}
	};
	static const FLOAT matrix2[MATRIX_WIDTH][MATRIX_WIDTH] = {
			{9, 8, 7, 6},
			{5, 4, 3, 2},
			{1, 2, 3, 4},
			{5, 6, 7, 8}
	};
	FLOAT product1[MATRIX_WIDTH][MATRIX_WIDTH];
	FLOAT product2[MATRIX_WIDTH][MATRIX_WIDTH];

	printf("Testing matrix multiplication...");
	if (TEST_VERBOSE) {
		printf("\nMatrix 1:\n");
		Matrix::print(matrix1);
		printf("Matrix 2:\n");
		Matrix::print(matrix2);
	}
	Matrix::matrixMultiplyMatrixUnoptimized(matrix1, matrix2, product1);
	Matrix::matrixMultiplyMatrix(matrix1, matrix2, product2);

	if (TEST_VERBOSE) {
		printf("Product 1 (unoptimized method):\n");
		Matrix::print(product1);
		printf("Product 2 (optimized method):\n");
		Matrix::print(product2);
	}

	for (unsigned int y = 0; y < MATRIX_WIDTH; ++y)
		for (unsigned int x = 0; x < MATRIX_WIDTH; ++x)
			if (product1[y][x] != product2[y][x]) {
				printf(failMessage);
				printf("\tReason: Found a difference between the unoptimized and optimized multiplication products.\n");
				return testName;
			}
	printf("  Passed!\n");

	return NULL;
}

const char* ccLibReadTest()
{
	const char* testName = "ccLibReadTest";
	std::string unitTestFile = testFilesDir + "/benzenesulfonamide.gamout";
	std::string unitTestFile2 = testFilesDir + "/transitionState.log";
	Structure structure;
	bool success;

	printf("\nTesting cclib...\n");

	printf("Reading the energy only from file: %s\n", unitTestFile.c_str());
	success = ExternalEnergy::readOutputFileWithCCLib(unitTestFile.c_str(), structure, false);

	if (!success) {
		printf("Testing of cclib failed!\n");
		printf("\tReason: cclib is not installed correctly!\n");
		return testName;
	}
	FLOAT expected = -121.4379729252;
	if (!floatsEqual(structure.getEnergy(), expected)) {
		printf("Testing of cclib failed!\n");
		printf("\tReason: unexpected energy value read from %s. Expected: %0.7lf au, Found: %0.7lf au\n",
				unitTestFile.c_str(), expected, structure.getEnergy());
		return testName;
	}
	printf("Energy: %0.7lf au\n", structure.getEnergy());

	printf("Reading energy and geometry from file: %s\n", unitTestFile.c_str());
	success = ExternalEnergy::readOutputFileWithCCLib(unitTestFile.c_str(), structure, true);
	if (!success) {
		printf("Testing of cclib failed!\n");
		printf("\tReason: cclib is not installed correctly!\n");
		return testName;
	}
	bool obtainedGeometry = structure.getNumberOfAtoms() == 27
			&& floatsEqual(*structure.getAtomCoordinates()[26][0], 5.5221902015)
			&& structure.getAtomicNumbers()[7] == 16;
	if (!obtainedGeometry) {
		printf("Testing of cclib failed!\n");
		printf("\tReason: Unable to read file: %s\n", unitTestFile.c_str());
		return testName;
	}
	if (TEST_VERBOSE)
		structure.print();
	if (structure.getIsTransitionState()) {
		printf("Testing of cclib failed!\n");
		printf("\tReason: The structure in the output file is not a transition state: %s\n", unitTestFile.c_str());
		return testName;
	}
	printf("Is a transition state: %d\n", structure.getIsTransitionState());

	std::cout << "Reading energy and geometry from file: " << unitTestFile2.c_str() << std::endl;
	success = ExternalEnergy::readOutputFileWithCCLib(unitTestFile2.c_str(), structure, true);
	if (!success) {
		printf("Testing of cclib failed!\n");
		std::cout << "\tReason: Unable to open test file: " << unitTestFile2 << std::endl;
		return testName;
	}
	obtainedGeometry = structure.getNumberOfAtoms() == 6
			&& floatsEqual(*structure.getAtomCoordinates()[5][0], 1.175736)
			&& structure.getAtomicNumbers()[4] == 8;
	if (!obtainedGeometry) {
		printf("Testing of cclib failed!\n");
		std::cout << "\tReason: Unable to read the geometry from: " << unitTestFile2 << std::endl;
		return testName;
	}
	if (TEST_VERBOSE)
		structure.print();
	if (!structure.getIsTransitionState()) {
		printf("Testing of cclib failed!\n");
		std::cout << "\tReason: The structure in the output file is a transition state: " << unitTestFile2 << std::endl;
		return testName;
	}
	std::cout << "Is a transition state: " << structure.getIsTransitionState() << std::endl;

	printf("Testing of cclib passed!\n");
	return NULL;
}

const char* testSeeding(void) {
	static const char* testName = "testSeeding";
	const char* failMessage = "Test of seeding failed!\n";

	printf("\nTesting seeding...\n");
	std::string inputFile = testFilesDir + "/testSeeding.xml";
	Input input;
	if (!input.load(inputFile.c_str())) {
		printf(failMessage);
		return testName;
	}

	Batch* batch = (Batch*)input.m_pAction;
	Structure* structure = batch->m_structures.front();
	const unsigned int expectedAtomicNumbers[] = {1,8,1,1,8,1,1,8,1,1,8};
	unsigned int expectedAtoms = sizeof(expectedAtomicNumbers) / sizeof(unsigned int*);
	if (structure->getNumberOfAtoms() != expectedAtoms) {
		printf("Unexpected number of atoms in seeded result. Expected: %u, actual: %u.\n", expectedAtoms, structure->getNumberOfAtoms());
		return testName;
	}
	unsigned int expectedAtomGroups = 4;
	if (structure->getNumberOfAtomGroups() != expectedAtomGroups) {
		printf("Unexpected number of atom groups in seeded result. Expected: %u, actual: %u.\n", expectedAtomGroups, structure->getNumberOfAtomGroups());
		return testName;
	}
	const unsigned int* atomicNumbers = structure->getAtomicNumbers();
	for (unsigned int i = 0; i < expectedAtoms; ++i) {
		if (atomicNumbers[i] != expectedAtomicNumbers[i]) {
			printf("Unexpected atomic number at position %u. Expected: %u, actual: %u.\n", (i+1), expectedAtomicNumbers[i], atomicNumbers[i]);
			return testName;
		}
	}

	printf("Test of seeding succeeded!\n");
	return NULL;
}

const char* testSeeding2(void) {
	static const char* testName = "testSeeding2";
	const char* failMessage = "Second test of seeding failed!\n";

	printf("\nPerforming second test of seeding...\n");
	std::string inputFile = testFilesDir + "/testSeeding2.xml";
	Input input;
	if (!input.load(inputFile.c_str())) {
		printf(failMessage);
		return testName;
	}

	Batch* batch = (Batch*)input.m_pAction;
	Structure* structure = batch->m_structures.front();
	const unsigned int expectedAtomicNumbers[] = {1,8,1,1,8};
	unsigned int expectedAtoms = sizeof(expectedAtomicNumbers) / sizeof(unsigned int*);
	if (structure->getNumberOfAtoms() != expectedAtoms) {
		printf("Unexpected number of atoms in seeded result. Expected: %u, actual: %u.\n", expectedAtoms, structure->getNumberOfAtoms());
		return testName;
	}
	unsigned int expectedAtomGroups = 2;
	if (structure->getNumberOfAtomGroups() != expectedAtomGroups) {
		printf("Unexpected number of atom groups in seeded result. Expected: %u, actual: %u.\n", expectedAtomGroups, structure->getNumberOfAtomGroups());
		return testName;
	}
	const unsigned int* atomicNumbers = structure->getAtomicNumbers();
	for (unsigned int i = 0; i < expectedAtoms; ++i) {
		if (atomicNumbers[i] != expectedAtomicNumbers[i]) {
			printf("Unexpected atomic number at position %u. Expected: %u, actual: %u.\n", (i+1), expectedAtomicNumbers[i], atomicNumbers[i]);
			return testName;
		}
	}

	printf("Second test of seeding succeeded!\n");
	return NULL;
}

const char* testSeeding3(void) {
	static const char* testName = "testSeeding3";
	const char* failMessage = "Third test of seeding failed!\n";

	printf("\nPerforming third testing of seeding...\n");
	std::string inputFile = testFilesDir + "/testSeeding3.xml";
	Input input;
	if (!input.load(inputFile.c_str())) {
		printf(failMessage);
		return testName;
	}

	Batch* batch = (Batch*)input.m_pAction;
	Structure structure1;
	Seed* seed = batch->m_structuresTemplate.m_pSeed;
	if (!ExternalEnergy::readOutputFile(seed->m_energyFileTypes[0], seed->m_energyFilePaths[0].c_str(), structure1, true))
		return testName;
	Structure* pStructure2 = batch->m_structures.front();
	const unsigned int expectedAtomicNumbers[] = {8,8,1,8,1,1,8,1,1,8,8,1};
	unsigned int expectedAtoms = sizeof(expectedAtomicNumbers) / sizeof(unsigned int*);
	if (pStructure2->getNumberOfAtoms() != expectedAtoms) {
		printf("Unexpected number of atoms in seeded result. Expected: %u, actual: %u.\n", expectedAtoms, pStructure2->getNumberOfAtoms());
		return testName;
	}
	unsigned int expectedAtomGroups = 4;
	if (pStructure2->getNumberOfAtomGroups() != expectedAtomGroups) {
		printf("Unexpected number of atom groups in seeded result. Expected: %u, actual: %u.\n", expectedAtomGroups, pStructure2->getNumberOfAtomGroups());
		return testName;
	}
	const unsigned int* atomicNumbers = pStructure2->getAtomicNumbers();
	for (unsigned int i = 0; i < expectedAtoms; ++i) {
		if (atomicNumbers[i] != expectedAtomicNumbers[i]) {
			printf("Unexpected atomic number at position %u. Expected: %u, actual: %u.\n", (i+1), expectedAtomicNumbers[i], atomicNumbers[i]);
			return testName;
		}
	}
	FLOAT x1 = *structure1.getAtomCoordinates()[structure1.getNumberOfAtoms()-1][0];
	FLOAT x2 = *pStructure2->getAtomCoordinates()[pStructure2->getNumberOfAtoms()-1][0];
	if (!floatsEqual(x1, x2)) {
		printf("The last x coordinate of the newly created structure and the read structure should be the same, but they're not. Values: %0.5lf, %0.5lf\n", x1, x2);
		return testName;
	}
	if (TEST_VERBOSE)
		printf("The last x coordinate of the newly created structure and the read structure are the same as they should be: %0.7lf, %0.7lf\n", x1, x2);

	printf("Third test of seeding succeeded!\n");
	return NULL;
}

const char* testSeeding4(void) {
	static const char* testName = "testSeeding4";
	const char* failMessage = "Fourth test of seeding failed!\n";

	printf("\nPerforming fourth testing of seeding...\n");
	std::string inputFile = testFilesDir + "/testSeeding4.xml";
	Input input;
	if (!input.load(inputFile.c_str())) {
		printf(failMessage);
		return testName;
	}

	Batch* batch = (Batch*)input.m_pAction;
	Structure* pStructure = batch->m_structures.front();
	const unsigned int expectedAtomicNumbers[] = {8,1,6,1,1};
	unsigned int expectedAtoms = sizeof(expectedAtomicNumbers) / sizeof(unsigned int*);
	if (pStructure->getNumberOfAtoms() != expectedAtoms) {
		printf("Unexpected number of atoms in seeded result. Expected: %u, actual: %u.\n", expectedAtoms, pStructure->getNumberOfAtoms());
		return testName;
	}
	unsigned int expectedAtomGroups = 5;
	if (pStructure->getNumberOfAtomGroups() != expectedAtomGroups) {
		printf("Unexpected number of atom groups in seeded result. Expected: %u, actual: %u.\n", expectedAtomGroups, pStructure->getNumberOfAtomGroups());
		return testName;
	}
	const unsigned int* atomicNumbers = pStructure->getAtomicNumbers();
	for (unsigned int i = 0; i < expectedAtoms; ++i) {
		if (atomicNumbers[i] != expectedAtomicNumbers[i]) {
			printf("Unexpected atomic number at position %u. Expected: %u, actual: %u.\n", (i+1), expectedAtomicNumbers[i], atomicNumbers[i]);
			return testName;
		}
	}

	printf("Third test of seeding succeeded!\n");
	return NULL;
}
/*
const char* testInitializationResults(Structure &moleculeSet, Point3D &box, FLOAT generalMin,
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

	std::cout << std::endl;
	printf("Testing initialization...\n");
	printf("Testing minimum and maximum distance constraints, fragmentation constraints, cube constraints, rotation, and translation.\n");

	FLOAT generalMin = 0.5;
	map<unsigned int, map<unsigned int,FLOAT> > minAtomicDistances;
	minAtomicDistances[1][1] = 0.7;
	minAtomicDistances[1][8] = 0.9;
	minAtomicDistances[8][8] = 1.2;
	setupMinDistances(generalMin, minAtomicDistances);
	FLOAT maxAtomDist = 2;
	FLOAT cubeLWH = 10;

	AtomGroup water;
	makeWater(water);

	Structure moleculeSet;

	std::cout << "Initializing 5 linear water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	FLOAT linearLengthAndWidth = 2.5;
	Point3D linearBox = box;
	linearBox.y = linearLengthAndWidth;
	linearBox.z = linearLengthAndWidth;
	if (!Init::initLinearMoleculeSet(moleculeSet, linearBox)) {
		std::cout << failMessage << std::endl;
		printf("\tReason: call to Init::initLinearMoleculeSet returned false.\n");
		return testName;
	}

	// Init::initLinearMoleculeSet centers everything inside box
	// Center everything inside linearBox, so we can test that it fits inside linearBox.
	moleculeSet.centerInBox(linearBox);

	reasonMessage = testInitializationResults(moleculeSet, linearBox, generalMin, minAtomicDistances, NULL, FRAGMENTED);
	if (reasonMessage != NULL) {
		std::cout << failMessage << std::endl;
		std::cout << reasonMessage << std::endl;
		return testName;
	}
	printf("  Passed!\n");

	std::cout << "Initializing 5 planar water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	Point3D planarBox = box;
	planarBox.z = 0;
	if (!Init::initPlanarMoleculeSet(moleculeSet, planarBox)) {
		std::cout << failMessage << std::endl;
		printf("\tReason: call to Init::initPlanarMoleculeSet returned false.\n");
		return testName;
	}

	FLOAT boxCenter = box.z / 2;
	FLOAT tolerance = 0.0001;
	Point3D centerOfMass;
	const AtomGroup* molecules = moleculeSet.getMolecules();
	for (unsigned int i = 0; i < (unsigned int)moleculeSet.getNumberOfMolecules(); ++i) {
		centerOfMass = molecules[i].getCenter();
		if (!distancesWithinTolerance(centerOfMass.z, boxCenter, tolerance)) {
			std::cout << failMessage << std::endl;
			std::cout << "\tReason: call to Init::initPlanarMoleculeSet returned did not place the center of mass z coordinate close enough to the expected value of " << boxCenter
			     << " (actual value: " << centerOfMass.z << ")." << std::endl;
			return testName;
		}
	}

	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances, NULL, FRAGMENTED);
	if (reasonMessage != NULL) {
		std::cout << failMessage << std::endl;
		std::cout << reasonMessage << std::endl;
		return testName;
	}
	printf("  Passed!\n");

	std::cout << "Initializing 5 3D water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	if (!Init::init3DMoleculeSet(moleculeSet, box)) {
		std::cout << failMessage << std::endl;
		printf("\tReason: call to Init::init3DMoleculeSet returned false.\n");
		return testName;
	}

	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances, NULL, FRAGMENTED);
	if (reasonMessage != NULL) {
		std::cout << reasonMessage << std::endl;
		std::cout << failMessage << std::endl;
		return testName;
	}
	printf("  Passed!\n");

	std::cout << "Initializing 5 3D partially non-fragmented water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	if (!Init::init3DMoleculeSetWithMaxDist(moleculeSet, box, maxAtomDist)) {
		std::cout << failMessage << std::endl;
		printf("\tReason: call to Init::init3DMoleculeSetWithMaxDist returned false.\n");
		return testName;
	}

	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances, &maxAtomDist, PARTIALLY_FRAGMENTED);
	if (reasonMessage != NULL) {
		std::cout << reasonMessage << std::endl;
		std::cout << failMessage << std::endl;
		return testName;
	}
	printf("  Passed!\n");

	std::cout << "Initializing 5 3D non-fragmented water molecules...";
	makeWaterSet(moleculeSet, water, 5);
	if (!Init::init3DNonFragMoleculeSetWithMaxDist(moleculeSet, box, maxAtomDist)) {
		std::cout << failMessage << std::endl;
		printf("\tReason: call to Init::init3DNonFragMoleculeSetWithMaxDist returned false.\n");
		return testName;
	}

	reasonMessage = testInitializationResults(moleculeSet, box, generalMin, minAtomicDistances, &maxAtomDist, NOT_FRAGMENTED);
	if (reasonMessage != NULL) {
		std::cout << reasonMessage << std::endl;
		std::cout << failMessage << std::endl;
		return testName;
	}
	printf("  Passed!\n");

	printf("Initialization test succeeded!\n");
	return NULL;
}*/

const char* testPlaceAtomGroupRelativeToAnother(void) {
	static const char* testName = "testInitialization";
	const char* failMessage = "Testing of initialization failed!";

	printf("\nTesting StructuresTemplate.placeAtomGroupRelativeToAnother...\n");

	std::string constraintsxml;
	constraintsxml.append("<constraints name='constraints' >");
	constraintsxml.append("   <cube size='10'/>");
	constraintsxml.append("   <atomicDistances>");
	constraintsxml.append("      <min value='0.7' Z1='H' Z2='H' />");
	constraintsxml.append("      <min value='0.9' Z1='H' Z2='O' />");
	constraintsxml.append("      <min value='1.2' Z1='O' Z2='O' />");
	constraintsxml.append("      <max value='2' />");
	constraintsxml.append("   </atomicDistances>");
	constraintsxml.append("</constraints>");
	TiXmlDocument xmlDocument;
	xmlDocument.Parse(constraintsxml.c_str(), 0, TIXML_ENCODING_UTF8);
	TiXmlHandle hDoc(&xmlDocument);
	TiXmlElement* pElem=hDoc.FirstChildElement().Element();
	Constraints constraints;
	std::map<std::string,Constraints*> constraintsMap;
	if (!constraints.load(pElem, Strings::instance(), constraintsMap)) {
		std::cout << failMessage << std::endl;
		printf("\tReason: failed to initialize test constraints.\n");
		return testName;
	}
	constraintsMap[constraints.m_sName] = &constraints;

	unsigned int numWaterGroups = 2;
	Structure structure;
	makeWaterSet(structure, numWaterGroups);

	unsigned int agToPlaceIndex = 1;
	unsigned int otherAgIndex = 0;
	AtomGroup* otherAg = structure.getAtomGroup(otherAgIndex);
	COORDINATE3	zeros = {0, 0, 0};
	otherAg->setCenter(zeros);
	otherAg->setAngles(zeros);
	otherAg->initRotationMatrix();
	otherAg->localToGlobal();

	AtomGroup* agToPlace = structure.getAtomGroup(agToPlaceIndex);
	COORDINATE3	agToPlaceAngles = {0, 0, -PIE_OVER_2};
	agToPlace->setAngles(agToPlaceAngles);

	COORDINATE3	unitVector = {1, 0, 0};

	StructuresTemplate::InitResult initResult =
			StructuresTemplate::placeAtomGroupRelativeToAnother(1, 0,
					structure, unitVector, constraints, TEST_VERBOSE ? 2 : 1);

	if (initResult != StructuresTemplate::Success) {
		std::cout << failMessage << std::endl;
		printf("\t");
		StructuresTemplate::printInitFailureMessage(initResult);
		return testName;
	}

	unsigned int targeti = 1;
	unsigned int targetj = 3;
	FLOAT distance = constraints.getMinDistance(structure.getAtomicNumbers()[targeti], structure.getAtomicNumbers()[targetj]);
	if (!floatsEqual(structure.getAtomDistanceMatrix()[targeti][targetj], distance)) {
		std::cout << failMessage << std::endl;
		printf("\tReason: expected the distance between atoms %u and %u to be %lf, but it was %lf.\n",
				targeti+1, targetj+1, distance, structure.getAtomDistanceMatrix()[targeti][targetj]);
		return testName;
	} else {
		printf("\tThe distance between atoms %u and %u was %lf as expected.\n",
						targeti+1, targetj+1, distance);
	}

	printf("PlaceAtomGroupRelativeToAnother test succeeded!\n");
	return NULL;
}

const char* testInitialization(void) {
	static const char* testName = "testInitialization";
	const char* failMessage = "Testing of initialization failed!";

	printf("\nTesting initialization...\n");
	printf("Testing minimum and maximum distance constraints, fragmentation constraints, cube constraints, rotation, and translation.\n");

	std::string constraintsxml;
	constraintsxml.append("<constraints name='constraints' >");
	constraintsxml.append("   <cube size='10'/>");
	constraintsxml.append("   <atomicDistances>");
	constraintsxml.append("      <min value='0.7' Z1='H' Z2='H' />");
	constraintsxml.append("      <min value='0.9' Z1='H' Z2='O' />");
	constraintsxml.append("      <min value='1.2' Z1='O' Z2='O' />");
	constraintsxml.append("      <max value='2' />");
	constraintsxml.append("   </atomicDistances>");
	constraintsxml.append("</constraints>");
	TiXmlDocument xmlDocument;
	xmlDocument.Parse(constraintsxml.c_str(), 0, TIXML_ENCODING_UTF8);
	TiXmlHandle hDoc(&xmlDocument);
	TiXmlElement* pElem=hDoc.FirstChildElement().Element();
	Constraints constraints;
	std::map<std::string,Constraints*> constraintsMap;
	if (!constraints.load(pElem, Strings::instance(), constraintsMap)) {
		std::cout << failMessage << std::endl;
		printf("\tReason: failed to initialize test constraints.\n");
		return testName;
	}
	constraintsMap[constraints.m_sName] = &constraints;

	unsigned int numWaterGroups = 5;
	printf("Initializing %u linear water molecules...\n", numWaterGroups);
	Structure structure;
	makeWaterSet(structure, numWaterGroups);
	unsigned int iTries, iGroupTries;

	StructuresTemplate::InitResult initResult =
			StructuresTemplate::initializeStructure(structure, constraints,
					StructuresTemplate::Linear,
					StructuresTemplate::defaultMaxInitializationTries, iTries,
					StructuresTemplate::defaultMaxGroupInitializationTries,
					iGroupTries, TEST_VERBOSE);

	if (TEST_VERBOSE) {
		printf("iMaxTries = %u, iTries = %u\n", StructuresTemplate::defaultMaxInitializationTries, iTries);
		printf("iMaxGroupTries = %u, iGroupTries = %u\n", StructuresTemplate::defaultMaxGroupInitializationTries, iGroupTries);
		structure.print(Structure::PRINT_LOCAL_COORDINATES & Structure::PRINT_DISTANCE_MATRIX);
	}

	if (!initResult == StructuresTemplate::Success) {
		std::cout << failMessage << std::endl;
		StructuresTemplate::printInitFailureMessage(initResult);
		return testName;
	}

	unsigned int i, j;
	for (i = 0; i < structure.getNumberOfAtomGroups(); ++i) {
		const FLOAT* center = structure.getAtomGroups()[i].getCenter();
		for (j = 0; j < 2; ++j) {
			if (std::abs(center[j]) > FLOAT_ERROR) {
				std::cout << failMessage << std::endl;
				printf("\tReason: not all the centers of the water molecules have zero values for the x and y coordinates.");
				return testName;
			}
		}
	}

	printf("  Passed!\n");

	printf("Initialization test succeeded!\n");
	return NULL;
}

const char* testLJ7(void) {
	static const char* testName = "testLJ7";
	const char* failMessage = "Testing of LJ7 initialization failed!";

	printf("\nPerforming LJ7 initialization test...\n");
	std::string inputFile = testFilesDir + "/batchLJ7.xml";
	Input input;
	if (!input.load(inputFile.c_str())) {
		printf(failMessage);
		return testName;
	}

	std::list<Structure*>* pStructures = &(input.m_pAction->m_structures);
	const Constraints* constraints = input.m_pAction->m_constraints[0];
	for (std::list<Structure*>::iterator it = pStructures->begin(); it != pStructures->end(); ++it) {
		(*it)->update();
		if (!constraints->validate(**it)) {
			std::cout << failMessage << std::endl;
			printf("\tReason: structure %d failed to meet the constraints:\n", (*it)->getId());
			(*it)->print(Structure::PRINT_DISTANCE_MATRIX);
			return testName;
		}
	}

	printf("LJ7 initialization test succeeded!\n");
	return NULL;
}
