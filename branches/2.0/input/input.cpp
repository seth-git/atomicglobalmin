
#include "input.h"

void Input::cleanUp() {
	if (m_pXMLDocument != NULL) {
		delete m_pXMLDocument;
		m_pXMLDocument= NULL;
	}
	m_iAction = -1;
}

bool Input::load(const char* pFilename)
{
	static const std::string elementNames[] = {"action", "energy", "constraints", "results"};
	static const unsigned int   minOccurs[] = {1       , 1       , 0            , 0        };
	XsdElementUtil atmlUtil("agml", XSD_ALL, elementNames, 4, minOccurs, NULL);
	static const std::string actionElementNames[] = {"simulatedAnnealing", "randomSearch", "particleSwarmOptimization", "geneticAlgorithm", "batch"};
	XsdElementUtil actionUtil("action", XSD_CHOICE, actionElementNames, 5, NULL, NULL);
	static const std::string energyElementNames[] = {"internal", "external"};
	XsdElementUtil energyUtil("energy", XSD_CHOICE, energyElementNames, 2, NULL, NULL);
	
	TiXmlElement** atgmlElements;
	TiXmlHandle hRoot(0);
	TiXmlHandle hChild(0);
	TiXmlElement* pElem;

	cleanUp();
	m_pXMLDocument = new TiXmlDocument(pFilename);
	if (!m_pXMLDocument->LoadFile()) {
		printf("Failed to load file \"%s\".\n", pFilename);
		return false;
	}
	printf("Opening %s...\n", pFilename);
	TiXmlHandle hDoc(m_pXMLDocument);
	
	pElem=hDoc.FirstChildElement().Element();
	static const std::string agml = "agml";
	if (!pElem || !pElem->Value() || agml != pElem->Value()) {
		printf("The agml element was not found.\n");
		return false;
	}
	m_pVersion = pElem->Attribute("version");

	if (pElem->NextSiblingElement()) {
		printf("There can be only one root element.\n");
		return false;
	}
	hRoot=TiXmlHandle(pElem);
	if (!atmlUtil.process(hRoot)) {
		return false;
	}
	atgmlElements = atmlUtil.getAllElements();
	
	pElem=atgmlElements[0];
	if (pElem->FirstAttribute()) {
		printf("The action element must not contain any attributes.\n");
		return false;
	}
	hChild=TiXmlHandle(pElem);
	if (!actionUtil.process(hChild)) {
		return false;
	}
	pElem = actionUtil.getChoiceElement();
	
	hChild=TiXmlHandle(atgmlElements[1]);
	if (!energyUtil.process(hChild)) {
		return false;
	}
	switch (energyUtil.getChoiceElementIndex()) {
		case 0:
			m_bExternalEnergy = false;
			if (!m_internalEnergy.load(energyUtil.getChoiceElement())) {
				return false;
			}
			break;
		default: // 1
			m_bExternalEnergy = true;
			break;
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
