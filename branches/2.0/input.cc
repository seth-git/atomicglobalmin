
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

bool Input::load(const char* pFilename) {
	m_sFileName = pFilename;
	TiXmlDocument xmlDocument(pFilename);
	const Strings* messagesDL = Strings::instance();
	printf(messagesDL->m_sReadingFile.c_str(), pFilename);
	if (!xmlDocument.LoadFile()) {
		const Strings* messagesDL = Strings::instance();
		if (strncmp(xmlDocument.ErrorDesc(), "Failed to open file", 20) == 0)
			printf(messagesDL->m_sError.c_str(), xmlDocument.ErrorDesc());
		else
			printf(messagesDL->m_sErrorOnLine.c_str(), xmlDocument.ErrorRow(), xmlDocument.ErrorCol(), xmlDocument.ErrorDesc());
		return false;
	}
	return load(xmlDocument);
}

bool Input::loadStr(const char* xml) {
	TiXmlDocument xmlDocument;
	xmlDocument.Parse(xml, 0, TIXML_ENCODING_UTF8);
	return load(xmlDocument);
}

const char* Input::s_agml = "agml";

const char* Input::s_attributeNames[]   = {"version", "language", "xmlns"                                           , "xmlns:xsi"                                , "xsi:schemaLocation"};
const bool  Input::s_required[]         = {true     , false     , false                                             , false                                      , false };
const char* Input::s_defaultValues[]    = {""       , "en"      , "http://sourceforge.net/projects/atomicglobalmin/", "http://www.w3.org/2001/XMLSchema-instance", "http://sourceforge.net/projects/atomicglobalmin/ agml.xsd"};

bool Input::load(TiXmlDocument &xmlDocument)
{
	cleanUp();
	TiXmlElement* pElem;
	
	if (xmlDocument.Error()) {
		const Strings* messagesDL = Strings::instance();
		if (strncmp(xmlDocument.ErrorDesc(), "Failed to open file", 20) == 0)
			printf(messagesDL->m_sError.c_str(), xmlDocument.ErrorDesc());
		else
			printf(messagesDL->m_sErrorOnLine.c_str(), xmlDocument.ErrorRow(), xmlDocument.ErrorCol(), xmlDocument.ErrorDesc());
		return false;
	}
	TiXmlHandle hDoc(&xmlDocument);
	
	pElem=hDoc.FirstChildElement().Element();
	if (!pElem || !pElem->Value() || strcmp(s_agml,pElem->Value()) != 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sElementNotFound.c_str(), s_agml);
		return false;
	}

	if (pElem->NextSiblingElement()) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sOneRootElement.c_str());
		return false;
	}
	
	const char** rootAttributeValues;
	XsdAttributeUtil rootAttUtil(pElem->Value(), s_attributeNames, s_required, s_defaultValues);
	if (!rootAttUtil.process(pElem))
		return false;
	rootAttributeValues = rootAttUtil.getAllAttributes();
	m_sVersion = rootAttributeValues[0];
	m_sLanguageCode = rootAttributeValues[1];
	m_messages = Strings::instance(m_sLanguageCode);
	
	const char* actionElementNames[] = {m_messages->m_sxSimulatedAnnealing.c_str(), m_messages->m_sxRandomSearch.c_str(), m_messages->m_sxParticleSwarmOptimization.c_str(), m_messages->m_sxGeneticAlgorithm.c_str(), m_messages->m_sxBatch.c_str()};
	XsdElementUtil agmlUtil(s_agml, XSD_CHOICE, actionElementNames);
	TiXmlHandle hRoot(0);
	hRoot=TiXmlHandle(pElem);
	if (!agmlUtil.process(hRoot))
		return false;
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

bool Input::save(TiXmlDocument &doc)
{
	const Strings* messagesDL = Strings::instance();
	m_messages = messagesDL;
	m_sLanguageCode = Strings::s_sDefaultLanguageCode;

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild(decl);

	TiXmlElement* agml = new TiXmlElement(s_agml);
	doc.LinkEndChild(agml);

	agml->SetAttribute(s_attributeNames[0], m_sVersion.c_str());
	agml->SetAttribute(s_attributeNames[1], m_sLanguageCode.c_str());
	agml->SetAttribute(s_attributeNames[2], s_defaultValues[2]);
	agml->SetAttribute(s_attributeNames[3], s_defaultValues[3]);
	agml->SetAttribute(s_attributeNames[4], s_defaultValues[4]);

	if (NULL != m_pAction) {
		const char* actionElementNames[] = {m_messages->m_sxSimulatedAnnealing.c_str(), m_messages->m_sxRandomSearch.c_str(), m_messages->m_sxParticleSwarmOptimization.c_str(), m_messages->m_sxGeneticAlgorithm.c_str(), m_messages->m_sxBatch.c_str()};
		TiXmlElement* action = new TiXmlElement(actionElementNames[m_iAction]);
		agml->LinkEndChild(action);
		if (!m_pAction->save(action, m_messages))
			return false;
	}
	return true;
}

bool Input::save(const char* pFilename)
{
	m_sFileName = pFilename;
	return save();
}

bool Input::save()
{
	const Strings* messagesDL = Strings::instance();
	printf(messagesDL->m_sWritingFile.c_str(), m_sFileName.c_str());
	
	TiXmlDocument doc;
	if (!save(doc))
		return false;
	
	std::string temporaryFileName;
	temporaryFileName.append(m_sFileName).append(".").append(messagesDL->m_spAbbrTemporary);
	
	if (!doc.SaveFile(temporaryFileName.c_str()))
		return false;
	
	std::string mvCommand;
	mvCommand.append("mv ").append(temporaryFileName).append(" ").append(m_sFileName);
	return !system(mvCommand.c_str());
}

bool Input::save(std::string &buffer) {
	TiXmlDocument doc;
	if (!save(doc))
		return false;
	TiXmlPrinter printer;
	doc.Accept(&printer);
	buffer = printer.CStr();
	return true;
}

bool Input::run(const char* fileName) {
	int iRank;
	int iMpiProcesses;
	MPI_Comm_rank(MPI_COMM_WORLD, &iRank);
	MPI_Comm_size(MPI_COMM_WORLD, &iMpiProcesses);
	if (0 == iRank) {
		if (!load(fileName))
			return false;
		if (iMpiProcesses >= 2) {
			if (PRINT_MPI_MESSAGES)
				printf("Master process 0 sending the loaded xml to %d other slave mpi processes.\n", (iMpiProcesses-1));
			std::string xml;
			save(xml);
			int temp = xml.length()+1;
			MPI_Bcast(&temp, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast((void*)xml.c_str(), xml.length()+1, MPI_CHAR, 0, MPI_COMM_WORLD);
		}
		return m_pAction->runMaster();
	} else {
		char* xml;
		int size;
		MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
		xml = new char[size];
		MPI_Bcast(xml, size, MPI_CHAR, 0, MPI_COMM_WORLD);
		if (PRINT_MPI_MESSAGES) {
			int iRank;
			MPI_Comm_rank(MPI_COMM_WORLD, &iRank);
			printf("Slave process %d received loaded xml from process 0.\n", iRank);
		}
		bool success = loadStr(xml);
		delete[] xml; // Always delete
		if (!success)
			return false;
		return m_pAction->runSlave();
	}
}
