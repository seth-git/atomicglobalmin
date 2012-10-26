
#include "populationTemplate.h"


//const char*      PopulationTemplate::s_elementNames[] = {"moleculeSetTemplate", "linear", "planar", "threeD", "bondRotationalSearch", "seed"};
const unsigned int PopulationTemplate::s_minOccurs[]    = {0                    , 0       , 0       , 0       , 0                     , 0     };

//const char*      PopulationTemplate::s_popTempElementNames[] = {"moleculeTemplate"};
const unsigned int PopulationTemplate::s_popTempMinOccurs[]    = {1                 };
const unsigned int PopulationTemplate::s_popTempMaxOccurs[]    = {XSD_UNLIMITED     };

PopulationTemplate::PopulationTemplate()
{
	m_iMoleculeTemplates = 0;
	m_moleculeTemplates = NULL;
}

PopulationTemplate::~PopulationTemplate()
{
	cleanUp();
}

void PopulationTemplate::cleanUp()
{
	if (m_moleculeTemplates != NULL) {
		delete[] m_moleculeTemplates;
		m_moleculeTemplates = NULL;
		m_iMoleculeTemplates = 0;
	}
}

bool PopulationTemplate::load(TiXmlElement *pPopulationTemplateElem, const Strings* messages)
{
	cleanUp();
	const char* elementNames[] = {messages->m_sxMoleculeSetTemplate.c_str(), messages->m_sxLinear.c_str(), messages->m_sxPlanar.c_str(), messages->m_sxThreeD.c_str(), messages->m_sxBondRotationalSearch.c_str(), messages->m_sxSeed.c_str()};
	XsdElementUtil ptUtil(pPopulationTemplateElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle hRoot(0);
	hRoot=TiXmlHandle(pPopulationTemplateElem);
	if (!ptUtil.process(hRoot))
		return false;
	TiXmlElement** ptElements = ptUtil.getAllElements();
	
	if (ptElements[0] != NULL) {
		const char* popTempElementNames[] = {messages->m_sxMoleculeTemplate.c_str()};
		XsdElementUtil mtUtil(ptElements[0]->Value(), XSD_SEQUENCE, popTempElementNames, s_popTempMinOccurs, s_popTempMaxOccurs);
		hRoot=TiXmlHandle(ptElements[0]);
		if (!mtUtil.process(hRoot))
			return false;
		std::vector<TiXmlElement*>* molTemplates = mtUtil.getSequenceElements();
		m_iMoleculeTemplates = molTemplates[0].size();
		m_moleculeTemplates = new MoleculeTemplate[m_iMoleculeTemplates];
		for (unsigned int i = 0; i < m_iMoleculeTemplates; ++i)
			if (!m_moleculeTemplates[i].load(molTemplates[0][i], messages))
				return false;
	}
	
	return true;
}

bool PopulationTemplate::save(TiXmlElement *pParentElem, const Strings* messages)
{
	unsigned int i;
	TiXmlElement* populationTemplate = new TiXmlElement(messages->m_sxPopulationTemplate.c_str());
	pParentElem->LinkEndChild(populationTemplate);
	
	if (m_iMoleculeTemplates > 0) {
		TiXmlElement* moleculeSetTemplate = new TiXmlElement(messages->m_sxMoleculeSetTemplate.c_str());
		populationTemplate->LinkEndChild(moleculeSetTemplate);
		
		for (i = 0; i < m_iMoleculeTemplates; ++i) {
			if (!m_moleculeTemplates[i].save(moleculeSetTemplate, messages))
				return false;
		}
	}

	
	return true;
}
