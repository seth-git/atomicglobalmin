
#ifndef __POPULATION_TEMPLATE_H_
#define __POPULATION_TEMPLATE_H_

#include "moleculeTemplate.h"

class PopulationTemplate {
public:
	PopulationTemplate();
	~PopulationTemplate();
	bool load(TiXmlElement *pPopulationTemplateElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);

private:
	void cleanUp();

	static const unsigned int s_minOccurs[];
	static const unsigned int s_popTempMinOccurs[];
	static const unsigned int s_popTempMaxOccurs[];
	
	unsigned int m_iMoleculeTemplates; // size of m_moleculeTemplates
	MoleculeTemplate* m_moleculeTemplates;
};

#endif

