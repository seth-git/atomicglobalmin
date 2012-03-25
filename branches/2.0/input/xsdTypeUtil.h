
#ifndef __XSD_TYPE_UTIL_H__
#define __XSD_TYPE_UTIL_H__

#include <stdio.h>
#include <string>
#include "xsdAttributeUtil.h"
#include "typedef.h"

class XsdTypeUtil {
	public:
		static bool getBoolValue(const char* attributeName, const char* attributeValue, bool &result, const char* elementName);
		static bool getEnumValue(const char* attributeName, const char* attributeValue, int &result, const char* elementName,
                                const std::string* possibleValues, unsigned int numPossibleValues, const int* responses);
		static bool readDirType(TiXmlElement *pElem, std::string &resultDir);
		static void checkDirectoryOrFileName(const char* sourceDir, std::string &newDir);

		static bool getInteger(const char* value, int &result, const char* attributeName, const char* elementName);
		static bool getPositiveInt(const char* value, unsigned int &result, const char* attributeName, const char* elementName);
		static bool getPositiveFloat(const char* value, FLOAT &result, const char* attributeName, const char* elementName);
		
		static bool readStrValueElement(TiXmlElement *pElem, std::string &result);
		static bool readStrValueElement(TiXmlElement *pElem, std::string &result, const std::string* attributeName);
		static bool readIntValueElement(TiXmlElement *pElem, int &result);
		static bool readPosIntValueElement(TiXmlElement *pElem, unsigned int &result);
		static bool readPosFloatValueElement(TiXmlElement *pElem, FLOAT &result);
		static bool readPosFloatValueElement(TiXmlElement *pElem, FLOAT &result, const std::string* attributeName);
		
		static bool readElementText(TiXmlElement *pElem, std::string &result);
	private:
		static void printError(const char* attributeName, const char* attributeValue, const char* elementName, const std::string* possibleValues, unsigned int numPossibleValues);
		
		static const std::string s_booleanValues[];
		static const int         s_booleanResponses[];
		static const std::string s_valueAttNames[];
		static const bool        s_valueAttReq[];
		static const std::string s_valueAttDef[];
};

#endif
