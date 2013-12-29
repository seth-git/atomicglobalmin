
#include "energyXml.h"

EnergyXml::EnergyXml() {
	m_pEnergy = NULL;
}

EnergyXml::~EnergyXml() {
	clear();
}

void EnergyXml::clear() {
	if (NULL != m_pEnergy) {
		delete m_pEnergy;
		m_pEnergy = NULL;
	}
}

bool EnergyXml::load(TiXmlElement *pEnergyElem, const Strings* messages) {
	clear();
	const char* energyElementNames[] = {messages->m_sxInternal.c_str(), messages->m_sxExternal.c_str()};
	XsdElementUtil energyUtil(messages->m_sxEnergy.c_str(), XSD_CHOICE, energyElementNames);
	TiXmlHandle hChild(0);
	hChild=TiXmlHandle(pEnergyElem);
	if (!energyUtil.process(hChild)) {
		return false;
	}
	m_bExternalEnergy = (bool)energyUtil.getChoiceElementIndex();
	if (m_bExternalEnergy) {
		if (!m_externalEnergyXml.load(energyUtil.getChoiceElement(), messages)) {
			return false;
		}
	} else {
		if (!m_internalEnergyXml.load(energyUtil.getChoiceElement(), messages)) {
			return false;
		}
	}

	return true;
}

bool EnergyXml::save(TiXmlElement *pParentOfEnergyElem, const Strings* messages) {
	TiXmlElement* energy = new TiXmlElement(messages->m_sxEnergy.c_str());
	pParentOfEnergyElem->LinkEndChild(energy);
	if (m_bExternalEnergy) {
		TiXmlElement* external = new TiXmlElement(messages->m_sxExternal.c_str());
		energy->LinkEndChild(external);
		if (!m_externalEnergyXml.save(external, messages))
			return false;
	} else {
		TiXmlElement* internal = new TiXmlElement(messages->m_sxInternal.c_str());
		energy->LinkEndChild(internal);
		if (!m_internalEnergyXml.save(internal, messages))
			return false;
	}

	return true;
}

Energy* EnergyXml::getEnergy() {
	if (NULL == m_pEnergy) {
		if (m_bExternalEnergy)
			m_pEnergy = ExternalEnergy::instance(m_externalEnergyXml.m_method, &m_externalEnergyXml);
		else
			m_pEnergy = InternalEnergy::instance(m_internalEnergyXml.m_method, &m_internalEnergyXml);
	}
	return m_pEnergy;
}
