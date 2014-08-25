
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

bool EnergyXml::load(const rapidxml::xml_node<>* pEnergyElem) {
	using namespace strings;
	clear();
	const char* energyElementNames[] = {xInternal, xExternal};
	XsdElementUtil energyUtil(XSD_CHOICE, energyElementNames);
	if (!energyUtil.process(pEnergyElem))
		return false;
	m_bExternalEnergy = (bool)energyUtil.getChoiceElementIndex();
	if (m_bExternalEnergy) {
		if (!m_externalEnergyXml.load(energyUtil.getChoiceElement()))
			return false;
	} else {
		if (!m_internalEnergyXml.load(energyUtil.getChoiceElement()))
			return false;
	}

	return true;
}

bool EnergyXml::save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pParentOfEnergyElem) {
	using namespace rapidxml;
	using namespace strings;
	xml_node<>* energy = doc.allocate_node(node_element, xEnergy);
	pParentOfEnergyElem->append_node(energy);
	if (m_bExternalEnergy) {
		xml_node<>* external = doc.allocate_node(node_element, xExternal);
		energy->append_node(external);
		if (!m_externalEnergyXml.save(doc, external))
			return false;
	} else {
		xml_node<>* internal = doc.allocate_node(node_element, xInternal);
		energy->append_node(internal);
		if (!m_internalEnergyXml.save(doc, internal))
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
