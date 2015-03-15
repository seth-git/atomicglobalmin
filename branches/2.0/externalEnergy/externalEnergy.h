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
#include "fileUtils.h"
#include <fstream>
#include <sstream>

class ExternalEnergy : public Energy {
public:
	enum Impl {ADF, GAMESS_UK, GAMESS_US, GAUSSIAN, FIREFLY, JAGUAR, MOLPRO, NWCHEM, ORCA};

	bool m_bReadGeometry;
	time_t m_tLongestExecutionTime;

	ExternalEnergy(const ExternalEnergyXml* pExternalEnergyXml);
	virtual ~ExternalEnergy() {};

	static ExternalEnergy* instance(Impl impl, const ExternalEnergyXml* pExternalEnergyXml);

	virtual bool createInputFile(const char* inputFileName, const Structure &structure) = 0;

	static bool readOutputFile(const char* outputFile, Structure &structure, bool readGeometry);

	static bool readOutputFile(Impl impl, const char* outputFile, Structure &structure, bool readGeometry);

	static bool isCCLibInstalled(std::string &error);

	static bool fileExists(const char* fileName);

	virtual bool setup(); // Call this in preparation for calling execute
	virtual bool execute(Structure &structure) = 0;
	bool stopFileExists();
	virtual bool cleanup(); // Call this when execute will no longer be called and before the process terminates

	static const char* getOutputFileExtension(Impl impl);

	static bool getEnum(const char* attributeName, const char* stringValue, Impl &result, const rapidxml::xml_node<>* pElem);
	static const char* getEnumString(Impl enumValue);

	static unsigned int s_iMaxEnergyCalcFailuresOnStructure;

	struct cmp_str {
		bool operator()(char const *a, char const *b) { return std::strcmp(a, b) < 0; }
	};

	static void replace(std::string &str, const std::string &find, const std::string &replace);

	template < class T >
	static std::string ToString(const T &arg) {
		std::ostringstream out;
		out << arg;
		return out.str();
	}

protected:
	const ExternalEnergyXml* m_pExternalEnergyXml;
	std::string m_sCalcDirectory;
	std::string m_sStopFile; // If this file exists, it's a message from the user that the program should terminate.

private:
	static const char* cclibPythonScript;
	static const char* s_methods[];
};

#endif /* EXTERNAL_ENERGY_H_ */
