////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing messages in different languages.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <string.h>
#include <iostream>
#include <map>
#include <utility>

#include <stdio.h>
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>

using std::ifstream;

class Strings
{
public:
	static bool init();
	static Strings* instance();
	static Strings* instance(std::string languageCode);
	static const std::string s_sDefaultLanguageCode;
	
	Strings () { m_bLoaded = false; }
	bool init(const char* languageCode);
	
	const char *getTrueFalseParam(bool boolValue) const;
	
	static const std::string trim(const std::string& pString);
	static const std::string trim(const char* pCharArr);

	std::string m_sxSimulatedAnnealing;
	std::string m_sxRandomSearch;
	std::string m_sxParticleSwarmOptimization;
	std::string m_sxGeneticAlgorithm;
	std::string m_sxBatch;
	std::string m_sxSetup;
	std::string m_sxConstraints;
	std::string m_sxEnergy;
	std::string m_sxResume;
	std::string m_sxResults;
	std::string m_sxInternal;
	std::string m_sxExternal;

	std::string m_sxCube;
	std::string m_sxAtomicDistances;
	std::string m_sxMin;
	std::string m_sxMax;
	std::string m_sxName;
	std::string m_sxBase;
	std::string m_sxSize;
	std::string m_sxValue;
	std::string m_sxZ1;
	std::string m_sxZ2;
	
	std::string m_sxMethod;
	std::string m_sxTransitionStateSearch;
	std::string m_sxSharedDirectory;
	std::string m_sxLocalDirectory;
	std::string m_sxResultsDirectory;
	std::string m_sxCharge;
	std::string m_sxMultiplicity;
	std::string m_sxHeader;
	std::string m_sxFooter;
	std::string m_sxPath;
	std::string m_sxMaxFiles;
	std::string m_sxFilePrefix;
	std::string m_sxMaster;
	std::string m_sxOpt;
	
	std::string m_sxStructuresTemplate;
	std::string m_sxTemperature;
	std::string m_sxAnnealingSchedule;
	std::string m_sxPerturbations;
	std::string m_sxStop;

	std::string m_sxDecreaseAfterIteration;
	std::string m_sxMaxTemperature;
	std::string m_sxMaxAcceptedPerturbations;
	std::string m_sxMinIterations;
	std::string m_sxSaveFrequency;
	std::string m_sxQueueSize;
	
	std::string m_sxStructureTemplate;
	std::string m_sxLinear;
	std::string m_sxPlanar;
	std::string m_sxThreeD;
	std::string m_sxBondRotationalSearch;
	std::string m_sxSeed;
	
	std::string m_sxMoleculeTemplate;
	std::string m_sxAtomTemplate;
	std::string m_sxNumber;
	std::string m_sxFormat;
	std::string m_sxDegrees;
	std::string m_sxType;

	std::string m_sxAgmlFile;
	std::string m_sxDirectory;
	std::string m_sxEnergyFile;
	std::string m_sxFreezingIterations;
	
	std::string m_sxSource;
	std::string m_sxPopulation;

	std::string m_sxKelvin;
	std::string m_sxAcceptedPerturbations;
	std::string m_sxPercent;
	std::string m_sxBoltzmannConstant;
	std::string m_sxQuenchingFactor;

	std::string m_sxTranslationVector;
	std::string m_sxRotationAngle;
	std::string m_sxRadians;
	std::string m_sxStartLength;
	std::string m_sxMinLength;
	std::string m_sxProbability;
	std::string m_sxStartDegrees;
	std::string m_sxMinDegrees;
	std::string m_sxNumberPerIteration;
	std::string m_sxPerturbationsPerIteration;

	std::string m_sxIteration;

	std::string m_sxAcceptedPertHistIt;

	std::string m_sxTotalEnergyCalculations;
	std::string m_sxElapsedSeconds;
	std::string m_sxRunComplete;
	std::string m_sxStructures;
	std::string m_sxStructure;
	std::string m_sxIsTransitionState;
	std::string m_sxId;
	std::string m_sxAtomGroup;
	std::string m_sxFrozen;
	std::string m_sxTranslation;
	std::string m_sxRadianAngles;
	std::string m_sxAtom;
	std::string m_sxX;
	std::string m_sxY;
	std::string m_sxZ;
	std::string m_sxBigZ;

	std::string m_sxMaxSize;
	std::string m_sxRmsDistance;

	std::string m_spADF;
	std::string m_spGAMESS;
	std::string m_spGAMESSUK;
	std::string m_spGaussian;
	std::string m_spFirefly;
	std::string m_spJaguar;
	std::string m_spMolpro;
	std::string m_spORCA;
	std::string m_spBest;
	std::string m_spTrue;
	std::string m_spFalse;
	std::string m_spLennardJones;
	std::string m_spCartesian;
	std::string m_spAll;

	std::string m_sLanguageCode;

	std::string m_sReadingFile;
	std::string m_sWritingFile;
	std::string m_sErrorOpeningFile;
	std::string m_spAbbrTemporary;
	
	std::string m_sDuplicateAttributes;
	std::string m_sUnrecognizedAttribute;
	std::string m_sMissingAttribute;
	std::string m_sMustNotContainAttributes;
	std::string m_sAvailableAttributes;
	
	std::string m_sMaxOf1ElementExceeded;
	std::string m_sUnrecognizedElement;
	std::string m_sElementNumNot1;
	std::string m_sElementRequiresExactly1Child;
	std::string m_sElementRequiresNChildren;
	std::string m_sElementRequires1ChildMin;
	std::string m_sMisplacedElement;
	std::string m_sMaxElementsExceeded;
	std::string m_sChoiceError;
	std::string m_sChoiceElementOrder;
	std::string m_spUnlimited;
	
	std::string m_sUnrecognizedAttributeValue;
	std::string m_sUnableToReadInteger;
	std::string m_sUnableToReadPositiveInteger;
	std::string m_sZeroNotAllowed;
	std::string m_sUnableToReadFloat;
	std::string m_sUnableToReadPositiveFloat;
	std::string m_sNegativeNotAllowed;
	std::string m_sUnableToReadElementText;
	std::string m_sErrorReadingAtomicNumber;
	std::string m_sErrorReadingAtomicNumber2;
	std::string m_sErrorAtomicNumOverMax;
	std::string m_sErrorAtomicNumOverMax2;
	std::string m_sErrorEmptyPath;
	std::string m_sRangeError;
	std::string m_sUnableToReadTime;

	std::string m_sElementNotFound;
	std::string m_sOneRootElement;
	std::string m_sTwoElementsWithSameName;

	std::string m_sConstraintNameMisMatch;

	std::string m_sErrorZ1Z2;
	std::string m_sErrorOneGeneralMin;
	std::string m_sErrorDuplicateMinDist;
	
	std::string m_sConstraintNotDefined;
	std::string m_sEmptyStructureTemplate;
	std::string m_sSeededStructureDoesntMatchTemplate;
	std::string m_sSeededStructureDoesntMatchConstraints;
	
	std::string m_sErrorEmptyMoleculeTemplate;
	std::string m_sMissingChildElements3;

	std::string m_sProbabilityMustTotalOne;

private:
	static std::map<std::string,Strings> s_instances;

	bool m_bLoaded;
	
	bool copy(std::map<std::string,std::string> &stringMap, const char* key, std::string &destination);
};

#endif
