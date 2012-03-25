
#ifndef __XSD_ELEMENT_UTIL_H__
#define __XSD_ELEMENT_UTIL_H__

#include <stdio.h>
#include <string>
#include <vector>
#include "../tinyxml/tinyxml.h"
using namespace std;

#define XSD_ALL              1
#define XSD_CHOICE           2
#define XSD_SEQUENCE         3

#define XSD_UNLIMITED        0

class XsdElementUtil {
	private:
		const char* m_sParentElement;
		int m_type;
		const std::string* m_elementNames;
		unsigned int m_iElements;
		const unsigned int* m_minOccurs;
		const unsigned int* m_maxOccurs;
		
		TiXmlElement* m_pChoiceElement;
		unsigned int m_pChoiceIndex;
		TiXmlElement** m_allElements;

		vector<TiXmlElement*>* m_sequenceElements;
		
	public:

		XsdElementUtil(const char* parentElement, int type, const std::string* elementNames, unsigned int numElements,
		               const unsigned int* minOccurs, const unsigned int* maxOccurs)
		{
			m_sParentElement = parentElement;
			m_type = type;
			m_elementNames = elementNames;
			m_iElements = numElements;
			m_minOccurs = minOccurs;
			m_maxOccurs = maxOccurs;

			m_pChoiceElement = NULL;
			m_allElements = NULL;
			m_sequenceElements = NULL;
		}

		~XsdElementUtil()
		{
			cleanUp();
		}

		void cleanUp()
		{
			unsigned int i;
			// We don't need to deallocate m_pChoiceElement
			if (m_allElements != NULL) {
				delete[] m_allElements;
				m_allElements = NULL;
			}
			if (m_sequenceElements != NULL) {
				for (i = 0; i < m_iElements; ++i) {
					m_sequenceElements[i].clear();
				}
				delete[] m_sequenceElements;
				m_sequenceElements = NULL;
			}
		}

		bool process(TiXmlHandle &handle);
		TiXmlElement** getAllElements();
		TiXmlElement* getChoiceElement();
		unsigned int getChoiceElementIndex();
		vector<TiXmlElement*>* getSequenceElements();
	private:
		void printChoiceError();
		void printSequenceError();
};

#endif
