
#include "populationTemplate.h"


//const char*      PopulationTemplate::s_elementNames[] = {"moleculeSetTemplate", "linear", "planar", "threeD", "bondRotationalSearch", "seed"};
const unsigned int PopulationTemplate::s_minOccurs[]    = {0                    , 0       , 0       , 0       , 0                     , 0     };

PopulationTemplate::PopulationTemplate()
{
}

bool PopulationTemplate::load(TiXmlElement *pPopulationTemplateElem, const Strings* messages)
{
	const char* elementNames[] = {messages->m_sxMoleculeSetTemplate.c_str(), messages->m_sxLinear.c_str(), messages->m_sxPlanar.c_str(), messages->m_sxThreeD.c_str(), messages->m_sxBondRotationalSearch.c_str(), messages->m_sxSeed.c_str()};
	XsdElementUtil ptUtil(pPopulationTemplateElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle hRoot(0);
	hRoot=TiXmlHandle(pPopulationTemplateElem);
	if (!ptUtil.process(hRoot)) {
		return false;
	}
	TiXmlElement** ptElements = ptUtil.getAllElements();
	return true;
}

bool PopulationTemplate::save(TiXmlElement *pParentElem, const Strings* messages)
{
	return true;
}
