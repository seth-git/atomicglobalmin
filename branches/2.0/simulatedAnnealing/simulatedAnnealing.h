
#ifndef __SIMULATED_ANNEALING_H_
#define __SIMULATED_ANNEALING_H_

#include "../action.h"
#include "saPerturbations.h"
#include "simulatedAnnealingRun.h"
#include "../xsd/mpiUtils.h"

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
	bool loadSetup(const rapidxml::xml_node<>* pSetupElem);
	bool saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem);
	bool loadResume(const rapidxml::xml_node<>* pResumeElem);
	bool saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem);

	bool runMaster();
	bool runSlave();

protected:
	void calculateRunComplete();
	void clear();
	void findRuns(const int* structureIds, unsigned int numStructureIds, std::list<SimulatedAnnealingRun*> &results);

	static const FLOAT s_fDefaultBoltzmannConstant;
	static const bool s_setupAttReq[];
	static const char* s_setupAttDef[];

	static const char* s_elementNames[];
	static const unsigned int s_minOccurs[];

	static const char* s_setupElemNames[];

	static const char* s_tempAttNames[];
	static const bool s_tempAttReq[];
	static const char* s_tempAttDef[];

	static const char* s_stopAttNames[];
	static const bool s_stopAttReq[];
	static const char* s_stopAttDef[];

	typedef std::pair<int*,MPI_Request*> SendRequestPair;
	std::list<SendRequestPair> m_sendRequests;
};

#endif
