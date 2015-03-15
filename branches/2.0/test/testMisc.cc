
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

const char* testSortResults() {
	using namespace std;
	static const char* testName = "testSortResults";
	const char* failMessage = "Test failed: testSortResults!";

	puts("\nPerforming result sorting test (testSortResults)...");
	string dir = filesDir + "/testSortResults";
	string file = dir + "/batch.xml";
	Input input;
	if (!input.load(file.c_str())) {
		puts(failMessage);
		return testName;
	}
	Batch* batch = (Batch*)input.m_pAction;
	list<Structure*> structures = batch->m_structures;
	string command;
	string resultsDir = batch->energyXml.m_externalEnergyXml.m_sResultsDir;
	if (FileUtils::exists(resultsDir.c_str()))
		if (!FileUtils::deleteFile(resultsDir.c_str())) {
			puts(failMessage);
			return testName;
		}
	if (!FileUtils::makeDirectory(resultsDir.c_str())) {
		puts(failMessage);
		return testName;
	}

	for (list<Structure*>::iterator it = structures.begin(); it != structures.end(); ++it) {
		(*it)->m_sFilePrefix = strings::pStructure + ExternalEnergy::ToString((*it)->getId());
		command = "touch " + resultsDir + "/" + (*it)->m_sFilePrefix + "." + Gaussian::s_sOutputFileExtension;
		if (!FileUtils::executeCommand(command.c_str())) {
			puts(failMessage);
			return testName;
		}
		batch->processResult(*it);
		batch->renameResultsFiles();
	}

	/************************** Check the results *************************/
	unsigned int expectedResultSize = 6;
	if (batch->m_results.size() != expectedResultSize) {
		puts(failMessage);
		printf("\tExpected number of best structures. Expected: %u, Actual: %u\n", expectedResultSize, (unsigned int)batch->m_results.size());
		return testName;
	}
	int expectedIds[] = {11, 5, 8, 6, 9, 10};
	int index = 0;
	string expectedPrefix;
	for (list<Structure*>::iterator it = batch->m_results.begin(); it != batch->m_results.end(); ++it) {
		if ((*it)->getId() != expectedIds[index]) {
			puts(failMessage);
			printf("\tUnexpected id at index %d in the list of best structures. Expected: %d. Actual: %d.\n", index, expectedIds[index], (*it)->getId());
			return testName;
		}
		if (index >= 4) {
			if ((*it)->m_sFilePrefix.length() > 0) {
				puts(failMessage);
				printf("\tUnexpected filePrefix at index %d in the list of best structures. Expected: none. Actual: '%s'.\n", index, (*it)->m_sFilePrefix.c_str());
				return testName;
			}
		} else {
			expectedPrefix = batch->energyXml.m_externalEnergyXml.m_sResultsFilePrefix + ExternalEnergy::ToString(index+1);
			if ((*it)->m_sFilePrefix != expectedPrefix) {
				puts(failMessage);
				printf("\tUnexpected filePrefix at index %d in the list of best structures. Expected: '%s'. Actual: '%s'.\n", index, expectedPrefix.c_str(), (*it)->m_sFilePrefix.c_str());
				return testName;
			}
		}
		++index;
		for (list<Structure*>::iterator it2 = it; it2 != batch->m_results.end(); ++it2)
			if (it != it2) {
				FLOAT rmsDist = RmsDistance::calculate(**it, **it2);
				if (rmsDist < batch->m_fResultsRmsDistance) {
					puts(failMessage);
					printf("\tThe structures %d and %d are at an RMS distance that is closer than expected. Minimum: '%lf'. Actual: '%lf'.\n", (*it)->getId(), (*it2)->getId(), batch->m_fResultsRmsDistance, rmsDist);
					return testName;
				}
			}
	}

	#if ACTION_DEBUG
		input.m_sFileName = dir + "/batch2.xml";
		if (!input.save()) {
			puts(failMessage);
			return testName;
		}
	#else
		string file2 = dir + "/batch2.xml";
		if (FileUtils::exists(file2.c_str())) {
			if (!FileUtils::deleteFile(file2.c_str())) {
				puts(failMessage);
				return testName;
			}
		}
		if (!FileUtils::deleteFile(resultsDir.c_str())) {
			puts(failMessage);
			return testName;
		}
	#endif

	puts("Test of result sorting succeeded!");
	return NULL;
}
