
#ifndef __CONSTRAINTS_H__
#define __CONSTRAINTS_H__

#include <stdio.h>
#include <string>
#include "xsdAttributeUtil.h"
#include "xsdElementUtil.h"
#include "xsdTypeUtil.h"

class Constraints {
	public:
		FLOAT* m_pfCubeLWH; // cube length, width, and height
		FLOAT* m_pfGeneralMinAtomicDistance;
		FLOAT* m_pfGeneralMaxAtomicDistance;
		FLOAT** m_rgMinAtomicDistances;
		std::string m_sName;

		Constraints()
		{
			m_pfCubeLWH = NULL;
			m_pfGeneralMinAtomicDistance = NULL;
			m_pfGeneralMaxAtomicDistance = NULL;
			m_rgMinAtomicDistances = NULL;
		}

		~Constraints()
		{
			cleanUp();
		}

		bool load(TiXmlElement *pElem);
		void save();
	
	private:
		static const std::string  s_elementNames[];
		static const unsigned int s_minOccurs[];
		
		static const std::string  s_distElementNames[];
		static const unsigned int s_distMinOccurs[];
		static const unsigned int s_distMaxOccurs[];

		bool addMinDist(TiXmlElement *pElem);
		
		void cleanUp();
};

#endif
