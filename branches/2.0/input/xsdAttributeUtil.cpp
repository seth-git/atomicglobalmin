
#include "xsdAttributeUtil.h"

bool XsdAttributeUtil::process (TiXmlElement* pElem)
{
	TiXmlAttribute* pAttrib;
	const char* pName;
	unsigned int i;
	bool bMatch;

	for (i = 0; i < m_iAttributes; ++i) {
		m_values[i] = NULL;
	}

	for (pAttrib=pElem->FirstAttribute(); pAttrib; pAttrib=pAttrib->Next())
	{
		pName = pAttrib->Name();
		bMatch = false;
		for (i = 0; i < m_iAttributes; ++i) {
			if (strncmp(m_attributeNames[i].c_str(), pName, m_attributeNames[i].length()+1) == 0) {
				if (m_values[i] != NULL) {
					printf("More than one attribute '%s' can't be listed on the element '%s'.\n", m_attributeNames[i].c_str(), m_sParentElement);
					return false;
				}
				m_values[i] = pAttrib->Value();
				bMatch = true;
				break;
			}
		}
		if (!bMatch) {
			printf("Unrecognized attribute '%s' on the element '%s'.\n", pName, m_sParentElement);
			return false;
		}
	}
	for (i = 0; i < m_iAttributes; ++i) {
		if (m_values[i] == NULL) {
			if (m_defaultValues[i].length() > 0) {
				m_values[i] = m_defaultValues[i].c_str();
			} else if (m_required[i]) {
				printf("The attribute '%s' is required on the element '%s'.\n", m_attributeNames[i].c_str(), m_sParentElement);
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
