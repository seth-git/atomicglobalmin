
#include "internalEnergyXml.h"

//const char* InternalEnergyXml::s_attributeNames[]  = {"method", "opt"};
const bool    InternalEnergyXml::s_required[]        = {true    , false };
//const char* InternalEnergyXml::s_defaultValues[]   = {""      , "false"};

bool InternalEnergyXml::load(TiXmlElement *pInternalEnergyElem, const Strings* messages)
{
	const char* attributeNames[]  = {messages->m_sxMethod.c_str(), messages->m_sxOpt.c_str()};
	const char* defaultValues[]   = {""                          , messages->m_spFalse.c_str()};
	const char** values;

	XsdAttributeUtil attUtil(pInternalEnergyElem->Value(), attributeNames, s_required, defaultValues);
	if (!attUtil.process(pInternalEnergyElem))
		return false;
	values = attUtil.getAllAttributes();

	if (!InternalEnergy::getEnum(attributeNames[0], values[0], m_method, pInternalEnergyElem, messages))
		return false;

	if (!XsdTypeUtil::getBoolValue(attributeNames[1], values[1], m_bLocalOptimization, pInternalEnergyElem, messages))
		return false;
	
	return true;
}

bool InternalEnergyXml::save(TiXmlElement *pInternalEnergyElem, const Strings* messages)
{
	pInternalEnergyElem->SetAttribute(messages->m_sxMethod.c_str(), InternalEnergy::getEnumString(m_method, messages));
	if (m_bLocalOptimization)
		pInternalEnergyElem->SetAttribute(messages->m_sxOpt.c_str(), messages->m_spTrue.c_str());
	return true;
}
