
#ifndef __ATOM_GROUP_TEMPLATE_H_
#define __ATOM_GROUP_TEMPLATE_H_

class Structure; // Forward declaration
class AtomGroup; // Forward declaration

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
	bool loadMolecule(const rapidxml::xml_node<>* pMoleculeTemplateElem, const Strings* messages);
	bool loadAtom(const rapidxml::xml_node<>* node, const Strings* messages);
	bool save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* parentNode, const Strings* messages);
	bool init(Structure &structure);
	bool atomicNumbersMatch(const AtomGroup &atomGroup) const;
	bool atomicNumbersMatch(const unsigned int* structureAtomicNumbers,
			unsigned int size) const;
	
private:
	static const bool s_molAttRequired[];
	static const int s_formatConstants[];
	
	static const bool s_atomAttRequired[];
	static const char* s_atomAttDefaults[];
	
	void clear();
};

#endif

