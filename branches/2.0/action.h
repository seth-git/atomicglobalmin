
#ifndef __ACTION_H_
#define __ACTION_H_

class Input; // Forward declaration

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "structuresTemplate.h"
#include "translation/strings.h"
#include "internalEnergy.h"
#include "externalEnergy/externalEnergy.h"
#include "constraints.h"

class Action {
public:
	unsigned int m_iStructures;
	Structure* m_structures;
	StructuresTemplate m_structuresTemplate;
	std::vector<Constraints*> m_constraints;
	std::map<std::string,Constraints*> m_constraintsMap;
	Constraints* m_pConstraints;
	bool m_bExternalEnergy;
	InternalEnergy m_internalEnergy;
	ExternalEnergy m_externalEnergy;

	unsigned int m_iEnergyCalculations;
	time_t m_tElapsedSeconds; // Time taken by previous runs
	time_t m_tStartTime; // Time stamp when the current run started

	Action(Input* input);
	~Action();

	void cleanUp();

	virtual bool load(TiXmlElement *pActionElem, const Strings* messages);
	virtual bool save(TiXmlElement *pActionElem, const Strings* messages);
	virtual bool loadSetup(TiXmlElement *pSetupElem, const Strings* messages) = 0;
	virtual bool saveSetup(TiXmlElement *pSetupElem, const Strings* messages) = 0;
	virtual bool loadResume(TiXmlElement *pResumeElem, const Strings* messages) = 0;
	virtual bool saveResume(TiXmlElement *pResumeElem, const Strings* messages) = 0;
	virtual bool run();

protected:
	Input* m_pInput;

private:
	static const unsigned int s_minOccurs[];
	static const unsigned int s_maxOccurs[];

	static const bool  s_required[];
	static const char* s_defaultValues[];
};

#endif
