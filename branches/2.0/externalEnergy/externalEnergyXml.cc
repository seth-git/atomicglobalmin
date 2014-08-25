
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

bool ExternalEnergyXml::load(const rapidxml::xml_node<>* pExternalElem)
{
	using namespace rapidxml;
	using namespace strings;
	clear();
	const char** values;
	
	const char* attributeNames[] = {xMethod, xTransitionStateSearch};
	const char* defaultValues[]   = {"", pFalse};
	const char* elementNames[] = {xSharedDirectory, xLocalDirectory,
			xResultsDirectory, xCharge, xMultiplicity,
			xHeader, xFooter};
	
	XsdAttributeUtil attUtil(attributeNames, s_required, defaultValues);
	if (!attUtil.process(pExternalElem))
		return false;
	values = attUtil.getAllAttributes();
	
	if (!ExternalEnergy::getEnum(attributeNames[0], values[0], m_method, pExternalElem))
		return false;

	if (!XsdTypeUtil::getBoolValue(attributeNames[1], values[1], m_bTransitionStateSearch, pExternalElem))
		return false;
	
	XsdElementUtil extUtil(XSD_ALL, elementNames, s_minOccurs);
	if (!extUtil.process(pExternalElem))
		return false;
	const xml_node<>** extElements = extUtil.getAllElements();
	
	if (!XsdTypeUtil::read1DirAtt(extElements[0], m_sSharedDir, xPath, true, NULL))
		return false;
	
	if (extElements[1] != NULL)
		if (!XsdTypeUtil::read1DirAtt(extElements[1], m_sLocalDir, xPath, true, NULL))
			return false;
	
	if (extElements[2] != NULL)
		if (!readResultsDir(extElements[2]))
			return false;
	
	if (!XsdTypeUtil::read1IntAtt(extElements[3], m_iCharge, xValue, true, NULL))
		return false;
	
	if (!XsdTypeUtil::read1PosIntAtt(extElements[4], m_iMultiplicity, xValue, true, NULL))
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

bool ExternalEnergyXml::readResultsDir(const rapidxml::xml_node<>* pElem) {
	using namespace strings;
	const char* resAttributeNames[] = {xPath, xMaxFiles, xFilePrefix};
	const char* resDefaultValues[]  = {"", "1", pBest};
	
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

bool ExternalEnergyXml::save(rapidxml::xml_document<> &doc, rapidxml::xml_node<>* pExternalElem)
{
	using namespace rapidxml;
	using namespace strings;
	pExternalElem->append_attribute(doc.allocate_attribute(xMethod, ExternalEnergy::getEnumString(m_method)));
	if (m_bTransitionStateSearch)
		pExternalElem->append_attribute(doc.allocate_attribute(xTransitionStateSearch, XsdTypeUtil::getTrueFalseParam(m_bTransitionStateSearch)));
	
	xml_node<>* sharedDir = doc.allocate_node(node_element, xSharedDirectory);
	pExternalElem->append_node(sharedDir);
	sharedDir->append_attribute(doc.allocate_attribute(xPath, m_sSharedDir.c_str()));
	
	if (m_sLocalDir.length() > 0) {
		xml_node<>* localDir = doc.allocate_node(node_element, xLocalDirectory);
		pExternalElem->append_node(localDir);
		localDir->append_attribute(doc.allocate_attribute(xPath, m_sLocalDir.c_str()));
	}
	
	if (m_sResultsDir.length() > 0) {
		xml_node<>* resultsDir = doc.allocate_node(node_element, xResultsDirectory);
		pExternalElem->append_node(resultsDir);
		resultsDir->append_attribute(doc.allocate_attribute(xPath, m_sResultsDir.c_str()));
		if (m_iMaxResultsFiles != 1)
			XsdTypeUtil::setAttribute(doc, resultsDir, xMaxFiles, m_iMaxResultsFiles);
		if (m_sResultsFilePrefix.compare(pBest) != 0)
			resultsDir->append_attribute(doc.allocate_attribute(xFilePrefix, m_sResultsFilePrefix.c_str()));
	}
	
	xml_node<>* charge = doc.allocate_node(node_element, xCharge);
	pExternalElem->append_node(charge);
	XsdTypeUtil::setAttribute(doc, charge, xValue, m_iCharge);
	
	xml_node<>* multiplicity = doc.allocate_node(node_element, xMultiplicity);
	pExternalElem->append_node(multiplicity);
	XsdTypeUtil::setAttribute(doc, multiplicity, xValue, m_iMultiplicity);
	
	xml_node<>* header = doc.allocate_node(node_element, xHeader);
	pExternalElem->append_node(header);
	xml_node<>* text = doc.allocate_node(node_cdata, NULL, m_sHeader.c_str(), 0, m_sHeader.size());
	header->append_node(text);
	
	if (m_sFooter.length() > 0) {
		xml_node<>* footer = doc.allocate_node(node_element, xFooter);
		pExternalElem->append_node(footer);
		text = doc.allocate_node(node_cdata, NULL, m_sFooter.c_str(), 0, m_sFooter.size());
		footer->append_node(text);
	}

	return true;
}
