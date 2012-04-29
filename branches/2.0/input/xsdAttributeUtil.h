
#ifndef __XSD_ATTRIBUTE_UTIL_H__
#define __XSD_ATTRIBUTE_UTIL_H__

#include <stdio.h>
#include <string>
#include "../tinyxml/tinyxml.h"

class XsdAttributeUtil {
	private:
		const char* m_sParentElement;
		const std::string* m_attributeNames;
		unsigned int m_iAttributes;
		const bool* m_required;
		const std::string* m_defaultValues;
		const char** m_values;
		
	public:
		XsdAttributeUtil(const char* parentElement, const std::string* attributeNames, unsigned int numAttributes,
		                 const bool* required, const std::string* defaultValues)
		{
			m_sParentElement = parentElement;
			m_attributeNames = attributeNames;
			m_iAttributes = numAttributes;
			m_required = required;
			m_defaultValues = defaultValues;
			
			if (numAttributes > 0) {
				m_values = new const char*[numAttributes];
			} else {
				m_values = NULL;
			}
		}

		~XsdAttributeUtil()
		{
			if (m_values != NULL) {
				delete[] m_values;
			}
		}
		
		bool process(TiXmlElement* pElem);
		const char** getAllAttributes();
		
		static bool hasNoAttributes(TiXmlElement *pElem, const char* elementName);
};

#endif
