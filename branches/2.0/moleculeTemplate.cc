
#include "moleculeTemplate.h"

//const char* MoleculeTemplate::s_attributeNames[]  = {"number", "format"};
const bool    MoleculeTemplate::s_required[]        = {true    , true };
//const char* MoleculeTemplate::s_defaultValues[]   = {"1"     , "Cartesian"};

const int MoleculeTemplate::s_formatConstants[] = {CARTESIAN};

MoleculeTemplate::MoleculeTemplate()
{
	
}

MoleculeTemplate::~MoleculeTemplate()
{
	cleanUp();
}

void MoleculeTemplate::cleanUp()
{
	  for (std::vector<FLOAT*>::iterator it = m_coordinates.begin() ; it < m_coordinates.end(); it++ ) {
		  delete[] *it;
	  }
	  m_coordinates.clear();
	  m_atomicNumbers.clear();
}

bool MoleculeTemplate::load(TiXmlElement *pMoleculeTemplateElem, const Strings* messages)
{
	cleanUp();
	
	const char** values;
	
	const char* attributeNames[] = {messages->m_sxNumber.c_str(), messages->m_sxFormat.c_str()};
	const char* defaultValues[]  = {"1"                         , messages->m_spCartesian.c_str()};
	const char* formats[] = {messages->m_spCartesian.c_str()};
	
	XsdAttributeUtil attUtil(pMoleculeTemplateElem->Value(), attributeNames, s_required, defaultValues);
	if (!attUtil.process(pMoleculeTemplateElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	if (!XsdTypeUtil::getPositiveInt(values[0], m_iNumber, attributeNames[0], pMoleculeTemplateElem)) {
		return false;
	}
	
	if (!XsdTypeUtil::getEnumValue(attributeNames[1], values[1], m_iFormat, pMoleculeTemplateElem, formats, s_formatConstants)) {
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

bool MoleculeTemplate::save(TiXmlElement *pMoleculeSetTemplate, const Strings* messages)
{
	TiXmlElement* pMoleculeTemplate = new TiXmlElement(messages->m_sxMoleculeTemplate.c_str());
	pMoleculeSetTemplate->LinkEndChild(pMoleculeTemplate);
	
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
		textstr.append(numString);
		c = m_coordinates[i];
		snprintf(numString, sizeof(numString), "%lf", c[0]);
		textstr.append(" ").append(numString);
		snprintf(numString, sizeof(numString), "%lf", c[1]);
		textstr.append(" ").append(numString);
		snprintf(numString, sizeof(numString), "%lf", c[2]);
		textstr.append(" ").append(numString);
		textstr.append("\n");
	}
	TiXmlText* text = new TiXmlText(textstr.c_str());
	text->SetCDATA(true); // helps protect formatting
	pMoleculeTemplate->LinkEndChild(text);
	
	return true;
}
