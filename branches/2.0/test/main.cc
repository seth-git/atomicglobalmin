
#include "main.h"

void addFailedTestName(std::string &failedUnitTests, const char* failedTestName)
{
	if (failedUnitTests.length() > 0)
		failedUnitTests.append(", ");
	failedUnitTests.append(failedTestName);
}

int main(int argc, char* argv[])
{
	const char * (*tests[])() = {&testMatrixMultiplication, &ccLibReadTest, &testPlaceAtomGroupRelativeToAnother, &testInitialization,
		&testSeeding, &testSeeding2, &testSeeding3, &testSeeding4, &testLJ7, &gaussianTest, &testUpdateAtomToCenterRanks, &xmlBatchTest, &xmlSimTest};

	Input::s_bCompactXml = false;

	if (0 != chdir("..")) {
		printf("Failed to perform 'cd ..', errno = %d\n", errno);
		return 1;
	}

	if (!Random::init(1))
		return 1;
	if (!Handbook::init())
		return 1;

	std::string failedUnitTests;
	unsigned int passedTests = 0;
	const char* failedTestName;
	const unsigned int totalTests = sizeof(tests)/sizeof(tests[0]);

	for (unsigned int i = 0; i < totalTests; ++i) {
		failedTestName = (*tests[i]) ();
		if (failedTestName == NULL)
			++passedTests;
		else
			addFailedTestName(failedUnitTests, failedTestName);
	}

	printf("\nTests run: %d  Tests passed: %d  Tests failed: %d\n\n", totalTests, passedTests, (totalTests - passedTests));
	if (totalTests == passedTests)
		printf("All tests passed!\n\n");
	else {
		printf("Failed tests: %s\n\n", failedUnitTests.c_str());
	}

	return 0;
}
