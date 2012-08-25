
#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdio.h>
#include <string>
#include "xsdElementUtil.h"
#include "xsdAttributeUtil.h"
#include "internalEnergy.h"
#include "externalEnergy.h"
#include "constraints.h"
#include "../translation/strings.h"

#define SIMULATED_ANNEALING              0
#define RANDOM_SEARCH                    1
#define PARTICLE_SWARM_OPTIMIZATION      2
#define GENETIC_ALGORITHM                3
#define BATCH                            4

class Input {
	public:
		std::string m_sFileName;
		std::string m_sVersion;
		std::string m_sLanguageCode; // 2 characters long
		int m_iAction; // a constant value and an index to s_actionElementNames
		bool m_bExternalEnergy;
		InternalEnergy m_internalEnergy;
		ExternalEnergy m_externalEnergy;
		std::vector<Constraints> m_constraints;
		
		Input()
		{
			m_iAction = -1;
			m_sLanguageCode = Strings::s_sDefaultLanguageCode;
			m_messages = Strings::instance();
		}
		
		~Input()
		{
			cleanUp();
		}
		
		bool load(const char* pFilename);
		bool save(const char* pFilename);
		bool save();
	private:
		const Strings*            m_messages; // messages for m_sLanguageCode
		
		static const char*        s_agml;
		
		static const char*        s_attributeNames[];
		static const bool         s_required[];
		static const char*        s_defaultValues[];
		
		static const unsigned int s_minOccurs[];
		static const unsigned int s_maxOccurs[];
		
		void cleanUp();
};

#endif
