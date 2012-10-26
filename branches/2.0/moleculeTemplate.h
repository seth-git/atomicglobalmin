
#ifndef __MOLECULE_TEMPLATE_H_
#define __MOLECULE_TEMPLATE_H_

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"

#define CARTESIAN 0

class MoleculeTemplate {
public:
	unsigned int m_iNumber;
	int m_iFormat;
	std::vector<unsigned int> m_atomicNumbers;
	std::vector<FLOAT*> m_coordinates; // vector of FLOAT[3]
	
	MoleculeTemplate();
	~MoleculeTemplate();
	bool load(TiXmlElement *pMoleculeTemplateElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);
	
private:
	static const bool s_required[];
	static const int s_formatConstants[];
	
	void cleanUp();
};

#endif

