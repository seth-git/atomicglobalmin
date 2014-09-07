/*
 * ExternalEnergyMethod.h
 *
 *  Created on: Dec 31, 2012
 *      Author: sethcall
 */

#ifndef EXTERNAL_ENERGY_H_
#define EXTERNAL_ENERGY_H_

class ExternalEnergyXml; // Forward declaration

#include "../energy.h"
#include "../xsd/xsdTypeUtil.h"
#include <fstream>

class ExternalEnergy : public Energy {
public:
	enum Impl {ADF, GAMESS, GAMESS_UK, GAUSSIAN, FIREFLY, JAGUAR, MOLPRO, ORCA};

	ExternalEnergy(const ExternalEnergyXml* pExternalEnergyXml);
	virtual ~ExternalEnergy() {};

	static ExternalEnergy* instance(Impl impl, const ExternalEnergyXml* pExternalEnergyXml);

	virtual bool createInputFile(const char* inputFileName, const Structure &structure) = 0;

	static void readOutputFile(const char* outputFile, FLOAT &energy,
			Structure* pStructure, bool &openedFile, bool &readEnergy,
			bool &obtainedGeometry);

	static bool readOutputFile(Impl impl, const char* outputFile, Structure &structure, bool readGeometry);

	static bool isCCLibInstalled(std::string &error);

	static bool readOutputFileWithCCLib(const char* fileName, Structure &structure, bool readGeometry);

	virtual bool setup() = 0; // Call this in preparation for calling execute
	virtual bool execute(Structure &structure) = 0;
	virtual bool clear() = 0; // Call this when execute will no longer be called and before the process terminates

	static const char* getOutputFileExtension(Impl impl);

	static bool getEnum(const char* attributeName, const char* stringValue, Impl &result, const rapidxml::xml_node<>* pElem);
	static const char* getEnumString(Impl enumValue);

	static unsigned int s_iMaxEnergyCalcFailuresOnStructure;

	struct cmp_str {
		bool operator()(char const *a, char const *b) { return std::strcmp(a, b) < 0; }
	};

	/**************************************************************************
	 * Purpose: Perform replacements in src putting the result in dest.
	 * Parameters: src - the source string
	 *             symbol - a character indicating a replacement is found
	 *                This method loads the string that comes after the symbol,
	 *                and searches for it as a key in the map. It then replaces
	 *                the symbol and the key with the value from the map.
	 *             map - pairs of search and replacement strings (see above).
	 *             dest - Results are appended here.
	 */
	static void replace(std::string src, const char symbol, std::map<const char*, const char*, cmp_str> &map, std::string &dest);

protected:
	const ExternalEnergyXml* m_pExternalEnergyXml;

private:
	static const char* cclibPythonScript;
};

#endif /* EXTERNAL_ENERGY_H_ */
