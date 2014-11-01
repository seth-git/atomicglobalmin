
#ifndef __TESTINIT_H_
#define __TESTINIT_H_

#include "settings.h"
#include "../input.h"
#include "../handbook/handbook.h"
#include "util.h"

const std::string testFilesDir = "test/testinit";

const char* testMatrixMultiplication();

const char* ccLibReadTest();

const char* testSeeding();
const char* testSeeding2();
const char* testSeeding3();
const char* testSeeding4();

//const char* testInitializationResults;

const char* testPlaceAtomGroupRelativeToAnother(void);

const char* testInitialization();

const char* testLJ7();

#endif
