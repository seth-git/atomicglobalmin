
#include "internalEnergy.h"

const std::string InternalEnergy::s_attributeNames[]  = {"method", "opt"};
const bool        InternalEnergy::s_required[]        = {true    , false };
const std::string InternalEnergy::s_defaultValues[]   = {""      , "false"};

const std::string InternalEnergy::s_methods[]         = {"Lennard Jones"};
const int         InternalEnergy::s_methodConstants[] = {LENNARD_JONES};

bool InternalEnergy::load(TiXmlElement *pElem)
{
	const char** values;

	XsdAttributeUtil attUtil(pElem->Value(), s_attributeNames, 2, s_required, s_defaultValues);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	if (!XsdTypeUtil::getEnumValue(s_attributeNames[0].c_str(), values[0], m_iMethod, pElem->Value(), s_methods, 1, s_methodConstants)) {
		return false;
	}

	if (!XsdTypeUtil::getBoolValue(s_attributeNames[1].c_str(), values[1], m_bLocalOptimization, pElem->Value())) {
		return false;
	}
	
	return true;
}

void InternalEnergy::save()
{
}
