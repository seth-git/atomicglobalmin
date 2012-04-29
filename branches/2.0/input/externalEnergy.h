
#ifndef __EXTERNAL_ENERGY_H__
#define __EXTERNAL_ENERGY_H__

#include <stdio.h>
#include <string>
#include "xsdElementUtil.h"
#include "xsdAttributeUtil.h"
#include "xsdTypeUtil.h"

#define GAMESS                              1
#define GAMESS_UK                           2
#define GAUSSIAN                            3

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

		bool load(TiXmlElement *pElem);
		void save();
		
	private:
		static const std::string  s_attributeNames[];
		static const bool         s_required[];
		static const std::string  s_defaultValues[];

		static const std::string  s_elementNames[];
		static const unsigned int s_minOccurs[];

		static const std::string  s_methods[];
		static const int          s_methodConstants[];
		
		static const std::string  s_resAttributeNames[];
		static const bool         s_resRequired[];
		static const std::string  s_resDefaultValues[];

		static const std::string  s_mpiAttributeNames[];
		static const bool         s_mpiRequired[];
		static const std::string  s_mpiDefaultValues[];

		bool readResultsDir(TiXmlElement *pElem);
		bool readMpiMaster(TiXmlElement *pElem);
};

#endif
