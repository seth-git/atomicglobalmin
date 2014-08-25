
#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdio.h>
#include <string>
#include "action.h"
#include "simulatedAnnealing/simulatedAnnealing.h"
#include "batch.h"

#define SIMULATED_ANNEALING              0
#define RANDOM_SEARCH                    1
#define PARTICLE_SWARM_OPTIMIZATION      2
#define GENETIC_ALGORITHM                3
#define BATCH                            4

class Input {
	public:
		std::string m_sFileName;
		std::string m_sVersion;
		int m_iAction; // SIMULATED_ANNEALING, RANDOM_SEARCH, PARTICLE_SWARM_OPTIMIZATION, etc.
		Action *m_pAction;
		
		Input();
		~Input();
		
		bool load(const char* pFilename);
		bool loadStr(char* xml);
		bool load(rapidxml::xml_document<> &doc);

		bool save(rapidxml::xml_document<> &doc);
		bool save(const char* pFilename);
		bool save();
		bool save(std::string &buffer);
		const char* getXml();
		bool run(const char* fileName);
	private:
		static const char*        s_agml;
		
		static const char*        s_attributeNames[];
		static const bool         s_required[];
		static const char*        s_defaultValues[];
		
		static const char* s_actionElementNames[];

		void clear();
};

#endif

