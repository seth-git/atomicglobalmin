
#include "input.h"

		
Input::Input()
{
	m_iAction = -1;
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;
	m_messages = Strings::instance();
	m_pAction = NULL;
}

Input::~Input()
{
	cleanUp();
}

void Input::cleanUp() {
	m_iAction = -1;
	if (m_pAction != NULL)
		delete m_pAction;
	m_pAction = NULL;
}

const char* Input::s_agml = "agml";

const char* Input::s_attributeNames[]   = {"version", "language", "xmlns"                                           , "xmlns:xsi"                                , "xsi:schemaLocation"};
const bool  Input::s_required[]         = {true     , false     , false                                             , false                                      , false };
const char* Input::s_defaultValues[]    = {""       , "en"      , "http://sourceforge.net/projects/atomicglobalmin/", "http://www.w3.org/2001/XMLSchema-instance", "http://sourceforge.net/projects/atomicglobalmin/ agml.xsd"};

bool Input::load(const char* pFilename)
{
	cleanUp();
	TiXmlDocument xmlDocument(pFilename);
	TiXmlElement* pElem;
	const Strings* messagesDL = Strings::instance();
	
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

	if (pElem->NextSiblingElement()) {
		printf(messagesDL->m_sOneRootElement.c_str());
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
	
	const char* actionElementNames[] = {m_messages->m_sxSimulatedAnnealing.c_str(), m_messages->m_sxRandomSearch.c_str(), m_messages->m_sxParticleSwarmOptimization.c_str(), m_messages->m_sxGeneticAlgorithm.c_str(), m_messages->m_sxBatch.c_str()};
	XsdElementUtil agmlUtil(s_agml, XSD_CHOICE, actionElementNames);
	TiXmlHandle hRoot(0);
	hRoot=TiXmlHandle(pElem);
	if (!agmlUtil.process(hRoot)) {
		return false;
	}
	pElem = agmlUtil.getChoiceElement();
	m_iAction = agmlUtil.getChoiceElementIndex();
	switch (m_iAction) {
	case SIMULATED_ANNEALING:
		m_pAction = new SimulatedAnnealing(this);
		break;
	case RANDOM_SEARCH:
	case PARTICLE_SWARM_OPTIMIZATION:
	case GENETIC_ALGORITHM:
		return false;
	case BATCH:
		m_pAction = new Batch(this);
		break;
	}
	
	return m_pAction->load(pElem, m_messages);
}

bool Input::save(const char* pFilename)
{
	m_sFileName = pFilename;
	return save();
}

bool Input::save()
{
	const Strings* messagesDL = Strings::instance();
	m_messages = messagesDL;
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;
	
	printf(messagesDL->m_sWritingFile.c_str(), m_sFileName.c_str());
	
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );
	
	TiXmlElement* agml = new TiXmlElement(s_agml);
	doc.LinkEndChild(agml);
	
	agml->SetAttribute(s_attributeNames[0], m_sVersion.c_str());
	agml->SetAttribute(s_attributeNames[1], m_sLanguageCode.c_str());
	agml->SetAttribute(s_attributeNames[2], s_defaultValues[2]);
	agml->SetAttribute(s_attributeNames[3], s_defaultValues[3]);
	agml->SetAttribute(s_attributeNames[4], s_defaultValues[4]);
	
	if (m_pAction != NULL) {
		const char* actionElementNames[] = {m_messages->m_sxSimulatedAnnealing.c_str(), m_messages->m_sxRandomSearch.c_str(), m_messages->m_sxParticleSwarmOptimization.c_str(), m_messages->m_sxGeneticAlgorithm.c_str(), m_messages->m_sxBatch.c_str()};
		TiXmlElement* action = new TiXmlElement(actionElementNames[m_iAction]);
		agml->LinkEndChild(action);
		if (!m_pAction->save(action, m_messages))
			return false;
	}
	
	std::string temporaryFileName;
	temporaryFileName.append(m_sFileName).append(".").append(messagesDL->m_spAbbrTemporary);
	
	if (!doc.SaveFile(temporaryFileName.c_str()))
		return false;
	
	std::string mvCommand;
	mvCommand.append("mv ").append(temporaryFileName).append(" ").append(m_sFileName);
	return !system(mvCommand.c_str());
}
