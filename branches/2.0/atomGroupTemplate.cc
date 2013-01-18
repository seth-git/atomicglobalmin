
#include "atomGroupTemplate.h"
#include "structure.h"

//const char* AtomGroupTemplate::s_molAttNames[]  = {"number", "format"};
const bool    AtomGroupTemplate::s_molAttRequired[]        = {true    , true };
//const char* AtomGroupTemplate::s_molAttDefaults[]   = {"1"     , "Cartesian"};

const int AtomGroupTemplate::s_formatConstants[]     = {CARTESIAN};

AtomGroupTemplate::AtomGroupTemplate()
{
	
}

AtomGroupTemplate::~AtomGroupTemplate()
{
	cleanUp();
}

void AtomGroupTemplate::cleanUp()
{
	  for (std::vector<FLOAT*>::iterator it = m_coordinates.begin() ; it < m_coordinates.end(); it++ ) {
		  delete[] *it;
	  }
	  m_coordinates.clear();
	  m_atomicNumbers.clear();
}

bool AtomGroupTemplate::loadMolecule(TiXmlElement *pMoleculeTemplateElem, const Strings* messages)
{
	cleanUp();
	
	const char** values;
	
	const char* molAttNames[] = {messages->m_sxNumber.c_str(), messages->m_sxFormat.c_str()};
	const char* molAttDefaults[]  = {"1"                         , messages->m_spCartesian.c_str()};
	const char* formats[] = {messages->m_spCartesian.c_str()};
	
	XsdAttributeUtil attUtil(pMoleculeTemplateElem->Value(), molAttNames, s_molAttRequired, molAttDefaults);
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
	
	unsigned int lineNumber = pMoleculeTemplateElem->Row();
	static const char* delimeters = "\n";
	bool readFirstLine = false;
	char* line = strtok(text, delimeters);
	while (line != NULL) {
		if (sscanf(line, "%s %lf %lf %lf", tempStr, &(c[0]), &(c[1]), &(c[2])) == 4) {
			readFirstLine = true;
			if (!XsdTypeUtil::getAtomicNumber(tempStr, atomicNumber, lineNumber)) {
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
		++lineNumber;
	}
	
	delete[] c;
	delete[] text;
	delete[] tempStr;
	
	if (m_atomicNumbers.size() == 0) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sErrorEmptyMoleculeTemplate.c_str(), pMoleculeTemplateElem->Row(), pMoleculeTemplateElem->Value());
		return false;
	}
	
	return true;
}

//const char* AtomGroupTemplate::s_atomAttNames[]    = {"number", "z" };
const bool    AtomGroupTemplate::s_atomAttRequired[] = {true    , true};
const char*   AtomGroupTemplate::s_atomAttDefaults[] = {"1"     , NULL};

bool AtomGroupTemplate::loadAtom(TiXmlElement *pAtomTemplateElem, const Strings* messages)
{
	cleanUp();
	
	const char** values;
	const char* atomAttNames[] = {messages->m_sxNumber.c_str(), messages->m_sxZ.c_str()};
	
	XsdAttributeUtil attUtil(pAtomTemplateElem->Value(), atomAttNames, s_atomAttRequired, s_atomAttDefaults);
	if (!attUtil.process(pAtomTemplateElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	
	if (!XsdTypeUtil::getPositiveInt(values[0], m_iNumber, atomAttNames[0], pAtomTemplateElem)) {
		return false;
	}
	
	unsigned int z;
	if (!XsdTypeUtil::getAtomicNumber(values[1], z, pAtomTemplateElem->Row(), atomAttNames[1], pAtomTemplateElem->Value())) {
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

bool AtomGroupTemplate::save(TiXmlElement *pStructureTemplate, const Strings* messages)
{
	if (m_atomicNumbers.size() > 1) {
		TiXmlElement* pMoleculeTemplate = new TiXmlElement(messages->m_sxMoleculeTemplate.c_str());
		pStructureTemplate->LinkEndChild(pMoleculeTemplate);
		
		if (m_iNumber != 1) {
			pMoleculeTemplate->SetAttribute(messages->m_sxNumber.c_str(), m_iNumber);
		}
		
		if (m_iFormat != CARTESIAN) {
			pMoleculeTemplate->SetAttribute(messages->m_sxFormat.c_str(), messages->m_spCartesian.c_str());
		}
		
		std::string textstr;
		textstr.append("\n");
		FLOAT* c;
		char numString[100];
		for (unsigned int i = 0; i < m_atomicNumbers.size(); ++i) {
			snprintf(numString, sizeof(numString), "%u", m_atomicNumbers[i]);
			textstr.append(numString).append(" ");
			c = m_coordinates[i];
			doubleToString(c[0], numString);
			textstr.append(numString).append(" ");
			doubleToString(c[1], numString);
			textstr.append(numString).append(" ");
			doubleToString(c[2], numString);
			textstr.append(numString).append("\n");
		}
		TiXmlText* text = new TiXmlText(textstr.c_str());
		text->SetCDATA(true); // helps protect formatting
		pMoleculeTemplate->LinkEndChild(text);
	} else {
		TiXmlElement* pAtomTemplate = new TiXmlElement(messages->m_sxAtomTemplate.c_str());
		pStructureTemplate->LinkEndChild(pAtomTemplate);
		
		if (m_iNumber != 1) {
			pAtomTemplate->SetAttribute(messages->m_sxNumber.c_str(), m_iNumber);
		}
		
		pAtomTemplate->SetAttribute(messages->m_sxZ.c_str(), m_atomicNumbers[0]);
	}
	
	return true;
}

bool AtomGroupTemplate::init(Structure &structure) {
	if (structure.getNumberOfAtomGroups() != 1) {
		printf("AtomGroupTemplate::init should not be called with a structure having more than one atom group.");
		return false;
	}
	cleanUp();
	m_iNumber = structure.getNumberOfAtomGroups();
	m_iFormat = CARTESIAN;
	FLOAT* c;
	std::size_t nBytes = sizeof(FLOAT) * 3;

	const unsigned int* atomicNumbers = structure.getAtomicNumbers();
	const COORDINATE4** atomCoordinates = structure.getAtomCoordinates();

	for (unsigned int i = 0; i < structure.getNumberOfAtoms(); ++i) {
		m_atomicNumbers.push_back(atomicNumbers[i]);
		c = new FLOAT[3];
		memcpy(c, atomCoordinates[i], nBytes);
		m_coordinates.push_back(c);
	}
	return true;
}
