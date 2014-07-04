
#include "simulatedAnnealing.h"
#include "input.h"

SimulatedAnnealing::SimulatedAnnealing(Input* input) : Action(input)
{
	m_pfStartingTemperature = NULL;
	m_pfPercentAcceptedPerturbations = NULL;
	m_pfMaxStoppingTemperature = NULL;
	m_pfMaxStoppingAcceptedPerturbations = NULL;
	m_piMinStoppingIterations = NULL;
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

bool SimulatedAnnealing::loadSetup(const rapidxml::xml_node<>* pSetupElem, const Strings* messages)
{
	using namespace rapidxml;
	clear();
	const char* elementNames[] = {messages->m_sxStructuresTemplate.c_str(), messages->m_sxTemperature.c_str(), messages->m_sxAnnealingSchedule.c_str(), messages->m_sxPerturbations.c_str(), messages->m_sxStop.c_str()};
	XsdElementUtil setupUtil(XSD_ALL, elementNames, s_minOccurs);
	if (!setupUtil.process(pSetupElem))
		return false;
	const xml_node<>** setupElements = setupUtil.getAllElements();
	
	if (!m_structuresTemplate.load(setupElements[0], m_constraintsMap, messages))
		return false;
	
	if (setupElements[1] != NULL) {
		const char* setupElemNames[] = {messages->m_sxKelvin.c_str(), messages->m_sxAcceptedPerturbations.c_str()};
		XsdElementUtil tempElemUtil(XSD_CHOICE, setupElemNames);
		if (!tempElemUtil.process(setupElements[1]))
			return false;
		unsigned int tempChoiceIndex = tempElemUtil.getChoiceElementIndex();
		const xml_node<>* tempChoiceElem = tempElemUtil.getChoiceElement();

		if (tempChoiceIndex == 0) {
			m_pfStartingTemperature = new FLOAT;
			if (!XsdTypeUtil::read1PosFloatAtt(tempChoiceElem, *m_pfStartingTemperature, messages->m_sxValue.c_str(), true, NULL))
				return false;
		} else {
			m_pfPercentAcceptedPerturbations = new FLOAT;
			if (!XsdTypeUtil::read1PosFloatAtt(tempChoiceElem, *m_pfPercentAcceptedPerturbations, messages->m_sxPercent.c_str(), true, NULL))
				return false;
			if (!XsdTypeUtil::inRange(*m_pfPercentAcceptedPerturbations, 0, 100, tempChoiceElem, messages->m_sxPercent.c_str()))
				return false;
			*m_pfPercentAcceptedPerturbations *= 0.01;
		}

		const char* tempAttNames[] = {messages->m_sxDecreaseAfterIteration.c_str(), messages->m_sxBoltzmannConstant.c_str()};
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
		if (!XsdTypeUtil::read1PosFloatAtt(setupElements[2], m_fQuenchingFactor, messages->m_sxQuenchingFactor.c_str(), true, NULL))
			return false;
	} else {
		m_fQuenchingFactor = 0.9995;
	}

	if (setupElements[3] != NULL) {
		if (!m_perturbations.loadSetup(setupElements[3], messages))
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
		const char* stopAttNames[] = {messages->m_sxMaxTemperature.c_str(), messages->m_sxMaxAcceptedPerturbations.c_str(), messages->m_sxMinIterations.c_str()};
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

	const char* setupAttNames[] = {messages->m_sxSaveFrequency.c_str(), messages->m_sxAcceptedPertHistIt.c_str()};
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

bool SimulatedAnnealing::saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem, const Strings* messages)
{
	using namespace rapidxml;
	xml_node<>* setup = doc.allocate_node(node_element, messages->m_sxSetup.c_str());
	pSimElem->append_node(setup);
	
	if (!m_structuresTemplate.save(doc, setup, messages))
		return false;
	
	xml_node<>* temperature = doc.allocate_node(node_element, messages->m_sxTemperature.c_str());
	setup->append_node(temperature);
	if (m_pfStartingTemperature != NULL) {
		xml_node<>* kelvin = doc.allocate_node(node_element, messages->m_sxKelvin.c_str());
		temperature->append_node(kelvin);
		XsdTypeUtil::setAttribute(doc, kelvin, messages->m_sxValue.c_str(), *m_pfStartingTemperature);
	} else if (m_pfPercentAcceptedPerturbations != NULL) {
		xml_node<>* acceptedPert = doc.allocate_node(node_element, messages->m_sxAcceptedPerturbations.c_str());
		temperature->append_node(acceptedPert);
		XsdTypeUtil::setAttribute(doc, acceptedPert, messages->m_sxPercent.c_str(), *m_pfPercentAcceptedPerturbations * 100.0);
	}
	if (m_iDecreaseTemperatureAfterIt != 1)
		XsdTypeUtil::setAttribute(doc, temperature, messages->m_sxDecreaseAfterIteration.c_str(), m_iDecreaseTemperatureAfterIt);
	if (m_fBoltzmannConstant != s_fDefaultBoltzmannConstant)
		XsdTypeUtil::setAttribute(doc, temperature, messages->m_sxBoltzmannConstant.c_str(), m_fBoltzmannConstant);

	xml_node<>* annealingSchedule = doc.allocate_node(node_element, messages->m_sxAnnealingSchedule.c_str());
	setup->append_node(annealingSchedule);
	XsdTypeUtil::setAttribute(doc, annealingSchedule, messages->m_sxQuenchingFactor.c_str(), m_fQuenchingFactor);

	if (!m_perturbations.saveSetup(doc, setup, messages))
		return false;
	
	if (m_pfMaxStoppingTemperature != NULL || m_pfMaxStoppingAcceptedPerturbations != NULL || m_piMinStoppingIterations != NULL) {
		xml_node<>* stop = doc.allocate_node(node_element, messages->m_sxStop.c_str());
		setup->append_node(stop);
		if (m_pfMaxStoppingTemperature != NULL)
			XsdTypeUtil::setAttribute(doc, stop, messages->m_sxMaxTemperature.c_str(), *m_pfMaxStoppingTemperature);
		if (m_pfMaxStoppingAcceptedPerturbations != NULL)
			XsdTypeUtil::setAttribute(doc, stop, messages->m_sxMaxAcceptedPerturbations.c_str(), *m_pfMaxStoppingAcceptedPerturbations * 100);
		if (m_piMinStoppingIterations != NULL)
			XsdTypeUtil::setAttribute(doc, stop, messages->m_sxMinIterations.c_str(), *m_piMinStoppingIterations);
	}
	if (m_iSaveFrequency != 1)
		XsdTypeUtil::setAttribute(doc, setup, messages->m_sxSaveFrequency.c_str(), m_iSaveFrequency);
	if (m_iAcceptedPertHistIt != 200)
		XsdTypeUtil::setAttribute(doc, setup, messages->m_sxAcceptedPertHistIt.c_str(), m_iAcceptedPertHistIt);

	return true;
}

bool SimulatedAnnealing::loadResume(const rapidxml::xml_node<>* pResumeElem, const Strings* messages)
{
	if (pResumeElem == NULL) {
		if (!m_structuresTemplate.initializeStructures(m_structures, m_pConstraints))
			return false;
		if (m_pfStartingTemperature != NULL)
			m_fTemperature = *m_pfStartingTemperature;
		else
			m_fTemperature = 1000000;
	} else {

	}
	return true;
}

bool SimulatedAnnealing::saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem, const Strings* messages)
{
	using namespace rapidxml;
	xml_node<>* resume = doc.allocate_node(node_element, messages->m_sxResume.c_str());
	pSimElem->append_node(resume);

	xml_node<>* totalEnergyCalculations = doc.allocate_node(node_element, messages->m_sxTotalEnergyCalculations.c_str());
	XsdTypeUtil::setAttribute(doc, totalEnergyCalculations, messages->m_sxValue.c_str(), m_iEnergyCalculations);
	resume->append_node(totalEnergyCalculations);
	
	xml_node<>* elapsedSeconds = doc.allocate_node(node_element, messages->m_sxElapsedSeconds.c_str());
	XsdTypeUtil::setAttribute(doc, elapsedSeconds, messages->m_sxValue.c_str(), getTotalElapsedSeconds());
	resume->append_node(elapsedSeconds);

	xml_node<>* structures = doc.allocate_node(node_element, messages->m_sxStructures.c_str());
	resume->append_node(structures);
	for (std::list<Structure*>::iterator it = m_structures.begin(); it != m_structures.end(); it++)
		if (!(*it)->save(doc, structures, messages))
			return false;

	return true;
}

bool SimulatedAnnealing::runMaster() {
	return true;
}

bool SimulatedAnnealing::runSlave() {
	return true;
}
