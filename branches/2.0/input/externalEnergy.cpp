
#include "externalEnergy.h"

const std::string ExternalEnergy::s_attributeNames[]   = {"method", "transitionStateSearch"};
const bool        ExternalEnergy::s_required[]         = {true    , false };
const std::string ExternalEnergy::s_defaultValues[]    = {""      , "false"};

const std::string  ExternalEnergy::s_elementNames[]    = {"sharedDirectory", "localDirectory", "resultsDirectory", "charge", "multiplicity", "header", "footer", "mpi"};
const unsigned int ExternalEnergy::s_minOccurs[]       = {1                , 0               , 0                 , 1       , 1             , 1       , 0       , 0    };

const std::string  ExternalEnergy::s_methods[]         = {"GAMESS", "GAMESS-UK", "Gaussian"};
const int          ExternalEnergy::s_methodConstants[] = {GAMESS,   GAMESS_UK,   GAUSSIAN};

bool ExternalEnergy::load(TiXmlElement *pExternalElem)
{
	const char** values;
	
	XsdAttributeUtil attUtil(pExternalElem->Value(), s_attributeNames, 2, s_required, s_defaultValues);
	if (!attUtil.process(pExternalElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	if (!XsdTypeUtil::getEnumValue(s_attributeNames[0].c_str(), values[0], m_iMethod, pExternalElem->Value(), s_methods, 3, s_methodConstants)) {
		return false;
	}

	if (!XsdTypeUtil::getBoolValue(s_attributeNames[1].c_str(), values[1], m_bTransitionStateSearch, pExternalElem->Value())) {
		return false;
	}

	XsdElementUtil extUtil(pExternalElem->Value(), XSD_ALL, s_elementNames, 8, s_minOccurs, NULL);
	TiXmlHandle handle(0);
	TiXmlElement** extElements;

	handle=TiXmlHandle(pExternalElem);
	if (!extUtil.process(handle)) {
		return false;
	}
	extElements = extUtil.getAllElements();

	if (!XsdTypeUtil::readDirType(extElements[0], m_sSharedDir)) {
		return false;
	}

	if (extElements[1] == NULL) {
		m_sLocalDir = "";
	} else if (!XsdTypeUtil::readDirType(extElements[1], m_sLocalDir)) {
		return false;
	}

	if (!readResultsDir(extElements[2])) {
		return false;
	}

	if (!XsdTypeUtil::readIntValueElement(extElements[3], m_iCharge)) {
		return false;
	}
	
	if (!XsdTypeUtil::readPosIntValueElement(extElements[4], m_iMultiplicity)) {
		return false;
	}

	if (!XsdTypeUtil::readElementText(extElements[5], m_sHeader)) {
		return false;
	}

	if (extElements[6] != NULL) {
		if (!XsdTypeUtil::readElementText(extElements[6], m_sFooter)) {
			return false;
		}
	} else {
		m_sFooter = "";
	}

	if (extElements[7] != NULL) {
		if (!readMpiMaster(extElements[7])) {
			return false;
		}
	} else {
		m_bMpiMaster = false;
	}
	
	return true;
}

const std::string ExternalEnergy::s_resAttributeNames[] = {"path", "maxFiles", "filePrefix"};
const bool        ExternalEnergy::s_resRequired[]       = {true  , false     , false};
const std::string ExternalEnergy::s_resDefaultValues[]  = {""    , "1"       , "best"};

bool ExternalEnergy::readResultsDir(TiXmlElement *pElem) {
	if (pElem == NULL) {
		m_sResultsDir == "";
		m_iMaxResultsFiles = 0;
		m_sResultsFilePrefix = "";
		return true;
	}

	const char** values;
	
	XsdAttributeUtil resultsDirUtil(pElem->Value(), s_resAttributeNames, 3, s_resRequired, s_resDefaultValues);
	if (!resultsDirUtil.process(pElem)) {
		return false;
	}
	values = resultsDirUtil.getAllAttributes();
	XsdTypeUtil::checkDirectoryOrFileName(values[0], m_sResultsDir);
	if (!XsdTypeUtil::getPositiveInt(values[1], m_iMaxResultsFiles, s_resAttributeNames[1].c_str(), pElem->Value())) {
		return false;
	}
	m_sResultsFilePrefix = values[2];
	return true;
}

const std::string ExternalEnergy::s_mpiAttributeNames[] = {"master"};
const bool        ExternalEnergy::s_mpiRequired[]       = {true};
const std::string ExternalEnergy::s_mpiDefaultValues[]  = {""};

bool ExternalEnergy::readMpiMaster(TiXmlElement *pElem) {
	const char** values;

	XsdAttributeUtil util(pElem->Value(), s_mpiAttributeNames, 1, s_mpiRequired, s_mpiDefaultValues);
	if (!util.process(pElem)) {
		return false;
	}
	values = util.getAllAttributes();

	if (!XsdTypeUtil::getBoolValue(s_mpiAttributeNames[0].c_str(), values[0], m_bMpiMaster, pElem->Value())) {
		return false;
	}
	return true;
}

void ExternalEnergy::save()
{
}
