/*
 * ExternalEnergyMethod.h
 *
 *  Created on: Dec 31, 2012
 *      Author: sethcall
 */

#ifndef EXTERNAL_ENERGY_METHOD_H_
#define EXTERNAL_ENERGY_METHOD_H_

class ExternalEnergy; // Forward declaration

#include "../structure.h"
#include "../xsd/xsdTypeUtil.h"

class ExternalEnergyMethod {
public:
	enum Impl {ADF, GAMESS, GAMESS_UK, GAUSSIAN, FIREFLY, JAGUAR, MOLPRO, ORCA};

	ExternalEnergyMethod(const ExternalEnergy* pExternalEnergy) {m_pExternalEnergy = pExternalEnergy;}

	static ExternalEnergyMethod* instance(Impl impl, const ExternalEnergy* pExternalEnergy);

	virtual bool createInputFile(Structure &structure,
			unsigned int populationMemberNumber, bool resetInputFileName,
			bool writeMetaData) = 0;

	static void readOutputFile(const char* outputFile, FLOAT &energy,
			Structure* pStructure, bool &openedFile, bool &readEnergy,
			bool &obtainedGeometry);

	static bool readOutputFile(Impl impl, const char* outputFile, Structure &structure, bool readGeometry);

	static bool isCCLibInstalled(std::string &error);

	static bool readOutputFileWithCCLib(const char* fileName, Structure &structure, bool readGeometry);

	virtual bool doEnergyCalculation(unsigned int populationMemberNumber) = 0;

	static const char* getOutputFileExtension(Impl impl);

	static bool getEnum(const char* attributeName, const char* stringValue, Impl &result, TiXmlElement *pElem, const Strings* messages);
	static const char* getEnumString(Impl enumValue, const Strings* messages);

protected:
	const ExternalEnergy* m_pExternalEnergy;

private:
	static const char* cclibPythonScript;
};

#endif /* EXTERNAL_ENERGY_METHOD_H_ */
