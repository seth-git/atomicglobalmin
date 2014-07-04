
#include "xsdElementUtil.h"

bool XsdElementUtil::process (const rapidxml::xml_node<>* node)
{
	const rapidxml::xml_node<>* child;
	const char *name;
	unsigned int i;
	bool bMatch;
	const Strings* messagesDL = Strings::instance();

	clear();
	switch(m_type) {
		case XSD_ALL:
			m_allElements = new const rapidxml::xml_node<>*[m_iElements];
			for (i = 0; i < m_iElements; ++i) {
				m_allElements[i] = NULL;
			}
			for (child = node->first_node(); child; child=child->next_sibling())
			{
				name=child->name();
				bMatch = false;
				for (i = 0; i < m_iElements; ++i) {
					if (strcmp(m_elementNames[i], name) == 0) {
						if (m_allElements[i] != NULL) {
							printf(messagesDL->m_sMaxOf1ElementExceeded.c_str(), name, node->name());
							return false;
						}
						m_allElements[i] = child;
						bMatch = true;
					break;
					}
				}
				if (!bMatch) {
					printf(messagesDL->m_sUnrecognizedElement.c_str(), name, node->name());
					return false;
				}
			}

			for (i = 0; i < m_iElements; ++i) {
				if (m_allElements[i] == NULL && m_minOccurs[i] == 1) {
					printf(messagesDL->m_sElementNumNot1.c_str(), m_elementNames[i], node->name());
					return false;
				}
			}

			break;
		case XSD_CHOICE:
			child = node->first_node();
			if (!child) {
				printChoiceError(node->name());
				return false;
			}
			name=child->name();
			bMatch = false;
			for (i = 0; i < m_iElements; ++i) {
				if (strcmp(m_elementNames[i], name) == 0) {
					bMatch = true;
					m_pChoiceIndex = i;
					break;
				}
			}
			if (!bMatch) {
				printChoiceError(node->name());
				return false;
			}
			m_pChoiceElement = child;
			child = child->next_sibling();
			if (child) {
				printf(messagesDL->m_sElementRequiresExactly1Child.c_str(), node->name());
				return false;
			}
			break;
		case XSD_SEQUENCE:
			m_sequenceElements = new std::vector<const rapidxml::xml_node<>*>[m_iElements];
			
			child = node->first_node();
			i = 0;
			while (child) {
				name=child->name();
				while (strcmp(m_elementNames[i], name) != 0) {
					if (m_sequenceElements[i].size() < m_minOccurs[i]) {
						if (m_minOccurs[i] > 1) {
							printf(messagesDL->m_sElementRequiresNChildren.c_str(), m_minOccurs[i], m_elementNames[i], node->name());
						} else if (m_minOccurs[i] > 0) {
							printf(messagesDL->m_sElementRequires1ChildMin.c_str(), m_elementNames[i], node->name());
						}
						printSequenceError(node->name());
						return false;
					}
					++i;
					if (i >= m_iElements) {
						printf(messagesDL->m_sMisplacedElement.c_str(), name, node->name());
						printSequenceError(node->name());
						return false;
					}
				}

				if (m_maxOccurs[i] != XSD_UNLIMITED && m_sequenceElements[i].size() == m_maxOccurs[i]) {
					printf(messagesDL->m_sMaxElementsExceeded.c_str(), m_maxOccurs[i], name, node->name());
					return false;
				}
				m_sequenceElements[i].push_back(child);

				child = child->next_sibling();
			}
			break;
		default:
			printf("Unrecognized type %u in XsdElementUtil::process for parent element '%s'.\n", m_type, node->name());
			return false;
	}
	return true;
}

void XsdElementUtil::printChoiceError(const char* nodeName) {
	const Strings* messagesDL = Strings::instance();
	std::string choiceElements;
	choiceElements.append("'").append(m_elementNames[0]).append("'");
	for (unsigned int i = 1; i < m_iElements; ++i) {
		choiceElements.append(", '").append(m_elementNames[i]).append("'");
	}
	printf(messagesDL->m_sChoiceError.c_str(), nodeName, choiceElements.c_str());
}

void XsdElementUtil::printSequenceError(const char* nodeName) {
	const Strings* messagesDL = Strings::instance();
	if (m_iElements > 1) {
		printf(messagesDL->m_sChoiceElementOrder.c_str(), nodeName);
		for (unsigned int i = 0; i < m_iElements; ++i) {
			printf("%u. %s", i+1, m_elementNames[i]);
			if (m_minOccurs[i] == m_maxOccurs[i] && m_maxOccurs[i] != XSD_UNLIMITED) {
				printf(" (%u)", m_minOccurs[i]);
			} else {
				printf(" (%u-", m_minOccurs[i]);
				if (m_maxOccurs[i] == XSD_UNLIMITED) {
					printf("%s)", messagesDL->m_spUnlimited.c_str());
				} else {
					printf("%u)", m_maxOccurs[i]);
				}
			}
			printf("\n");
		}
	}
}

const rapidxml::xml_node<>** XsdElementUtil::getAllElements() {
	return m_allElements;
}

const rapidxml::xml_node<>* XsdElementUtil::getChoiceElement() {
	return m_pChoiceElement;
}

unsigned int XsdElementUtil::getChoiceElementIndex() {
	return m_pChoiceIndex;
}

std::vector<const rapidxml::xml_node<>*>* XsdElementUtil::getSequenceElements() {
	return m_sequenceElements;
}


