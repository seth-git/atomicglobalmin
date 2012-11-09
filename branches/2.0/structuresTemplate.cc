
#include "structuresTemplate.h"

//const char* StructuresTemplate::s_attributeNames[] = {"constraints"};
const bool    StructuresTemplate::s_attRequired[]    = {false};
const char*   StructuresTemplate::s_attDefaults[]    = {NULL};


//const char*      StructuresTemplate::s_elementNames[] = {"structureTemplate", "linear", "planar", "threeD", "bondRotationalSearch", "seed"};
const unsigned int StructuresTemplate::s_minOccurs[]    = {0                  , 0       , 0       , 0       , 0                     , 0     };

//const char*      StructuresTemplate::s_popTempElementNames[] = {"moleculeTemplate", "atomTemplate"};
const unsigned int StructuresTemplate::s_popTempMinOccurs[]    = {0                 , 0             };
const unsigned int StructuresTemplate::s_popTempMaxOccurs[]    = {XSD_UNLIMITED     , XSD_UNLIMITED };

StructuresTemplate::StructuresTemplate()
{
	m_iAtomGroupTemplates = 0;
	m_atomGroupTemplates = NULL;
	
	m_pConstraints = NULL;
	m_iLinear = 0;
	m_pLinearConstraints = NULL;
	m_iPlanar = 0;
	m_pPlanarConstraints = NULL;
	m_i3D = 0;
	m_p3DConstraints = NULL;
	
	m_bondRotationalSearchAngle = NULL;
	
	m_pSeed = NULL;
}

StructuresTemplate::~StructuresTemplate()
{
	cleanUp();
}

void StructuresTemplate::cleanUp()
{
	m_iAtomGroupTemplates = 0;
	if (m_atomGroupTemplates != NULL) {
		delete[] m_atomGroupTemplates;
		m_atomGroupTemplates = NULL;
	}
	
	m_pConstraints = NULL;
	m_iLinear = 0;
	m_pLinearConstraints = NULL;
	m_iPlanar = 0;
	m_pPlanarConstraints = NULL;
	m_i3D = 0;
	m_p3DConstraints = NULL;
	
	if (m_bondRotationalSearchAngle != NULL) {
		delete m_bondRotationalSearchAngle;
		m_bondRotationalSearchAngle = NULL;
	}
	
	if (m_pSeed != NULL) {
		delete m_pSeed;
		m_pSeed = NULL;
	}
}

bool StructuresTemplate::load(TiXmlElement *pStructuresTemplateElem, std::map<std::string,Constraints*> &constraintsMap, const Strings* messages)
{
	cleanUp();
	
	unsigned int i;
	const char** values;
	const char* attributeNames[]  = {messages->m_sxConstraints.c_str()};
	XsdAttributeUtil attUtil(pStructuresTemplateElem->Value(), attributeNames, s_attRequired, s_attDefaults);
	if (!attUtil.process(pStructuresTemplateElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	if (values[0] != NULL) {
		m_pConstraints = constraintsMap[values[0]];
		if (m_pConstraints == NULL) {
			const Strings* messagesDL = Strings::instance();
			printf(messagesDL->m_sConstraintNameMisMatch.c_str(), pStructuresTemplateElem->Row(), messages->m_sxConstraints.c_str(), values[0]);
			return false;
		}
	}
	
	const char* elementNames[] = {messages->m_sxStructureTemplate.c_str(), messages->m_sxLinear.c_str(), messages->m_sxPlanar.c_str(), messages->m_sxThreeD.c_str(), messages->m_sxBondRotationalSearch.c_str(), messages->m_sxSeed.c_str()};
	XsdElementUtil ptUtil(pStructuresTemplateElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle hRoot(0);
	hRoot=TiXmlHandle(pStructuresTemplateElem);
	if (!ptUtil.process(hRoot))
		return false;
	TiXmlElement** ptElements = ptUtil.getAllElements();
	
	if (ptElements[0] != NULL) {
		const char* popTempElementNames[] = {messages->m_sxMoleculeTemplate.c_str(), messages->m_sxAtomTemplate.c_str()};
		XsdElementUtil mtUtil(ptElements[0]->Value(), XSD_SEQUENCE, popTempElementNames, s_popTempMinOccurs, s_popTempMaxOccurs);
		hRoot=TiXmlHandle(ptElements[0]);
		if (!mtUtil.process(hRoot))
			return false;
		std::vector<TiXmlElement*>* templates = mtUtil.getSequenceElements();
		m_iAtomGroupTemplates = templates[0].size() + templates[1].size();
		if (m_iAtomGroupTemplates == 0) {
			const Strings* messagesDL = Strings::instance();
			printf(messagesDL->m_sEmptyStructureTemplate.c_str(), ptElements[0]->Value(), ptElements[0]->Row(), messages->m_sxMoleculeTemplate.c_str(), messages->m_sxAtomTemplate.c_str());
			return false;
		}
		m_atomGroupTemplates = new AtomGroupTemplate[m_iAtomGroupTemplates];
		for (i = 0; i < templates[0].size(); ++i)
			if (!m_atomGroupTemplates[i].loadMolecule(templates[0][i], messages))
				return false;
		for (i = 0; i < templates[1].size(); ++i)
			if (!m_atomGroupTemplates[i+templates[0].size()].loadAtom(templates[1][i], messages))
				return false;
	}
	
	if (ptElements[1] != NULL)
		if (!readInitializationType(ptElements[1], constraintsMap, m_iLinear, &m_pLinearConstraints, messages))
			return false;
	
	if (ptElements[2] != NULL)
		if (!readInitializationType(ptElements[2], constraintsMap, m_iPlanar, &m_pPlanarConstraints, messages))
			return false;
	
	if (ptElements[3] != NULL)
		if (!readInitializationType(ptElements[3], constraintsMap, m_i3D, &m_p3DConstraints, messages))
			return false;
	
	if (ptElements[4] != NULL) {
		m_bondRotationalSearchAngle = new FLOAT;
		if (!XsdTypeUtil::readPosFloatValueElement(ptElements[4], *m_bondRotationalSearchAngle, messages->m_sxDegrees.c_str()))
			return false;
		*m_bondRotationalSearchAngle *= DEG_TO_RAD;
	}
	
	if (ptElements[5] != NULL) {
		m_pSeed = new Seed();
		if (!m_pSeed->load(ptElements[5], messages))
			return false;
	}
	
	return true;
}

//const char* StructuresTemplate::s_initTypeAttNames[]    = {"number", "constraints"};
const bool    StructuresTemplate::s_initTypeAttRequired[] = {true    , false};
const char*   StructuresTemplate::s_initTypeAttDefaults[] = {NULL    , NULL};

bool StructuresTemplate::readInitializationType(TiXmlElement *pElem, std::map<std::string,Constraints*> &constraintsMap, unsigned int &numberOfThisType, Constraints** pConstraints, const Strings* messages) {
	const char** values;
	const char* attributeNames[] = {messages->m_sxNumber.c_str(), messages->m_sxConstraints.c_str()};
	XsdAttributeUtil attUtil(pElem->Value(), attributeNames, s_initTypeAttRequired, s_initTypeAttDefaults);
	if (!attUtil.process(pElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	
	if (!XsdTypeUtil::getPositiveInt(values[0], numberOfThisType, attributeNames[0], pElem)) {
		return false;
	}
	
	if (values[1] != NULL) {
		(*pConstraints) = constraintsMap[values[1]];
		if ((*pConstraints) == NULL) {
			const Strings* messagesDL = Strings::instance();
			printf(messagesDL->m_sConstraintNameMisMatch.c_str(), pElem->Row(), messages->m_sxConstraints.c_str(), values[1]);
			return false;
		}
	}
	return true;
}

bool StructuresTemplate::save(TiXmlElement *pParentElem, const Strings* messages)
{
	unsigned int i;
	TiXmlElement* structuresTemplate = new TiXmlElement(messages->m_sxStructuresTemplate.c_str());
	pParentElem->LinkEndChild(structuresTemplate);
	
	if (m_pConstraints != NULL)
		structuresTemplate->SetAttribute(messages->m_sxConstraints.c_str(), m_pConstraints->m_sName.c_str());
	
	if (m_iAtomGroupTemplates > 0) {
		TiXmlElement* structureTemplate = new TiXmlElement(messages->m_sxStructureTemplate.c_str());
		structuresTemplate->LinkEndChild(structureTemplate);
		
		for (i = 0; i < m_iAtomGroupTemplates; ++i)
			if (!m_atomGroupTemplates[i].save(structureTemplate, messages))
				return false;
	}
	
	if (m_iLinear > 0) {
		TiXmlElement* linear = new TiXmlElement(messages->m_sxLinear.c_str());
		structuresTemplate->LinkEndChild(linear);
		linear->SetAttribute(messages->m_sxNumber.c_str(), m_iLinear);
		if (m_pLinearConstraints != NULL)
			linear->SetAttribute(messages->m_sxConstraints.c_str(), m_pLinearConstraints->m_sName.c_str());
	}
	
	if (m_iPlanar > 0) {
		TiXmlElement* linear = new TiXmlElement(messages->m_sxPlanar.c_str());
		structuresTemplate->LinkEndChild(linear);
		linear->SetAttribute(messages->m_sxNumber.c_str(), m_iPlanar);
		if (m_pPlanarConstraints != NULL)
			linear->SetAttribute(messages->m_sxConstraints.c_str(), m_pPlanarConstraints->m_sName.c_str());
	}
	
	if (m_i3D > 0) {
		TiXmlElement* linear = new TiXmlElement(messages->m_sxThreeD.c_str());
		structuresTemplate->LinkEndChild(linear);
		linear->SetAttribute(messages->m_sxNumber.c_str(), m_i3D);
		if (m_p3DConstraints != NULL)
			linear->SetAttribute(messages->m_sxConstraints.c_str(), m_p3DConstraints->m_sName.c_str());
	}
	
	if (m_bondRotationalSearchAngle != NULL) {
		TiXmlElement* bondRotationalSearch = new TiXmlElement(messages->m_sxBondRotationalSearch.c_str());
		structuresTemplate->LinkEndChild(bondRotationalSearch);
		bondRotationalSearch->SetDoubleAttribute(messages->m_sxDegrees.c_str(), *m_bondRotationalSearchAngle * RAD_TO_DEG);
	}
	
	if (m_pSeed != NULL)
		if (!m_pSeed->save(structuresTemplate, messages))
			return false;
	
	return true;
}
