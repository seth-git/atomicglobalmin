
#ifndef __EXTERNAL_ENERGY_H__
#define __EXTERNAL_ENERGY_H__

#include <stdio.h>
#include <string>
#include "xsd/xsdElementUtil.h"
#include "xsd/xsdAttributeUtil.h"
#include "xsd/xsdTypeUtil.h"

class ExternalEnergy {
	public:
		enum Method {ADF, GAMESS, GAMESS_UK, GAUSSIAN, FIREFLY, JAGUAR, MOLPRO, ORCA};
	
		bool m_bTransitionStateSearch;
		Method m_method;
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
		
		bool load(TiXmlElement *pExternalElem, const Strings* messages);
		bool save(TiXmlElement *pExternalElem, const Strings* messages);

		static bool getMethodEnum(const char* attributeName, const char* stringValue, Method& result, TiXmlElement *pElem, const Strings* messages);
		static const char* getMethodString(Method enumValue, const Strings* messages);
		
	private:
		void cleanUp();
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
