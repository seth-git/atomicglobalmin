
#include "action.h"

//const char* Action::s_attributeNames[] = {"constraints"};
const bool    Action::s_required[]       = {false};
const char*   Action::s_defaultValues[]  = {""};

//const char*      Action::s_elementNames[] = {"setup", "constraints", "energy", "resume"     , "results"};
const unsigned int Action::s_minOccurs[]    = {1      , 0            , 1       , 0            , 0        };
const unsigned int Action::s_maxOccurs[]    = {1      , XSD_UNLIMITED, 1       , XSD_UNLIMITED, 1        };

//const char*      Action::s_resultsElementNames[] = {"structure"};
const unsigned int Action::s_resultsMinOccurs[]    = {0};
const unsigned int Action::s_resultsMaxOccurs[]    = {XSD_UNLIMITED};

//const char* Action::s_resultsAttributeNames[] = {"maxSize", "rmsDistance"};
const bool    Action::s_resultsRequired[]       = {false    , false};
const char*   Action::s_resultsDefaultValues[]  = {"0"      , "0"};

Action::Action(Input* input)
{
	m_pInput = input;
	m_iEnergyCalculations = 0;
	m_tPrevElapsedSeconds = 0;
	m_tStartTime = time (NULL);
	m_pEnergy = NULL;
	m_fResultsRmsDistance = 0;
	m_pConstraints = NULL;
	m_iMpiProcesses = 0;
	m_iMaxResults = 0;
	m_iSaveFrequency = 0;
	m_iMpiRank = 0;
	m_bRunComplete = false;
}

Action::~Action()
{
	clear();
}

void Action::clear() {
	for (unsigned int i = 0; i < m_constraints.size(); ++i) {
		delete m_constraints[i];
	}
	m_constraints.clear();
	m_constraintsMap.clear();
	m_pConstraints = NULL;
	m_pEnergy = NULL;
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); it++)
		delete *it;
	m_structures.clear();
	for (std::list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); it++)
		delete *it;
	m_results.clear();
	m_fResultsRmsDistance = 0;
}

bool Action::load(const rapidxml::xml_node<>* pActionElem)
{
	using namespace rapidxml;
	using namespace strings;
	clear();
	
	const char* attributeNames[] = {xConstraints};
	const char** attributeValues;
	XsdAttributeUtil attUtil(attributeNames, s_required, s_defaultValues);
	if (!attUtil.process(pActionElem)) {
		return false;
	}
	attributeValues = attUtil.getAllAttributes();
	const char* constraintsName = attributeValues[0];
	
	unsigned int i, j;
	const char* elementNames[] = {xSetup, xConstraints, xEnergy, xResume, xResults};
	XsdElementUtil actionUtil(XSD_SEQUENCE, elementNames, s_minOccurs, s_maxOccurs);
	if (!actionUtil.process(pActionElem))
		return false;
	std::vector<const xml_node<>*>* actionElements = actionUtil.getSequenceElements();
	
	Constraints* pConstraints;
	for (std::vector<const xml_node<>*>::iterator it = actionElements[1].begin(); it != actionElements[1].end(); ++it) {
		pConstraints = new Constraints();
		if (!pConstraints->load(*it, m_constraintsMap)) {
			delete pConstraints;
			return false;
		}
		for (j = 0; j < m_constraints.size(); ++j) {
			if (m_constraints[j]->m_sName == pConstraints->m_sName) {
				printf(TwoElementsWithSameName, elementNames[1], pConstraints->m_sName.c_str());
				delete pConstraints;
				return false;
			}
		}
		m_constraints.push_back(pConstraints);
		m_constraintsMap[pConstraints->m_sName] = pConstraints;
	}
	if (constraintsName != NULL) {
		m_pConstraints = m_constraintsMap[constraintsName];
		if (m_pConstraints == NULL) {
			printf(ConstraintNotDefined, pActionElem->name(), xConstraints, constraintsName);
			return false;
		}
	}

	if (!loadSetup(actionElements[0][0])) // Do this after loading constraints
		return false;
	
	if (!energyXml.load(actionElements[2][0]))
		return false;

	if (actionElements[3].size() > 0) {
		for (i = 0; i < actionElements[3].size(); ++i)
			if (!loadResume(actionElements[3][i]))
				return false;
	} else {
		if (!loadResume(NULL))
			return false;
		if (energyXml.m_bExternalEnergy
				&& energyXml.m_externalEnergyXml.m_sResultsDir.length() > 0
				&& energyXml.m_externalEnergyXml.m_iMaxResultFiles < 0)
			energyXml.m_externalEnergyXml.m_iMaxResultFiles = m_structures.size();
	}
	
	if (actionElements[4].size() > 0) {
		const xml_node<>* resultsElem = actionElements[4][0];
		const char* resultsElementNames[] = {xStructure};
		XsdElementUtil resultsUtil(XSD_SEQUENCE, resultsElementNames, s_resultsMinOccurs, s_resultsMaxOccurs);
		if (!resultsUtil.process(resultsElem))
			return false;
		std::vector<const xml_node<>*>* structureElements = &(resultsUtil.getSequenceElements()[0]);
		Structure* pStructure;
		for (std::vector<const xml_node<>*>::iterator it = structureElements->begin(); it != structureElements->end(); ++it) {
			pStructure = new Structure();
			m_results.push_back(pStructure);
			if (!pStructure->load(*it))
				return false;
		}

		const char* resultsAttributeNames[] = {xMaxSize, xRmsDistance};
		XsdAttributeUtil resultsAttUtil(resultsAttributeNames, s_resultsRequired, s_resultsDefaultValues);
		if (!resultsAttUtil.process(resultsElem))
			return false;
		const char** resultsAttributeValues = resultsAttUtil.getAllAttributes();
		if (!XsdTypeUtil::getNonNegativeInt(resultsAttributeValues[0], m_iMaxResults, resultsAttributeNames[0], resultsElem))
			return false;
		if (0 == m_iMaxResults)
			m_iMaxResults = m_structures.size();
		if (!XsdTypeUtil::getNonNegativeFloat(resultsAttributeValues[1], m_fResultsRmsDistance, resultsAttributeNames[1], resultsElem))
			return false;
		if (0 != m_fResultsRmsDistance)
			for (std::list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); ++it) {
				(*it)->updateAtomDistanceMatrix();
				RmsDistance::updateAtomToCenterRanks(**it);
			}
	} else {
		m_iMaxResults = m_structures.size();
		m_fResultsRmsDistance = 0;
	}

	return true;
}

bool Action::save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pActionElem)
{
	using namespace rapidxml;
	using namespace strings;
	xml_attribute<>* attr;
	if (m_pConstraints != NULL) {
		attr = doc.allocate_attribute(xConstraints, m_pConstraints->m_sName.c_str());
		pActionElem->append_attribute(attr);
	}
	
	if (!saveSetup(doc, pActionElem))
		return false;
	
	xml_node<>* constraints;
	for (unsigned int i = 0; i < m_constraints.size(); ++i) {
		constraints = doc.allocate_node(node_element, xConstraints);
		pActionElem->append_node(constraints);
		if (!m_constraints[i]->save(doc, constraints))
			return false;
	}
	
	if (!energyXml.save(doc, pActionElem))
		return false;
	
	if (!saveResume(doc, pActionElem))
		return false;
	
	xml_node<>* results = doc.allocate_node(node_element, xResults);
	pActionElem->append_node(results);
	
	if (m_structures.size() != m_iMaxResults)
		XsdTypeUtil::setAttribute(doc, results, xMaxSize, m_iMaxResults);
	if (0 != m_fResultsRmsDistance)
		XsdTypeUtil::setAttribute(doc, results, xRmsDistance, m_fResultsRmsDistance);
	for (std::list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); it++)
		if (!(*it)->save(doc, results))
			return false;

	return true;
}

time_t Action::getTotalElapsedSeconds() {
	return m_tPrevElapsedSeconds + (time(NULL) - m_tStartTime);
}

bool Action::calculateTimeToQuit() {
	if (energyXml.m_bExternalEnergy) {
		time_t wallTime = energyXml.m_externalEnergyXml.m_tWallTime;
		ExternalEnergy* pExternalEnergy = ((ExternalEnergy*)m_pEnergy);
		if (wallTime > 0) {
			time_t elapsed = time(NULL) - m_tStartTime;
			if (elapsed + pExternalEnergy->m_tLongestExecutionTime + 60 > wallTime) {
				#if MPI_DEBUG
					printf("Process %u detected time to quit.\n", m_iMpiRank);
				#endif
				return true;
			}
		}
		if (pExternalEnergy->stopFileExists()) {
			#if MPI_DEBUG
				printf("Process %u detected stop file.\n", m_iMpiRank);
			#endif
			return true;
		}
	}
	return false;
}

bool Action::run() {
	int temp;
	MPI_Comm_rank(MPI_COMM_WORLD, &temp);
	m_iMpiRank = (unsigned int)temp;
	MPI_Comm_size(MPI_COMM_WORLD, &temp);
	m_iMpiProcesses = (unsigned int)temp;
	m_tStartTime = time (NULL);
	m_pEnergy = energyXml.getEnergy();
	if (NULL == m_pEnergy)
		return false;
	if (!m_pEnergy->setup())
		return false;

	return true;
}

bool Action::cleanupRun() {
	if (NULL != m_pEnergy)
		if (!m_pEnergy->cleanup())
			return false;
	return true;
}

unsigned int Action::s_iMaxEnergyCalcFailures = 3;
FLOAT Action::s_fMaxMPIProcessFailures = 0.2;

void Action::updateResults(Structure &structure) {
	Structure* pStructure = new Structure();
	pStructure->copy(structure);
	updateResults(pStructure);
}

void Action::updateResults(Structure* pStructure) {
	unsigned int index = 0;
	if (0 != m_fResultsRmsDistance) {
		pStructure->updateAtomDistanceMatrix();
		RmsDistance::updateAtomToCenterRanks(*pStructure);
	}
	for (std::list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); it++) {
		if (pStructure->getEnergy() < (*it)->getEnergy()) {
			#if ACTION_DEBUG
				printf("In Action::updateResults, inserting structure %d at index %u.\n", pStructure->getId(), index);
			#endif
			m_results.insert(it, pStructure);
			checkResults(pStructure, it, index);
			return;
		} else if (0 != m_fResultsRmsDistance && RmsDistance::calculate(*pStructure,**it) < m_fResultsRmsDistance) {
			#if ACTION_DEBUG
				printf("In Action::updateResults, decided not to insert structure %d because it's within the RMS distance of an equal or lower energy structure %d.\n", pStructure->getId(), (*it)->getId());
			#endif
			deleteStructureFiles(*pStructure);
			delete pStructure;
			return;
		}
		++index;
	}
	#if ACTION_DEBUG
		printf("In Action::updateResults, inserting structure %d at index %u.\n", pStructure->getId(), (unsigned int)(m_results.size()));
	#endif
	m_results.push_back(pStructure);
	checkResults(pStructure, m_results.end(), m_results.size()-1);
}

void Action::checkResults(Structure* pStructure, std::list<Structure*>::iterator resultsIt, unsigned int insertionIndex) {
	FLOAT dist;
	std::list<Structure*>::iterator insertionPointIt = resultsIt;
	--insertionPointIt;

	if (0 != m_fResultsRmsDistance) {
		std::list<Structure*>::iterator tempIt;
		while (resultsIt != m_results.end()) {
			dist = RmsDistance::calculate(*pStructure,**resultsIt);
			if (dist < m_fResultsRmsDistance) {
				#if ACTION_DEBUG
					printf("In Action::checkResults, removing structure %d because it's distance to structure %d(%lf) is within the RMS distance(%lf).\n", (*resultsIt)->getId(), pStructure->getId(), dist, m_fResultsRmsDistance);
				#endif
				tempIt = resultsIt;
				++resultsIt;
				deleteStructureFiles(**tempIt);
				delete *tempIt;
				m_results.erase(tempIt);
			} else {
				++resultsIt;
			}
		}
	}
	if (m_results.size() > m_iMaxResults) {
		Structure* structure = m_results.back();
		#if ACTION_DEBUG
			printf("In Action::checkResults, removing structure %d because we have too many structures.\n", structure->getId());
		#endif
		deleteStructureFiles(*structure);
		delete structure;
		m_results.pop_back();
		if (structure == pStructure)
			insertionPointIt = m_results.end();
	}
	// Check our maximum files limit
	if (insertionPointIt != m_results.end() && energyXml.m_bExternalEnergy && energyXml.m_externalEnergyXml.m_sResultsDir.length() > 0) {
		unsigned int maxFiles = (unsigned int)energyXml.m_externalEnergyXml.m_iMaxResultFiles;
		if (insertionIndex >= maxFiles) {
			deleteStructureFiles(**insertionPointIt);
		} else {
			std::list<Structure*>::iterator it = insertionPointIt;
			unsigned int index = insertionIndex;
			while (it != m_results.end()) {
				if (index == maxFiles) {
					deleteStructureFiles(**it);
					break;
				}
				++it;
				++index;
			}
		}
	}
}

void Action::deleteStructureFiles(Structure &structure) {
	if (structure.m_sFilePrefix.length() == 0)
		return;
	std::string fullPath = energyXml.m_externalEnergyXml.m_sResultsDir + "/" + structure.m_sFilePrefix;
	FileUtils::deletePrefixFiles(fullPath);
	structure.m_sFilePrefix = "";
}

void Action::renameResultsFiles() {
	using namespace std;
	if (!energyXml.m_bExternalEnergy ||
			energyXml.m_externalEnergyXml.m_sResultsDir.length() == 0 ||
			energyXml.m_externalEnergyXml.m_iMaxResultFiles == 0)
		return;

	#if ACTION_DEBUG
		printf("In Action::renameResultsFiles, before prefixes: ");
		for (list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); ++it) {
			if (it != m_results.begin())
				printf(", ");
			printf("%s(%d)", (*it)->m_sFilePrefix.c_str(), (*it)->getId());
		}
		printf("\n");
	#endif

	unsigned int maxFiles = (unsigned int)energyXml.m_externalEnergyXml.m_iMaxResultFiles;
	unsigned int filesEnd = m_results.size();
	if (maxFiles < filesEnd)
		filesEnd = maxFiles;
	string* dir = &(energyXml.m_externalEnergyXml.m_sResultsDir);
	string start = energyXml.m_externalEnergyXml.m_sResultsFilePrefix;
	string scanfStr = energyXml.m_externalEnergyXml.m_sResultsFilePrefix + "%u";

	string newPrefix;
	string* prefix;
	bool isResultsFilePrefix;
	list<Structure*>::iterator it2;
	int expectedNumber2;
	string* prefix2;
	int changeBegin = -1;
	int changeEnd = -1;
	unsigned int expectedNumber = 0;
	unsigned int number = 0;
	list<Structure*>::iterator itStart;
	for (list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); ++it) {
		if (++expectedNumber > filesEnd)
			break; // No more files
		prefix = &((*it)->m_sFilePrefix);
		isResultsFilePrefix = (1 == sscanf(prefix->c_str(), scanfStr.c_str(), &number));

		if (prefix->length() > 0 && (!isResultsFilePrefix || number != expectedNumber)) {
			itStart = it;
			changeBegin = expectedNumber;
			if (changeEnd == -1)
				changeEnd = expectedNumber;
			if (expectedNumber < filesEnd && (!isResultsFilePrefix || number < expectedNumber))
				continue;
		}
		if (changeEnd != -1) {
			it2 = itStart;
			for (expectedNumber2 = changeBegin; expectedNumber2 >= changeEnd; --expectedNumber2) {
				prefix2 = &((*it2)->m_sFilePrefix);
				newPrefix = start + ExternalEnergy::ToString(expectedNumber2);
				#if ACTION_DEBUG
					printf("In Action::renameResultsFiles changing %s to %s\n", prefix2->c_str(), newPrefix.c_str());
				#endif
				FileUtils::changeFilePrefix(*prefix2, newPrefix, *dir);
				*prefix2 = newPrefix;
				--it2;
			}
			changeEnd = -1;
		}
	}
	#if ACTION_DEBUG
		printf("In Action::renameResultsFiles, after prefixes: ");
		for (list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); ++it) {
			if (it != m_results.begin())
				printf(", ");
			printf("%s(%d)", (*it)->m_sFilePrefix.c_str(), (*it)->getId());
		}
		printf("\n");
	#endif
}
