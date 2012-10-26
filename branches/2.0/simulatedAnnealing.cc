
#include "simulatedAnnealing.h"
#include "input.h"

SimulatedAnnealing::SimulatedAnnealing(Input* input) : Action(input)
{
}

//const char*      SimulatedAnnealing::s_elementNames[] = {"populationTemplate", "temperature", "annealingSchedule", "perturbations", "stop"};
const unsigned int SimulatedAnnealing::s_minOccurs[]    = {1                   , 1            , 1                  , 1              , 1     };

bool SimulatedAnnealing::loadSetup(TiXmlElement *pSetupElem, const Strings* messages)
{
	const char* elementNames[] = {messages->m_sxPopulationTemplate.c_str(), messages->m_sxTemperature.c_str(), messages->m_sxAnnealingSchedule.c_str(), messages->m_sxPerturbations.c_str(), messages->m_sxStop.c_str()};
	XsdElementUtil setupUtil(pSetupElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle hRoot(0);
	hRoot=TiXmlHandle(pSetupElem);
	if (!setupUtil.process(hRoot)) {
		return false;
	}
	TiXmlElement** setupElements = setupUtil.getAllElements();
	
	m_populationTemplate.load(setupElements[0], messages);
	
	return true;
}

bool SimulatedAnnealing::saveSetup(TiXmlElement *pSimElem, const Strings* messages)
{
	TiXmlElement* setup = new TiXmlElement(messages->m_sxSetup.c_str());
	pSimElem->LinkEndChild(setup);
	
	m_populationTemplate.save(setup, messages);
	
	return true;
}

bool SimulatedAnnealing::loadResume(TiXmlElement *pResumeElem, const Strings* messages)
{
	return true;
}

bool SimulatedAnnealing::saveResume(TiXmlElement *pSimElem, const Strings* messages)
{
	TiXmlElement* resume = new TiXmlElement(messages->m_sxResume.c_str());
	pSimElem->LinkEndChild(resume);
	
	return true;
}

bool SimulatedAnnealing::run()
{
	m_pInput->save();
	return true;
}
