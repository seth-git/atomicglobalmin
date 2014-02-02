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

class ExternalEnergy : public Energy {
public:
	enum Impl {ADF, GAMESS, GAMESS_UK, GAUSSIAN, FIREFLY, JAGUAR, MOLPRO, ORCA};

	ExternalEnergy(const ExternalEnergyXml* pExternalEnergyXml);

	static ExternalEnergy* instance(Impl impl, const ExternalEnergyXml* pExternalEnergyXml);

	virtual bool createInputFile(Structure &structure,
			unsigned int populationMemberNumber, bool resetInputFileName,
			bool writeMetaData) = 0;

	static void readOutputFile(const char* outputFile, FLOAT &energy,
			Structure* pStructure, bool &openedFile, bool &readEnergy,
			bool &obtainedGeometry);

	static bool readOutputFile(Impl impl, const char* outputFile, Structure &structure, bool readGeometry);

	static bool isCCLibInstalled(std::string &error);

	static bool readOutputFileWithCCLib(const char* fileName, Structure &structure, bool readGeometry);

	virtual bool setup() = 0;
	virtual bool execute(Structure &structure) = 0;
	virtual bool cleanup() = 0;

	static const char* getOutputFileExtension(Impl impl);

	static bool getEnum(const char* attributeName, const char* stringValue, Impl &result, TiXmlElement *pElem, const Strings* messages);
	static const char* getEnumString(Impl enumValue, const Strings* messages);

	static unsigned int s_iMaxEnergyCalcFailuresOnStructure;

protected:
	const ExternalEnergyXml* m_pExternalEnergyXml;

private:
	static const char* cclibPythonScript;
};

#endif /* EXTERNAL_ENERGY_H_ */
