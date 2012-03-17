
#include "xsdTypeUtil.h"

const std::string XsdTypeUtil::s_booleanValues[] = {"true", "false"};
const int XsdTypeUtil::s_booleanResponses[]      = {1     , 0};

int XsdTypeUtil::getBoolValue(const char* attributeName, const char* attributeValue, const char* elementName) {
	return getEnumValue(attributeName, attributeValue, elementName, s_booleanValues, 2, s_booleanResponses);
}

int XsdTypeUtil::getEnumValue(const char* attributeName, const char* attributeValue, const char* elementName,
                              const std::string* possibleValues, unsigned int numPossibleValues, const int* responses) {
	for (unsigned int i = 0; i < numPossibleValues; ++i) {
		if (strncmp(possibleValues[i].c_str(), attributeValue, possibleValues[i].length() + 1) == 0) {
			return responses[i];
		}
	}
	printError(attributeName, attributeValue, elementName, possibleValues, numPossibleValues);
	return ERROR_VALUE;
}

void XsdTypeUtil::printError(const char* attributeName, const char* attributeValue, const char* elementName, const std::string* possibleValues, unsigned int numPossibleValues) {
	printf("Value '%s' is unrecognized for attribute '%s' in element '%s'.  The available values are '%s'", attributeValue, attributeName, elementName, possibleValues[0].c_str());
	for (unsigned int i = 1; i < numPossibleValues; ++i) {
		printf(", '%s'", possibleValues[i].c_str());
	}
	printf(".\n");
}
