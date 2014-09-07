
#ifndef __EXTERNAL_ENERGY_XML_H__
#define __EXTERNAL_ENERGY_XML_H__

#include <stdio.h>
#include <string>
#include "../xsd/xsdElementUtil.h"
#include "../xsd/xsdAttributeUtil.h"
#include "../xsd/xsdTypeUtil.h"
#include "externalEnergy.h"

class ExternalEnergyXml {
public:
	bool m_bTransitionStateSearch;
	ExternalEnergy::Impl m_method;
	ExternalEnergy* m_pMethodImpl;
	std::string m_sTemporaryDir;
	std::string m_sResultsDir;
	unsigned int m_iMaxResultsFiles;
	std::string m_sResultsFilePrefix;
	std::string m_sHeader;

	ExternalEnergyXml();
	~ExternalEnergyXml();

	bool load(const rapidxml::xml_node<>* pExternalElem);
	bool save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pExternalElem);

private:
	void clear();
	static const char*        s_attributeNames[];
	static const bool         s_required[];
	static const char*        s_defaultValues[];

	static const char*        s_elementNames[];
	static const unsigned int s_minOccurs[];

	static const char*        s_methods[];
	static const int          s_methodConstants[];

	static const char*        s_resAttributeNames[];
	static const bool         s_resRequired[];
	static const char*        s_resDefaultValues[];

	bool readResultsDir(const rapidxml::xml_node<>* pElem);
};

#endif
