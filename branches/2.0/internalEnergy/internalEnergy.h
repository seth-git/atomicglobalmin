
#ifndef INTERNAL_ENERGY_H_
#define INTERNAL_ENERGY_H_

class InternalEnergyXml; // Forward declaration

#include "../energy.h"
#include "../xsd/xsdTypeUtil.h"

class InternalEnergy : public Energy {
public:
	enum Impl {LENNARD_JONES};

	InternalEnergy(const InternalEnergyXml* pInternalEnergyXml);
	virtual ~InternalEnergy() {};

	static InternalEnergy* instance(Impl impl, const InternalEnergyXml* pInternalEnergyXml);

	bool setup() { return true; };
	bool cleanup() { return true; };

	virtual bool execute(Structure &structure);

	virtual bool calculateEnergy(Structure &structure) = 0;

	virtual bool localOptimization(Structure &structure) = 0;

	static bool getEnum(const char* attributeName, const char* stringValue, Impl &result, const rapidxml::xml_node<>* pElem);
	static const char* getEnumString(Impl enumValue);

protected:
	const InternalEnergyXml* m_pInternalEnergyXml;
};

#endif
