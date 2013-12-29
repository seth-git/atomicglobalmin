
#ifndef INTERNAL_ENERGY_H_
#define INTERNAL_ENERGY_H_

class InternalEnergyXml; // Forward declaration

#include "../energy.h"
#include "../xsd/xsdTypeUtil.h"

class InternalEnergy : public Energy {
public:
	enum Impl {LENNARD_JONES};

	InternalEnergy(const InternalEnergyXml* pInternalEnergyXml);

	static InternalEnergy* instance(Impl impl, const InternalEnergyXml* pInternalEnergyXml);

	bool setup() { return true; };
	bool cleanup() { return true; };

	virtual bool execute(Structure &structure);

	virtual bool calculateEnergy(Structure &structure) = 0;

	virtual bool localOptimization(Structure &structure) = 0;

	static bool getEnum(const char* attributeName, const char* stringValue, Impl &result, TiXmlElement *pElem, const Strings* messages);
	static const char* getEnumString(Impl enumValue, const Strings* messages);

protected:
	const InternalEnergyXml* m_pInternalEnergyXml;
};

#endif
