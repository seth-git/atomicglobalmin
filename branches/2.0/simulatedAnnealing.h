
#ifndef __SIMULATED_ANNEALING_H_
#define __SIMULATED_ANNEALING_H_

#include "action.h"

class SimulatedAnnealing : public Action {
	public:
		SimulatedAnnealing() {}
		
		bool loadSetup(TiXmlElement *pSetupElem, const Strings* messages);
		bool loadResume(TiXmlElement *pResumeElem, const Strings* messages);
		bool saveSetup(TiXmlElement *pSetupElem, const Strings* messages);
		bool saveResume(TiXmlElement *pResumeElem, const Strings* messages);
		bool run();
};

#endif
