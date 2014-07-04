
#include "xsdAttributeUtil.h"

bool XsdAttributeUtil::process (const rapidxml::xml_node<>* node)
{
	const char* name;
	const char* defaultValue;
	unsigned int i;
	bool bMatch;
	const Strings* messagesDL = Strings::instance();
	
	for (i = 0; i < m_iAttributes; ++i) {
		m_values[i] = NULL;
	}
	
	for (rapidxml::xml_attribute<> *attr = node->first_attribute(); attr; attr = attr->next_attribute())
	{
		name = attr->name();
		bMatch = false;
		for (i = 0; i < m_iAttributes; ++i) {
			// Note: strncmp is not necessary on the next line because both strings are always null terminated.
			// m_attributeNames[i] comes from a constant string or an std::string, and name comes from a successfully parsed xml file.
			if (strcmp(m_attributeNames[i], name) == 0) {
				if (m_values[i] != NULL) {
					printf(messagesDL->m_sDuplicateAttributes.c_str(), name, node->name(), m_values[i], attr->value());
					return false;
				}
				m_values[i] = attr->value();
				bMatch = true;
				break;
			}
		}
		if (!bMatch) {
			printf(messagesDL->m_sUnrecognizedAttribute.c_str(), name, node->name());
			printAvailableAttributes(node->name());
			return false;
		}
	}

	for (i = 0; i < m_iAttributes; ++i) {
		if (m_values[i] == NULL) {
			defaultValue = m_defaultValues[i];
			if (defaultValue != NULL && defaultValue[0] != '\0') { // If the length is not zero
				m_values[i] = defaultValue;
				continue;
			} else if (m_required[i]) {
				printf(messagesDL->m_sMissingAttribute.c_str(), m_attributeNames[i], node->name());
				return false;
			}
		}
	}

	return true;
}

void XsdAttributeUtil::printAvailableAttributes(const char* elementName) {
	const Strings* messagesDL = Strings::instance();
	std::string availableAttributes;
	availableAttributes.append("'").append(m_attributeNames[0]).append("'");
	for (unsigned int i = 1; i < m_iAttributes; ++i) {
		availableAttributes.append(", '").append(m_attributeNames[i]).append("'");
	}
	printf(messagesDL->m_sAvailableAttributes.c_str(), elementName, availableAttributes.c_str());
}

const char** XsdAttributeUtil::getAllAttributes()
{
	return m_values;
}

bool XsdAttributeUtil::hasNoAttributes(const rapidxml::xml_node<>* node) {
	if (node->first_attribute()) {
		const Strings* messagesDL = Strings::instance();
		printf(messagesDL->m_sMustNotContainAttributes.c_str(), node->name());
		return false;
	}
	return true;
}
