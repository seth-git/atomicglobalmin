
#ifndef __INTERNAL_ENERGY_H__
#define __INTERNAL_ENERGY_H__

#include <stdio.h>
#include <string>
#include "xsdAttributeUtil.h"
#include "xsdTypeUtil.h"

#define LENNARD_JONES              1

class InternalEnergy {
	public:
		bool m_bLocalOptimization;
		int m_iMethod;

		InternalEnergy()
		{
			m_bLocalOptimization = false;
			m_iMethod = LENNARD_JONES;
		}

		bool load(TiXmlElement *pElem);
		void save();

	private:
		static const char* s_attributeNames[];
		static const bool  s_required[];
		static const char* s_defaultValues[];
		static const char* s_methods[];
		static const int   s_methodConstants[];
};

#endif
