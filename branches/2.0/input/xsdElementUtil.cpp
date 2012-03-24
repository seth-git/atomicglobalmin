
#include "xsdElementUtil.h"

bool XsdElementUtil::process (TiXmlHandle &handle)
{
	TiXmlHandle hRoot(0);
	TiXmlHandle hChild(0);
	TiXmlElement* pElem;
	const char *pName;
	unsigned int i;
	bool bMatch;
	const char* pPrevName;

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
					if (strncmp(m_elementNames[i].c_str(), pName, m_elementNames[i].length()+1) == 0) {
						if (m_allElements[i] != NULL) {
							printf("More than one element '%s' can't be inside the '%s' element.\n", m_elementNames[i].c_str(), m_sParentElement);
							return false;
						}
						m_allElements[i] = pElem;
						bMatch = true;
					break;
					}
				}
				if (!bMatch) {
					printf("Unrecognized element '%s' inside the '%s' element.\n", pName, m_sParentElement);
					return false;
				}
			}

			for (i = 0; i < m_iElements; ++i) {
				if (m_allElements[i] == NULL && m_minOccurs[i] == 1) {
					printf("There must be one '%s' element inside the '%s' element.\n", m_elementNames[i].c_str(), m_sParentElement);
					return false;
				}
			}

			break;
		case XSD_CHOICE:
			pElem = handle.FirstChild().Element();
			if (!pElem) {
				printChoiceError();
				return false;
			}
			pName=pElem->Value();
			bMatch = false;
			for (i = 0; i < m_iElements; ++i) {
				if (strncmp(m_elementNames[i].c_str(), pName, m_elementNames[i].length()+1) == 0) {
					bMatch = true;
					m_pChoiceIndex = i;
					break;
				}
			}
			if (!bMatch) {
				printChoiceError();
				return false;
			}
			m_pChoiceElement = pElem;
			if (pElem->NextSiblingElement()) {
				printf("The '%s' element must contain exactly one child element.\n", m_sParentElement);
				return false;
			}
			break;
		case XSD_SEQUENCE:
			m_sequenceElements = new vector<TiXmlElement*>[m_iElements];
			
			pElem = handle.FirstChild().Element();
			pPrevName = NULL;
			i = 0;
			while (pElem) {
				pName=pElem->Value();
				while (strncmp(m_elementNames[i].c_str(), pName, m_elementNames[i].length()+1) != 0) {
					if (m_sequenceElements[i].size() < m_minOccurs[i]) {
						if (m_minOccurs[i] > 1) {
							printf("There must be at least %u '%s' elements inside the '%s' element.\n", m_minOccurs[i], m_elementNames[i].c_str(), m_sParentElement);
						} else if (m_minOccurs[i] > 0) {
							printf("There must be at least one '%s' element inside the '%s' element.\n", m_elementNames[i].c_str(), m_sParentElement);
						}
						printSequenceError();
						return false;
					}
					++i;
					if (i >= m_iElements) {
						printf("Unrecognized or misplaced '%s' element inside the '%s' element.\n", pName, m_sParentElement);
						printSequenceError();
						return false;
					}
				}

				if (m_maxOccurs[i] != XSD_UNLIMITED && m_sequenceElements[i].size() == m_maxOccurs[i]) {
					printf("There can't be more than %u '%s' element(s) inside the '%s' element.\n", m_maxOccurs[i], pName, m_sParentElement);
					return false;
				}
				m_sequenceElements[i].push_back(pElem);

				pElem = pElem->NextSiblingElement();
			}
			break;
	}
	return true;
}

void XsdElementUtil::printChoiceError() {
	printf("The element '%s' must contain one of these elements: '%s'", m_sParentElement, m_elementNames[0].c_str());
	for (unsigned int i = 1; i < m_iElements; ++i) {
		printf(", '%s'", m_elementNames[i].c_str());
	}
	printf(".\n");
}

void XsdElementUtil::printSequenceError() {
	if (m_iElements > 1) {
		printf("Elements inside the '%s' element must be listed in this order:\n", m_sParentElement);
		for (unsigned int i = 0; i < m_iElements; ++i) {
			printf("%u. %s", i+1, m_elementNames[i].c_str());
			if (m_minOccurs[i] == m_maxOccurs[i] && m_maxOccurs[i] != XSD_UNLIMITED) {
				printf(" (%u)", m_minOccurs[i]);
			} else {
				printf(" (%u-", m_minOccurs[i]);
				if (m_maxOccurs[i] == XSD_UNLIMITED) {
					printf("unlimited)");
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

vector<TiXmlElement*>* XsdElementUtil::getSequenceElements() {
	return m_sequenceElements;
}


