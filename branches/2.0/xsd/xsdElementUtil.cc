
#include "xsdElementUtil.h"

bool XsdElementUtil::process (const rapidxml::xml_node<>* node)
{
	using namespace strings;
	const rapidxml::xml_node<>* child;
	const char *name;
	unsigned int i;
	bool bMatch;
	
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
							printf(MaxOf1ElementExceeded, name, node->name());
							return false;
						}
						m_allElements[i] = child;
						bMatch = true;
					break;
					}
				}
				if (!bMatch) {
					printf(UnrecognizedElement, name, node->name());
					return false;
				}
			}

			for (i = 0; i < m_iElements; ++i) {
				if (m_allElements[i] == NULL && m_minOccurs[i] == 1) {
					printf(ElementNumNot1, m_elementNames[i], node->name());
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
				printf(ElementRequiresExactly1Child, node->name());
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
							printf(ElementRequiresNChildren, m_minOccurs[i], m_elementNames[i], node->name());
						} else if (m_minOccurs[i] > 0) {
							printf(ElementRequires1ChildMin, m_elementNames[i], node->name());
						}
						printSequenceError(node->name());
						return false;
					}
					++i;
					if (i >= m_iElements) {
						printf(MisplacedElement, name, node->name());
						printSequenceError(node->name());
						return false;
					}
				}

				if (m_maxOccurs[i] != XSD_UNLIMITED && m_sequenceElements[i].size() == m_maxOccurs[i]) {
					printf(MaxElementsExceeded, m_maxOccurs[i], name, node->name());
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
	using namespace strings;
	std::string choiceElements;
	choiceElements.append("'").append(m_elementNames[0]).append("'");
	for (unsigned int i = 1; i < m_iElements; ++i) {
		choiceElements.append(", '").append(m_elementNames[i]).append("'");
	}
	printf(ChoiceError, nodeName, choiceElements.c_str());
}

void XsdElementUtil::printSequenceError(const char* nodeName) {
	using namespace strings;
	if (m_iElements > 1) {
		printf(ChoiceElementOrder, nodeName);
		for (unsigned int i = 0; i < m_iElements; ++i) {
			printf("%u. %s", i+1, m_elementNames[i]);
			if (m_minOccurs[i] == m_maxOccurs[i] && m_maxOccurs[i] != XSD_UNLIMITED) {
				printf(" (%u)", m_minOccurs[i]);
			} else {
				printf(" (%u-", m_minOccurs[i]);
				if (m_maxOccurs[i] == XSD_UNLIMITED) {
					printf("%s)", pUnlimited);
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


