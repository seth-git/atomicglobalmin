
#ifndef __ACTION_H_
#define __ACTION_H_

class Input; // Forward declaration

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"
#include "internalEnergy.h"
#include "externalEnergy/externalEnergy.h"
#include "constraints.h"

class Action {
public:
	std::vector<Constraints*> m_constraints;
	std::map<std::string,Constraints*> m_constraintsMap;
	Constraints* m_pConstraints;
	bool m_bExternalEnergy;
	InternalEnergy m_internalEnergy;
	ExternalEnergy m_externalEnergy;

	Action(Input* input);
	~Action();

	void cleanUp();

	virtual bool load(TiXmlElement *pActionElem, const Strings* messages);
	virtual bool save(TiXmlElement *pActionElem, const Strings* messages);
	virtual bool loadSetup(TiXmlElement *pSetupElem, const Strings* messages) = 0;
	virtual bool saveSetup(TiXmlElement *pSetupElem, const Strings* messages) = 0;
	virtual bool loadResume(TiXmlElement *pResumeElem, const Strings* messages) = 0;
	virtual bool saveResume(TiXmlElement *pResumeElem, const Strings* messages) = 0;
	virtual bool run() = 0;

protected:
	Input* m_pInput;

private:
	static const unsigned int s_minOccurs[];
	static const unsigned int s_maxOccurs[];

	static const bool  s_required[];
	static const char* s_defaultValues[];
};

#endif
