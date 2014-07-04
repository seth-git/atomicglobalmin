
#ifndef __INTERNAL_ENERGY_XML_H__
#define __INTERNAL_ENERGY_XML_H__

#include <stdio.h>
#include <string>
#include "../xsd/xsdAttributeUtil.h"
#include "../xsd/xsdTypeUtil.h"
#include "internalEnergy.h"

class InternalEnergyXml {
public:
	bool m_bLocalOptimization;
	InternalEnergy::Impl m_method;

	InternalEnergyXml() {}

	bool load(const rapidxml::xml_node<>* pInternalEnergyElem, const Strings* messages);
	bool save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pInternalEnergyElem, const Strings* messages);

private:
	static const bool  s_required[];
	static const int   s_methodConstants[];
};

#endif
