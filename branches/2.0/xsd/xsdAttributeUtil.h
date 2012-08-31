
#ifndef __XSD_ATTRIBUTE_UTIL_H__
#define __XSD_ATTRIBUTE_UTIL_H__

#include <stdio.h>
#include <string>
#include "../tinyxml/tinyxml.h"
#include "../translation/strings.h"

class XsdAttributeUtil {
	private:
		const char*  m_sParentElement;
		const char** m_attributeNames;
		size_t m_iAttributes;
		const bool*  m_required;
		const char** m_defaultValues;
		const char** m_values;
		
	public:
		
		/***********************************************************************************************
		 * Purpose: This constructor collects information necessary to read attributes from an xml element.
		 * Parameters: parentElement - name of the element
		 *             attributeNames - array of strings containing the attribute names
		 *             required - array containing the required status of each attribute
		 *             defaultValues - array containing optional default attribute values
		 */
		template <std::size_t iAttributes>
		XsdAttributeUtil(const char* parentElement, const char* (&attributeNames)[iAttributes],
		                 const bool (&required)[iAttributes], const char* (&defaultValues)[iAttributes])
		{
			m_sParentElement = parentElement;
			m_attributeNames = attributeNames;
			m_iAttributes = iAttributes;
			m_required = required;
			m_defaultValues = defaultValues;
			
			if (iAttributes > 0) {
				m_values = new const char*[iAttributes];
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
		
		/***********************************************************************************************
		 * Purpose: Call this to read xml attributes from an element.
		 * Parameters: pElem - the tiny xml element
		 * Returns: true, if there were no errors
		 */
		bool process(TiXmlElement* pElem);
		
		/***********************************************************************************************
		 * Purpose: Call this to get the attributes that were read.
		 * Parameters: none
		 * Returns: an array of strings containing attribute values
		 */
		const char** getAllAttributes();
		
		/***********************************************************************************************
		 * Purpose: Call this to ensure there are no attributes on an element.
		 * Parameters: pElem - the tiny xml element
		 * Returns: true, if there were no errors
		 */
		static bool hasNoAttributes(TiXmlElement *pElem);
	private:
		/***********************************************************************************************
		 * Purpose: This method is called if there are errors reading the attributes.  It displays
		 *    the list of allowed attributes.
		 */
		void printAvailableAttributes();
};

#endif
