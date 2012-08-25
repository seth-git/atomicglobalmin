
#ifndef __CONSTRAINTS_H__
#define __CONSTRAINTS_H__

#include <stdio.h>
#include <string>
#include "xsdAttributeUtil.h"
#include "xsdElementUtil.h"
#include "xsdTypeUtil.h"
#include "../translation/strings.h"

class Constraints {
	public:
		std::string m_sName;
		FLOAT* m_pfCubeLWH; // cube length, width, and height
		FLOAT* m_pfGeneralMinAtomicDistance;
		FLOAT* m_pfGeneralMaxAtomicDistance;
		FLOAT** m_rgMinAtomicDistances;
		std::map<unsigned int, std::map<unsigned int,FLOAT> > m_mapMinAtomicDistances;

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

		bool load(TiXmlElement *pConstraintsElem, const Strings* messages);
		void save(TiXmlElement *pConstraintsElem, const Strings* messages);
	
	private:
		static const unsigned int s_minOccurs[];
		
		static const unsigned int s_distMinOccurs[];
		static const unsigned int s_distMaxOccurs[];
		
		static const bool         s_required[];
		static const char*        s_defaultValues[];

		bool addMinDist(TiXmlElement *pElem, const Strings* messages);
		
		void cleanUp();
};

#endif
