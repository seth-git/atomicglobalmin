
#include "input.h"

void Input::cleanUp() {
	m_iAction = -1;
	m_constraints.clear();
}

const char* Input::s_agml = "agml";

const char* Input::s_attributeNames[]   = {"version", "language", "xmlns"                                           , "xmlns:xsi"                                , "xsi:schemaLocation"};
const bool  Input::s_required[]         = {true     , false     , false                                             , false                                      , false };
const char* Input::s_defaultValues[]    = {""       , "en"      , "http://sourceforge.net/projects/atomicglobalmin/", "http://www.w3.org/2001/XMLSchema-instance", "http://sourceforge.net/projects/atomicglobalmin/ agml.xsd"};

//const char*      Input::s_elementNames[] = {"action", "constraints", "energy", "results"};
const unsigned int Input::s_minOccurs[]    = {1       , 0            , 1       , 0        };
const unsigned int Input::s_maxOccurs[]    = {1       , XSD_UNLIMITED, 1       , 1        };

//const char* Input::s_actionElementNames[] = {"simulatedAnnealing", "randomSearch", "particleSwarmOptimization", "geneticAlgorithm", "batch"};
//const char* Input::s_energyElementNames[] = {"internal", "external"};

bool Input::load(const char* pFilename)
{
	TiXmlDocument xmlDocument(pFilename);
	std::vector<TiXmlElement*>* atgmlElements;
	TiXmlHandle hRoot(0);
	TiXmlHandle hChild(0);
	TiXmlElement* pElem;
	unsigned int i, j;
	const Strings* messagesDL = Strings::instance();
	
	cleanUp();
	printf(messagesDL->m_sReadingFile.c_str(), pFilename);
	if (!xmlDocument.LoadFile() || xmlDocument.Error()) {
		if (strncmp(xmlDocument.ErrorDesc(), "Failed to open file", 20) == 0)
			printf(messagesDL->m_sError.c_str(), xmlDocument.ErrorDesc());
		else
			printf(messagesDL->m_sErrorOnLine.c_str(), xmlDocument.ErrorRow(), xmlDocument.ErrorCol(), xmlDocument.ErrorDesc());
		return false;
	}
	TiXmlHandle hDoc(&xmlDocument);
	m_sFileName = pFilename;

	pElem=hDoc.FirstChildElement().Element();
	if (!pElem || !pElem->Value() || strcmp(s_agml,pElem->Value()) != 0) {
		printf(messagesDL->m_sElementNotFound.c_str(), s_agml);
		return false;
	}

	const char** rootAttributeValues;
	XsdAttributeUtil rootAttUtil(pElem->Value(), s_attributeNames, s_required, s_defaultValues);
	if (!rootAttUtil.process(pElem)) {
		return false;
	}
	rootAttributeValues = rootAttUtil.getAllAttributes();
	m_sVersion = rootAttributeValues[0];
	m_sLanguageCode = rootAttributeValues[1];
	m_messages = Strings::instance(m_sLanguageCode);
	const char* elementNames[] = {m_messages->m_sxAction.c_str(), m_messages->m_sxConstraints.c_str(), m_messages->m_sxEnergy.c_str(), m_messages->m_sxResults.c_str()};
	const char* actionElementNames[] = {m_messages->m_sxSimulatedAnnealing.c_str(), m_messages->m_sxRandomSearch.c_str(), m_messages->m_sxParticleSwarmOptimization.c_str(), m_messages->m_sxGeneticAlgorithm.c_str(), m_messages->m_sxBatch.c_str()};
	const char* energyElementNames[] = {m_messages->m_sxInternal.c_str(), m_messages->m_sxExternal.c_str()};

	if (pElem->NextSiblingElement()) {
		printf(messagesDL->m_sOneRootElement.c_str());
		return false;
	}

	XsdElementUtil atmlUtil(s_agml, XSD_SEQUENCE, elementNames, s_minOccurs, s_maxOccurs);
	hRoot=TiXmlHandle(pElem);
	if (!atmlUtil.process(hRoot)) {
		return false;
	}
	atgmlElements = atmlUtil.getSequenceElements();

	pElem=atgmlElements[0][0];
	if (!XsdAttributeUtil::hasNoAttributes(pElem)) {
		return false;
	}
	XsdElementUtil actionUtil(elementNames[0], XSD_CHOICE, actionElementNames);
	hChild=TiXmlHandle(pElem);
	if (!actionUtil.process(hChild)) {
		return false;
	}
	pElem = actionUtil.getChoiceElement();
	m_iAction = actionUtil.getChoiceElementIndex();

	for (i = 0; i < atgmlElements[1].size(); ++i) {
		m_constraints.push_back(Constraints());
		m_constraints[i].load(atgmlElements[1][i], m_messages);
		for (j = 0; j < i; ++j) {
			if (m_constraints[j].m_sName == m_constraints[i].m_sName) {
				printf(messagesDL->m_sTwoElementsWithSameName.c_str(), elementNames[1], m_constraints[i].m_sName.c_str());
				return false;
			}
		}
	}

	XsdElementUtil energyUtil(elementNames[2], XSD_CHOICE, energyElementNames);
	hChild=TiXmlHandle(atgmlElements[2][0]);
	if (!energyUtil.process(hChild)) {
		return false;
	}
	m_bExternalEnergy = (bool)energyUtil.getChoiceElementIndex();
	if (m_bExternalEnergy) {
		if (!m_externalEnergy.load(energyUtil.getChoiceElement(), m_messages)) {
			return false;
		}
	} else {
		if (!m_internalEnergy.load(energyUtil.getChoiceElement(), m_messages)) {
			return false;
		}
	}
	
	return true;
}

void Input::save(const char* pFilename)
{
	m_sFileName = pFilename;
	save();
}

void Input::save()
{
	const Strings* messagesDL = Strings::instance();
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;
	m_messages = Strings::instance(m_sLanguageCode);
	
	printf(messagesDL->m_sWritingFile.c_str(), m_sFileName.c_str());

	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	
	TiXmlElement* pElem = new TiXmlElement(s_agml);
	doc.LinkEndChild(pElem);
	
	pElem->SetAttribute(s_attributeNames[0], m_sVersion.c_str());
	pElem->SetAttribute(s_attributeNames[1], m_sLanguageCode.c_str());
	pElem->SetAttribute(s_attributeNames[2], s_defaultValues[2]);
	pElem->SetAttribute(s_attributeNames[3], s_defaultValues[3]);
	pElem->SetAttribute(s_attributeNames[4], s_defaultValues[4]);
	
	doc.SaveFile(m_sFileName.c_str());

//      These are commented out to prevent a compiler warning.  They are needed.
//	const char* elementNames[] = {m_messages->m_sxAction.c_str(), m_messages->m_sxConstraints.c_str(), m_messages->m_sxEnergy.c_str(), m_messages->m_sxResults.c_str()};
//	const char* actionElementNames[] = {m_messages->m_sxSimulatedAnnealing.c_str(), m_messages->m_sxRandomSearch.c_str(), m_messages->m_sxParticleSwarmOptimization.c_str(), m_messages->m_sxGeneticAlgorithm.c_str(), m_messages->m_sxBatch.c_str()};
//	const char* energyElementNames[] = {m_messages->m_sxInternal.c_str(), m_messages->m_sxExternal.c_str()};
}
