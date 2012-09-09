
#ifndef __XSD_TYPE_UTIL_H__
#define __XSD_TYPE_UTIL_H__

#include <stdio.h>
#include <string>
#include "xsdAttributeUtil.h"
#include "typedef.h"

class XsdTypeUtil {
	public:
		static bool getBoolValue(const char* attributeName, const char* attributeValue, bool &result, TiXmlElement *pElem, const Strings* messages);
		static bool getEnumValue(const char* attributeName, const char* attributeValue, int &result, TiXmlElement *pElem,
                                 const char** possibleValues, unsigned int numPossibleValues, const int* responses);
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
	private:
		static void printError(const char* attributeName, const char* attributeValue, TiXmlElement *pElem, const char** possibleValues, unsigned int numPossibleValues);
		
		static const int   s_booleanResponses[];
		static const bool  s_valueAttReq[];
		static const char* s_valueAttDef[];
};

#endif