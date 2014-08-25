
#include "atomGroupTemplate.h"
#include "structure.h"
#include "atomGroup.h"

//const char* AtomGroupTemplate::s_molAttNames[]  = {"number", "format"};
const bool    AtomGroupTemplate::s_molAttRequired[]        = {true    , true };
//const char* AtomGroupTemplate::s_molAttDefaults[]   = {"1"     , "Cartesian"};

const int AtomGroupTemplate::s_formatConstants[]     = {CARTESIAN};

AtomGroupTemplate::AtomGroupTemplate()
{
	
}

AtomGroupTemplate::~AtomGroupTemplate()
{
	clear();
}

void AtomGroupTemplate::clear()
{
	for (std::vector<FLOAT*>::iterator it = m_coordinates.begin() ; it < m_coordinates.end(); it++ )
		delete[] *it;
	m_coordinates.clear();
	m_atomicNumbers.clear();
}

bool AtomGroupTemplate::loadMolecule(const rapidxml::xml_node<>* pMoleculeTemplateElem)
{
	using namespace rapidxml;
	using namespace strings;
	clear();
	
	const char** values;
	
	const char* molAttNames[] = {xNumber, xFormat};
	const char* molAttDefaults[]  = {"1"                         , pCartesian};
	const char* formats[] = {pCartesian};
	
	XsdAttributeUtil attUtil(molAttNames, s_molAttRequired, molAttDefaults);
	if (!attUtil.process(pMoleculeTemplateElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	if (!XsdTypeUtil::getPositiveInt(values[0], m_iNumber, molAttNames[0], pMoleculeTemplateElem)) {
		return false;
	}
	
	if (!XsdTypeUtil::getEnumValue(molAttNames[1], values[1], m_iFormat, pMoleculeTemplateElem, formats, s_formatConstants)) {
		return false;
	}
	
	std::string sText;
	if (!XsdTypeUtil::readElementText(pMoleculeTemplateElem, sText)) {
		return false;
	}
	unsigned int atomicNumber;
	
	FLOAT* c = new FLOAT[3];
	char* text = new char[sText.length() + 1];
	char* tempStr = new char[sText.length() + 1];
	strncpy(text, sText.c_str(), sText.length() + 1); // make a copy since strtok is will need to modify it
	
	static const char* delimeters = "\n";
	bool readFirstLine = false;
	char* line = strtok(text, delimeters);
	while (line != NULL) {
		if (sscanf(line, "%s %lf %lf %lf", tempStr, &(c[0]), &(c[1]), &(c[2])) == 4) {
			readFirstLine = true;
			if (!XsdTypeUtil::getAtomicNumber(tempStr, atomicNumber)) {
				delete[] c;
				delete[] text;
				delete[] tempStr;
				return false;
			}
			m_atomicNumbers.push_back(atomicNumber);
			m_coordinates.push_back(c);
			c = new FLOAT[3];
		} else if (readFirstLine) {
			break; // data must be on consecutive lines
		}
		line = strtok(NULL, delimeters);
	}
	
	delete[] c;
	delete[] text;
	delete[] tempStr;
	
	if (m_atomicNumbers.size() == 0) {
		printf(ErrorEmptyMoleculeTemplate, pMoleculeTemplateElem->name());
		return false;
	}
	
	return true;
}

//const char* AtomGroupTemplate::s_atomAttNames[]    = {"number", "z" };
const bool    AtomGroupTemplate::s_atomAttRequired[] = {true    , true};
const char*   AtomGroupTemplate::s_atomAttDefaults[] = {"1"     , NULL};

bool AtomGroupTemplate::loadAtom(const rapidxml::xml_node<>* pAtomTemplateElem)
{
	using namespace strings;
	clear();
	
	const char** values;
	const char* atomAttNames[] = {xNumber, xBigZ};
	
	XsdAttributeUtil attUtil(atomAttNames, s_atomAttRequired, s_atomAttDefaults);
	if (!attUtil.process(pAtomTemplateElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	
	if (!XsdTypeUtil::getPositiveInt(values[0], m_iNumber, atomAttNames[0], pAtomTemplateElem)) {
		return false;
	}
	
	unsigned int z;
	if (!XsdTypeUtil::getAtomicNumber(values[1], z, atomAttNames[1], pAtomTemplateElem->name())) {
		return false;
	}
	m_atomicNumbers.push_back(z);
	FLOAT* c = new FLOAT[3];
	c[0] = 0;
	c[1] = 0;
	c[2] = 0;
	m_coordinates.push_back(c);
	
	return true;
}

bool AtomGroupTemplate::save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pStructureTemplate)
{
	using namespace rapidxml;
	using namespace strings;
	if (m_atomicNumbers.size() > 1) {
		std::string textstr;
		textstr.append("\n");
		FLOAT* c;
		char numString[100];
		for (unsigned int i = 0; i < m_atomicNumbers.size(); ++i) {
			snprintf(numString, sizeof(numString), "%u", m_atomicNumbers[i]);
			textstr.append(numString).append(" ");
			c = m_coordinates[i];
			XsdTypeUtil::createFloat(c[0], numString, sizeof(numString));
			textstr.append(numString).append(" ");
			XsdTypeUtil::createFloat(c[1], numString, sizeof(numString));
			textstr.append(numString).append(" ");
			XsdTypeUtil::createFloat(c[2], numString, sizeof(numString));
			textstr.append(numString).append("\n");
		}
		rapidxml::xml_node<>* pMoleculeTemplate = doc.allocate_node(node_element, xMoleculeTemplate);
		pStructureTemplate->append_node(pMoleculeTemplate);
		size_t len = textstr.length();
		rapidxml::xml_node<>* text = doc.allocate_node(node_cdata, NULL, doc.allocate_string(textstr.c_str(), len), 0, len);
		pMoleculeTemplate->append_node(text);

		if (m_iNumber != 1)
			XsdTypeUtil::setAttribute(doc, pMoleculeTemplate, xNumber, m_iNumber);

		if (m_iFormat != CARTESIAN)
			pMoleculeTemplate->append_attribute(doc.allocate_attribute(xFormat, pCartesian));
	} else {
		xml_node<>* pAtomTemplate = doc.allocate_node(node_element, xAtomTemplate);
		pStructureTemplate->append_node(pAtomTemplate);
		
		if (m_iNumber != 1)
			XsdTypeUtil::setAttribute(doc, pAtomTemplate, xNumber, m_iNumber);
		
		XsdTypeUtil::setAttribute(doc, pAtomTemplate, xBigZ, m_atomicNumbers[0]);
	}
	
	return true;
}

bool AtomGroupTemplate::init(Structure &structure) {
	if (structure.getNumberOfAtomGroups() != 1) {
		printf("AtomGroupTemplate::init should not be called with a structure having more than one atom group.");
		return false;
	}
	clear();
	m_iNumber = structure.getNumberOfAtomGroups();
	m_iFormat = CARTESIAN;
	FLOAT* c;

	const unsigned int* atomicNumbers = structure.getAtomicNumbers();
	const COORDINATE4* const* atomCoordinates = structure.getAtomCoordinates();

	for (unsigned int i = 0; i < structure.getNumberOfAtoms(); ++i) {
		m_atomicNumbers.push_back(atomicNumbers[i]);
		c = new FLOAT[3];
		memcpy(c, atomCoordinates[i], SIZEOF_COORDINATE3);
		m_coordinates.push_back(c);
	}
	return true;
}

bool AtomGroupTemplate::atomicNumbersMatch(const AtomGroup &atomGroup) const {
	unsigned int m = atomGroup.getNumberOfAtoms();
	if (m != m_atomicNumbers.size())
		return false;
	const unsigned int* atomicNumbers = atomGroup.getAtomicNumbers();
	for (unsigned int i = 0; i < m; ++i)
		if (atomicNumbers[i] != m_atomicNumbers[i])
			return false;
	return true;
}

bool AtomGroupTemplate::atomicNumbersMatch(const unsigned int* structureAtomicNumbers,
		unsigned int size) const {
	unsigned int m = m_atomicNumbers.size();
	if (size < m)
		return false;
	for (unsigned int i = 0; i < m; ++i)
		if (structureAtomicNumbers[i] != m_atomicNumbers[i])
			return false;
	return true;
}
