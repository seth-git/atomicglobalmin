
#include "externalEnergyXml.h"

const char*      ExternalEnergyXml::s_attributeNames[] = {strings::xMethod, strings::xTransitionStateSearch, strings::xWallTime};
const bool         ExternalEnergyXml::s_required[]     = {true            , false                          , false };
const char*      ExternalEnergyXml::s_defaultValues[]  = {""              , strings::pFalse                , ""};

const char* ExternalEnergyXml::s_elementNames[] = {strings::xTemporaryDirectory, strings::xResultsDirectory, strings::xHeader};
const unsigned int ExternalEnergyXml::s_minOccurs[] = {0                       , 0                         , 1};

ExternalEnergyXml::ExternalEnergyXml() {
}

ExternalEnergyXml::~ExternalEnergyXml() {
	clear();
}

void ExternalEnergyXml::clear() {
	m_sTemporaryDir = "";
	m_sResultsDir = "";
	m_iMaxResultFiles = -1;
	m_sResultsFilePrefix = "";
	m_sHeader = "";
	m_sWallTime = "";
	m_tWallTime = 0;
}

bool ExternalEnergyXml::load(const rapidxml::xml_node<>* pExternalElem)
{
	using namespace rapidxml;
	using namespace strings;
	clear();
	const char** values;
	
	XsdAttributeUtil attUtil(s_attributeNames, s_required, s_defaultValues);
	if (!attUtil.process(pExternalElem))
		return false;
	values = attUtil.getAllAttributes();
	
	if (!ExternalEnergy::getEnum(s_attributeNames[0], values[0], m_method, pExternalElem))
		return false;

	if (!XsdTypeUtil::getBoolValue(s_attributeNames[1], values[1], m_bTransitionStateSearch, pExternalElem))
		return false;

	if (NULL != values[2]) {
		m_sWallTime = values[2];
		unsigned int hours, minutes, seconds;
		if (sscanf(m_sWallTime.c_str(), "%u:%u:%u", &hours, &minutes, &seconds) != 3) {
			printf(WallTimeError, m_sWallTime.c_str());
			return false;
		}
		m_tWallTime = (hours*3600) + (minutes*60) + seconds;
	}

	XsdElementUtil extUtil(XSD_ALL, s_elementNames, s_minOccurs);
	if (!extUtil.process(pExternalElem))
		return false;
	const xml_node<>** extElements = extUtil.getAllElements();
	
	if (extElements[0] != NULL)
		if (!XsdTypeUtil::read1DirAtt(extElements[0], m_sTemporaryDir, xPath, true, NULL))
			return false;
	
	if (extElements[1] != NULL)
		if (!readResultsDir(extElements[1]))
			return false;

	if (m_sTemporaryDir.length() == 0 && m_sResultsDir.length() == 0) {
		printf(strings::ErrorMissingEnergyDir, strings::xExternal, strings::xTemporaryDirectory, strings::xResultsDirectory);
		return false;
	}

	if (!XsdTypeUtil::readElementText(extElements[2], m_sHeader))
		return false;

	return true;
}

const char* ExternalEnergyXml::s_resAttributeNames[] = {strings::xPath, strings::xMaxFiles, strings::xFilePrefix};
const bool  ExternalEnergyXml::s_resRequired[]       = {true          , false             , false};
const char* ExternalEnergyXml::s_resDefaultValues[]  = {""            , "-1"               , strings::pBest};

bool ExternalEnergyXml::readResultsDir(const rapidxml::xml_node<>* pElem) {
	using namespace strings;
	
	if (pElem == NULL) {
		m_sResultsDir == "";
		m_iMaxResultFiles = -1;
		m_sResultsFilePrefix = "";
		return true;
	}
	
	XsdAttributeUtil resultsDirUtil(s_resAttributeNames, s_resRequired, s_resDefaultValues);
	if (!resultsDirUtil.process(pElem))
		return false;
	const char** values = resultsDirUtil.getAllAttributes();

	if (!XsdTypeUtil::checkDirectoryOrFileName(values[0], m_sResultsDir, s_resAttributeNames[0], pElem))
		return false;
	if (!XsdTypeUtil::getInteger(values[1], m_iMaxResultFiles, s_resAttributeNames[1], pElem))
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
	if (m_sWallTime.length() > 0)
		pExternalElem->append_attribute(doc.allocate_attribute(xWallTime, m_sWallTime.c_str(), sizeof(xWallTime)-1, m_sWallTime.length()));
	
	if (m_sTemporaryDir.length() > 0) {
		xml_node<>* temporaryDir = doc.allocate_node(node_element, xTemporaryDirectory, NULL, sizeof(xTemporaryDirectory)-1);
		pExternalElem->append_node(temporaryDir);
		temporaryDir->append_attribute(doc.allocate_attribute(xPath, m_sTemporaryDir.c_str(), sizeof(xPath)-1, m_sTemporaryDir.length()));
	}
	if (m_sResultsDir.length() > 0) {
		xml_node<>* resultsDir = doc.allocate_node(node_element, xResultsDirectory, NULL, sizeof(xResultsDirectory)-1);
		pExternalElem->append_node(resultsDir);
		resultsDir->append_attribute(doc.allocate_attribute(xPath, m_sResultsDir.c_str(), sizeof(xPath)-1, m_sResultsDir.length()));
		XsdTypeUtil::setAttribute(doc, resultsDir, xMaxFiles, m_iMaxResultFiles);
		if (m_sResultsFilePrefix.compare(pBest) != 0)
			resultsDir->append_attribute(doc.allocate_attribute(xFilePrefix, m_sResultsFilePrefix.c_str(), sizeof(xFilePrefix)-1, m_sResultsFilePrefix.length()));
	}
	
	xml_node<>* header = doc.allocate_node(node_element, xHeader, NULL, sizeof(xHeader)-1);
	pExternalElem->append_node(header);
	xml_node<>* text = doc.allocate_node(node_cdata, NULL, m_sHeader.c_str(), 0, m_sHeader.size());
	header->append_node(text);

	return true;
}
