
#ifndef __XSD_TYPE_UTIL_H__
#define __XSD_TYPE_UTIL_H__

#include <stdio.h>
#include <string>
#include <string.h>

#define ERROR_VALUE               -1

class XsdTypeUtil {
	public:
		static const std::string s_booleanValues[];
		static const int s_booleanResponses[];

		static int getBoolValue(const char* attributeName, const char* attributeValue, const char* elementName);
		static int getEnumValue(const char* attributeName, const char* attributeValue, const char* elementName,
                                const std::string* possibleValues, unsigned int numPossibleValues, const int* responses);
	private:
		static void printError(const char* attributeName, const char* attributeValue, const char* elementName, const std::string* possibleValues, unsigned int numPossibleValues);
};

#endif
