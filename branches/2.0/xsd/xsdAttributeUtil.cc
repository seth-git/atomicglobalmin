
#include "xsdAttributeUtil.h"

bool XsdAttributeUtil::process (TiXmlElement* pElem)
{
	TiXmlAttribute* pAttrib;
	const char* pName;
	unsigned int i;
	bool bMatch;
	const Strings* messagesDL = Strings::instance();
	
	for (i = 0; i < m_iAttributes; ++i) {
		m_values[i] = NULL;
	}
	
	for (pAttrib=pElem->FirstAttribute(); pAttrib; pAttrib=pAttrib->Next())
	{
		pName = pAttrib->Name();
		bMatch = false;
		for (i = 0; i < m_iAttributes; ++i) {
			// Note: strncmp is not necessary on the next line because both strings are always null terminated.
			// m_attributeNames[i] comes from a constant string or an std::string, and pName comes from a successfully parsed xml file.
			if (strcmp(m_attributeNames[i], pName) == 0) {
				if (m_values[i] != NULL) {
					printf(messagesDL->m_sDuplicateAttributes.c_str(), m_attributeNames[i], pAttrib->Row(), m_sParentElement);
					return false;
				}
				m_values[i] = pAttrib->Value();
				bMatch = true;
				break;
			}
		}
		if (!bMatch) {
			printf(messagesDL->m_sUnrecognizedAttribute.c_str(), pName, pAttrib->Row(), m_sParentElement);
			printAvailableAttributes();
			return false;
		}
	}

	for (i = 0; i < m_iAttributes; ++i) {
		if (m_values[i] == NULL) {
			if (m_defaultValues[i] != NULL && m_defaultValues[i][0] != '\0') { // If the length is not zero
				m_values[i] = m_defaultValues[i];
				continue;
			} else if (m_required[i]) {
				printf(messagesDL->m_sMissingAttribute.c_str(), m_attributeNames[i], m_sParentElement, pElem->Row());
				return false;
			}
		}
	}

	return true;
}

void XsdAttributeUtil::printAvailableAttributes() {
	const Strings* messagesDL = Strings::instance();
	std::string availableAttributes;
	availableAttributes.append("'").append(m_attributeNames[0]).append("'");
	for (unsigned int i = 1; i < m_iAttributes; ++i) {
		availableAttributes.append(", '").append(m_attributeNames[i]).append("'");
	}
	printf(messagesDL->m_sAvailableAttributes.c_str(), m_sParentElement, availableAttributes.c_str());
}

const char** XsdAttributeUtil::getAllAttributes()
{
	return m_values;
}

bool XsdAttributeUtil::hasNoAttributes(TiXmlElement *pElem) {
	if (pElem->FirstAttribute()) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sMustNotContainAttributes.c_str(), pElem->Value(), pElem->Row());
		return false;
	}
	return true;
}
