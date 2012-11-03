
#include "externalEnergy.h"

//const char*      ExternalEnergy::s_attributeNames[]  = {"method", "transitionStateSearch"};
const bool         ExternalEnergy::s_required[]        = {true    , false };
//const char*      ExternalEnergy::s_defaultValues[]   = {""      , "false"};

//const char*      ExternalEnergy::s_elementNames[]    = {"sharedDirectory", "localDirectory", "resultsDirectory", "charge", "multiplicity", "header", "footer", "mpi"};
const unsigned int ExternalEnergy::s_minOccurs[]       = {1                , 0               , 0                 , 1       , 1             , 1       , 0       , 0    };

bool ExternalEnergy::load(TiXmlElement *pExternalElem, const Strings* messages)
{
	const char** values;
	
	const char* attributeNames[] = {messages->m_sxMethod.c_str(), messages->m_sxTransitionStateSearch.c_str()};
	const char* defaultValues[]   = {"", messages->m_spFalse.c_str()};
	const char* elementNames[] = {messages->m_sxSharedDirectory.c_str(), messages->m_sxLocalDirectory.c_str(),
			messages->m_sxResultsDirectory.c_str(), messages->m_sxCharge.c_str(), messages->m_sxMultiplicity.c_str(),
			messages->m_sxHeader.c_str(), messages->m_sxFooter.c_str(), messages->m_sxMpi.c_str()};
	
	XsdAttributeUtil attUtil(pExternalElem->Value(), attributeNames, s_required, defaultValues);
	if (!attUtil.process(pExternalElem)) {
		return false;
	}
	values = attUtil.getAllAttributes();
	
	if (!getMethodEnum(attributeNames[0], values[0], m_method, pExternalElem, messages)) {
		return false;
	}
	
	if (!XsdTypeUtil::getBoolValue(attributeNames[1], values[1], m_bTransitionStateSearch, pExternalElem, messages)) {
		return false;
	}
	
	XsdElementUtil extUtil(pExternalElem->Value(), XSD_ALL, elementNames, s_minOccurs);
	TiXmlHandle handle(0);
	TiXmlElement** extElements;
	
	handle=TiXmlHandle(pExternalElem);
	if (!extUtil.process(handle)) {
		return false;
	}
	extElements = extUtil.getAllElements();
	
	if (!XsdTypeUtil::readDirType(extElements[0], m_sSharedDir, messages)) {
		return false;
	}
	
	if (extElements[1] == NULL) {
		m_sLocalDir = "";
	} else if (!XsdTypeUtil::readDirType(extElements[1], m_sLocalDir, messages)) {
		return false;
	}
	
	if (!readResultsDir(extElements[2], messages)) {
		return false;
	}
	
	if (!XsdTypeUtil::readIntValueElement(extElements[3], m_iCharge, messages)) {
		return false;
	}
	
	if (!XsdTypeUtil::readPosIntValueElement(extElements[4], m_iMultiplicity, messages)) {
		return false;
	}

	if (!XsdTypeUtil::readElementText(extElements[5], m_sHeader)) {
		return false;
	}
	m_sHeader = Strings::trim(m_sHeader);

	if (extElements[6] != NULL) {
		if (!XsdTypeUtil::readElementText(extElements[6], m_sFooter)) {
			return false;
		}
		m_sFooter = Strings::trim(m_sFooter);
	} else {
		m_sFooter = "";
	}

	if (extElements[7] != NULL) {
		if (!readMpiMaster(extElements[7], messages)) {
			return false;
		}
	} else {
		m_bMpiMaster = false;
	}
	
	return true;
}

bool ExternalEnergy::getMethodEnum(const char* attributeName, const char* stringValue, Method& result, TiXmlElement *pElem, const Strings* messages) {
	const char* methods[] = {messages->m_spADF.c_str(), messages->m_spGAMESS.c_str(), messages->m_spGAMESSUK.c_str(), messages->m_spGaussian.c_str(),
			messages->m_spFirefly.c_str(), messages->m_spJaguar.c_str(), messages->m_spMolpro.c_str(), messages->m_spORCA.c_str()};
	return XsdTypeUtil::getEnumValue(attributeName, stringValue, result, pElem, methods);
}

//const char* ExternalEnergy::s_resAttributeNames[] = {"path", "maxFiles", "filePrefix"};
const bool    ExternalEnergy::s_resRequired[]       = {true  , false     , false};
//const char* ExternalEnergy::s_resDefaultValues[]  = {""    , "1"       , "best"};

bool ExternalEnergy::readResultsDir(TiXmlElement *pElem, const Strings* messages) {
	const char* resAttributeNames[] = {messages->m_sxPath.c_str(), messages->m_sxMaxFiles.c_str(), messages->m_sxFilePrefix.c_str()};
	const char* resDefaultValues[]  = {""                        , "1"                           , messages->m_spBest.c_str()};
	
	if (pElem == NULL) {
		m_sResultsDir == "";
		m_iMaxResultsFiles = 0;
		m_sResultsFilePrefix = "";
		return true;
	}

	const char** values;
	
	XsdAttributeUtil resultsDirUtil(pElem->Value(), resAttributeNames, s_resRequired, resDefaultValues);
	if (!resultsDirUtil.process(pElem)) {
		return false;
	}
	values = resultsDirUtil.getAllAttributes();
	XsdTypeUtil::checkDirectoryOrFileName(values[0], m_sResultsDir);
	if (!XsdTypeUtil::getPositiveInt(values[1], m_iMaxResultsFiles, resAttributeNames[1], pElem)) {
		return false;
	}
	m_sResultsFilePrefix = values[2];
	return true;
}

//const char* ExternalEnergy::s_mpiAttributeNames[] = {"master"};
const bool    ExternalEnergy::s_mpiRequired[]       = {true};
const char*   ExternalEnergy::s_mpiDefaultValues[]  = {""};

bool ExternalEnergy::readMpiMaster(TiXmlElement *pElem, const Strings* messages) {
	const char* mpiAttributeNames[] = {messages->m_sxMaster.c_str()};
	const char** values;
	
	XsdAttributeUtil util(pElem->Value(), mpiAttributeNames, s_mpiRequired, s_mpiDefaultValues);
	if (!util.process(pElem)) {
		return false;
	}
	values = util.getAllAttributes();

	if (!XsdTypeUtil::getBoolValue(mpiAttributeNames[0], values[0], m_bMpiMaster, pElem, messages)) {
		return false;
	}
	return true;
}

const char* ExternalEnergy::getMethodString(Method enumValue, const Strings* messages) {
	const char* methods[] = {messages->m_spADF.c_str(), messages->m_spGAMESS.c_str(), messages->m_spGAMESSUK.c_str(), messages->m_spGaussian.c_str(),
			messages->m_spFirefly.c_str(), messages->m_spJaguar.c_str(), messages->m_spMolpro.c_str(), messages->m_spORCA.c_str()};
	return methods[enumValue];
}

bool ExternalEnergy::save(TiXmlElement *pExternalElem, const Strings* messages)
{
	pExternalElem->SetAttribute(messages->m_sxMethod.c_str(), getMethodString(m_method, messages));
	if (m_bTransitionStateSearch)
		pExternalElem->SetAttribute(messages->m_sxTransitionStateSearch.c_str(), messages->getTrueFalseParam(m_bTransitionStateSearch));
	
	TiXmlElement* sharedDir = new TiXmlElement(messages->m_sxSharedDirectory.c_str());  
	pExternalElem->LinkEndChild(sharedDir);
	sharedDir->SetAttribute(messages->m_sxPath.c_str(), m_sSharedDir.c_str());
	
	if (m_sLocalDir.length() > 0) {
		TiXmlElement* localDir = new TiXmlElement(messages->m_sxLocalDirectory.c_str());  
		pExternalElem->LinkEndChild(localDir);
		localDir->SetAttribute(messages->m_sxPath.c_str(), m_sLocalDir.c_str());
	}
	
	if (m_sResultsDir.length() > 0) {
		TiXmlElement* resultsDir = new TiXmlElement(messages->m_sxResultsDirectory.c_str());  
		pExternalElem->LinkEndChild(resultsDir);
		resultsDir->SetAttribute(messages->m_sxPath.c_str(), m_sResultsDir.c_str());
		if (m_iMaxResultsFiles != 1)
			resultsDir->SetAttribute(messages->m_sxMaxFiles.c_str(), m_iMaxResultsFiles);
		if (m_sResultsFilePrefix != messages->m_spBest)
			resultsDir->SetAttribute(messages->m_sxFilePrefix.c_str(), m_sResultsFilePrefix.c_str());
	}
	
	TiXmlElement* charge = new TiXmlElement(messages->m_sxCharge.c_str());  
	pExternalElem->LinkEndChild(charge);
	charge->SetAttribute(messages->m_sxValue.c_str(), m_iCharge);
	
	TiXmlElement* multiplicity = new TiXmlElement(messages->m_sxMultiplicity.c_str());  
	pExternalElem->LinkEndChild(multiplicity);
	multiplicity->SetAttribute(messages->m_sxValue.c_str(), m_iMultiplicity);
	
	TiXmlElement* header = new TiXmlElement(messages->m_sxHeader.c_str());
	TiXmlText* text = new TiXmlText(m_sHeader.c_str());
	text->SetCDATA(true); // helps protect formatting
	header->LinkEndChild(text);
	pExternalElem->LinkEndChild(header);
	
	if (m_sFooter.length() > 0) {
		TiXmlElement* footer = new TiXmlElement(messages->m_sxFooter.c_str());
		TiXmlText* text = new TiXmlText(m_sFooter.c_str());
		text->SetCDATA(true); // helps protect formatting
		footer->LinkEndChild(text);
		pExternalElem->LinkEndChild(footer);
	}
	
	if (m_bMpiMaster) {
		TiXmlElement* mpi = new TiXmlElement(messages->m_sxMpi.c_str());
		mpi->SetAttribute(messages->m_sxMaster.c_str(), messages->getTrueFalseParam(m_bMpiMaster));
		pExternalElem->LinkEndChild(mpi);
	}
	return true;
}
