
#ifndef __SIMULATED_ANNEALING_RUN_H_
#define __SIMULATED_ANNEALING_RUN_H_

#include "../structure.h"

class SimulatedAnnealing;

class SimulatedAnnealingRun {
public:
	const SimulatedAnnealing* m_sharedData;

	unsigned int m_iEnergyCalculations;
	time_t m_tPrevElapsedSeconds; // Time taken by previous runs
	unsigned int m_iIteration;
	bool m_bRunComplete;

	Structure* m_pStructure;
	FLOAT m_fTemperature;
	FLOAT m_fPerturbationsPerIteration;
	FLOAT m_fRotationRadians;
	FLOAT m_fTranslationVectorLength;

	SimulatedAnnealingRun(const SimulatedAnnealing* sharedData, Structure* pStructure);

	void init();

	bool loadResume(const rapidxml::xml_node<>* pResumeElem);
	bool saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pSimElem);

	static bool iterationComparator(const SimulatedAnnealingRun* a, const SimulatedAnnealingRun* b) { return a->m_iIteration < b->m_iIteration; }

	/**************************************************************************
	 * Purpose: This method performs a perturbation of the m_pStructure, then
	 *    calculates the energy of that structure and updates the temperature.
	 * Returns: True if the perturbation was accepted.
	 */
	bool performIteration();

protected:
	Structure m_structureBackup;

	time_t getTotalElapsedSeconds();

	static const char* s_resumeElemNames[];
	static const unsigned int s_resumeMinOccurs[];
};

#endif
