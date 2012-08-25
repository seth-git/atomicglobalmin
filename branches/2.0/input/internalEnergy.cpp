
#include "internalEnergy.h"

//const char* InternalEnergy::s_attributeNames[]  = {"method", "opt"};
const bool    InternalEnergy::s_required[]        = {true    , false };
//const char* InternalEnergy::s_defaultValues[]   = {""      , "false"};

//const char* InternalEnergy::s_methods[]         = {"Lennard Jones"};
const int   InternalEnergy::s_methodConstants[] = {LENNARD_JONES};

bool InternalEnergy::load(TiXmlElement *pInternalEnergyElem, const Strings* messages)
{
	const char* attributeNames[]  = {messages->m_sxMethod.c_str(), messages->m_sxOpt.c_str()};
	const char* defaultValues[]   = {""                          , messages->m_spFalse.c_str()};
	const char* methods[]         = {messages->m_spLennardJones.c_str()};
	const char** values;

	XsdAttributeUtil attUtil(pInternalEnergyElem->Value(), attributeNames, s_required, defaultValues);
	if (!attUtil.process(pInternalEnergyElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	if (!XsdTypeUtil::getEnumValue(attributeNames[0], values[0], m_iMethod, pInternalEnergyElem, methods, 1, s_methodConstants)) {
		return false;
	}

	if (!XsdTypeUtil::getBoolValue(attributeNames[1], values[1], m_bLocalOptimization, pInternalEnergyElem, messages)) {
		return false;
	}
	
	return true;
}

void InternalEnergy::save(TiXmlElement *pInternalEnergyElem, const Strings* messages)
{
	const char* methods[] = {messages->m_spLennardJones.c_str()};
	pInternalEnergyElem->SetAttribute(messages->m_sxMethod.c_str(), methods[m_iMethod]);
	if (m_bLocalOptimization)
		pInternalEnergyElem->SetAttribute(messages->m_sxOpt.c_str(), messages->m_spTrue.c_str());
}
