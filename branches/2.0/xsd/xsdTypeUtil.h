
#ifndef __XSD_TYPE_UTIL_H__
#define __XSD_TYPE_UTIL_H__

#include <stdio.h>
#include <string>
#include "xsdAttributeUtil.h"
#include "typedef.h"
#include "../handbook/handbook.h"
#include <stddef.h>

class XsdTypeUtil {
	public:
		static bool getBoolValue(const char* attributeName, const char* attributeValue, bool &result, const rapidxml::xml_node<>* node);

		// Templates can't be declared in .cc files
		template <std::size_t iResponses>
		static bool getEnumValue(const char* attributeName, const char* attributeValue, int &result, const rapidxml::xml_node<>* node,
		                               const char* (&possibleValues)[iResponses], const int (&responses)[iResponses]) {
			for (unsigned int i = 0; i < iResponses; ++i) {
				if (strcmp(possibleValues[i], attributeValue) == 0) {
					result = responses[i];
					return true;
				}
			}
			printError(attributeName, attributeValue, node, possibleValues, iResponses);
			return false;
		}

		template <std::size_t iResponses, typename EnumType>
		static bool getEnumValue(const char* attributeName, const char* attributeValue, EnumType &result,
		                         const rapidxml::xml_node<>* node, const char* (&possibleValues)[iResponses]) {
			for (unsigned int i = 0; i < iResponses; ++i) {
				if (strcmp(possibleValues[i], attributeValue) == 0) {
					result = static_cast<EnumType>(i);
					return true;
				}
			}
			printError(attributeName, attributeValue, node, possibleValues, iResponses);
			return false;
		}
		
		static bool checkDirectoryOrFileName(const char* sourceDir, std::string &newDir, const char* attributeName, const rapidxml::xml_node<>* node);
		static bool getInteger(const char* value, int &result, const char* attributeName, const rapidxml::xml_node<>* node);
		static bool getPositiveInt(const char* value, unsigned int &result, const char* attributeName, const rapidxml::xml_node<>* node);
		static bool getNonNegativeInt(const char* value, unsigned int &result, const char* attributeName, const rapidxml::xml_node<>* node);
		static bool getFloat(const char* value, FLOAT &result, const char* attributeName, const rapidxml::xml_node<>* node);
		static bool getPositiveFloat(const char* value, FLOAT &result, const char* attributeName, const rapidxml::xml_node<>* node);
		static bool getNonNegativeFloat(const char* value, FLOAT &result, const char* attributeName, const rapidxml::xml_node<>* node);
		static bool readTimeT(const char* value, time_t &result, const char* attributeName, const rapidxml::xml_node<>* node);
		
		static bool read1StrAtt(const rapidxml::xml_node<>* node, std::string &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1DirAtt(const rapidxml::xml_node<>* node, std::string &resultDir, const char* attributeName, bool required, const char* defaultValue);
		static bool read1BoolAtt(const rapidxml::xml_node<>* node, bool &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1IntAtt(const rapidxml::xml_node<>* node, int &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1PosIntAtt(const rapidxml::xml_node<>* node, unsigned int &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1NonNegativeIntAtt(const rapidxml::xml_node<>* node, unsigned int &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1PosFloatAtt(const rapidxml::xml_node<>* node, FLOAT &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1NonNegativeFloatAtt(const rapidxml::xml_node<>* node, FLOAT &result, const char* attributeName, bool required, const char* defaultValue);
		static bool read1TimeT(const rapidxml::xml_node<>* node, time_t &result, const char* attributeName, bool required, const char* defaultValue);
		
		static bool readElementText(const rapidxml::xml_node<>* node, std::string &result);
		
		static bool getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber, const char* attributeName, const char* elementName);
		static bool getAtomicNumber(const char* numberOrSymbol, unsigned int &iAtomicNumber);
		
		static bool inRange(FLOAT number, FLOAT lo, FLOAT hi, const rapidxml::xml_node<>* node, const char* attName);

		static size_t createFloat(FLOAT value, char* buffer, size_t size);
		static char* createFloat(FLOAT value, rapidxml::xml_document<> &doc);
		static char* createInt(int value, rapidxml::xml_document<> &doc);
		static char* createUnsignedInt(unsigned int value, rapidxml::xml_document<> &doc);

		static char* createTimeT(time_t value, rapidxml::xml_document<> &doc);
		static const char* createBool(bool value);

		static void setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, unsigned int value);
		static void setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, int value);
		static void setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, FLOAT value);
		static void setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, time_t value);
		static void setAttribute(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* node, const char* attribute, bool value);

		static const std::string trim(const std::string& pString);
		static const std::string trim(const char * pCharArr);
		static const char * getTrueFalseParam(bool value);

	private:
		static void printError(const char* attributeName, const char* attributeValue, const rapidxml::xml_node<>* node, const char** possibleValues, unsigned int numPossibleValues);

		static const char* s_booleanValues[];
		static const int   s_booleanResponses[];
};

#endif
