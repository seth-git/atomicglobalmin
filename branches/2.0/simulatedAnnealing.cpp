
#include "simulatedAnnealing.h"

bool SimulatedAnnealing::loadSetup(TiXmlElement *pSetupElem, const Strings* messages)
{
		
	return true;
}

bool SimulatedAnnealing::loadResume(TiXmlElement *pResumeElem, const Strings* messages)
{
	return true;
}

bool SimulatedAnnealing::saveSetup(TiXmlElement *pSimElem, const Strings* messages)
{
	TiXmlElement* setup = new TiXmlElement(messages->m_sxSetup.c_str());
	pSimElem->LinkEndChild(setup);
	
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
	return true;
}
