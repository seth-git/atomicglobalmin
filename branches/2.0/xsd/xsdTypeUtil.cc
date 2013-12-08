
#include "xsdTypeUtil.h"

//const char* XsdTypeUtil::s_booleanValues[] = {"true", "false"};
const int  XsdTypeUtil::s_booleanResponses[] = {1     , 0};

bool XsdTypeUtil::getBoolValue(const char* attributeName, const char* attributeValue, bool &result, TiXmlElement *pElem, const Strings* messages) {
	const char* booleanValues[] = {messages->m_spTrue.c_str(), messages->m_spFalse.c_str()};
	int val;
	if (!getEnumValue(attributeName, attributeValue, val, pElem, booleanValues, s_booleanResponses))
		return false;
	result = (bool)val;
	return true;
}

void XsdTypeUtil::printError(const char* attributeName, const char* attributeValue, TiXmlElement *pElem, const char** possibleValues, unsigned int numPossibleValues) {
	const Strings* messagesDL = Strings::instance();
	std::string possibleValuesConcatenated;
	possibleValuesConcatenated.append("'").append(possibleValues[0]).append("'");
	for (unsigned int i = 1; i < numPossibleValues; ++i)
		possibleValuesConcatenated.append(", '").append(possibleValues[i]).append("'");
	printf(messagesDL->m_sUnrecognizedAttributeValue.c_str(), attributeValue, attributeName, pElem->Value(), pElem->Row(), possibleValuesConcatenated.c_str());
}

/////////////////////////////////////////////////////////////////////
// Purpose: This function ensures that directory names have only forward
//     slashes and no back slashes.  It also ensures that there
//     is no trailing forward slash.
// Parameters: sourceDir - the source directory (or file name)
//             newDir - the destination directory (or file name)
// Returns: true if the directory or file name is not empty
bool XsdTypeUtil::checkDirectoryOrFileName(const char* sourceDir, std::string &newDir, const char* attributeName, TiXmlElement *pElem) {
	unsigned int sourceDirLength = strlen(sourceDir);
	char *dir;
	char *dirIndex;
	int dirLength;

	if (sourceDirLength == 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sErrorEmptyPath.c_str(), pElem->Row(), attributeName, pElem->Value());
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

bool XsdTypeUtil::getInteger(const char* value, int &result, const char* attributeName, TiXmlElement *pElem)
{
	if (sscanf(value, "%d", &result) != 1) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sUnableToReadInteger.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	return true;
}

bool XsdTypeUtil::getPositiveInt(const char* value, unsigned int &result, const char* attributeName, TiXmlElement *pElem)
{
	int signedResult;
	if (!getInteger(value, signedResult, attributeName, pElem))
		return false;
	if (signedResult == 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sZeroNotAllowed.c_str(), attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	if (signedResult < 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sNegativeNotAllowed.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	result = (unsigned int)signedResult;
	return true;
}

bool XsdTypeUtil::getNonNegativeInt(const char* value, unsigned int &result, const char* attributeName, TiXmlElement *pElem)
{
	int signedResult;
	if (!getInteger(value, signedResult, attributeName, pElem))
		return false;
	if (signedResult < 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sNegativeNotAllowed.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	result = (unsigned int)signedResult;
	return true;
}

bool XsdTypeUtil::getFloat(const char* value, FLOAT &result, const char* attributeName, TiXmlElement *pElem)
{
	if (sscanf(value, "%lf", &result) != 1) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sUnableToReadPositiveFloat.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	return true;
}

bool XsdTypeUtil::getPositiveFloat(const char* value, FLOAT &result, const char* attributeName, TiXmlElement *pElem)
{
	if (sscanf(value, "%lf", &result) != 1) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sUnableToReadPositiveFloat.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	if (result == 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sZeroNotAllowed.c_str(), attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	if (result < 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sNegativeNotAllowed.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	return true;
}

bool XsdTypeUtil::getNonNegativeFloat(const char* value, FLOAT &result, const char* attributeName, TiXmlElement *pElem)
{
	if (sscanf(value, "%lf", &result) != 1) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sUnableToReadFloat.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	if (result < 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sNegativeNotAllowed.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	return true;
}

bool XsdTypeUtil::readTimeT(const char* value, time_t &result, const char* attributeName, TiXmlElement *pElem) {
	long long num;
	if (sscanf(value, "%lld", &num) != 1) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sUnableToReadTime.c_str(), value, attributeName, pElem->Value(), pElem->Row());
		return false;
	}
	result = num;
	return true;
}

bool XsdTypeUtil::read1StrAtt(TiXmlElement *pElem, std::string &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* valueAttNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), valueAttNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(pElem))
		return false;
	values = valueUtil.getAllAttributes();
	result = values[0];
	return true;
}

bool XsdTypeUtil::read1DirAtt(TiXmlElement *pElem, std::string &resultDir, const char* attributeName, bool required, const char* defaultValue) {
	static const char* valueAttNames[] = {attributeName};
	static const bool  valueAttReq[]   = {required};
	static const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil dirUtil(pElem->Value(), valueAttNames, valueAttReq, valueAttDef);
	if (!dirUtil.process(pElem)) {
		return false;
	}
	values = dirUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return checkDirectoryOrFileName(values[0], resultDir, attributeName, pElem);
}

bool XsdTypeUtil::read1BoolAtt(TiXmlElement *pElem, bool &result, const char* attributeName, bool required, const char* defaultValue, const Strings* messages) {
	std::string resultStr;
	if (!read1StrAtt(pElem, resultStr, attributeName, required, defaultValue))
		return false;
	return getBoolValue(attributeName, resultStr.c_str(), result, pElem, messages);
}


bool XsdTypeUtil::read1IntAtt(TiXmlElement *pElem, int &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* valueAttNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), valueAttNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(pElem))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return getInteger(values[0], result, valueAttNames[0], pElem);
}

bool XsdTypeUtil::read1PosIntAtt(TiXmlElement *pElem, unsigned int &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* valueAttNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), valueAttNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(pElem))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return getPositiveInt(values[0], result, valueAttNames[0], pElem);
}

bool XsdTypeUtil::read1NonNegativeIntAtt(TiXmlElement *pElem, unsigned int &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* valueAttNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), valueAttNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(pElem))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return getNonNegativeInt(values[0], result, valueAttNames[0], pElem);
}

bool XsdTypeUtil::read1PosFloatAtt(TiXmlElement *pElem, FLOAT &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* attributeNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), attributeNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(pElem))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return getPositiveFloat(values[0], result, attributeName, pElem);
}

bool XsdTypeUtil::read1TimeT(TiXmlElement *pElem, time_t &result, const char* attributeName, bool required, const char* defaultValue) {
	const char* attributeNames[] = {attributeName};
	const bool  valueAttReq[]   = {required};
	const char* valueAttDef[]   = {defaultValue};
	const char** values;

	XsdAttributeUtil valueUtil(pElem->Value(), attributeNames, valueAttReq, valueAttDef);
	if (!valueUtil.process(pElem))
		return false;
	values = valueUtil.getAllAttributes();
	if (!required && (values[0] == NULL || values[0][0] == '\0'))
		return true;
	return readTimeT(values[0], result, attributeName, pElem);
}

bool XsdTypeUtil::readElementText(TiXmlElement *pElem, std::string &result) {
	if (pElem->GetText() == NULL) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sUnableToReadElementText.c_str(), pElem->Value(), pElem->Value(), pElem->Row());
		return false;
	}
	result = Strings::trim(pElem->GetText());
	return true;
}

bool XsdTypeUtil::getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber, unsigned int line, const char* attributeName, const char* elementName) {
	int signedNumber;
	iAtomicNumber = Handbook::getAtomicNumber(numberOrSymbol); // look for symbol
	if (iAtomicNumber == 0) {
		if (sscanf(numberOrSymbol, "%d", &signedNumber) != 1) { // look for number
			const Strings* messagesDL = Strings::instance();
			if (attributeName != NULL && elementName != NULL)
				printf(messagesDL->m_sErrorReadingAtomicNumber.c_str(), line, numberOrSymbol, attributeName, elementName);
			else
				printf(messagesDL->m_sErrorReadingAtomicNumber2.c_str(), line, numberOrSymbol);
			return false;
		}
		if (signedNumber <= 0) {
			const Strings* messagesDL = Strings::instance();
			if (attributeName != NULL && elementName != NULL)
				printf(messagesDL->m_sErrorReadingAtomicNumber.c_str(), line, numberOrSymbol, attributeName, elementName);
			else
				printf(messagesDL->m_sErrorReadingAtomicNumber2.c_str(), line, numberOrSymbol);
			return false;
		}
		iAtomicNumber = signedNumber;
		if (iAtomicNumber > MAX_ATOMIC_NUMBERS) {
			const Strings* messagesDL = Strings::instance();
			if (attributeName != NULL && elementName != NULL)
				printf(messagesDL->m_sErrorAtomicNumOverMax.c_str(), line, attributeName, elementName, iAtomicNumber, MAX_ATOMIC_NUMBERS);
			else
				printf(messagesDL->m_sErrorAtomicNumOverMax2.c_str(), line, iAtomicNumber, MAX_ATOMIC_NUMBERS);
			return false;
		}
	}
	return true;
}

bool XsdTypeUtil::getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber, unsigned int line) {
	return getAtomicNumber(numberOrSymbol, iAtomicNumber, line, NULL, NULL);
}

bool XsdTypeUtil::inRange(FLOAT number, FLOAT lo, FLOAT hi, TiXmlElement *pElem, const char* attName) {
	if (number < lo || number > hi) {
		const Strings* messagesDL = Strings::instance();
		char loStr[100], hiStr[100];
		doubleToString(lo, loStr);
		doubleToString(hi, hiStr);
		printf(messagesDL->m_sRangeError.c_str(), pElem->Row(), attName, pElem->Value(), loStr, hiStr);
		return false;
	}
	return true;
}

void XsdTypeUtil::writeTimeT(time_t value, TiXmlElement *pElem, const char* attName) {
	char temp[100];
	snprintf(temp, sizeof(temp), "%lld", (long long)value);
	pElem->SetAttribute(attName, temp);
}

void XsdTypeUtil::writeBool(bool value, TiXmlElement *pElem, const char* attName, const Strings* messages) {
	const char* booleanValues[] = {messages->m_spTrue.c_str(), messages->m_spFalse.c_str()};
	pElem->SetAttribute(attName, booleanValues[(unsigned int)value]);
}


