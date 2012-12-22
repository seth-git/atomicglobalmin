
#ifndef __XSD_TYPE_UTIL_H__
#define __XSD_TYPE_UTIL_H__

#include <stdio.h>
#include <string>
#include "xsdAttributeUtil.h"
#include "typedef.h"

class XsdTypeUtil {
	public:
		static bool getBoolValue(const char* attributeName, const char* attributeValue, bool &result, TiXmlElement *pElem, const Strings* messages);

		// Templates can't be declared in .cc files
		template <std::size_t iResponses>
		static bool getEnumValue(const char* attributeName, const char* attributeValue, int &result, TiXmlElement *pElem,
		                               const char* (&possibleValues)[iResponses], const int (&responses)[iResponses]) {
			for (unsigned int i = 0; i < iResponses; ++i) {
				if (strcmp(possibleValues[i], attributeValue) == 0) {
					result = responses[i];
					return true;
				}
			}
			printError(attributeName, attributeValue, pElem, possibleValues, iResponses);
			return false;
		}

		template <std::size_t iResponses, typename EnumType>
		static bool getEnumValue(const char* attributeName, const char* attributeValue, EnumType &result,
		                         TiXmlElement *pElem, const char* (&possibleValues)[iResponses]) {
			for (unsigned int i = 0; i < iResponses; ++i) {
				if (strcmp(possibleValues[i], attributeValue) == 0) {
					result = static_cast<EnumType>(i);
					return true;
				}
			}
			printError(attributeName, attributeValue, pElem, possibleValues, iResponses);
			return false;
		}
		
		static bool checkDirectoryOrFileName(const char* sourceDir, std::string &newDir, const char* attributeName, TiXmlElement *pElem);
		static bool getInteger(const char* value, int &result, const char* attributeName, TiXmlElement *pElem);
		static bool getPositiveInt(const char* value, unsigned int &result, const char* attributeName, TiXmlElement *pElem);
		static bool getNonNegativeInt(const char* value, unsigned int &result, const char* attributeName, TiXmlElement *pElem);
		static bool getPositiveFloat(const char* value, FLOAT &result, const char* attributeName, TiXmlElement *pElem);
		static bool getNonNegativeFloat(const char* value, FLOAT &result, const char* attributeName, TiXmlElement *pElem);
		
		static bool read1StrAtt(TiXmlElement *pElem, std::string &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1DirAtt(TiXmlElement *pElem, std::string &resultDir, const char* attributeName, bool required, const char* defaultValue);
		static bool read1IntAtt(TiXmlElement *pElem, int &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1PosIntAtt(TiXmlElement *pElem, unsigned int &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1NonNegativeIntAtt(TiXmlElement *pElem, unsigned int &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1PosFloatAtt(TiXmlElement *pElem, FLOAT &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1NonNegativeFloatAtt(TiXmlElement *pElem, FLOAT &result, const char* attributeName, bool required, const char* defaultValue);
		
		static bool readElementText(TiXmlElement *pElem, std::string &result);
		
		static bool getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber, unsigned int line, const char* attributeName, const char* elementName);
		static bool getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber, unsigned int line);
		
		static bool inRange(FLOAT number, FLOAT lo, FLOAT hi, TiXmlElement *pElem, const char* attName);
	private:
		static void printError(const char* attributeName, const char* attributeValue, TiXmlElement *pElem, const char** possibleValues, unsigned int numPossibleValues);
		
		static const int   s_booleanResponses[];
};

#endif
