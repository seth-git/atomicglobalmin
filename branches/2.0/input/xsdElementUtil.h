
#ifndef __XSD_ELEMENT_UTIL_H__
#define __XSD_ELEMENT_UTIL_H__

#include <stdio.h>
#include <string>
#include "../tinyxml/tinyxml.h"

#define XSD_ALL              1
#define XSD_CHOICE           2
#define XSD_SEQUENCE         3

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
		
//		vector<TiXmlElement*> m_childElements;
		
//		std::string* m_attributeNames;
//		unsigned int m_iAttributes;
		
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
		}

		~XsdElementUtil()
		{
			// We don't need to deallocate m_pChoiceElement
			if (m_allElements != NULL) {
				delete[] m_allElements;
			}
		}

		bool process(TiXmlHandle &handle);
		TiXmlElement** getAllElements();
		TiXmlElement* getChoiceElement();
		unsigned int getChoiceElementIndex();
	private:
		void printChoiceError();
};

#endif
