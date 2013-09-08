
#include "main.h"

void addFailedTestName(std::string &failedUnitTests, const char* failedTestName)
{
	if (failedUnitTests.length() > 0)
		failedUnitTests.append(", ");
	failedUnitTests.append(failedTestName);
}

int main(int argc, char* argv[])
{
	const char* (*tests[])() = {&testMatrixMultiplication, &ccLibReadTest, &testPlaceAtomGroupRelativeToAnother, &testInitialization};

	if (0 != chdir("..")) {
		printf("Failed to perform 'cd ..', errno = %d\n", errno);
	}

	if (!Random::init(1))
		return 0;
	if (!Strings::init())
		return 0;
	if (!Handbook::init())
		return 0;
	TiXmlBase::SetCondenseWhiteSpace(false);

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

	printf("Tests run: %d  Tests passed: %d  Tests failed: %d\n\n", totalTests, passedTests, (totalTests - passedTests));
	if (totalTests == passedTests)
		printf("All tests passed!\n\n");
	else {
		printf("Failed tests: %s\n\n", failedUnitTests.c_str());
	}

	return 0;
}
