
#include "xsdTypeUtil.h"

const std::string XsdTypeUtil::s_booleanValues[] = {"true", "false"};
const int XsdTypeUtil::s_booleanResponses[]      = {1     , 0};

const std::string XsdTypeUtil::s_valueAttNames[] = {"value"};
const bool        XsdTypeUtil::s_valueAttReq[]   = {true};
const std::string XsdTypeUtil::s_valueAttDef[]   = {""};


bool XsdTypeUtil::getBoolValue(const char* attributeName, const char* attributeValue, bool &result, const char* elementName) {
	int val;
	if (!getEnumValue(attributeName, attributeValue, val, elementName, s_booleanValues, 2, s_booleanResponses)) {
		return false;
	}
	result = (bool)val;
	return true;
}

bool XsdTypeUtil::getEnumValue(const char* attributeName, const char* attributeValue, int &result, const char* elementName,
                              const std::string* possibleValues, unsigned int numPossibleValues, const int* responses) {
	for (unsigned int i = 0; i < numPossibleValues; ++i) {
		if (strncmp(possibleValues[i].c_str(), attributeValue, possibleValues[i].length() + 1) == 0) {
			result = responses[i];
			return true;
		}
	}
	printError(attributeName, attributeValue, elementName, possibleValues, numPossibleValues);
	return false;
}

void XsdTypeUtil::printError(const char* attributeName, const char* attributeValue, const char* elementName, const std::string* possibleValues, unsigned int numPossibleValues) {
	printf("Value '%s' is unrecognized for attribute '%s' in element '%s'.  The available values are '%s'", attributeValue, attributeName, elementName, possibleValues[0].c_str());
	for (unsigned int i = 1; i < numPossibleValues; ++i) {
		printf(", '%s'", possibleValues[i].c_str());
	}
	printf(".\n");
}

bool XsdTypeUtil::readDirType(TiXmlElement *pElem, std::string &resultDir) {
	static const std::string attributeNames[] = {"path"};
	static const bool        required[]       = {true};
	static const std::string defaultValues[]  = {""};
	const char** values;

	XsdAttributeUtil dirUtil(pElem->Value(), attributeNames, 1, required, defaultValues);
	if (!dirUtil.process(pElem)) {
		return false;
	}
	values = dirUtil.getAllAttributes();
	checkDirectoryOrFileName(values[0], resultDir);
	return true;
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function ensures that directory names have only forward
//     slashes and no back slashes.  It also ensures that there
//     is no trailing forward slash.
// Parameters: sourceDir - the source directory (or file name)
//             newDir - the destination directory (or file name)
// Returns: nothing
void XsdTypeUtil::checkDirectoryOrFileName(const char* sourceDir, std::string &newDir)
{
	unsigned int sourceDirLength = strlen(sourceDir);
	char *dir;
	char *dirIndex;
	int dirLength;
	
	dir = new char[sourceDirLength+1];
	strncpy(dir,sourceDir,sourceDirLength+1);
	
	dirIndex = dir;
	dirLength = 0;
	
	// Change forward slashes to back slashes
	while (*dirIndex != '\0')
	{
		++dirLength;
		if (*dirIndex == '\\')
			*dirIndex = '/';
		++dirIndex;
	}

	// Check for the trailing back slash
	if (dirLength > 0)
		if (dir[dirLength-1] == '/')
			dir[dirLength-1] = '\0';
	
	newDir = dir;
	delete[] dir;
}

bool XsdTypeUtil::getInteger(const char* value, int &result, const char* attributeName, const char* elementName)
{
	if (sscanf(value, "%d", &result) != 1) {
		printf("Unable to read integer '%s' from attribute '%s' in element '%s'.\n", value, attributeName, elementName);
		return false;
	}
	return true;
}

bool XsdTypeUtil::getPositiveInt(const char* value, unsigned int &result, const char* attributeName, const char* elementName)
{
	if (sscanf(value, "%u", &result) != 1) {
		printf("Unable to read positive integer '%s' from attribute '%s' in element '%s'.\n", value, attributeName, elementName);
		return false;
	}
	if (result == 0) {
		printf("Zero is not allowed for the attribute '%s' in the element '%s'.\n", attributeName, elementName);
		return false;
	}
	return true;
}


bool XsdTypeUtil::readStrValueElement(TiXmlElement *pElem, std::string &result) {
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), s_valueAttNames, 1, s_valueAttReq, s_valueAttDef);
	if (!valueUtil.process(pElem)) {
		return false;
	}
	values = valueUtil.getAllAttributes();
	result = values[0];
	return true;
}

bool XsdTypeUtil::readIntValueElement(TiXmlElement *pElem, int &result) {
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), s_valueAttNames, 1, s_valueAttReq, s_valueAttDef);
	if (!valueUtil.process(pElem)) {
		return false;
	}
	values = valueUtil.getAllAttributes();
	if (!getInteger(values[0], result, s_valueAttNames[0].c_str(), pElem->Value())) {
		return false;
	}
	return true;
}

bool XsdTypeUtil::readPosIntValueElement(TiXmlElement *pElem, unsigned int &result) {
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), s_valueAttNames, 1, s_valueAttReq, s_valueAttDef);
	if (!valueUtil.process(pElem)) {
		return false;
	}
	values = valueUtil.getAllAttributes();
	if (!getPositiveInt(values[0], result, s_valueAttNames[0].c_str(), pElem->Value())) {
		return false;
	}
	return true;
}

bool XsdTypeUtil::readElementText(TiXmlElement *pElem, std::string &result) {
	if (pElem->GetText() == NULL) {
		printf("The element %s requires closing and opening tags with text in between (<%s>Text</%s>).\n", pElem->Value(), pElem->Value(), pElem->Value());
		return false;
	}
	result = pElem->GetText();
	return true;
}
