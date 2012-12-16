
#ifndef __CONSTRAINTS_H__
#define __CONSTRAINTS_H__

#include <stdio.h>
#include <string>
#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdTypeUtil.h"
#include "translation/strings.h"

#define MIN_DIST_ARRAY_SIZE  MAX_ATOMIC_NUMBERS+1  
typedef FLOAT MinDistArray[MIN_DIST_ARRAY_SIZE];

class Constraints {
	public:
		std::string m_sName;
		Constraints* m_pBase;
		FLOAT* m_pfCubeLWH; // cube length, width, and height
		FLOAT* m_pfGeneralMinAtomicDistance;
		FLOAT* m_pfGeneralMaxAtomicDistance;
		MinDistArray* m_rgMinAtomicDistances; // 2D array, insert two atomic numbers, and get the minimum distance back (will be -1 if there is no value)

		Constraints()
		{
			m_pBase = NULL;
			m_pfCubeLWH = NULL;
			m_pfGeneralMinAtomicDistance = NULL;
			m_pfGeneralMaxAtomicDistance = NULL;
			m_rgMinAtomicDistances = NULL;
		}

		~Constraints()
		{
			cleanUp();
		}

		bool load(TiXmlElement *pConstraintsElem, const Strings* messages, std::map<std::string,Constraints*> &constraintsMap);
		bool save(TiXmlElement *pConstraintsElem, const Strings* messages);
		
		void copy(Constraints &other);
		
		FLOAT getSmallestMinDistance(); // Returns 0 if there are no specific minimum distances

	private:
		std::map<unsigned int, std::map<unsigned int,FLOAT> > m_mapMinAtomicDistances; // 2D map (similar to m_rgMinAtomicDistances, except this is used for faster writing of the constraints to a file)
		
		static const unsigned int s_minOccurs[];
		
		static const unsigned int s_distMinOccurs[];
		static const unsigned int s_distMaxOccurs[];
		
		static const bool         s_required[];
		static const char*        s_defaultValues[];
		
		static const bool         s_minRequired[];
		static const char*        s_minDefaultValues[];

		bool addMinDist(TiXmlElement *pElem, unsigned int &timesReadGeneralMin, const Strings* messages);
		
		void cleanUp();
		bool specificMinDistNotInBase();
};

#endif
