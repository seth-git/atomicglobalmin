
#ifndef __POPULATION_TEMPLATE_H_
#define __POPULATION_TEMPLATE_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"

class PopulationTemplate {
public:
	PopulationTemplate();
	bool load(TiXmlElement *pPopulationTemplateElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);

private:
	static const unsigned int s_minOccurs[];
};

#endif

