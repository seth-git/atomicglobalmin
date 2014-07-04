
#include "externalEnergyXml.h"

//const char*      ExternalEnergyXml::s_attributeNames[]  = {"method", "transitionStateSearch"};
const bool         ExternalEnergyXml::s_required[]        = {true    , false };
//const char*      ExternalEnergyXml::s_defaultValues[]   = {""      , "false"};

//const char*      ExternalEnergyXml::s_elementNames[]    = {"sharedDirectory", "localDirectory", "resultsDirectory", "charge", "multiplicity", "header", "footer"};
const unsigned int ExternalEnergyXml::s_minOccurs[]       = {1                , 0               , 0                 , 1       , 1             , 1       , 0       };

ExternalEnergyXml::ExternalEnergyXml() {
}

ExternalEnergyXml::~ExternalEnergyXml() {
	clear();
}

void ExternalEnergyXml::clear() {
	m_sSharedDir = "";
	m_sLocalDir = "";
	m_sResultsDir = "";
	m_iMaxResultsFiles = 0;
	m_sResultsFilePrefix = "";
	m_sHeader = "";
	m_sFooter = "";
}

bool ExternalEnergyXml::load(const rapidxml::xml_node<>* pExternalElem, const Strings* messages)
{
	using namespace rapidxml;
	clear();
	const char** values;
	
	const char* attributeNames[] = {messages->m_sxMethod.c_str(), messages->m_sxTransitionStateSearch.c_str()};
	const char* defaultValues[]   = {"", messages->m_spFalse.c_str()};
	const char* elementNames[] = {messages->m_sxSharedDirectory.c_str(), messages->m_sxLocalDirectory.c_str(),
			messages->m_sxResultsDirectory.c_str(), messages->m_sxCharge.c_str(), messages->m_sxMultiplicity.c_str(),
			messages->m_sxHeader.c_str(), messages->m_sxFooter.c_str()};
	
	XsdAttributeUtil attUtil(attributeNames, s_required, defaultValues);
	if (!attUtil.process(pExternalElem))
		return false;
	values = attUtil.getAllAttributes();
	
	if (!ExternalEnergy::getEnum(attributeNames[0], values[0], m_method, pExternalElem, messages))
		return false;

	if (!XsdTypeUtil::getBoolValue(attributeNames[1], values[1], m_bTransitionStateSearch, pExternalElem, messages))
		return false;
	
	XsdElementUtil extUtil(XSD_ALL, elementNames, s_minOccurs);
	if (!extUtil.process(pExternalElem))
		return false;
	const xml_node<>** extElements = extUtil.getAllElements();
	
	if (!XsdTypeUtil::read1DirAtt(extElements[0], m_sSharedDir, messages->m_sxPath.c_str(), true, NULL))
		return false;
	
	if (extElements[1] != NULL)
		if (!XsdTypeUtil::read1DirAtt(extElements[1], m_sLocalDir, messages->m_sxPath.c_str(), true, NULL))
			return false;
	
	if (extElements[2] != NULL)
		if (!readResultsDir(extElements[2], messages))
			return false;
	
	if (!XsdTypeUtil::read1IntAtt(extElements[3], m_iCharge, messages->m_sxValue.c_str(), true, NULL))
		return false;
	
	if (!XsdTypeUtil::read1PosIntAtt(extElements[4], m_iMultiplicity, messages->m_sxValue.c_str(), true, NULL))
		return false;

	if (!XsdTypeUtil::readElementText(extElements[5], m_sHeader))
		return false;

	if (extElements[6] != NULL) {
		if (!XsdTypeUtil::readElementText(extElements[6], m_sFooter))
			return false;
	} else {
		m_sFooter = "";
	}

	return true;
}

//const char* ExternalEnergyXml::s_resAttributeNames[] = {"path", "maxFiles", "filePrefix"};
const bool    ExternalEnergyXml::s_resRequired[]       = {true  , false     , false};
//const char* ExternalEnergyXml::s_resDefaultValues[]  = {""    , "1"       , "best"};

bool ExternalEnergyXml::readResultsDir(const rapidxml::xml_node<>* pElem, const Strings* messages) {
	const char* resAttributeNames[] = {messages->m_sxPath.c_str(), messages->m_sxMaxFiles.c_str(), messages->m_sxFilePrefix.c_str()};
	const char* resDefaultValues[]  = {""                        , "1"                           , messages->m_spBest.c_str()};
	
	if (pElem == NULL) {
		m_sResultsDir == "";
		m_iMaxResultsFiles = 0;
		m_sResultsFilePrefix = "";
		return true;
	}

	const char** values;
	
	XsdAttributeUtil resultsDirUtil(resAttributeNames, s_resRequired, resDefaultValues);
	if (!resultsDirUtil.process(pElem))
		return false;
	values = resultsDirUtil.getAllAttributes();

	if (!XsdTypeUtil::checkDirectoryOrFileName(values[0], m_sResultsDir, resAttributeNames[0], pElem))
		return false;
	if (!XsdTypeUtil::getPositiveInt(values[1], m_iMaxResultsFiles, resAttributeNames[1], pElem))
		return false;
	m_sResultsFilePrefix = values[2];
	return true;
}

bool ExternalEnergyXml::save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pExternalElem, const Strings* messages)
{
	using namespace rapidxml;
	pExternalElem->append_attribute(doc.allocate_attribute(messages->m_sxMethod.c_str(), ExternalEnergy::getEnumString(m_method, messages)));
	if (m_bTransitionStateSearch)
		pExternalElem->append_attribute(doc.allocate_attribute(messages->m_sxTransitionStateSearch.c_str(), messages->getTrueFalseParam(m_bTransitionStateSearch)));
	
	xml_node<>* sharedDir = doc.allocate_node(node_element, messages->m_sxSharedDirectory.c_str());
	pExternalElem->append_node(sharedDir);
	sharedDir->append_attribute(doc.allocate_attribute(messages->m_sxPath.c_str(), m_sSharedDir.c_str()));
	
	if (m_sLocalDir.length() > 0) {
		xml_node<>* localDir = doc.allocate_node(node_element, messages->m_sxLocalDirectory.c_str());
		pExternalElem->append_node(localDir);
		localDir->append_attribute(doc.allocate_attribute(messages->m_sxPath.c_str(), m_sLocalDir.c_str()));
	}
	
	if (m_sResultsDir.length() > 0) {
		xml_node<>* resultsDir = doc.allocate_node(node_element, messages->m_sxResultsDirectory.c_str());
		pExternalElem->append_node(resultsDir);
		resultsDir->append_attribute(doc.allocate_attribute(messages->m_sxPath.c_str(), m_sResultsDir.c_str()));
		if (m_iMaxResultsFiles != 1)
			XsdTypeUtil::setAttribute(doc, resultsDir, messages->m_sxMaxFiles.c_str(), m_iMaxResultsFiles);
		if (m_sResultsFilePrefix != messages->m_spBest)
			resultsDir->append_attribute(doc.allocate_attribute(messages->m_sxFilePrefix.c_str(), m_sResultsFilePrefix.c_str()));
	}
	
	xml_node<>* charge = doc.allocate_node(node_element, messages->m_sxCharge.c_str());
	pExternalElem->append_node(charge);
	XsdTypeUtil::setAttribute(doc, charge, messages->m_sxValue.c_str(), m_iCharge);
	
	xml_node<>* multiplicity = doc.allocate_node(node_element, messages->m_sxMultiplicity.c_str());
	pExternalElem->append_node(multiplicity);
	XsdTypeUtil::setAttribute(doc, multiplicity, messages->m_sxValue.c_str(), m_iMultiplicity);
	
	xml_node<>* header = doc.allocate_node(node_element, messages->m_sxHeader.c_str());
	pExternalElem->append_node(header);
	xml_node<>* text = doc.allocate_node(node_cdata, NULL, m_sHeader.c_str(), 0, m_sHeader.size());
	header->append_node(text);
	
	if (m_sFooter.length() > 0) {
		xml_node<>* footer = doc.allocate_node(node_element, messages->m_sxFooter.c_str());
		pExternalElem->append_node(footer);
		text = doc.allocate_node(node_cdata, NULL, m_sFooter.c_str(), 0, m_sFooter.size());
		footer->append_node(text);
	}

	return true;
}
