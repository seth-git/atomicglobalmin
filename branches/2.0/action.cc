
#include "action.h"

//const char* Action::s_attributeNames[] = {"constraints"};
const bool    Action::s_required[]       = {false};
const char*   Action::s_defaultValues[]  = {""};

//const char*      Action::s_elementNames[] = {"setup", "constraints", "energy", "resume", "results"};
const unsigned int Action::s_minOccurs[]    = {1      , 0            , 1       , 0       , 0        };
const unsigned int Action::s_maxOccurs[]    = {1      , XSD_UNLIMITED, 1       , 1       , 1        };

Action::Action(Input* input)
{
	m_pInput = input;
	m_iEnergyCalculations = 0;
	m_tElapsedSeconds = 0;
	m_pEnergy = NULL;
}

Action::~Action()
{
	cleanUp();
}

void Action::cleanUp() {
	for (unsigned int i = 0; i < m_constraints.size(); ++i) {
		delete m_constraints[i];
	}
	m_constraints.clear();
	m_constraintsMap.clear();
	m_pConstraints = NULL;
	m_pEnergy = NULL;
}

bool Action::load(TiXmlElement *pActionElem, const Strings* messages)
{
	cleanUp();
	const Strings* messagesDL = Strings::instance();
	
	const char* attributeNames[] = {messages->m_sxConstraints.c_str()};
	const char** attributeValues;
	XsdAttributeUtil attUtil(pActionElem->Value(), attributeNames, s_required, s_defaultValues);
	if (!attUtil.process(pActionElem)) {
		return false;
	}
	attributeValues = attUtil.getAllAttributes();
	const char* constraintsName = attributeValues[0];
	
	unsigned int i, j;
	TiXmlHandle hRoot(0);
	hRoot=TiXmlHandle(pActionElem);
	const char* elementNames[] = {messages->m_sxSetup.c_str(), messages->m_sxConstraints.c_str(), messages->m_sxEnergy.c_str(), messages->m_sxResume.c_str(), messages->m_sxResults.c_str()};
	XsdElementUtil actionUtil(pActionElem->Value(), XSD_SEQUENCE, elementNames, s_minOccurs, s_maxOccurs);
	if (!actionUtil.process(hRoot)) {
		return false;
	}
	std::vector<TiXmlElement*>* actionElements = actionUtil.getSequenceElements();
	
	for (i = 0; i < actionElements[1].size(); ++i) {
		m_constraints.push_back(new Constraints());
		if (!m_constraints[i]->load(actionElements[1][i], messages, m_constraintsMap))
			return false;
		for (j = 0; j < i; ++j) {
			if (m_constraints[j]->m_sName == m_constraints[i]->m_sName) {
				printf(messagesDL->m_sTwoElementsWithSameName.c_str(), elementNames[1], m_constraints[i]->m_sName.c_str());
				return false;
			}
		}
		m_constraintsMap[m_constraints[i]->m_sName] = m_constraints[i];
	}
	if (constraintsName != NULL) {
		m_pConstraints = m_constraintsMap[constraintsName];
		if (m_pConstraints == NULL) {
			printf(messagesDL->m_sConstraintNameMisMatch.c_str(), pActionElem->Row(), messages->m_sxConstraints.c_str(), constraintsName);
			return false;
		}
	}
	
	if (!loadSetup(actionElements[0][0], messages)) // Do this after loading constraints
		return false;
	
	if (!energyXml.load(actionElements[2][0], messages))
		return false;
	
	if (actionElements[3].size() > 0) {
		for (i = 0; i < actionElements[3].size(); ++i)
			if (!loadResume(actionElements[3][i], messages))
				return false;
	} else {
		if (!loadResume(NULL, messages))
			return false;
	}
	
	// Todo: load the results
	
	return true;
}

bool Action::save(TiXmlElement *pActionElem, const Strings* messages)
{
	if (m_pConstraints != NULL) {
		pActionElem->SetAttribute(messages->m_sxConstraints.c_str(), m_pConstraints->m_sName.c_str());
	}
	
	if (!saveSetup(pActionElem, messages))
		return false;
	
	TiXmlElement* constraints;
	for (unsigned int i = 0; i < m_constraints.size(); ++i) {
		constraints = new TiXmlElement(messages->m_sxConstraints.c_str());
		pActionElem->LinkEndChild(constraints);
		if (!m_constraints[i]->save(constraints, messages))
			return false;
	}
	
	if (!energyXml.save(pActionElem, messages))
		return false;
	
	if (!saveResume(pActionElem, messages))
		return false;
	
	TiXmlElement* results = new TiXmlElement(messages->m_sxResults.c_str());  
	pActionElem->LinkEndChild(results);
	
	return true;
}

bool Action::run() {
	m_tStartTime = time (NULL);
	m_pEnergy = energyXml.getEnergy();
	return NULL != m_pEnergy;
}
