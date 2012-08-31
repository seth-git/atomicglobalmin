
#include "simulatedAnnealing.h"

bool SimulatedAnnealing::load(TiXmlElement *pSimElem, const Strings* messages)
{
	return true;
}

bool SimulatedAnnealing::save(TiXmlElement *pActionElem, const Strings* messages)
{
	TiXmlElement* pSimElem = new TiXmlElement(messages->m_sxSimulatedAnnealing.c_str());
	pActionElem->LinkEndChild(pSimElem);
	
	return true;
}

bool SimulatedAnnealing::run()
{
	return true;
}
