
#include "simulatedAnnealing.h"
#include "../input.h"
#include <algorithm>

SimulatedAnnealing::SimulatedAnnealing(Input* input) : Action(input)
{
	m_pfStartingTemperature = NULL;
	m_pfPercentAcceptedPerturbations = NULL;
	m_pfMaxStoppingTemperature = NULL;
	m_pfMaxStoppingAcceptedPerturbations = NULL;
	m_piMinStoppingIterations = NULL;
	m_iDecreaseTemperatureAfterIt = 0;
	m_iAcceptedPertHistIt = 0;
	m_fBoltzmannConstant = 0;
	m_fQuenchingFactor = 1;
}

SimulatedAnnealing::~SimulatedAnnealing()
{
	clear();
}

void SimulatedAnnealing::clear()
{
	Action::clear();
	if (m_pfStartingTemperature != NULL) {
		delete m_pfStartingTemperature;
		m_pfStartingTemperature = NULL;
	}
	if (m_pfPercentAcceptedPerturbations != NULL) {
		delete m_pfPercentAcceptedPerturbations;
		m_pfPercentAcceptedPerturbations = NULL;
	}
	if (m_pfMaxStoppingTemperature != NULL) {
		delete m_pfMaxStoppingTemperature;
		m_pfMaxStoppingTemperature = NULL;
	}
	if (m_pfMaxStoppingAcceptedPerturbations != NULL) {
		delete m_pfMaxStoppingAcceptedPerturbations;
		m_pfMaxStoppingAcceptedPerturbations = NULL;
	}
	if (m_piMinStoppingIterations != NULL) {
		delete m_piMinStoppingIterations;
		m_piMinStoppingIterations = NULL;
	}
	for (std::list<SimulatedAnnealingRun*>::iterator it = m_runs.begin(); it != m_runs.end(); it++)
		delete *it;
	m_runs.clear();
}

//const char*      SimulatedAnnealing::s_elementNames[] = {"structuresTemplate", "temperature", "annealingSchedule", "perturbations", "stop"};
const unsigned int SimulatedAnnealing::s_minOccurs[]    = {1                   , 0            , 0                  , 0              , 0     };

const bool SimulatedAnnealing::s_tempAttReq[] = {true,true};
const char* SimulatedAnnealing::s_tempAttDef[] = {"1", "3.16689e-6"};
const FLOAT SimulatedAnnealing::s_fDefaultBoltzmannConstant = 3.16689e-6;

const bool SimulatedAnnealing::s_stopAttReq[] = {false,false,false};
const char* SimulatedAnnealing::s_stopAttDef[] = {"", "", ""};

const bool SimulatedAnnealing::s_setupAttReq[] = {true,true};
const char* SimulatedAnnealing::s_setupAttDef[] = {"1", "200"};

bool SimulatedAnnealing::loadSetup(const rapidxml::xml_node<>* pSetupElem)
{
	using namespace rapidxml;
	using namespace strings;
	clear();
	const char* elementNames[] = {xStructuresTemplate, xTemperature, xAnnealingSchedule, xPerturbations, xStop};
	XsdElementUtil setupUtil(XSD_ALL, elementNames, s_minOccurs);
	if (!setupUtil.process(pSetupElem))
		return false;
	const xml_node<>** setupElements = setupUtil.getAllElements();
	
	if (!m_structuresTemplate.load(setupElements[0], m_constraintsMap))
		return false;
	
	if (setupElements[1] != NULL) {
		const char* setupElemNames[] = {xKelvin, xAcceptedPerturbations};
		XsdElementUtil tempElemUtil(XSD_CHOICE, setupElemNames);
		if (!tempElemUtil.process(setupElements[1]))
			return false;
		unsigned int tempChoiceIndex = tempElemUtil.getChoiceElementIndex();
		const xml_node<>* tempChoiceElem = tempElemUtil.getChoiceElement();

		if (tempChoiceIndex == 0) {
			m_pfStartingTemperature = new FLOAT;
			if (!XsdTypeUtil::read1PosFloatAtt(tempChoiceElem, *m_pfStartingTemperature, xValue, true, NULL))
				return false;
		} else {
			m_pfPercentAcceptedPerturbations = new FLOAT;
			if (!XsdTypeUtil::read1PosFloatAtt(tempChoiceElem, *m_pfPercentAcceptedPerturbations, xPercent, true, NULL))
				return false;
			if (!XsdTypeUtil::inRange(*m_pfPercentAcceptedPerturbations, 0, 100, tempChoiceElem, xPercent))
				return false;
			*m_pfPercentAcceptedPerturbations *= 0.01;
		}

		const char* tempAttNames[] = {xDecreaseAfterIteration, xBoltzmannConstant};
		XsdAttributeUtil tempAttUtil(tempAttNames, s_tempAttReq, s_tempAttDef);
		if (!tempAttUtil.process(setupElements[1]))
			return false;
		const char** tempAttValues = tempAttUtil.getAllAttributes();
		if (!XsdTypeUtil::getPositiveInt(tempAttValues[0], m_iDecreaseTemperatureAfterIt, tempAttNames[0], setupElements[1]))
			return false;
		if (!XsdTypeUtil::getPositiveFloat(tempAttValues[1], m_fBoltzmannConstant, tempAttNames[1], setupElements[1]))
			return false;
		if (NULL != m_pfPercentAcceptedPerturbations && m_iDecreaseTemperatureAfterIt == 1)
			m_iDecreaseTemperatureAfterIt = 200;
	} else {
		m_pfPercentAcceptedPerturbations = new FLOAT;
		*m_pfPercentAcceptedPerturbations = 0.9;
		m_iDecreaseTemperatureAfterIt = 200;
		m_fBoltzmannConstant = s_fDefaultBoltzmannConstant;
	}

	if (setupElements[2] != NULL) {
		if (!XsdTypeUtil::read1PosFloatAtt(setupElements[2], m_fQuenchingFactor, xQuenchingFactor, true, NULL))
			return false;
	} else {
		m_fQuenchingFactor = 0.9995;
	}

	if (setupElements[3] != NULL) {
		if (!m_perturbations.loadSetup(setupElements[3]))
			return false;
	} else {
		bool moleculesPresent = false;
		for (unsigned int i = 0; i < m_structuresTemplate.m_iAtomGroupTemplates; ++i)
			if (m_structuresTemplate.m_atomGroupTemplates[i].m_atomicNumbers.size() > 1) {
				moleculesPresent = true;
				break;
			}
		m_perturbations.loadDefaults(m_structuresTemplate.m_iAtomGroupTemplates, moleculesPresent, m_pConstraints);
	}

	if (setupElements[4] != NULL) {
		const char* stopAttNames[] = {xMaxTemperature, xMaxAcceptedPerturbations, xMinIterations};
		XsdAttributeUtil stopAttUtil(stopAttNames, s_stopAttReq, s_stopAttDef);
		if (!stopAttUtil.process(setupElements[4])) {
			return false;
		}
		const char** stopAttValues = stopAttUtil.getAllAttributes();
		if (stopAttValues[0] != NULL && stopAttValues[0][0] != '\0') {
			m_pfMaxStoppingTemperature = new FLOAT;
			if (!XsdTypeUtil::getPositiveFloat(stopAttValues[0], *m_pfMaxStoppingTemperature, stopAttNames[0], setupElements[4]))
				return false;
		}
		if (stopAttValues[1] != NULL && stopAttValues[1][0] != '\0') {
			m_pfMaxStoppingAcceptedPerturbations = new FLOAT;
			if (!XsdTypeUtil::getNonNegativeFloat(stopAttValues[1], *m_pfMaxStoppingAcceptedPerturbations, stopAttNames[1], setupElements[4]))
				return false;
			*m_pfMaxStoppingAcceptedPerturbations *= 0.01;
			if (!XsdTypeUtil::inRange(*m_pfMaxStoppingAcceptedPerturbations, 0, 1, setupElements[4], stopAttNames[1]))
				return false;
		}
		if (stopAttValues[2] != NULL && stopAttValues[2][0] != '\0') {
			m_piMinStoppingIterations = new unsigned int;
			if (!XsdTypeUtil::getPositiveInt(stopAttValues[2], *m_piMinStoppingIterations, stopAttNames[2], setupElements[4]))
				return false;
		}
	}
	if (m_pfMaxStoppingTemperature == NULL && m_pfMaxStoppingAcceptedPerturbations == NULL && m_piMinStoppingIterations == NULL) {
		m_pfMaxStoppingAcceptedPerturbations = new FLOAT;
		*m_pfMaxStoppingAcceptedPerturbations = 0.1;
	}

	const char* setupAttNames[] = {xSaveFrequency, xAcceptedPertHistIt};
	XsdAttributeUtil setupAttUtil(setupAttNames, s_setupAttReq, s_setupAttDef);
	if (!setupAttUtil.process(pSetupElem)) {
		return false;
	}
	const char** setupAttValues = setupAttUtil.getAllAttributes();
	if (!XsdTypeUtil::getPositiveInt(setupAttValues[0], m_iSaveFrequency, setupAttNames[0], pSetupElem))
		return false;
	if (!XsdTypeUtil::getPositiveInt(setupAttValues[1], m_iAcceptedPertHistIt, setupAttNames[1], pSetupElem))
		return false;

	return true;
}

bool SimulatedAnnealing::saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem)
{
	using namespace rapidxml;
	using namespace strings;
	xml_node<>* setup = doc.allocate_node(node_element, xSetup);
	pSimElem->append_node(setup);
	
	if (!m_structuresTemplate.save(doc, setup))
		return false;
	
	xml_node<>* temperature = doc.allocate_node(node_element, xTemperature);
	setup->append_node(temperature);
	if (m_pfStartingTemperature != NULL) {
		xml_node<>* kelvin = doc.allocate_node(node_element, xKelvin);
		temperature->append_node(kelvin);
		XsdTypeUtil::setAttribute(doc, kelvin, xValue, *m_pfStartingTemperature);
	} else if (m_pfPercentAcceptedPerturbations != NULL) {
		xml_node<>* acceptedPert = doc.allocate_node(node_element, xAcceptedPerturbations);
		temperature->append_node(acceptedPert);
		XsdTypeUtil::setAttribute(doc, acceptedPert, xPercent, *m_pfPercentAcceptedPerturbations * 100.0);
	}
	if (m_iDecreaseTemperatureAfterIt != 1)
		XsdTypeUtil::setAttribute(doc, temperature, xDecreaseAfterIteration, m_iDecreaseTemperatureAfterIt);
	if (m_fBoltzmannConstant != s_fDefaultBoltzmannConstant)
		XsdTypeUtil::setAttribute(doc, temperature, xBoltzmannConstant, m_fBoltzmannConstant);

	xml_node<>* annealingSchedule = doc.allocate_node(node_element, xAnnealingSchedule);
	setup->append_node(annealingSchedule);
	XsdTypeUtil::setAttribute(doc, annealingSchedule, xQuenchingFactor, m_fQuenchingFactor);

	if (!m_perturbations.saveSetup(doc, setup))
		return false;
	
	if (m_pfMaxStoppingTemperature != NULL || m_pfMaxStoppingAcceptedPerturbations != NULL || m_piMinStoppingIterations != NULL) {
		xml_node<>* stop = doc.allocate_node(node_element, xStop);
		setup->append_node(stop);
		if (m_pfMaxStoppingTemperature != NULL)
			XsdTypeUtil::setAttribute(doc, stop, xMaxTemperature, *m_pfMaxStoppingTemperature);
		if (m_pfMaxStoppingAcceptedPerturbations != NULL)
			XsdTypeUtil::setAttribute(doc, stop, xMaxAcceptedPerturbations, *m_pfMaxStoppingAcceptedPerturbations * 100);
		if (m_piMinStoppingIterations != NULL)
			XsdTypeUtil::setAttribute(doc, stop, xMinIterations, *m_piMinStoppingIterations);
	}
	if (m_iSaveFrequency != 1)
		XsdTypeUtil::setAttribute(doc, setup, xSaveFrequency, m_iSaveFrequency);
	if (m_iAcceptedPertHistIt != 200)
		XsdTypeUtil::setAttribute(doc, setup, xAcceptedPertHistIt, m_iAcceptedPertHistIt);

	return true;
}

bool SimulatedAnnealing::loadResume(const rapidxml::xml_node<>* pResumeElem)
{
	if (pResumeElem == NULL) {
		if (!m_structuresTemplate.initializeStructures(m_structures, m_pConstraints))
			return false;
		for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); it++) {
			SimulatedAnnealingRun* run = new SimulatedAnnealingRun(this, *it);
			m_runs.push_back(run);
			run->init();
		}
	} else {
		Structure* pStructure = new Structure();
		m_structures.push_back(pStructure);
		SimulatedAnnealingRun* run = new SimulatedAnnealingRun(this, pStructure);
		m_runs.push_back(run);
		if (!run->loadResume(pResumeElem))
			return false;
	}
	return true;
}

bool SimulatedAnnealing::saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem)
{
	for (std::list<SimulatedAnnealingRun*>::iterator it = m_runs.begin(); it != m_runs.end(); it++)
		if (!(*it)->saveResume(doc, pSimElem))
			return false;
	return true;
}

bool SimulatedAnnealing::verifyNotFinished() {
	m_bRunComplete = true;
	for (std::list<SimulatedAnnealingRun*>::iterator run = m_runs.begin(); run != m_runs.end(); ++run)
		if (!(*run)->m_bRunComplete) {
			m_bRunComplete = false;
			break;
		}
	if (m_bRunComplete) {
		printf("All runs have been completed.\n");
		return false;
	}
	return true;
}

bool SimulatedAnnealing::runMaster() {
	if (!verifyNotFinished())
		return false;
	if (!Action::run())
		return false;

	std::vector<SimulatedAnnealingRun*> runs(m_runs.size());
	for (std::list<SimulatedAnnealingRun*>::iterator it = m_runs.begin(); it != m_runs.end(); ++it)
		runs.push_back(*it);
	sort(runs.begin(), runs.end(), SimulatedAnnealingRun::iterationComparator);

	return true;
}

bool SimulatedAnnealing::runSlave() {
	if (!verifyNotFinished())
		return false;
	if (!Action::run())
		return false;

	return true;
}
