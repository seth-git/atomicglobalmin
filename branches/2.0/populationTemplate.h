
#ifndef __POPULATION_TEMPLATE_H_
#define __POPULATION_TEMPLATE_H_

#include "atomGroupTemplate.h"
#include "constraints.h"
#include "seed.h"

class PopulationTemplate {
public:
	unsigned int m_iAtomGroupTemplates; // size of m_atomGroupTemplates (will be zero if there are none)
	AtomGroupTemplate* m_atomGroupTemplates;

	Constraints* m_pConstraints;
	
	unsigned int m_iLinear;
	Constraints* m_pLinearConstraints;
	unsigned int m_iPlanar;
	Constraints* m_pPlanarConstraints;
	unsigned int m_i3D;
	Constraints* m_p3DConstraints;
	
	FLOAT* m_bondRotationalSearchAngle; // Stored in radians
	
	Seed* m_pSeed;
	
	PopulationTemplate();
	~PopulationTemplate();
	bool load(TiXmlElement *pPopulationTemplateElem, std::map<std::string,Constraints*> &constraintsMap, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);

private:
	void cleanUp();
	bool readInitializationType(TiXmlElement *pElem, std::map<std::string,Constraints*> &constraintsMap, unsigned int &numberOfThisType, Constraints** pConstraints, const Strings* messages);
	
	static const bool         s_attRequired[];
	static const char*        s_attDefaults[];

	static const unsigned int s_minOccurs[];
	static const unsigned int s_popTempMinOccurs[];
	static const unsigned int s_popTempMaxOccurs[];
	
	static const bool s_initTypeAttRequired[];
	static const char* s_initTypeAttDefaults[];
};

#endif

