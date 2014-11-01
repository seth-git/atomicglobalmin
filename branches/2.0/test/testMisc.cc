
#include "testMisc.h"
#include "../externalEnergy/gaussian.h"

const char* gaussianTest() {
	static const char* testName = "gaussianInput";
	const char* failMessage = "Testing Gaussian input file generation failed.";

	printf("\nPerforming Gaussian input test...\n");
	std::string dir = filesDir + "/gaussianTest";
	std::string inputFile = dir + "/batchGaussian.xml";
	Input input;
	if (!input.load(inputFile.c_str())) {
		puts(failMessage);
		return testName;
	}

	Gaussian* gaussian = (Gaussian*)input.m_pAction->energyXml.getEnergy();
	std::string gaussianFile = dir + "/gaussian.com";
	if (!gaussian->createInputFile(gaussianFile.c_str(), **(input.m_pAction->m_structures.begin()))) {
		puts(failMessage);
		return testName;
	}

	printf("Gaussian input test succeeded!\n");
	return NULL;
}

const char* testChangeFilePrefix() {
	static const char* testName = "testChangeFilePrefix";
	const char* failMessage = "Test failed: testChangeFilePrefix!";

	puts("\nPerforming test of the FileUtils::changeFilePrefix method...");

	std::string dir = filesDir + "/testChangeFilePrefix";
	std::string prefix = "renameMe";
	std::string changedPrefix = "changed";

	if (!FileUtils::changeFilePrefix(prefix, changedPrefix, dir)) {
		puts(failMessage);
		puts("\tReason: the FileUtils::changeFilePrefix method didn't work. Your operating system might not have the 'rename' command or this command may not be implemented as expected.");
		puts("\tPlease find a similar command and update the method.");
		return testName;
	}
	std::string expectedFile = dir + "/" + changedPrefix + ".txt";
	if (!FileUtils::exists(expectedFile.c_str())) {
		puts(failMessage);
		printf("\tReason: unable to find file '%s'. The rename didn't work.\n", expectedFile.c_str());
		return testName;
	}
	if (!FileUtils::changeFilePrefix(changedPrefix, prefix, dir)) {
		puts(failMessage);
		puts("\tReason: reverse rename didn't work.");
		return testName;
	}

	puts("Test of FileUtils::changeFilePrefix succeeded!");
	return NULL;
}

