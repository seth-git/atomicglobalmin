
#ifndef __SIMULATED_ANNEALING_H_
#define __SIMULATED_ANNEALING_H_

#include "action.h"

class SimulatedAnnealing : public IAction {
	public:
		SimulatedAnnealing() {}
		
		bool load(TiXmlElement *pSimElem, const Strings* messages);
		bool save(TiXmlElement *pActionElem, const Strings* messages);
		bool run();
};

#endif
