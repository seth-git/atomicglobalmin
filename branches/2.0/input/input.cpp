
#include "input.h"

void Input::cleanUp() {
	if (m_pXMLDocument != NULL) {
		delete m_pXMLDocument;
		m_pXMLDocument= NULL;
	}
	m_iAction = -1;
	m_constraints.clear();
}

const std::string  Input::s_agml = "agml";

const std::string  Input::s_attributeNames[]   = {"version", "xmlns",                                            "xmlns:xsi",                                 "xsi:schemaLocation"};
const bool         Input::s_required[]         = {true     , false  ,                                            false      ,                                 false };
const std::string  Input::s_defaultValues[]    = {""       , "http://sourceforge.net/projects/atomicglobalmin/", "http://www.w3.org/2001/XMLSchema-instance", "http://sourceforge.net/projects/atomicglobalmin/ agml.xsd"};

const std::string  Input::s_elementNames[] = {"action", "constraints", "energy", "results"};
const unsigned int Input::s_minOccurs[]    = {1       , 0            , 1       , 0        };
const unsigned int Input::s_maxOccurs[]    = {1       , XSD_UNLIMITED, 1       , 1        };

const std::string  Input::s_actionElementNames[] = {"simulatedAnnealing", "randomSearch", "particleSwarmOptimization", "geneticAlgorithm", "batch"};
const std::string  Input::s_energyElementNames[] = {"internal", "external"};

bool Input::load(const char* pFilename)
{
	vector<TiXmlElement*>* atgmlElements;
	TiXmlHandle hRoot(0);
	TiXmlHandle hChild(0);
	TiXmlElement* pElem;
	unsigned int i, j;

	cleanUp();
	printf("Opening %s...\n", pFilename);
	m_pXMLDocument = new TiXmlDocument(pFilename);
	if (!m_pXMLDocument->LoadFile() || m_pXMLDocument->Error()) {
		if (strncmp(m_pXMLDocument->ErrorDesc(), "Failed to open file", 20) == 0)
			printf("Error: %s\n", m_pXMLDocument->ErrorDesc());
		else
			printf("Error on line %d and character %d: %s\n", m_pXMLDocument->ErrorRow(), m_pXMLDocument->ErrorCol(), m_pXMLDocument->ErrorDesc());
		return false;
	}
	TiXmlHandle hDoc(m_pXMLDocument);

	pElem=hDoc.FirstChildElement().Element();
	if (!pElem || !pElem->Value() || s_agml != pElem->Value()) {
		printf("The %s element was not found.\n", s_agml.c_str());
		return false;
	}

	const char** rootAttributeValues;
	XsdAttributeUtil rootAttUtil(pElem->Value(), s_attributeNames, 4, s_required, s_defaultValues);
	if (!rootAttUtil.process(pElem)) {
		return false;
	}
	rootAttributeValues = rootAttUtil.getAllAttributes();
	m_sVersion = rootAttributeValues[0];

	if (pElem->NextSiblingElement()) {
		printf("There can be only one root element.\n");
		return false;
	}

	XsdElementUtil atmlUtil(s_agml.c_str(), XSD_SEQUENCE, s_elementNames, 4, s_minOccurs, s_maxOccurs);
	hRoot=TiXmlHandle(pElem);
	if (!atmlUtil.process(hRoot)) {
		return false;
	}
	atgmlElements = atmlUtil.getSequenceElements();

	pElem=atgmlElements[0][0];
	if (!XsdAttributeUtil::hasNoAttributes(pElem, s_elementNames[0].c_str())) {
		return false;
	}
	XsdElementUtil actionUtil(s_elementNames[0].c_str(), XSD_CHOICE, s_actionElementNames, 5, NULL, NULL);
	hChild=TiXmlHandle(pElem);
	if (!actionUtil.process(hChild)) {
		return false;
	}
	pElem = actionUtil.getChoiceElement();
	m_iAction = actionUtil.getChoiceElementIndex();

	for (i = 0; i < atgmlElements[1].size(); ++i) {
		m_constraints.push_back(Constraints());
		m_constraints[i].load(atgmlElements[1][i]);
		for (j = 0; j < i; ++j) {
			if (m_constraints[j].m_sName == m_constraints[i].m_sName) {
				printf("Two elements '%s' have the same name '%s'.\n", s_elementNames[1].c_str(), m_constraints[i].m_sName.c_str());
				return false;
			}
		}
	}

	XsdElementUtil energyUtil(s_elementNames[2].c_str(), XSD_CHOICE, s_energyElementNames, 2, NULL, NULL);
	hChild=TiXmlHandle(atgmlElements[2][0]);
	if (!energyUtil.process(hChild)) {
		return false;
	}
	m_bExternalEnergy = (bool)energyUtil.getChoiceElementIndex();
	if (m_bExternalEnergy) {
		if (!m_externalEnergy.load(energyUtil.getChoiceElement())) {
			return false;
		}
	} else {
		if (!m_internalEnergy.load(energyUtil.getChoiceElement())) {
			return false;
		}
	}
	
	return true;
}

void Input::save(const char* pFilename)
{
	if (m_pXMLDocument == NULL) {
		return;
	}
	printf("Saving %s...\n", pFilename);
	m_pXMLDocument->SaveFile(pFilename);
}
