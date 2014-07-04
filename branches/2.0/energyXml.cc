
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

bool EnergyXml::load(const rapidxml::xml_node<>* pEnergyElem, const Strings* messages) {
	clear();
	const char* energyElementNames[] = {messages->m_sxInternal.c_str(), messages->m_sxExternal.c_str()};
	XsdElementUtil energyUtil(XSD_CHOICE, energyElementNames);
	if (!energyUtil.process(pEnergyElem))
		return false;
	m_bExternalEnergy = (bool)energyUtil.getChoiceElementIndex();
	if (m_bExternalEnergy) {
		if (!m_externalEnergyXml.load(energyUtil.getChoiceElement(), messages))
			return false;
	} else {
		if (!m_internalEnergyXml.load(energyUtil.getChoiceElement(), messages))
			return false;
	}

	return true;
}

bool EnergyXml::save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentOfEnergyElem, const Strings* messages) {
	using namespace rapidxml;
	xml_node<>* energy = doc.allocate_node(node_element, messages->m_sxEnergy.c_str());
	pParentOfEnergyElem->append_node(energy);
	if (m_bExternalEnergy) {
		xml_node<>* external = doc.allocate_node(node_element, messages->m_sxExternal.c_str());
		energy->append_node(external);
		if (!m_externalEnergyXml.save(doc, external, messages))
			return false;
	} else {
		xml_node<>* internal = doc.allocate_node(node_element, messages->m_sxInternal.c_str());
		energy->append_node(internal);
		if (!m_internalEnergyXml.save(doc, internal, messages))
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
