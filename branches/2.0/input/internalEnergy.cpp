
#include "internalEnergy.h"

bool InternalEnergy::load(TiXmlElement *pElem)
{
	static const std::string attributeNames[] = {"method", "opt"};
	static const bool        required[]       = {true    , false };
	static const std::string defaultValues[]  = {""      , "false"};
	const char** values;
	
	static const std::string methods[] = {"Lennard Jones"};
	static const int methodConstants[] = {LENNARD_JONES};

	XsdAttributeUtil attUtil(pElem->Value(), attributeNames, 2, required, defaultValues);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	m_iMethod = XsdTypeUtil::getEnumValue(attributeNames[0].c_str(), values[0], pElem->Value(), methods, 1, methodConstants);
	if (m_iMethod == ERROR_VALUE) {
		return false;
	}

	int val = XsdTypeUtil::getBoolValue(attributeNames[1].c_str(), values[1], pElem->Value());
	if (val == ERROR_VALUE) {
		return false;
	}
	m_bLocalOptimization = (bool)val;
	
	return true;
}

void InternalEnergy::save()
{
}
