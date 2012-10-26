
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
		
		static bool readDirType(TiXmlElement *pElem, std::string &resultDir, const Strings* messages);
		static void checkDirectoryOrFileName(const char* sourceDir, std::string &newDir);

		static bool getInteger(const char* value, int &result, const char* attributeName, TiXmlElement *pElem);
		static bool getPositiveInt(const char* value, unsigned int &result, const char* attributeName, TiXmlElement *pElem);
		static bool getPositiveFloat(const char* value, FLOAT &result, const char* attributeName, TiXmlElement *pElem);
		
		static bool readStrValueElement(TiXmlElement *pElem, std::string &result, const Strings* messages);
		static bool readStrValueElement(TiXmlElement *pElem, std::string &result, const char* attributeName);
		static bool readIntValueElement(TiXmlElement *pElem, int &result, const Strings* messages);
		static bool readPosIntValueElement(TiXmlElement *pElem, unsigned int &result, const Strings* messages);
		static bool readPosFloatValueElement(TiXmlElement *pElem, FLOAT &result, const Strings* messages);
		static bool readPosFloatValueElement(TiXmlElement *pElem, FLOAT &result, const char* attributeName);
		
		static bool readElementText(TiXmlElement *pElem, std::string &result);
		
		static bool getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber, unsigned int line, const char* attributeName, const char* elementName);
		static bool getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber, unsigned int line);
	private:
		static void printError(const char* attributeName, const char* attributeValue, TiXmlElement *pElem, const char** possibleValues, unsigned int numPossibleValues);
		
		static const int   s_booleanResponses[];
		static const bool  s_valueAttReq[];
		static const char* s_valueAttDef[];
};

#endif
