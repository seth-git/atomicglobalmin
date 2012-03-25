
#include "externalEnergy.h"

bool ExternalEnergy::load(TiXmlElement *pExternalElem)
{
	static const std::string attributeNames[] = {"method", "transitionStateSearch"};
	static const bool        required[]       = {true    , false };
	static const std::string defaultValues[]  = {""      , "false"};
	const char** values;
	
	XsdAttributeUtil attUtil(pExternalElem->Value(), attributeNames, 2, required, defaultValues);
	if (!attUtil.process(pExternalElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();

	static const std::string methods[] = {"GAMESS", "GAMESS-UK", "Gaussian"};
	static const int methodConstants[] = {GAMESS, GAMESS_UK, GAUSSIAN};
	if (!XsdTypeUtil::getEnumValue(attributeNames[0].c_str(), values[0], m_iMethod, pExternalElem->Value(), methods, 3, methodConstants)) {
		return false;
	}

	if (!XsdTypeUtil::getBoolValue(attributeNames[1].c_str(), values[1], m_bTransitionStateSearch, pExternalElem->Value())) {
		return false;
	}

	static const std::string elementNames[] = {"sharedDirectory", "localDirectory", "resultsDirectory", "charge", "multiplicity", "header", "footer", "mpi"};
	static const unsigned int   minOccurs[] = {1                , 0               , 0                 , 1       , 1             , 1       , 0       , 0    };
	XsdElementUtil extUtil(pExternalElem->Value(), XSD_ALL, elementNames, 8, minOccurs, NULL);
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

bool ExternalEnergy::readResultsDir(TiXmlElement *pElem) {
	if (pElem == NULL) {
		m_sResultsDir == "";
		m_iMaxResultsFiles = 0;
		m_sResultsFilePrefix = "";
		return true;
	}

	static const std::string attributeNames[] = {"path", "maxFiles", "filePrefix"};
	static const bool        required[]       = {true  , false     , false};
	static const std::string defaultValues[]  = {""    , "1"       , "best"};
	const char** values;
	
	XsdAttributeUtil resultsDirUtil(pElem->Value(), attributeNames, 3, required, defaultValues);
	if (!resultsDirUtil.process(pElem)) {
		return false;
	}
	values = resultsDirUtil.getAllAttributes();
	XsdTypeUtil::checkDirectoryOrFileName(values[0], m_sResultsDir);
	if (!XsdTypeUtil::getPositiveInt(values[1], m_iMaxResultsFiles, attributeNames[1].c_str(), pElem->Value())) {
		return false;
	}
	m_sResultsFilePrefix = values[2];
	return true;
}

bool ExternalEnergy::readMpiMaster(TiXmlElement *pElem) {
	static const std::string attributeNames[] = {"master"};
	static const bool        required[]       = {true};
	static const std::string defaultValues[]  = {""};
	const char** values;

	XsdAttributeUtil util(pElem->Value(), attributeNames, 1, required, defaultValues);
	if (!util.process(pElem)) {
		return false;
	}
	values = util.getAllAttributes();

	if (!XsdTypeUtil::getBoolValue(attributeNames[0].c_str(), values[0], m_bMpiMaster, pElem->Value())) {
		return false;
	}
	return true;
}

void ExternalEnergy::save()
{
}
