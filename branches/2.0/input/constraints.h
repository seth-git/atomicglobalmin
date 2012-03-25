
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
		
		void cleanUp() {
			unsigned int i;
			if (m_pfCubeLWH != NULL) {
				delete m_pfCubeLWH;
				m_pfCubeLWH = NULL;
			}
			if (m_pfGeneralMinAtomicDistance != NULL) {
				delete m_pfGeneralMinAtomicDistance;
				m_pfGeneralMinAtomicDistance = NULL;
			}
			if (m_pfGeneralMaxAtomicDistance != NULL) {
				delete m_pfGeneralMaxAtomicDistance;
				m_pfGeneralMaxAtomicDistance = NULL;
			}
			if (m_rgMinAtomicDistances != NULL) {
				for (i = 1; i <= MAX_ATOMIC_NUMBERS; ++i) {
					delete[] m_rgMinAtomicDistances[i];
				}
				delete[] m_rgMinAtomicDistances;
				m_rgMinAtomicDistances = NULL;
			}
		}

		bool load(TiXmlElement *pElem);
		void save();
	
	private:
		bool addMinDist(TiXmlElement *pElem);
};

#endif
