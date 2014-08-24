
#include "testxml.h"
#include <string>

char* readFileToString(const char* fileName) {
	FILE* f = fopen(fileName, "r");
	if (f == NULL) {
	    printf("Unable to read file: %s\n", fileName);
	    return NULL;
	}

	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* string = (char*)malloc(fsize + 1);
	size_t size_read = fread(string, fsize, 1, f);
	if (size_read != 1) {
		delete[] string;
		printf("Error reading file: %s, fread returned %zu\n", fileName, size_read);
		return NULL;
	}
	fclose(f);

	string[fsize] = 0;
	return string;
}

const char* xmlBatchTest() {
	static const char* testName = "xmlBatchTest";
	const char* failMessage = "Testing of batch xml read/write failed!";

	printf("\nPerforming batch xml read/write test...\n");
	std::string inputFile = testXmlDir + "/batch.xml";
	std::string inputFile2 = testXmlDir + "/batch2.xml";
	Input input;
	if (!input.load(inputFile.c_str())) {
		puts(failMessage);
		return testName;
	}

	if (!input.save(inputFile2.c_str())) {
		puts(failMessage);
		return testName;
	}

	char* file1Text = readFileToString(inputFile.c_str());
	if (NULL == file1Text) {
		puts(failMessage);
		return testName;
	}
	char* file2Text = readFileToString(inputFile2.c_str());
	if (NULL == file2Text) {
		puts(failMessage);
		return testName;
	}
	bool same = strcmp(file1Text, file2Text) == 0;
	delete[] file1Text;
	delete[] file2Text;
	if (!same) {
		puts(failMessage);
		printf("Error: files should be identical, but they're not: '%s' and '%s'.\n", inputFile.c_str(), inputFile2.c_str());
		return testName;
	}

	std::string rmCommand;
	rmCommand.append("rm ").append(inputFile2.c_str());
	bool deleteSuccess = !system(rmCommand.c_str());
	if (!deleteSuccess) {
		puts(failMessage);
		printf("Unable to delete file: '%s'\n", inputFile2.c_str());
		return testName;
	}

	printf("Batch xml read/write test succeeded!\n");
	return NULL;
}
