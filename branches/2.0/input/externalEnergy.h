
#ifndef __EXTERNAL_ENERGY_H__
#define __EXTERNAL_ENERGY_H__

#include <stdio.h>
#include <string>
#include "xsdElementUtil.h"
#include "xsdAttributeUtil.h"
#include "xsdTypeUtil.h"

#define ADF                                 1
#define GAMESS                              2
#define GAMESS_UK                           3
#define GAUSSIAN                            4
#define FIREFLY                             5
#define JAGUAR                              6
#define MOLPRO                              7
#define ORCA                                8

class ExternalEnergy {
	public:
		bool m_bTransitionStateSearch;
		int m_iMethod;
		std::string m_sSharedDir;
		std::string m_sLocalDir;
		std::string m_sResultsDir;
		unsigned int m_iMaxResultsFiles;
		std::string m_sResultsFilePrefix;
		int m_iCharge;
		unsigned int m_iMultiplicity;
		std::string m_sHeader;
		std::string m_sFooter;
		bool m_bMpiMaster;

		bool load(TiXmlElement *pElem, const Strings* messages);
		void save(const Strings* messages);
		
	private:
		static const bool         s_required[];

		static const char*        s_elementNames[];
		static const unsigned int s_minOccurs[];

		static const char*        s_methods[];
		static const int          s_methodConstants[];
		
		static const bool         s_resRequired[];

		static const bool         s_mpiRequired[];
		static const char*        s_mpiDefaultValues[];

		bool readResultsDir(TiXmlElement *pElem, const Strings* messages);
		bool readMpiMaster(TiXmlElement *pElem, const Strings* messages);
};

#endif
