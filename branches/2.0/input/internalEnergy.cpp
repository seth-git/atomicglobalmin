
#include "internalEnergy.h"

bool InternalEnergy::load(TiXmlElement *pElem)
{
	static const std::string attributeNames[] = {"method", "opt"};
	static const bool        required[]       = {true    , false };
	static const std::string defaultValues[]  = {""      , "false"};
	const char** values;

	XsdAttributeUtil attUtil(pElem->Value(), attributeNames, 2, required, defaultValues);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	static const std::string methods[] = {"Lennard Jones"};
	static const int methodConstants[] = {LENNARD_JONES};
	if (!XsdTypeUtil::getEnumValue(attributeNames[0].c_str(), values[0], m_iMethod, pElem->Value(), methods, 1, methodConstants)) {
		return false;
	}

	if (!XsdTypeUtil::getBoolValue(attributeNames[1].c_str(), values[1], m_bLocalOptimization, pElem->Value())) {
		return false;
	}
	
	return true;
}

void InternalEnergy::save()
{
}
