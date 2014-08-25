
#include "internalEnergyXml.h"

const char* InternalEnergyXml::s_attributeNames[]  = {strings::xMethod, strings::xOpt};
const bool  InternalEnergyXml::s_required[]        = {true            , false        };
const char* InternalEnergyXml::s_defaultValues[]   = {""              , strings::pFalse};

bool InternalEnergyXml::load(const rapidxml::xml_node<>* pInternalEnergyElem)
{
	const char** values;

	XsdAttributeUtil attUtil(s_attributeNames, s_required, s_defaultValues);
	if (!attUtil.process(pInternalEnergyElem))
		return false;
	values = attUtil.getAllAttributes();

	if (!InternalEnergy::getEnum(s_attributeNames[0], values[0], m_method, pInternalEnergyElem))
		return false;

	if (!XsdTypeUtil::getBoolValue(s_attributeNames[1], values[1], m_bLocalOptimization, pInternalEnergyElem))
		return false;
	
	return true;
}

bool InternalEnergyXml::save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pInternalEnergyElem)
{
	using namespace strings;
	pInternalEnergyElem->append_attribute(doc.allocate_attribute(xMethod, InternalEnergy::getEnumString(m_method)));
	if (m_bLocalOptimization)
		pInternalEnergyElem->append_attribute(doc.allocate_attribute(xOpt, pTrue));
	return true;
}
