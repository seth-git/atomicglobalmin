
#include "xsdElementUtil.h"

bool XsdElementUtil::process (TiXmlHandle &handle)
{
	TiXmlHandle hRoot(0);
	TiXmlHandle hChild(0);
	TiXmlElement* pElem;
	const char *pName;
	unsigned int i;
	bool bMatch;
	const Strings* messages = Strings::instance();

	cleanUp();
	switch(m_type) {
		case XSD_ALL:
			m_allElements = new TiXmlElement*[m_iElements];
			for (i = 0; i < m_iElements; ++i) {
				m_allElements[i] = NULL;
			}
			for (pElem = handle.FirstChild().Element(); pElem; pElem=pElem->NextSiblingElement())
			{
				pName=pElem->Value();
				bMatch = false;
				for (i = 0; i < m_iElements; ++i) {
					if (strcmp(m_elementNames[i], pName) == 0) {
						if (m_allElements[i] != NULL) {
							printf(messages->m_sMaxOf1ElementExceeded.c_str(), m_elementNames[i], m_sParentElement, pElem->Row());
							return false;
						}
						m_allElements[i] = pElem;
						bMatch = true;
					break;
					}
				}
				if (!bMatch) {
					printf(messages->m_sUnrecognizedElement.c_str(), pName, m_sParentElement, pElem->Row());
					return false;
				}
			}

			for (i = 0; i < m_iElements; ++i) {
				if (m_allElements[i] == NULL && m_minOccurs[i] == 1) {
					printf(messages->m_sElementNumNot1.c_str(), m_elementNames[i], m_sParentElement, handle.Element()->Row());
					return false;
				}
			}

			break;
		case XSD_CHOICE:
			pElem = handle.FirstChild().Element();
			if (!pElem) {
				printChoiceError(handle.Element()->Row());
				return false;
			}
			pName=pElem->Value();
			bMatch = false;
			for (i = 0; i < m_iElements; ++i) {
				if (strcmp(m_elementNames[i], pName) == 0) {
					bMatch = true;
					m_pChoiceIndex = i;
					break;
				}
			}
			if (!bMatch) {
				printChoiceError(pElem->Row());
				return false;
			}
			m_pChoiceElement = pElem;
			pElem = pElem->NextSiblingElement();
			if (pElem) {
				printf(messages->m_sElementRequiresExactly1Child.c_str(), m_sParentElement, pElem->Row());
				return false;
			}
			break;
		case XSD_SEQUENCE:
			m_sequenceElements = new std::vector<TiXmlElement*>[m_iElements];
			
			pElem = handle.FirstChild().Element();
			i = 0;
			while (pElem) {
				pName=pElem->Value();
				while (strcmp(m_elementNames[i], pName) != 0) {
					if (m_sequenceElements[i].size() < m_minOccurs[i]) {
						if (m_minOccurs[i] > 1) {
							printf(messages->m_sElementRequiresNChildren.c_str(), m_minOccurs[i], m_elementNames[i], m_sParentElement, pElem->Row());
						} else if (m_minOccurs[i] > 0) {
							printf(messages->m_sElementRequires1ChildMin.c_str(), m_elementNames[i], m_sParentElement, pElem->Row());
						}
						printSequenceError();
						return false;
					}
					++i;
					if (i >= m_iElements) {
						printf(messages->m_sMisplacedElement.c_str(), pName, m_sParentElement, pElem->Row());
						printSequenceError();
						return false;
					}
				}

				if (m_maxOccurs[i] != XSD_UNLIMITED && m_sequenceElements[i].size() == m_maxOccurs[i]) {
					printf(messages->m_sMaxElementsExceeded.c_str(), m_maxOccurs[i], pName, m_sParentElement, pElem->Row());
					return false;
				}
				m_sequenceElements[i].push_back(pElem);

				pElem = pElem->NextSiblingElement();
			}
			break;
	}
	return true;
}

void XsdElementUtil::printChoiceError(int lineNumber) {
	const Strings* messages = Strings::instance();
	std::string choiceElements;
	choiceElements.append("'").append(m_elementNames[0]).append("'");
	for (unsigned int i = 1; i < m_iElements; ++i) {
		choiceElements.append(", '").append(m_elementNames[i]).append("'");
	}
	printf(messages->m_sChoiceError.c_str(), lineNumber, m_sParentElement, choiceElements.c_str());
}

void XsdElementUtil::printSequenceError() {
	const Strings* messages = Strings::instance();
	if (m_iElements > 1) {
		printf(messages->m_sChoiceElementOrder.c_str(), m_sParentElement);
		for (unsigned int i = 0; i < m_iElements; ++i) {
			printf("%u. %s", i+1, m_elementNames[i]);
			if (m_minOccurs[i] == m_maxOccurs[i] && m_maxOccurs[i] != XSD_UNLIMITED) {
				printf(" (%u)", m_minOccurs[i]);
			} else {
				printf(" (%u-", m_minOccurs[i]);
				if (m_maxOccurs[i] == XSD_UNLIMITED) {
					printf("%s)", messages->m_spUnlimited.c_str());
				} else {
					printf("%u)", m_maxOccurs[i]);
				}
			}
			printf("\n");
		}
	}
}

TiXmlElement** XsdElementUtil::getAllElements() {
	return m_allElements;
}

TiXmlElement* XsdElementUtil::getChoiceElement() {
	return m_pChoiceElement;
}

unsigned int XsdElementUtil::getChoiceElementIndex() {
	return m_pChoiceIndex;
}

std::vector<TiXmlElement*>* XsdElementUtil::getSequenceElements() {
	return m_sequenceElements;
}


