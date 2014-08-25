
#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <string>
#include <string.h>

namespace strings {
	const char * const xSimulatedAnnealing = "simulatedAnnealing";
	const char * const xRandomSearch = "randomSearch";
	const char * const xParticleSwarmOptimization = "particleSwarmOptimization";
	const char * const xGeneticAlgorithm = "geneticAlgorithm";
	const char * const xBatch = "batch";
	const char * const xSetup = "setup";
	const char * const xConstraints = "constraints";
	const char * const xEnergy = "energy";
	const char * const xResume = "resume";
	const char * const xResults = "results";
	const char * const xInternal = "internal";
	const char * const xExternal = "external";
	const char * const xCube = "cube";
	const char * const xAtomicDistances = "atomicDistances";
	const char * const xMin = "min";
	const char * const xMax = "max";
	const char * const xName = "name";
	const char * const xBase = "base";
	const char * const xSize = "size";
	const char * const xValue = "value";
	const char * const xZ1 = "Z1";
	const char * const xZ2 = "Z2";
	const char * const xMethod = "method";
	const char * const xTransitionStateSearch = "transitionStateSearch";
	const char * const xSharedDirectory = "sharedDirectory";
	const char * const xLocalDirectory = "localDirectory";
	const char * const xResultsDirectory = "resultsDirectory";
	const char * const xCharge = "charge";
	const char * const xMultiplicity = "multiplicity";
	const char * const xHeader = "header";
	const char * const xFooter = "footer";
	const char * const xPath = "path";
	const char * const xMaxFiles = "maxFiles";
	const char * const xFilePrefix = "filePrefix";
	const char * const xMaster = "master";
	const char * const xOpt = "opt";

	const char * const xStructuresTemplate = "structuresTemplate";
	const char * const xTemperature = "temperature";
	const char * const xAnnealingSchedule = "annealingSchedule";
	const char * const xPerturbations = "perturbations";
	const char * const xStop = "stop";

	const char * const xDecreaseAfterIteration = "decreaseAfterIteration";
	const char * const xMaxTemperature = "maxTemperature";
	const char * const xMaxAcceptedPerturbations = "maxAcceptedPerturbations";
	const char * const xMinIterations = "minIterations";
	const char * const xSaveFrequency = "saveFrequency";
	const char * const xQueueSize = "queueSize";

	const char * const xStructureTemplate = "structureTemplate";
	const char * const xLinear = "linear";
	const char * const xPlanar = "planar";
	const char * const xThreeD = "threeD";
	const char * const xBondRotationalSearch = "bondRotationalSearch";
	const char * const xSeed = "seed";

	const char * const xMoleculeTemplate = "moleculeTemplate";
	const char * const xAtomTemplate = "atomTemplate";
	const char * const xNumber = "number";
	const char * const xFormat = "format";
	const char * const xDegrees = "degrees";
	const char * const xType = "type";

	const char * const xAgmlFile = "agmlFile";
	const char * const xDirectory = "directory";
	const char * const xEnergyFile = "energyFile";
	const char * const xFreezingIterations = "freezingIterations";

	const char * const xSource = "source";
	const char * const xPopulation = "population";

	const char * const xKelvin = "kelvin";
	const char * const xAcceptedPerturbations = "acceptedPerturbations";
	const char * const xPercent = "percent";

	const char * const xBoltzmannConstant = "boltzmannConstant";
	const char * const xQuenchingFactor = "quenchingFactor";

	const char * const xTranslationVector = "translationVector";
	const char * const xRotationAngle = "rotationAngle";
	const char * const xRadians = "radians";
	const char * const xStartLength = "startLength";
	const char * const xMinLength = "minLength";
	const char * const xProbability = "probability";
	const char * const xStartDegrees = "startDegrees";
	const char * const xMinDegrees = "minDegrees";
	const char * const xNumberPerIteration = "numberPerIteration";
	const char * const xPerturbationsPerIteration = "perturbationsPerIteration";
	const char * const xIteration = "iteration";

	const char * const xAcceptedPertHistIt = "acceptedPertHistIt";

	const char * const xTotalEnergyCalculations = "totalEnergyCalculations";
	const char * const xElapsedSeconds = "elapsedSeconds";
	const char * const xRunComplete = "runComplete";
	const char * const xStructures = "structures";
	const char * const xStructure = "structure";
	const char * const xIsTransitionState = "isTransitionState";
	const char * const xId = "id";
	const char * const xAtomGroup = "atomGroup";
	const char * const xFrozen = "frozen";
	const char * const xTranslation = "translation";
	const char * const xRadianAngles = "radianAngles";
	const char * const xAtom = "atom";
	const char * const xX = "x";
	const char * const xY = "y";
	const char * const xZ = "z";
	const char * const xBigZ = "Z";

	const char * const xMaxSize = "maxSize";
	const char * const xRmsDistance = "rmsDistance";

	const char * const pADF = "ADF";
	const char * const pGAMESS = "GAMESS";
	const char * const pGAMESSUK = "GAMESS-UK";
	const char * const pGaussian = "Gaussian";
	const char * const pFirefly = "Firefly";
	const char * const pJaguar = "Jaguar";
	const char * const pMolpro = "Molpro";
	const char * const pORCA = "ORCA";
	const char * const pBest = "best";
	const char * const pTrue = "true";
	const char * const pFalse = "false";
	const char * const pLennardJones = "Lennard Jones";
	const char * const pCartesian = "Cartesian";
	const char * const pAll = "All";

	const char * const ReadingFile = "Reading file: %1$s\n";
	const char * const WritingFile = "Writing file: %1$s\n";
	const char * const ErrorOpeningFile = "Error opening file: %1$s\n";
	const char * const pAbbrTemporary = "temp";

	// Error messages in xsdAttributeUtil.cc
	const char * const DuplicateAttributes = "Duplicate attribute '%1$s' found for element '%2$s' with values '%3$s' and '%4$s'.\n";
	const char * const UnrecognizedAttribute = "Unrecognized attribute '%1$s' found in the element '%2$s'.\n";
	const char * const MissingAttribute = "The attribute '%1$s' is required on the element '%2$s'.\n";
	const char * const MustNotContainAttributes = "The '%1$s' element must not contain any attributes.\n";
	const char * const AvailableAttributes = "Available attributes on the element '%1$s' can be: %2$s.\n";

	// Error messages in xsdElementUtil.cc
	const char * const MaxOf1ElementExceeded = "More than one element '%1$s' can't be inside an '%2$s' element.\n";
	const char * const UnrecognizedElement = "Unrecognized element '%1$s' inside an element '%2$s'.\n";
	const char * const ElementNumNot1 = "There must be one '%1$s' element inside each element '%2$s'.\n";
	const char * const ElementRequiresExactly1Child = "The '%1$s' element cannot contain more than one child element.\n";
	const char * const ElementRequiresNChildren = "There must be at least %1$u '%2$s' elements inside the '%3$s' element.\n";
	const char * const ElementRequires1ChildMin = "There must be at least one '%1$s' element inside the '%2$s' element.\n";
	const char * const MisplacedElement = "Unrecognized or misplaced '%1$s' element inside the '%2$s' element.\n";
	const char * const MaxElementsExceeded = "There can't be more than %1$u '%2$s' element(s) inside the '%3$s' element.\n";
	const char * const ChoiceError = "The element '%1$s' must contain one of these elements: %2$s.\n";
	const char * const ChoiceElementOrder = "Elements inside the '%1$s' element must be listed in this order:\n";
	const char * const pUnlimited = "unlimited";

	// Error messages in xsdTypeUtil.cc
	const char * const UnrecognizedAttributeValue = "The value '%1$s' is unrecognized for the attribute '%2$s' on the element '%3$s'. Possible values are %4$s.\n";
	const char * const UnableToReadInteger = "Unable to read integer '%1$s' from attribute '%2$s' in element '%3$s'.\n";
	const char * const UnableToReadPositiveInteger = "Unable to read positive integer '%1$s' from attribute '%2$s' in element '%3$s'.\n";
	const char * const ZeroNotAllowed = "Zero is not allowed for the attribute '%1$s' in the element '%2$s'.\n";
	const char * const UnableToReadFloat = "Unable to read real number '%1$s' from attribute '%2$s' in element '%3$s'.\n";
	const char * const UnableToReadPositiveFloat = "Unable to read positive real number '%1$s' from attribute '%2$s' in element '%3$s'.\n";
	const char * const NegativeNotAllowed = "Negative numbers such as %1$s are not allowed for the attribute '%2$s' in the element '%3$s'.\n";
	const char * const UnableToReadElementText = "The element '%1$s' requires closing and opening tags with text in between (<%1$s>Text</%1$s>).\n";
	const char * const ErrorReadingAtomicNumber = "Error reading an atomic number '%1$s' on the attribute '%2$s' on the element '%3$s'.\n";
	const char * const ErrorReadingAtomicNumber2 = "Error reading the atomic number: '%1$s'.\n";
	const char * const ErrorAtomicNumOverMax = "An atomic number '%1$s' in the element '%2$s' is %3$u, which is greater than the maximum of %4$u.\n";
	const char * const ErrorAtomicNumOverMax2 = "An atomic number %1$u is greater than the maximum of %2$u.\n";
	const char * const ErrorEmptyPath = "There is an empty directory or file for the '%1$s' attribute on the '%2$s' element.\n";
	const char * const RangeError = "The '%1$s' attribute on the '%2$s' element must be in the range [%3$s,%4$s].\n";
	const char * const UnableToReadTime = "Unable to read time '%1$s' from attribute '%2$s' in element '%3$s'.\n";

	// Error messages in input.cc
	const char * const ElementNotFound = "The '%1$s' element was not found.\n";
	const char * const OneRootElement = "There can be only one root element.";
	const char * const TwoElementsWithSameName = "Two elements '%1$s' have the same name '%2$s'.\n";

	// Error messages in action.cc
	const char * const ConstraintNameMisMatch = "On the %1$s element with %2$s '%3$s', the %4$s %1$s '%5$s' have not been defined.\n";

	// Error messages in constraints.cc
	const char * const ErrorZ1Z2 = "In the element '%1$s', '%2$s' and '%3$s' must both be included or both must be excluded.\n";
	const char * const ErrorOneGeneralMin = "A second element '%1$s' was found without '%2$s' and '%3$s'.  There can be only one general minimum distance constraint.\n";
	const char * const ErrorDuplicateMinDist = "The minimum distance('%1$s') between %2$s=%3$u and %4$s=%5$u has already been set.\n";

	// Error messages in structuresTemplate.cc
	const char * const ConstraintNotDefined = "On the element %1$s, the %2$s '%3$s' have not been defined.\n";
	const char * const EmptyStructureTemplate = "The '%1$s' element must contain one or more '%2$s' or '%3$s' elements.\n";
	const char * const SeededStructureDoesntMatchTemplate = "Seeded structure %u is not compatible with the structuresTemplate.\n";
	const char * const SeededStructureDoesntMatchConstraints = "Seeded structure %u does not meet the constraints.\n";

	// Error messages in atomGroupTemplate.cc
	const char * const ErrorEmptyMoleculeTemplate = "Please specify at least one atom in the format 'ATOMIC_NUMBER_OR_SYMBOL x y z' between <%1$s></%1$s> tags.\n";
	const char * const MissingChildElements3 = "The element '%1$s' must contain at least one of these elements: '%2$s', '%3$s', or '%4$s'.\n";

	// Error messages in perturbations.cc
	const char * const ProbabilityMustTotalOne = "In the element '%1$s', the total probability of each perturbation type must total 1.0.\n";
}

#endif
