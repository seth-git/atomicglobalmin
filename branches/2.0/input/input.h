
#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdio.h>
#include <string>
#include "xsdElementUtil.h"
#include "xsdAttributeUtil.h"
#include "internalEnergy.h"
#include "externalEnergy.h"
#include "constraints.h"

#define SIMULATED_ANNEALING              0
#define RANDOM_SEARCH                    1
#define PARTICLE_SWARM_OPTIMIZATION      2
#define GENETIC_ALGORITHM                3
#define BATCH                            4

class Input {
	public:
		std::string m_sVersion;
		int m_iAction; // a constant value and an index to s_actionElementNames
		bool m_bExternalEnergy;
		InternalEnergy m_internalEnergy;
		ExternalEnergy m_externalEnergy;
		vector<Constraints> m_constraints;
		
		Input()
		{
			m_pXMLDocument = NULL;
			m_iAction = -1;
		}

		~Input()
		{
			cleanUp();
		}

		bool load(const char* pFilename);
		void save(const char* pFilename);
	private:
		TiXmlDocument*            m_pXMLDocument;

		static const std::string  s_agml;
		
		static const std::string  s_attributeNames[];
		static const bool         s_required[];
		static const std::string  s_defaultValues[];
		
		static const std::string  s_elementNames[];
		static const unsigned int s_minOccurs[];
		static const unsigned int s_maxOccurs[];

		static const std::string  s_actionElementNames[];
		static const std::string  s_energyElementNames[];

		void cleanUp();
};

#endif
