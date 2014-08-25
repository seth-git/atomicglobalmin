
#include "input.h"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"
		
Input::Input()
{
	m_iAction = -1;
	m_pAction = NULL;
}

Input::~Input()
{
	clear();
}

void Input::clear() {
	m_iAction = -1;
	if (m_pAction != NULL)
		delete m_pAction;
	m_pAction = NULL;
}

bool Input::load(const char* pFilename) {
	using namespace rapidxml;
	using namespace strings;
	m_sFileName = pFilename;
	printf(ReadingFile, pFilename);
	file<> xmlFile(pFilename);
	return loadStr(xmlFile.data());
}

bool Input::loadStr(char* xml) {
	using namespace rapidxml;
	xml_document<> doc;
	try {
		doc.parse<parse_validate_closing_tags>(xml);
	} catch (parse_error e) {
		printf("XML parsing error: %s\n", e.what());
//		printf(e.where());
		return false;
	}
	return load(doc);
}

const char* Input::s_agml = "agml";

const char* Input::s_attributeNames[]   = {"version", "xmlns"                                           , "xmlns:xsi"                                , "xsi:schemaLocation"};
const bool  Input::s_required[]         = {true     , false                                             , false                                      , false };
const char* Input::s_defaultValues[]    = {""       , "http://sourceforge.net/projects/atomicglobalmin/", "http://www.w3.org/2001/XMLSchema-instance", "http://sourceforge.net/projects/atomicglobalmin/ agml.xsd"};

const char* Input::s_actionElementNames[] = {strings::xSimulatedAnnealing, strings::xRandomSearch, strings::xParticleSwarmOptimization, strings::xGeneticAlgorithm, strings::xBatch};

bool Input::load(rapidxml::xml_document<> &doc)
{
	using namespace rapidxml;
	using namespace strings;
	clear();
	const xml_node<>* pElem = doc.first_node();
	if (!pElem || strcmp(s_agml,pElem->name()) != 0) {
		printf(ElementNotFound, s_agml);
		return false;
	}

	if (pElem->next_sibling()) {
		puts(OneRootElement);
		return false;
	}
	
	const char** rootAttributeValues;
	XsdAttributeUtil rootAttUtil(s_attributeNames, s_required, s_defaultValues);
	if (!rootAttUtil.process(pElem))
		return false;
	rootAttributeValues = rootAttUtil.getAllAttributes();
	m_sVersion = rootAttributeValues[0];
	
	XsdElementUtil agmlUtil(XSD_CHOICE, s_actionElementNames);
	if (!agmlUtil.process(pElem))
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
	
	return m_pAction->load(pElem);
}

bool Input::save(rapidxml::xml_document<> &doc)
{
	using namespace rapidxml;
	using namespace strings;

	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	xml_node<>* agml = doc.allocate_node(node_element, s_agml);
	doc.append_node(agml);

	agml->append_attribute(doc.allocate_attribute(s_attributeNames[0], m_sVersion.c_str()));
	agml->append_attribute(doc.allocate_attribute(s_attributeNames[1], s_defaultValues[1]));
	agml->append_attribute(doc.allocate_attribute(s_attributeNames[2], s_defaultValues[2]));
	agml->append_attribute(doc.allocate_attribute(s_attributeNames[3], s_defaultValues[3]));

	if (NULL != m_pAction) {
		const char* actionElementNames[] = {xSimulatedAnnealing, xRandomSearch, xParticleSwarmOptimization, xGeneticAlgorithm, xBatch};
		xml_node<>* action = doc.allocate_node(node_element, actionElementNames[m_iAction]);
		agml->append_node(action);
		if (!m_pAction->save(doc, action))
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
	using namespace rapidxml;
	using namespace strings;
	printf(WritingFile, m_sFileName.c_str());

	xml_document<> doc;
	if (!save(doc))
		return false;

	std::string temporaryFileName;
	temporaryFileName.append(m_sFileName).append(".").append(pAbbrTemporary);

	FILE *f = fopen(temporaryFileName.c_str(), "w");
	if (f == NULL) {
	    printf(ErrorOpeningFile, temporaryFileName.c_str());
	    return false;
	}

	std::string s;
	print(std::back_inserter(s), doc, 0);
	fputs(s.c_str(), f);
	fclose(f);

	std::string mvCommand;
	mvCommand.append("mv ").append(temporaryFileName).append(" ").append(m_sFileName);
	return !system(mvCommand.c_str());
}

bool Input::save(std::string &buffer) {
	rapidxml::xml_document<> doc;
	if (!save(doc))
		return false;
	print(std::back_inserter(buffer), doc, 0);
	return true;
}

bool Input::run(const char* fileName) {
	int iRank;
	int iMpiProcesses;
	MPI_Comm_rank(MPI_COMM_WORLD, &iRank);
	MPI_Comm_size(MPI_COMM_WORLD, &iMpiProcesses);
	if (0 == iRank) {
		if (!load(fileName)) {
			int size = 0;
			MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
			return false;
		}
		if (iMpiProcesses >= 2) {
			#if MPI_DEBUG
				printf("Master process 0 sending the loaded xml to %d other slave mpi processes.\n", (iMpiProcesses-1));
			#endif
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
		if (0 == size)
			return false;
		xml = new char[size];
		MPI_Bcast(xml, size, MPI_CHAR, 0, MPI_COMM_WORLD);
		#if MPI_DEBUG
			int iRank;
			MPI_Comm_rank(MPI_COMM_WORLD, &iRank);
			printf("Slave process %d received loaded xml from process 0.\n", iRank);
		#endif
		bool success = loadStr(xml);
		delete[] xml; // Always delete
		if (!success)
			return false;
		return m_pAction->runSlave();
	}
}
