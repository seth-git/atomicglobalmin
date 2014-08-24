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
	char szKey[MAX_LINE_LENGTH];
	char szValue[MAX_LINE_LENGTH];
	std::string key, value;
	std::map<std::string,std::string> stringMap;
	bool error = false;
	unsigned int line = 0;

	m_bLoaded = false;

	if (!infile)
	{
		printf("Can't open the file: %s\n", fileName);
		return false;
	}

	while (infile.getline(fileLine, MAX_LINE_LENGTH)) {
		++line;
		if (sscanf(fileLine, "input.%[^=]=%[^\n]", szKey, szValue) == 2) {
			key = trim(szKey);
			value = trim(szValue);
			if (stringMap[key].length() > 0) {
				printf("Error: Found two strings with the same key: '%s'.\n", key.c_str());
				error = true;
			}
			stringMap[key] = value;
			if (key.c_str()[0] != 'p' && key.c_str()[0] != 'x')
				stringMap[key].append("\n");
			if (key.c_str()[0] == 'x' && strchr(value.c_str(), ' ') != NULL) {
				printf("Error: the xml element or attribute '%s' has a value '%s' which contains spaces on line %u.  Please correct this.\n", key.c_str(), value.c_str(), line);
				error = true;
			}
			if (key.length() == 0) {
				printf("Error: found a string with a blank key '%s' on line %u.\n", key.c_str(), line);
				error = true;
			}
			if (value.length() == 0) {
				printf("Error: found a blank string value on line %u having key '%s'.\n", line, key.c_str());
				error = true;
			}
		} else if (sscanf(fileLine, "input.%s", szKey) == 1) {
			printf("Error reading string with key: '%s' on line %u.\n", szKey, line);
			error = true;
		}
	}
	infile.close();

	std::map<std::string,std::string> valueMap;
	std::map<std::string,std::string>::iterator iter;   
	for (iter = stringMap.begin(); iter != stringMap.end(); iter++ ) {
		if (valueMap[iter->second].length() > 0 && iter->first.find("Option") == std::string::npos) {
			printf("Error: Found two strings with the same value: '%s'.  String keys: '%s' and '%s'.\n", iter->second.c_str(), valueMap[iter->second].c_str(), iter->first.c_str());
			error = true;
		}
		valueMap[iter->second] = iter->first;
	}
	if (error)
		return false;
	
	if (!copy(stringMap, "xSimulatedAnnealing", m_sxSimulatedAnnealing))
		return false;
	if (!copy(stringMap, "xRandomSearch", m_sxRandomSearch))
		return false;
	if (!copy(stringMap, "xParticleSwarmOptimization", m_sxParticleSwarmOptimization))
		return false;
	if (!copy(stringMap, "xGeneticAlgorithm", m_sxGeneticAlgorithm))
		return false;
	if (!copy(stringMap, "xBatch", m_sxBatch))
		return false;
	if (!copy(stringMap, "xSetup", m_sxSetup))
		return false;
	if (!copy(stringMap, "xConstraints", m_sxConstraints))
		return false;
	if (!copy(stringMap, "xEnergy", m_sxEnergy))
		return false;
	if (!copy(stringMap, "xResume", m_sxResume))
		return false;
	if (!copy(stringMap, "xResults", m_sxResults))
		return false;
	if (!copy(stringMap, "xInternal", m_sxInternal))
		return false;
	if (!copy(stringMap, "xExternal", m_sxExternal))
		return false;
	if (!copy(stringMap, "xCube", m_sxCube))
		return false;
	if (!copy(stringMap, "xAtomicDistances", m_sxAtomicDistances))
		return false;
	if (!copy(stringMap, "xMin", m_sxMin))
		return false;
	if (!copy(stringMap, "xMax", m_sxMax))
		return false;
	if (!copy(stringMap, "xName", m_sxName))
		return false;
	if (!copy(stringMap, "xBase", m_sxBase))
		return false;
	if (!copy(stringMap, "xSize", m_sxSize))
		return false;
	if (!copy(stringMap, "xValue", m_sxValue))
		return false;
	if (!copy(stringMap, "xZ1", m_sxZ1))
		return false;
	if (!copy(stringMap, "xZ2", m_sxZ2))
		return false;
	if (!copy(stringMap, "xMethod", m_sxMethod))
		return false;
	if (!copy(stringMap, "xTransitionStateSearch", m_sxTransitionStateSearch))
		return false;
	if (!copy(stringMap, "xSharedDirectory", m_sxSharedDirectory))
		return false;
	if (!copy(stringMap, "xLocalDirectory", m_sxLocalDirectory))
		return false;
	if (!copy(stringMap, "xResultsDirectory", m_sxResultsDirectory))
		return false;
	if (!copy(stringMap, "xCharge", m_sxCharge))
		return false;
	if (!copy(stringMap, "xMultiplicity", m_sxMultiplicity))
		return false;
	if (!copy(stringMap, "xHeader", m_sxHeader))
		return false;
	if (!copy(stringMap, "xFooter", m_sxFooter))
		return false;
	if (!copy(stringMap, "xPath", m_sxPath))
		return false;
	if (!copy(stringMap, "xMaxFiles", m_sxMaxFiles))
		return false;
	if (!copy(stringMap, "xFilePrefix", m_sxFilePrefix))
		return false;
	if (!copy(stringMap, "xMaster", m_sxMaster))
		return false;
	if (!copy(stringMap, "xOpt", m_sxOpt))
		return false;
	
	if (!copy(stringMap, "xStructuresTemplate", m_sxStructuresTemplate))
		return false;
	if (!copy(stringMap, "xTemperature", m_sxTemperature))
		return false;
	if (!copy(stringMap, "xAnnealingSchedule", m_sxAnnealingSchedule))
		return false;
	if (!copy(stringMap, "xPerturbations", m_sxPerturbations))
		return false;
	if (!copy(stringMap, "xStop", m_sxStop))
		return false;

	if (!copy(stringMap, "xDecreaseAfterIteration", m_sxDecreaseAfterIteration))
		return false;
	if (!copy(stringMap, "xMaxTemperature", m_sxMaxTemperature))
		return false;
	if (!copy(stringMap, "xMaxAcceptedPerturbations", m_sxMaxAcceptedPerturbations))
		return false;
	if (!copy(stringMap, "xMinIterations", m_sxMinIterations))
		return false;
	if (!copy(stringMap, "xSaveFrequency", m_sxSaveFrequency))
		return false;
	if (!copy(stringMap, "xQueueSize", m_sxQueueSize))
		return false;
	
	if (!copy(stringMap, "xStructureTemplate", m_sxStructureTemplate))
		return false;
	if (!copy(stringMap, "xLinear", m_sxLinear))
		return false;
	if (!copy(stringMap, "xPlanar", m_sxPlanar))
		return false;
	if (!copy(stringMap, "xThreeD", m_sxThreeD))
		return false;
	if (!copy(stringMap, "xBondRotationalSearch", m_sxBondRotationalSearch))
		return false;
	if (!copy(stringMap, "xSeed", m_sxSeed))
		return false;
	
	if (!copy(stringMap, "xMoleculeTemplate", m_sxMoleculeTemplate))
		return false;
	if (!copy(stringMap, "xAtomTemplate", m_sxAtomTemplate))
		return false;
	if (!copy(stringMap, "xNumber", m_sxNumber))
		return false;
	if (!copy(stringMap, "xFormat", m_sxFormat))
		return false;
	if (!copy(stringMap, "xDegrees", m_sxDegrees))
		return false;
	if (!copy(stringMap, "xType", m_sxType))
		return false;
	
	if (!copy(stringMap, "xAgmlFile", m_sxAgmlFile))
		return false;
	if (!copy(stringMap, "xDirectory", m_sxDirectory))
		return false;
	if (!copy(stringMap, "xEnergyFile", m_sxEnergyFile))
		return false;
	if (!copy(stringMap, "xFreezingIterations", m_sxFreezingIterations))
		return false;
	
	if (!copy(stringMap, "xSource", m_sxSource))
		return false;
	if (!copy(stringMap, "xPopulation", m_sxPopulation))
		return false;
	
	if (!copy(stringMap, "xKelvin", m_sxKelvin))
		return false;
	if (!copy(stringMap, "xAcceptedPerturbations", m_sxAcceptedPerturbations))
		return false;
	if (!copy(stringMap, "xPercent", m_sxPercent))
		return false;

	if (!copy(stringMap, "xBoltzmannConstant", m_sxBoltzmannConstant))
		return false;
	if (!copy(stringMap, "xQuenchingFactor", m_sxQuenchingFactor))
		return false;

	if (!copy(stringMap, "xTranslationVector", m_sxTranslationVector))
		return false;
	if (!copy(stringMap, "xRotationAngle", m_sxRotationAngle))
		return false;
	if (!copy(stringMap, "xRadians", m_sxRadians))
		return false;
	if (!copy(stringMap, "xStartLength", m_sxStartLength))
		return false;
	if (!copy(stringMap, "xMinLength", m_sxMinLength))
		return false;
	if (!copy(stringMap, "xProbability", m_sxProbability))
		return false;
	if (!copy(stringMap, "xStartDegrees", m_sxStartDegrees))
		return false;
	if (!copy(stringMap, "xMinDegrees", m_sxMinDegrees))
		return false;
	if (!copy(stringMap, "xNumberPerIteration", m_sxNumberPerIteration))
		return false;
	if (!copy(stringMap, "xPerturbationsPerIteration", m_sxPerturbationsPerIteration))
		return false;

	if (!copy(stringMap, "xIteration", m_sxIteration))
		return false;

	if (!copy(stringMap, "xAcceptedPertHistIt", m_sxAcceptedPertHistIt))
		return false;

	if (!copy(stringMap, "xTotalEnergyCalculations", m_sxTotalEnergyCalculations))
		return false;
	if (!copy(stringMap, "xElapsedSeconds", m_sxElapsedSeconds))
		return false;
	if (!copy(stringMap, "xRunComplete", m_sxRunComplete))
		return false;
	if (!copy(stringMap, "xStructures", m_sxStructures))
		return false;
	if (!copy(stringMap, "xStructure", m_sxStructure))
		return false;
	if (!copy(stringMap, "xIsTransitionState", m_sxIsTransitionState))
		return false;
	if (!copy(stringMap, "xId", m_sxId))
		return false;
	if (!copy(stringMap, "xAtomGroup", m_sxAtomGroup))
		return false;
	if (!copy(stringMap, "xFrozen", m_sxFrozen))
		return false;
	if (!copy(stringMap, "xTranslation", m_sxTranslation))
		return false;
	if (!copy(stringMap, "xRadianAngles", m_sxRadianAngles))
		return false;
	if (!copy(stringMap, "xAtom", m_sxAtom))
		return false;
	if (!copy(stringMap, "xX", m_sxX))
		return false;
	if (!copy(stringMap, "xY", m_sxY))
		return false;
	if (!copy(stringMap, "xZ", m_sxZ))
		return false;
	if (!copy(stringMap, "xBigZ", m_sxBigZ))
		return false;

	if (!copy(stringMap, "xMaxSize", m_sxMaxSize))
		return false;
	if (!copy(stringMap, "xRmsDistance", m_sxRmsDistance))
		return false;

	if (!copy(stringMap, "pADF", m_spADF))
		return false;
	if (!copy(stringMap, "pGAMESS", m_spGAMESS))
		return false;
	if (!copy(stringMap, "pGAMESSUK", m_spGAMESSUK))
		return false;
	if (!copy(stringMap, "pGaussian", m_spGaussian))
		return false;
	if (!copy(stringMap, "pFirefly", m_spFirefly))
		return false;
	if (!copy(stringMap, "pJaguar", m_spJaguar))
		return false;
	if (!copy(stringMap, "pMolpro", m_spMolpro))
		return false;
	if (!copy(stringMap, "pORCA", m_spORCA))
		return false;
	if (!copy(stringMap, "pBest", m_spBest))
		return false;
	if (!copy(stringMap, "pTrue", m_spTrue))
		return false;
	if (!copy(stringMap, "pFalse", m_spFalse))
		return false;
	if (!copy(stringMap, "pLennardJones", m_spLennardJones))
		return false;
	if (!copy(stringMap, "pCartesian", m_spCartesian))
		return false;
	if (!copy(stringMap, "pAll", m_spAll))
		return false;

	if (!copy(stringMap, "ReadingFile", m_sReadingFile))
		return false;
	if (!copy(stringMap, "WritingFile", m_sWritingFile))
		return false;
	if (!copy(stringMap, "ErrorOpeningFile", m_sErrorOpeningFile))
		return false;
	if (!copy(stringMap, "pAbbrTemporary", m_spAbbrTemporary))
		return false;
	
	if (!copy(stringMap, "DuplicateAttributes", m_sDuplicateAttributes))
		return false;
	if (!copy(stringMap, "UnrecognizedAttribute", m_sUnrecognizedAttribute))
		return false;
	if (!copy(stringMap, "MissingAttribute", m_sMissingAttribute))
		return false;
	if (!copy(stringMap, "MustNotContainAttributes", m_sMustNotContainAttributes))
		return false;
	if (!copy(stringMap, "AvailableAttributes", m_sAvailableAttributes))
		return false;
	
	if (!copy(stringMap, "MaxOf1ElementExceeded", m_sMaxOf1ElementExceeded))
		return false;
	if (!copy(stringMap, "UnrecognizedElement", m_sUnrecognizedElement))
		return false;
	if (!copy(stringMap, "ElementNumNot1", m_sElementNumNot1))
		return false;
	if (!copy(stringMap, "ElementRequiresExactly1Child", m_sElementRequiresExactly1Child))
		return false;
	if (!copy(stringMap, "ElementRequiresNChildren", m_sElementRequiresNChildren))
		return false;
	if (!copy(stringMap, "ElementRequires1ChildMin", m_sElementRequires1ChildMin))
		return false;
	if (!copy(stringMap, "MisplacedElement", m_sMisplacedElement))
		return false;
	if (!copy(stringMap, "MaxElementsExceeded", m_sMaxElementsExceeded))
		return false;
	if (!copy(stringMap, "ChoiceError", m_sChoiceError))
		return false;
	if (!copy(stringMap, "ChoiceElementOrder", m_sChoiceElementOrder))
		return false;
	if (!copy(stringMap, "pUnlimited", m_spUnlimited))
		return false;
	
	if (!copy(stringMap, "UnrecognizedAttributeValue", m_sUnrecognizedAttributeValue))
		return false;
	if (!copy(stringMap, "UnableToReadInteger", m_sUnableToReadInteger))
		return false;
	if (!copy(stringMap, "UnableToReadPositiveInteger", m_sUnableToReadPositiveInteger))
		return false;
	if (!copy(stringMap, "ZeroNotAllowed", m_sZeroNotAllowed))
		return false;
	if (!copy(stringMap, "UnableToReadFloat", m_sUnableToReadFloat))
		return false;
	if (!copy(stringMap, "UnableToReadPositiveFloat", m_sUnableToReadPositiveFloat))
		return false;
	if (!copy(stringMap, "NegativeNotAllowed", m_sNegativeNotAllowed))
		return false;
	if (!copy(stringMap, "UnableToReadElementText", m_sUnableToReadElementText))
		return false;
	if (!copy(stringMap, "ErrorReadingAtomicNumber", m_sErrorReadingAtomicNumber))
		return false;
	if (!copy(stringMap, "ErrorReadingAtomicNumber2", m_sErrorReadingAtomicNumber2))
		return false;
	if (!copy(stringMap, "ErrorAtomicNumOverMax", m_sErrorAtomicNumOverMax))
		return false;
	if (!copy(stringMap, "ErrorAtomicNumOverMax2", m_sErrorAtomicNumOverMax2))
		return false;
	if (!copy(stringMap, "ErrorEmptyPath", m_sErrorEmptyPath))
		return false;
	if (!copy(stringMap, "RangeError", m_sRangeError))
		return false;
	if (!copy(stringMap, "UnableToReadTime", m_sUnableToReadTime))
		return false;
	
	if (!copy(stringMap, "ElementNotFound", m_sElementNotFound))
		return false;
	if (!copy(stringMap, "OneRootElement", m_sOneRootElement))
		return false;
	if (!copy(stringMap, "TwoElementsWithSameName", m_sTwoElementsWithSameName))
		return false;

	if (!copy(stringMap, "ConstraintNameMisMatch", m_sConstraintNameMisMatch))
		return false;
	
	if (!copy(stringMap, "ErrorZ1Z2", m_sErrorZ1Z2))
		return false;
	if (!copy(stringMap, "ErrorOneGeneralMin", m_sErrorOneGeneralMin))
		return false;
	if (!copy(stringMap, "ErrorDuplicateMinDist", m_sErrorDuplicateMinDist))
		return false;
	
	if (!copy(stringMap, "ConstraintNotDefined", m_sConstraintNotDefined))
		return false;
	if (!copy(stringMap, "EmptyStructureTemplate", m_sEmptyStructureTemplate))
		return false;
	if (!copy(stringMap, "SeededStructureDoesntMatchTemplate", m_sSeededStructureDoesntMatchTemplate))
		return false;
	if (!copy(stringMap, "SeededStructureDoesntMatchConstraints", m_sSeededStructureDoesntMatchConstraints))
		return false;
	
	if (!copy(stringMap, "ErrorEmptyMoleculeTemplate", m_sErrorEmptyMoleculeTemplate))
		return false;
	
	if (!copy(stringMap, "MissingChildElements3", m_sMissingChildElements3))
		return false;

	if (!copy(stringMap, "ProbabilityMustTotalOne", m_sProbabilityMustTotalOne))
		return false;

	m_bLoaded = true;
	return true;
}

// This function helps detect typos in key names
bool Strings::copy(std::map<std::string,std::string> &stringMap, const char* key, std::string &destination) {
	destination = stringMap[key];
	if (destination.length() == 0) {
		printf("Copy from map with key '%s' returned an empty string in stings.cc.  Is this a typo?", key);
		return false;
	}
	return true;
}

const std::string Strings::trim(const std::string& pString)
{
	static const std::string& pWhitespace = " \t\n\r";
    const size_t beginStr = pString.find_first_not_of(pWhitespace);
    if (beginStr == std::string::npos)
    {
        // no content
        return "";
    }

    const size_t endStr = pString.find_last_not_of(pWhitespace);
    const size_t range = endStr - beginStr + 1;

    return pString.substr(beginStr, range);
}

const std::string Strings::trim(const char* pCharArr)
{
	const std::string& pString = pCharArr;
	return trim(pString);
}

const char *Strings::getTrueFalseParam(bool boolValue) const
{
        if (boolValue)
                return m_spTrue.c_str();
        else
                return m_spFalse.c_str();
}
