
#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdio.h>
#include <string>
#include "xsdElementUtil.h"
#include "internalEnergy.h"

#define SIMULATED_ANNEALING              1
#define RANDOM_SEARCH                    2
#define PARTICLE_SWARM_OPTIMIZATION      3
#define GENETIC_ALGORITHM                4
#define BATCH                            5

class Input {
	private:
		TiXmlDocument *m_pXMLDocument;

	public:
		std::string m_pVersion;
		int m_iAction;
		bool m_bExternalEnergy;
		InternalEnergy m_internalEnergy;
		
		Input()
		{
			m_pXMLDocument = NULL;
			m_iAction = -1;
		}

		~Input()
		{
			cleanUp();
		}

		void cleanUp();
		bool load(const char* pFilename);
		void save(const char* pFilename);
};

#endif
