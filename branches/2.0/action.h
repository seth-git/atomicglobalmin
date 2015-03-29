
#ifndef __ACTION_H_
#define __ACTION_H_

class Input; // Forward declaration

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "structuresTemplate.h"
#include "translation/strings.h"
#include "energyXml.h"
#include "energy.h"
#include "constraints.h"
#include "xsd/mpiUtils.h"
#include "rmsDistance.h"
#include <list>

#define ACTION_DEBUG false

class Action {
public:
	std::list<Structure*> m_structures;
	StructuresTemplate m_structuresTemplate;
	std::vector<Constraints*> m_constraints;
	std::map<std::string,Constraints*> m_constraintsMap;
	Constraints* m_pConstraints;
	EnergyXml energyXml;
	Energy* m_pEnergy;

	unsigned int m_iSaveFrequency; // Save xml file after each set of this number of iterations

	unsigned int m_iMaxResults;
	FLOAT m_fResultsRmsDistance;

	enum OrderBy {ENERGY, FILE, ID};
	OrderBy m_orderBy;

	std::list<Structure*> m_results;

	bool m_bRunComplete;

	unsigned int m_iEnergyCalculations;
	time_t m_tPrevElapsedSeconds; // Time taken by previous runs
	time_t m_tStartTime; // Time stamp when the current run started

	Action(Input* input);
	virtual ~Action();

	virtual void clear();

	virtual bool load(const rapidxml::xml_node<>* pActionElem);
	virtual bool save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pActionElem);
	virtual bool loadSetup(const rapidxml::xml_node<>* pSetupElem) = 0;
	virtual bool saveSetup(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pActionElem) = 0;
	virtual bool loadResume(const rapidxml::xml_node<>* pResumeElem) = 0;
	virtual bool saveResume(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pActionElem) = 0;
	virtual bool run();
	virtual bool cleanupRun();
	virtual bool runMaster() = 0;
	virtual bool runSlave() = 0;

	// These are public for testing purposes
	void renameResultsFiles();
	void updateResults(Structure &structure);
	void updateResults(Structure* pStructure);

protected:
	void checkResults(Structure* pStructure, std::list<Structure*>::iterator resultsIt, unsigned int insertionIndex);
	void deleteStructureFiles(Structure &structure);

	Input* m_pInput;
	unsigned int m_iMpiRank;
	unsigned int m_iMpiProcesses;

	time_t getTotalElapsedSeconds();

	bool calculateTimeToQuit();

	static unsigned int s_iMaxEnergyCalcFailures;
	static FLOAT s_fMaxMPIProcessFailures;

private:
	static const char* s_attributeNames[];
	static const unsigned int s_minOccurs[];
	static const unsigned int s_maxOccurs[];

	static const char* s_elementNames[];
	static const bool  s_required[];
	static const char* s_defaultValues[];

	static const char* s_resultsElementNames[];
	static const unsigned int s_resultsMinOccurs[];
	static const unsigned int s_resultsMaxOccurs[];

	static const char* s_resultsAttributeNames[];
	static const bool  s_resultsRequired[];
	static const char* s_resultsDefaultValues[];

	static const char* s_orderByOptions[];
};

#endif
