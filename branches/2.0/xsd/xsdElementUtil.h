
#ifndef __XSD_ELEMENT_UTIL_H__
#define __XSD_ELEMENT_UTIL_H__

#include <stdio.h>
#include <string>
#include <vector>
#include "../tinyxml/tinyxml.h"
#include "../translation/strings.h"

#define XSD_ALL              1
#define XSD_CHOICE           2
#define XSD_SEQUENCE         3

#define XSD_UNLIMITED        0

class XsdElementUtil {
	private:
		const char* m_sParentElement;
		unsigned int m_type;
		const char** m_elementNames;
		unsigned int m_iElements;
		const unsigned int* m_minOccurs;
		const unsigned int* m_maxOccurs;
		
		TiXmlElement* m_pChoiceElement;
		unsigned int m_pChoiceIndex;
		TiXmlElement** m_allElements;

		std::vector<TiXmlElement*>* m_sequenceElements; // array of vectors
		
	public:

		template <std::size_t iElements>
		XsdElementUtil(const char* parentElement, int type, const char* (&elementNames)[iElements],
		               const unsigned int (&minOccurs)[iElements], const unsigned int (&maxOccurs)[iElements])
		{
			init(parentElement, type, elementNames);
			m_minOccurs = minOccurs;
			m_maxOccurs = maxOccurs;
		}

		template <std::size_t iElements>
		XsdElementUtil(const char* parentElement, int type, const char* (&elementNames)[iElements],
		               const unsigned int (&minOccurs)[iElements])
		{
			init(parentElement, type, elementNames);
			m_minOccurs = minOccurs;
		}

		template <std::size_t iElements>
		XsdElementUtil(const char* parentElement, int type, const char* (&elementNames)[iElements])
		{
			init(parentElement, type, elementNames);
		}

		~XsdElementUtil()
		{
			cleanUp();
		}

		bool process(TiXmlHandle &handle);
		TiXmlElement** getAllElements();
		TiXmlElement* getChoiceElement();
		unsigned int getChoiceElementIndex();
		std::vector<TiXmlElement*>* getSequenceElements(); // array of vectors
	private:
		void printChoiceError(int lineNumber);
		void printSequenceError();
		
		// This is needed because one constructor cannot call another or the destructor gets called too many times
		template <std::size_t iElements>
		void init(const char* parentElement, int type, const char* (&elementNames)[iElements])
		{
			m_sParentElement = parentElement;
			m_type = type;
			m_elementNames = elementNames;
			m_iElements = iElements;
			m_minOccurs = NULL;
			m_maxOccurs = NULL;
			
			m_pChoiceElement = NULL;
			m_allElements = NULL;
			m_sequenceElements = NULL;
		}

		void cleanUp()
		{
			// We don't need to deallocate m_pChoiceElement
			if (m_allElements != NULL) {
				delete[] m_allElements;
				m_allElements = NULL;
			}
			if (m_sequenceElements != NULL) {
				for (unsigned int i = 0; i < m_iElements; ++i) {
					m_sequenceElements[i].clear();
				}
				delete[] m_sequenceElements;
				m_sequenceElements = NULL;
			}
		}
};

#endif
