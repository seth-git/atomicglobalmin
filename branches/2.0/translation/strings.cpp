////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing messages in different languages.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "strings.h"

const std::string Strings::s_sDefaultLanguageCode = "en";
std::map<std::string,Strings> Strings::s_instances;

bool Strings::init()
{
	return s_instances[s_sDefaultLanguageCode].init(s_sDefaultLanguageCode.c_str());
}

Strings* Strings::instance()
{
	return &s_instances[s_sDefaultLanguageCode];
}

Strings* Strings::instance(std::string languageCode)
{
	if (!s_instances[languageCode].m_bLoaded)
		if (!s_instances[languageCode].init(languageCode.c_str()))
			return NULL;
	return &s_instances[languageCode];
}

bool Strings::init (const char* languageCode)
{
	char fileName[100];
	sprintf(fileName, "translation/input_%s.properties", languageCode);
	ifstream infile(fileName);
	const int MAX_LINE_LENGTH = 2000;
	char fileLine[MAX_LINE_LENGTH];
	char name[MAX_LINE_LENGTH];
	char str[MAX_LINE_LENGTH];
	std::map<std::string,std::string> stringMap;
	bool duplicates = false;

	m_bLoaded = false;

	if (!infile)
	{
		printf("Can't open the file: %s\n", fileName);
		return false;
	}

	while (infile.getline(fileLine, MAX_LINE_LENGTH)) {
		if (sscanf(fileLine, "input.%s = %[^\t\n]", name, str) == 2) {
			if (stringMap[name].length() > 0) {
				printf("Error: Found two strings with the same key: '%s'.\n", name);
				duplicates = true;
			}
			stringMap[name] = str;
			if (name[0] != 'p' && name[0] != 'x')
				stringMap[name].append("\n");
			if (name[0] == 'x' && strchr(str, ' ') != NULL)
				printf("The xml element or attribute '%s' has a value '%s' which contains spaces.  Please correct this.\n", name, str);
		} else if (sscanf(fileLine, "input.%s", name) == 1) {
			printf("Error reading string with key: '%s'.\n", name);
			return false;
		}
	}
	infile.close();

	const bool bValidate = true;
	if (bValidate) {
		std::map<std::string,std::string> valueMap;
		std::map<std::string,std::string>::iterator iter;   
		for (iter = stringMap.begin(); iter != stringMap.end(); iter++ ) {
			if (valueMap[iter->second].length() > 0 && iter->first.find("Option") == std::string::npos) {
				printf("Error: Found two strings with the same value: '%s'.\n", iter->second.c_str());
				duplicates = true;
			}
			valueMap[iter->second] = iter->first;
		}
		if (duplicates)
			return false;
	}

	m_sxAction = stringMap["xAction"];
	m_sxConstraints = stringMap["xConstraints"];
	m_sxEnergy = stringMap["xEnergy"];
	m_sxResults = stringMap["xResults"];
	m_sxSimulatedAnnealing = stringMap["xSimulatedAnnealing"];
	m_sxRandomSearch = stringMap["xRandomSearch"];
	m_sxParticleSwarmOptimization = stringMap["xParticleSwarmOptimization"];
	m_sxGeneticAlgorithm = stringMap["xGeneticAlgorithm"];
	m_sxBatch = stringMap["xBatch"];
	m_sxInternal = stringMap["xInternal"];
	m_sxExternal = stringMap["xExternal"];
	m_sxCube = stringMap["xCube"];
	m_sxAtomicDistances = stringMap["xAtomicDistances"];
	m_sxMin = stringMap["xMin"];
	m_sxMax = stringMap["xMax"];
	m_sxName = stringMap["xName"];
	m_sxSize = stringMap["xSize"];
	m_sxValue = stringMap["xValue"];
	m_sxZ1 = stringMap["xZ1"];
	m_sxZ2 = stringMap["xZ2"];
	m_sxMethod = stringMap["xMethod"];
	m_sxTransitionStateSearch = stringMap["xTransitionStateSearch"];
	m_sxSharedDirectory = stringMap["xSharedDirectory"];
	m_sxLocalDirectory = stringMap["xLocalDirectory"];
	m_sxResultsDirectory = stringMap["xResultsDirectory"];
	m_sxCharge = stringMap["xCharge"];
	m_sxMultiplicity = stringMap["xMultiplicity"];
	m_sxHeader = stringMap["xHeader"];
	m_sxFooter = stringMap["xFooter"];
	m_sxMpi = stringMap["xMpi"];
	m_sxPath = stringMap["xPath"];
	m_sxMaxFiles = stringMap["xMaxFiles"];
	m_sxFilePrefix = stringMap["xFilePrefix"];
	m_sxMaster = stringMap["xMaster"];
	m_sxOpt = stringMap["xOpt"];
	
	m_spADF = stringMap["pADF"];
	m_spGAMESS = stringMap["pGAMESS"];
	m_spGAMESSUK = stringMap["pGAMESSUK"];
	m_spGaussian = stringMap["pGaussian"];
	m_spFirefly = stringMap["pFirefly"];
	m_spJaguar = stringMap["pJaguar"];
	m_spMolpro = stringMap["pMolpro"];
	m_spORCA = stringMap["pORCA"];
	m_spBest = stringMap["pBest"];
	m_spTrue = stringMap["pTrue"];
	m_spFalse = stringMap["pFalse"];
	m_spLennardJones = stringMap["pLennardJones"];
	
	m_sReadingFile = stringMap["ReadingFile"];
	
	m_sDuplicateAttributes = stringMap["DuplicateAttributes"];
	m_sUnrecognizedAttribute = stringMap["UnrecognizedAttribute"];
	m_sMissingAttribute = stringMap["MissingAttribute"];
	m_sMustNotContainAttributes = stringMap["MustNotContainAttributes"];
	m_sAvailableAttributes = stringMap["AvailableAttributes"];
	
	m_sMaxOf1ElementExceeded = stringMap["MaxOf1ElementExceeded"];
	m_sUnrecognizedElement = stringMap["UnrecognizedElement"];
	m_sElementNumNot1 = stringMap["ElementNumNot1"];
	m_sElementRequiresExactly1Child = stringMap["ElementRequiresExactly1Child"];
	m_sElementRequiresNChildren = stringMap["ElementRequiresNChildren"];
	m_sElementRequires1ChildMin = stringMap["ElementRequires1ChildMin"];
	m_sMisplacedElement = stringMap["MisplacedElement"];
	m_sMaxElementsExceeded = stringMap["MaxElementsExceeded"];
	m_sChoiceError = stringMap["ChoiceError"];
	m_sChoiceElementOrder = stringMap["ChoiceElementOrder"];
	m_spUnlimited = stringMap["pUnlimited"];
	
	m_sUnrecognizedAttributeValue = stringMap["UnrecognizedAttributeValue"];
	m_sUnableToReadInteger = stringMap["UnableToReadInteger"];
	m_sUnableToReadPositiveInteger = stringMap["UnableToReadPositiveInteger"];
	m_sZeroNotAllowed = stringMap["ZeroNotAllowed"];
	m_sUnableToReadPositiveFloat = stringMap["UnableToReadPositiveFloat"];
	m_sNegativeNotAllowed = stringMap["NegativeNotAllowed"];
	m_sUnableToReadElementText = stringMap["UnableToReadElementText"];
	
	m_sError = stringMap["Error"];
	m_sErrorOnLine = stringMap["ErrorOnLine"];
	m_sElementNotFound = stringMap["ElementNotFound"];
	m_sOneRootElement = stringMap["OneRootElement"];
	m_sTwoElementsWithSameName = stringMap["TwoElementsWithSameName"];

	m_sErrorZ1Z2 = stringMap["ErrorZ1Z2"];
	m_sErrorOneGeneralMin = stringMap["ErrorOneGeneralMin"];
	m_sErrorAtomicNumOverMax = stringMap["ErrorAtomicNumOverMax"];
	m_sErrorDuplicateMinDist = stringMap["ErrorDuplicateMinDist"];
	
	m_bLoaded = true;
	return true;
}


