
#include "xsdAttributeUtil.h"

bool XsdAttributeUtil::process (TiXmlElement* pElem)
{
	TiXmlAttribute* pAttrib;
	const char* pName;
	unsigned int i;
	bool bMatch;
	const Strings* messages = Strings::instance();
	for (i = 0; i < m_iAttributes; ++i) {
		m_values[i] = NULL;
	}

	for (pAttrib=pElem->FirstAttribute(); pAttrib; pAttrib=pAttrib->Next())
	{
		pName = pAttrib->Name();
		bMatch = false;
		for (i = 0; i < m_iAttributes; ++i) {
			if (strcmp(m_attributeNames[i], pName) == 0) {
				if (m_values[i] != NULL) {
					printf(messages->m_sDuplicateAttributes.c_str(), m_attributeNames[i], pAttrib->Row(), m_sParentElement);
					return false;
				}
				m_values[i] = pAttrib->Value();
				bMatch = true;
				break;
			}
		}
		if (!bMatch) {
			printf(messages->m_sUnrecognizedAttribute.c_str(), pName, pAttrib->Row(), m_sParentElement);
			return false;
		}
	}
	for (i = 0; i < m_iAttributes; ++i) {
		if (m_values[i] == NULL) {
			if (m_defaultValues != NULL && m_defaultValues[i][0] != '\0') { // If the length is not zero
				m_values[i] = m_defaultValues[i];
				continue;
			} else if (m_required[i]) {
				printf(messages->m_sMissingAttribute.c_str(), m_attributeNames[i], m_sParentElement, pElem->Row());
				return false;
			}
		}
	}
	return true;
}

const char** XsdAttributeUtil::getAllAttributes()
{
	return m_values;
}

bool XsdAttributeUtil::hasNoAttributes(TiXmlElement *pElem, const char* elementName) {
	if (pElem->FirstAttribute()) {
		const Strings* messages = Strings::instance();
		printf(messages->m_sMustNotContainAttributes.c_str(), elementName, pElem->Row());
		return false;
	}
	return true;
}
