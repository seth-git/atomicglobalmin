
#include "xsdTypeUtil.h"

using namespace strings;

const char* XsdTypeUtil::s_booleanValues[] = {pFalse, pTrue};
const int  XsdTypeUtil::s_booleanResponses[] = {0, 1};

bool XsdTypeUtil::getBoolValue(const char* attributeName, const char* attributeValue, bool &result, const rapidxml::xml_node<>* node) {
	int val;
	if (!getEnumValue(attributeName, attributeValue, val, node, s_booleanValues, s_booleanResponses))
		return false;
	result = (bool)val;
	return true;
}

void XsdTypeUtil::printError(const char* attributeName, const char* attributeValue, const rapidxml::xml_node<>* node, const char** possibleValues, unsigned int numPossibleValues) {
	std::string possibleValuesConcatenated;
	possibleValuesConcatenated.append("'").append(possibleValues[0]).append("'");
	for (unsigned int i = 1; i < numPossibleValues; ++i)
		possibleValuesConcatenated.append(", '").append(possibleValues[i]).append("'");
	printf(UnrecognizedAttributeValue, attributeValue, attributeName, node->name(), possibleValuesConcatenated.c_str());
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function ensures that directory names have only forward
//     slashes and no back slashes.  It also ensures that there
//     is no trailing forward slash.
// Parameters: sourceDir - the source directory (or file name)
//             newDir - the destination directory (or file name)
// Returns: true if the directory or file name is not empty
bool XsdTypeUtil::checkDirectoryOrFileName(const char* sourceDir, std::string &newDir, const char* attributeName, const rapidxml::xml_node<>* node) {
	unsigned int sourceDirLength = strlen(sourceDir);
	char *dir;
	char *dirIndex;
	int dirLength;

	if (sourceDirLength == 0) {
		printf(ErrorEmptyPath, attributeName, node->name());
		return false;
	}

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

	return true;
}

bool XsdTypeUtil::getInteger(const char* value, int &result, const char* attributeName, const rapidxml::xml_node<>* node)
{
	if (sscanf(value, "%d", &result) != 1) {
		printf(UnableToReadInteger, value, attributeName, node->name());
		return false;
	}
	return true;
}

bool XsdTypeUtil::getPositiveInt(const char* value, unsigned int &result, const char* attributeName, const rapidxml::xml_node<>* node)
{
	int signedResult;
	if (!getInteger(value, signedResult, attributeName, node))
		return false;
	if (signedResult == 0) {
		printf(ZeroNotAllowed, attributeName, node->name());
		return false;
	}
	if (signedResult < 0) {
		printf(NegativeNotAllowed, value, attributeName, node->name());
		return false;
	}
	result = (unsigned int)signedResult;
	return true;
}

bool XsdTypeUtil::getNonNegativeInt(const char* value, unsigned int &result, const char* attributeName, const rapidxml::xml_node<>* node)
{
	int signedResult;
	if (!getInteger(value, signedResult, attributeName, node))
		return false;
	if (signedResult < 0) {
		printf(NegativeNotAllowed, value, attributeName, node->name());
		return false;
	}
	result = (unsigned int)signedResult;
	return true;
}

bool XsdTypeUtil::getFloat(const char* value, FLOAT &result, const char* attributeName, const rapidxml::xml_node<>* node)
{
	if (sscanf(value, "%lf", &result) != 1) {
		printf(UnableToReadPositiveFloat, value, attributeName, node->name());
		return false;
	}
	return true;
}

bool XsdTypeUtil::getPositiveFloat(const char* value, FLOAT &result, const char* attributeName, const rapidxml::xml_node<>* node)
{
	if (sscanf(value, "%lf", &result) != 1) {
		printf(UnableToReadPositiveFloat, value, attributeName, node->name());
		return false;
	}
	if (result == 0) {
		printf(ZeroNotAllowed, attributeName, node->name());
		return false;
	}
	if (result < 0) {
		printf(NegativeNotAllowed, value, attributeName, node->name());
		return false;
	}
	return true;
}

bool XsdTypeUtil::getNonNegativeFloat(const char* value, FLOAT &result, const char* attributeName, const rapidxml::xml_node<>* node)
{
	if (sscanf(value, "%lf", &result) != 1) {
		printf(UnableToReadFloat, value, attributeName, node->name());
		return false;
	}
	if (result < 0) {
		printf(NegativeNotAllowed, value, attributeName, node->name());
		return false;
	}
	return true;
}

bool XsdTypeUtil::readTimeT(const char* value, time_t &result, const char* attributeName, const rapidxml::xml_node<>* node) {
	long long num;
	if (sscanf(value, "%lld", &num) != 1) {
		printf(UnableToReadTime, value, attributeName, node->name());
		return false;
	}
	result = num;
	return true;
}

bool XsdTypeUtil::read1StrAtt(const rapidxml::xml_node<>* node, std::string &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* valueAttNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(valueAttNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(node))
		return false;
	values = valueUtil.getAllAttributes();
	result = values[0];
	return true;
}

bool XsdTypeUtil::read1DirAtt(const rapidxml::xml_node<>* node, std::string &resultDir, const char* attributeName, bool required, const char* defaultValue) {
	static const char* valueAttNames[] = {attributeName};
	static const bool  valueAttReq[]   = {required};
	static const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil dirUtil(valueAttNames, valueAttReq, valueAttDef);
	if (!dirUtil.process(node)) {
		return false;
	}
	values = dirUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return checkDirectoryOrFileName(values[0], resultDir, attributeName, node);
}

bool XsdTypeUtil::read1BoolAtt(const rapidxml::xml_node<>* node, bool &result, const char* attributeName, bool required, const char* defaultValue) {
	std::string resultStr;
	if (!read1StrAtt(node, resultStr, attributeName, required, defaultValue))
		return false;
	return getBoolValue(attributeName, resultStr.c_str(), result, node);
}


bool XsdTypeUtil::read1IntAtt(const rapidxml::xml_node<>* node, int &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* valueAttNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(valueAttNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(node))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return getInteger(values[0], result, valueAttNames[0], node);
}

bool XsdTypeUtil::read1PosIntAtt(const rapidxml::xml_node<>* node, unsigned int &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* valueAttNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(valueAttNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(node))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return getPositiveInt(values[0], result, valueAttNames[0], node);
}

bool XsdTypeUtil::read1NonNegativeIntAtt(const rapidxml::xml_node<>* node, unsigned int &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* valueAttNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(valueAttNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(node))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return getNonNegativeInt(values[0], result, valueAttNames[0], node);
}

bool XsdTypeUtil::read1PosFloatAtt(const rapidxml::xml_node<>* node, FLOAT &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* attributeNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(attributeNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(node))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return getPositiveFloat(values[0], result, attributeName, node);
}

bool XsdTypeUtil::read1TimeT(const rapidxml::xml_node<>* node, time_t &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* attributeNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(attributeNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(node))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return readTimeT(values[0], result, attributeName, node);
}

bool XsdTypeUtil::readElementText(const rapidxml::xml_node<>* node, std::string &result) {
	char* text = node->first_node()->value();
	if (text == NULL) {
		printf(UnableToReadElementText, node->name());
		return false;
	}
	result = trim(text);
	return true;
}

bool XsdTypeUtil::getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber, const char* attributeName, const char* elementName) {
	int signedNumber;
	iAtomicNumber = Handbook::getAtomicNumber(numberOrSymbol); // look for symbol
	if (iAtomicNumber == 0) {
		if (sscanf(numberOrSymbol, "%d", &signedNumber) != 1) { // look for number
			if (attributeName != NULL && elementName != NULL)
				printf(ErrorReadingAtomicNumber, numberOrSymbol, attributeName, elementName);
			else
				printf(ErrorReadingAtomicNumber2, numberOrSymbol);
			return false;
		}
		if (signedNumber <= 0) {
			if (attributeName != NULL && elementName != NULL)
				printf(ErrorReadingAtomicNumber, numberOrSymbol, attributeName, elementName);
			else
				printf(ErrorReadingAtomicNumber2, numberOrSymbol);
			return false;
		}
		iAtomicNumber = signedNumber;
		if (iAtomicNumber > MAX_ATOMIC_NUMBERS) {
			if (attributeName != NULL && elementName != NULL)
				printf(ErrorAtomicNumOverMax, attributeName, elementName, iAtomicNumber, MAX_ATOMIC_NUMBERS);
			else
				printf(ErrorAtomicNumOverMax2, iAtomicNumber, MAX_ATOMIC_NUMBERS);
			return false;
		}
	}
	return true;
}

bool XsdTypeUtil::getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber) {
	return getAtomicNumber(numberOrSymbol, iAtomicNumber, NULL, NULL);
}

bool XsdTypeUtil::inRange(FLOAT number, FLOAT lo, FLOAT hi, const rapidxml::xml_node<>* node, const char* attName) {
	if (number < lo || number > hi) {
		char loStr[100], hiStr[100];
		createFloat(lo, loStr, sizeof(loStr));
		createFloat(hi, hiStr, sizeof(hiStr));
		printf(RangeError, attName, node->name(), loStr, hiStr);
		return false;
	}
	return true;
}

size_t XsdTypeUtil::createFloat(FLOAT value, char* buffer, size_t size) {
	snprintf(buffer, size, "%0.15f", value);

	// Remove trailing zeros
	size_t i = 0;
	while (buffer[i] != '\0')
		++i;
	--i;
	while (buffer[i] == '0') {
		buffer[i] = '\0';
		--i;
		if (buffer[i] == '.') {
			buffer[i] = '\0';
			--i;
			break;
		}
	}
	return i+1; // don't include the '\0' character
}

char* XsdTypeUtil::createFloat(FLOAT value, rapidxml::xml_document<> &doc) {
	char buffer[100];
	size_t len = createFloat(value, buffer, sizeof(buffer));
	return doc.allocate_string(buffer, len+1);
}

char* XsdTypeUtil::createInt(int value, rapidxml::xml_document<> &doc) {
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%d", value);
	return doc.allocate_string(buffer);
}

char* XsdTypeUtil::createUnsignedInt(unsigned int value, rapidxml::xml_document<> &doc) {
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%u", value);
	return doc.allocate_string(buffer);
}

char* XsdTypeUtil::createTimeT(time_t value, rapidxml::xml_document<> &doc) {
	char buffer[100];
	snprintf(buffer, sizeof(buffer), "%lld", (long long)value);
	return doc.allocate_string(buffer);
}

const char* XsdTypeUtil::createBool(bool value) {
	return s_booleanValues[(unsigned int)value];
}

void XsdTypeUtil::setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, unsigned int value) {
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%u", value);
	const char* allocatedString = doc.allocate_string(buffer);
	node->append_attribute(doc.allocate_attribute(attribute, allocatedString));
}

void XsdTypeUtil::setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, int value) {
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "%d", value);
	const char* allocatedString = doc.allocate_string(buffer);
	node->append_attribute(doc.allocate_attribute(attribute, allocatedString));
}

void XsdTypeUtil::setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, FLOAT value) {
	const char* allocatedString = createFloat(value, doc);
	node->append_attribute(doc.allocate_attribute(attribute, allocatedString));
}

void XsdTypeUtil::setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, time_t value) {
	node->append_attribute(doc.allocate_attribute(attribute, createTimeT(value, doc)));
}

void XsdTypeUtil::setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, bool value) {
	node->append_attribute(doc.allocate_attribute(attribute, createBool(value)));
}

const std::string XsdTypeUtil::trim(const std::string& pString)
{
	static const std::string& pWhitespace = " \t\n\r";
    const size_t beginStr = pString.find_first_not_of(pWhitespace);
    if (beginStr == std::string::npos)
    {
        // no content
        return "";
    }

    const size_t endStr = pString.find_last_not_of(pWhitespace);
    const size_t range = endStr - beginStr + 1;

    return pString.substr(beginStr, range);
}

const std::string XsdTypeUtil::trim(const char * pCharArr)
{
	const std::string& pString = pCharArr;
	return trim(pString);
}

const char * XsdTypeUtil::getTrueFalseParam(bool value) {
	if (value)
		return pTrue;
	else
		return pFalse;
}
