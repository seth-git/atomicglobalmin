
#ifndef __ATOM_GROUP_TEMPLATE_H_
#define __ATOM_GROUP_TEMPLATE_H_

class Structure; // Forward declaration

#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"

#define CARTESIAN 0

class AtomGroupTemplate {
public:
	unsigned int m_iNumber;
	int m_iFormat;
	std::vector<unsigned int> m_atomicNumbers;
	std::vector<FLOAT*> m_coordinates; // vector of FLOAT[3]
	
	AtomGroupTemplate();
	~AtomGroupTemplate();
	bool loadMolecule(TiXmlElement *pMoleculeTemplateElem, const Strings* messages);
	bool loadAtom(TiXmlElement *pAtomTemplateElem, const Strings* messages);
	bool save(TiXmlElement *pParentElem, const Strings* messages);
	bool init(Structure &structure);
	
private:
	static const bool s_molAttRequired[];
	static const int s_formatConstants[];
	
	static const bool s_atomAttRequired[];
	static const char* s_atomAttDefaults[];
	
	void cleanUp();
};

#endif

