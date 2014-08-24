
#ifndef __SIMULATED_ANNEALING_H_
#define __SIMULATED_ANNEALING_H_

#include "../action.h"
#include "saPerturbations.h"
#include "simulatedAnnealingRun.h"

class SimulatedAnnealing : public Action {
public:
	FLOAT* m_pfStartingTemperature;
	FLOAT* m_pfPercentAcceptedPerturbations; // Value between 0 and 1.0
	unsigned int m_iDecreaseTemperatureAfterIt;
	FLOAT m_fBoltzmannConstant;
	FLOAT m_fQuenchingFactor;
	
	SAPerturbations m_perturbations;

	FLOAT* m_pfMaxStoppingTemperature;
	FLOAT* m_pfMaxStoppingAcceptedPerturbations; // Value between 0 and 1.0
	unsigned int* m_piMinStoppingIterations;

	unsigned int m_iAcceptedPertHistIt;

	std::list<SimulatedAnnealingRun*> m_runs;

	SimulatedAnnealing(Input* input);
	~SimulatedAnnealing();
	bool loadSetup(const rapidxml::xml_node<>* pSetupElem, const Strings* messages);
	bool saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem, const Strings* messages);
	bool loadResume(const rapidxml::xml_node<>* pResumeElem, const Strings* messages);
	bool saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem, const Strings* messages);

	bool runMaster();
	bool runSlave();

protected:
	bool verifyNotFinished();

private:
	static const FLOAT s_fDefaultBoltzmannConstant;
	static const bool s_setupAttReq[];
	static const char* s_setupAttDef[];
	static const unsigned int s_minOccurs[];
	static const bool s_tempAttReq[];
	static const char* s_tempAttDef[];
	static const bool s_stopAttReq[];
	static const char* s_stopAttDef[];
	
	void clear();
};

#endif
