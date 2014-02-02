
#include "testRmsDistance.h"

const char* testUpdateAtomToCenterRanks() {
	static const char* testName = "testUpdateAtomToCenterRanks";
	const char* failMessage = "Testing of updateAtomToCenterRanks failed!";

	printf("\nPerforming test on RmsDistance::updateAtomToCenterRanks method...\n");
	Input input;
	std::string seedFile = testFilesDir+"/rmsDist.xml";
	if (!input.load(seedFile.c_str())) {
		printf(failMessage);
		return testName;
	}
	std::list<Structure*>::iterator it = input.m_pAction->m_structures.begin();
	Structure* pStructure1 = *it;
	RmsDistance::updateAtomToCenterRanks(*pStructure1);
	++it;
	Structure* pStructure2 = *it;
	RmsDistance::updateAtomToCenterRanks(*pStructure2);
	FLOAT dist = RmsDistance::calculate(*pStructure1, *pStructure2);
	FLOAT expected = 0.9124017;
	if (TEST_VERBOSE) {
		printf("\nStructure 1:\n");
		pStructure1->print(Structure::PRINT_DISTANCE_MATRIX);
		printf("Structure 2:\n");
		pStructure1->print(Structure::PRINT_DISTANCE_MATRIX);
		printf("Distance: %0.7lf\n");
	}
	if (!floatsEqual(dist, expected)) {
		printf("Testing of updateAtomToCenterRanks failed!\n");
		printf("\tReason: unexpected distance value. Expected: %0.7lf au, Found: %0.7lf au\n",
				expected, dist);
		return testName;
	}

	printf("Test of RmsDistance::updateAtomToCenterRanks succeeded!\n");
	return NULL;
}
