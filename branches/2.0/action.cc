
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
	} else {
		m_iMaxResults = m_structures.size();
		m_fResultsRmsDistance = 0;
	}

	if (0 != m_fResultsRmsDistance)
		for (std::list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); it++)
			RmsDistance::updateAtomToCenterRanks(**it);

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
	return m_tPrevElapsedSeconds + (time (NULL) - m_tStartTime);
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
	return true;
}

unsigned int Action::s_iMaxEnergyCalcFailures = 5;
FLOAT Action::s_fMaxMPIProcessFailures = 0.2;

void Action::updateResults(Structure &structure) {
	Structure* pStructure = new Structure();
	pStructure->copy(structure);
	updateResults(pStructure);
}

void Action::updateResults(Structure* pStructure) {
	for (std::list<Structure*>::iterator it = m_results.begin(); it != m_results.end(); it++)
		if (pStructure->getEnergy() <= (*it)->getEnergy()) {
			m_results.insert(it, pStructure);
			checkResults(pStructure, it);
			return;
		}
	m_results.push_back(pStructure);
	checkResults(pStructure, m_results.end());
}

void Action::checkResults(Structure* pStructure, std::list<Structure*>::iterator resultsIt) {
	if (0 != m_fResultsRmsDistance) {
		std::list<Structure*>::iterator tempIt;
		RmsDistance::updateAtomToCenterRanks(*pStructure);
		while (resultsIt != m_results.end()) {
			if (RmsDistance::calculate(*pStructure,**resultsIt) < m_fResultsRmsDistance) {
				tempIt = resultsIt;
				++resultsIt;
				delete *tempIt;
				m_results.erase(tempIt);
			} else {
				++resultsIt;
			}
		}
	}
	if (m_results.size() > m_iMaxResults) {
		delete m_results.back();
		m_results.pop_back();
	}
}

