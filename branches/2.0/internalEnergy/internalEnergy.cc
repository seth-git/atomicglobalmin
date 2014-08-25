
#include "internalEnergy.h"
#include "internalEnergyXml.h"
#include "lennardJones.h"


InternalEnergy::InternalEnergy(const InternalEnergyXml* pInternalEnergyXml) : Energy()
{
	m_pInternalEnergyXml = pInternalEnergyXml;
}

InternalEnergy* InternalEnergy::instance(Impl impl,
		const InternalEnergyXml* pInternalEnergyXml) {
	switch(impl) {
	case LENNARD_JONES:
		return new LennardJones(pInternalEnergyXml);
	default:
		printf("There is no method for creating '%1$s' input files. ", getEnumString(impl));
		printf("Please create a new class for this method that extends InternalEnergyMethod.\n");
		return NULL;
	}
}

bool InternalEnergy::execute(Structure &structure) {
	if (m_pInternalEnergyXml->m_bLocalOptimization)
		return localOptimization(structure);
	else
		return calculateEnergy(structure);
}

bool InternalEnergy::getEnum(const char* attributeName, const char* stringValue, Impl &result, const rapidxml::xml_node<>* pElem) {
	using namespace strings;
	const char* methods[] = {pLennardJones};
	return XsdTypeUtil::getEnumValue(attributeName, stringValue, result, pElem, methods);
}

const char* InternalEnergy::getEnumString(Impl enumValue) {
	using namespace strings;
	const char* methods[] = {pLennardJones};
	return methods[enumValue];
}
