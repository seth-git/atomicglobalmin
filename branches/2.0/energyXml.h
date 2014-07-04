
#ifndef __ENERGY_XML_H__
#define __ENERGY_XML_H__

#include <stdio.h>
#include <string>
#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "externalEnergy/externalEnergyXml.h"
#include "internalEnergy/internalEnergyXml.h"

class Energy;

class EnergyXml {
public:
	bool m_bExternalEnergy;
	ExternalEnergyXml m_externalEnergyXml;
	InternalEnergyXml m_internalEnergyXml;

	EnergyXml();
	~EnergyXml();
	void clear();

	bool load(const rapidxml::xml_node<>* pEnergyElem, const Strings* messages);
	bool save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentOfEnergyElem, const Strings* messages);
	Energy* getEnergy();

protected:
	Energy* m_pEnergy;
};

#endif
