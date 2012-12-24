
#include "handbook.h"

std::string Handbook::s_rgAtomcSymbols[MAX_ATOMIC_NUMBERS+1];
FLOAT Handbook::s_rgAtomicMasses[MAX_ATOMIC_NUMBERS+1];
std::map<std::string,unsigned int> Handbook::s_symbolAtomicNumberMap;

bool Handbook::init() {
	if (!initAtomicMasses())
		return false;
	return true;
}

bool Handbook::initAtomicMasses() {
	char fileLine[500];
	char* myString;
	char tempString[100];
	unsigned int atomicNumber;
	unsigned int massNumber;
	FLOAT mass, abundance, highestAbundance;
	const char* fileName = "handbook/periodic_table.txt";

	ifstream infile(fileName);
	if (!infile)
	{
		printf("Unable to open the file: %1$s\n", fileName);
		return false;
	}

	// Read one blank line
	if (!infile.getline(fileLine, sizeof(fileLine)))
	{
		printf("Unable to read the file: %1$s\n", fileName);
		return false;
	}
	highestAbundance = -1;
	atomicNumber = 0;
	while(true)
	{
		if (!infile.getline(fileLine, sizeof(fileLine)))
			break;
		if (strlen(fileLine) == 0)
			break;
		if (fileLine[0] != '\t') {
			myString = strtok(fileLine, "\t");
			if (myString == NULL)
				break;
			atomicNumber = atoi(myString);
			highestAbundance = -1;
			if (atomicNumber > MAX_ATOMIC_NUMBERS)
			{
				printf("Haven't allocated enough memeory for atomic numbers above %d.\n", MAX_ATOMIC_NUMBERS);
				printf("Please change the value for MAX_ATOMIC_NUMBERS in typedef.h.");
				return false;
			}
			if (atomicNumber < 1)
			{
				printf("Atomic numbers below 1 are not allowed.\n");
				return false;
			}
			myString = strtok(NULL, "\t"); // get the element name
			if (myString == NULL) {
				printf("Missing element name in %$1s.\n", fileName);
				return false;
			}
			myString = strtok(NULL, "\t"); // get the element symbol
		} else {
			myString = strtok(fileLine, "\t"); // get the element symbol
		}

		if (myString == NULL) {
			printf("Missing element symbol in %$1s.\n", fileName);
			return false;
		}
		if (sscanf(myString, "%d%s", &massNumber, tempString) != 2) {
			printf("Unable to read element symbol in %$1s.\n", fileName);
			return false;
		}
		s_rgAtomcSymbols[atomicNumber] = tempString;
		s_symbolAtomicNumberMap[tempString] = atomicNumber;
		myString = strtok(NULL, "\t"); // mass
		if (myString == NULL) {
			printf("Unable to read element mass in %$1s.\n", fileName);
			return false;
		}
		mass = atof(myString);
		myString = strtok(NULL, "\t"); // abundance
		if (myString == NULL) {
			printf("Unable to read element abundance in %$1s.\n", fileName);
			return false;
		}
		abundance = atof(myString);
		if (abundance > highestAbundance) {
			highestAbundance = abundance;
			s_rgAtomicMasses[atomicNumber] = mass;
		}
	}
	infile.close();
	return true;
}


FLOAT Handbook::getMass(unsigned int atomicNumber) {
	return s_rgAtomicMasses[atomicNumber];
}

const char* Handbook::getAtomicSymbol(unsigned int atomicNumber) {
	return s_rgAtomcSymbols[atomicNumber].c_str();
}

unsigned int Handbook::getAtomicNumber(const char* atomicSymbol) {
	std::map<std::string,unsigned int>::iterator it = s_symbolAtomicNumberMap.find(atomicSymbol);
	if (it == s_symbolAtomicNumberMap.end())
		return 0;
	else
		return (*it).second;
}
