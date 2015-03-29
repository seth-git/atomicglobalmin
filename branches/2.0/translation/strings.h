
#ifndef __STRINGS_H__
#define __STRINGS_H__

#include <string>
#include <string.h>

namespace strings {
	const char xSimulatedAnnealing[] = "simulatedAnnealing";
	const char xRandomSearch[] = "randomSearch";
	const char xParticleSwarmOptimization[] = "particleSwarmOptimization";
	const char xGeneticAlgorithm[] = "geneticAlgorithm";
	const char xBatch[] = "batch";
	const char xSetup[] = "setup";
	const char xConstraints[] = "constraints";
	const char xEnergy[] = "energy";
	const char xResume[] = "resume";
	const char xResults[] = "results";
	const char xInternal[] = "internal";
	const char xExternal[] = "external";
	const char xCube[] = "cube";
	const char xAtomicDistances[] = "atomicDistances";
	const char xMin[] = "min";
	const char xMax[] = "max";
	const char xName[] = "name";
	const char xBase[] = "base";
	const char xSize[] = "size";
	const char xValue[] = "value";
	const char xZ1[] = "Z1";
	const char xZ2[] = "Z2";
	const char xMethod[] = "method";
	const char xTransitionStateSearch[] = "transitionStateSearch";
	const char xWallTime[] = "wallTime";
	const char xTemporaryDirectory[] = "temporaryDirectory";
	const char xResultsDirectory[] = "resultsDirectory";
	const char xHeader[] = "header";
	const char xPath[] = "path";
	const char xMaxFiles[] = "maxFiles";
	const char xFilePrefix[] = "filePrefix";
	const char xMaster[] = "master";
	const char xOpt[] = "opt";

	const char xStructuresTemplate[] = "structuresTemplate";
	const char xTemperature[] = "temperature";
	const char xAnnealingSchedule[] = "annealingSchedule";
	const char xPerturbations[] = "perturbations";
	const char xStop[] = "stop";

	const char xDecreaseAfterIteration[] = "decreaseAfterIteration";
	const char xMaxTemperature[] = "maxTemperature";
	const char xMaxAcceptedPerturbations[] = "maxAcceptedPerturbations";
	const char xMinIterations[] = "minIterations";
	const char xSaveFrequency[] = "saveFrequency";

	const char xStructureTemplate[] = "structureTemplate";
	const char xLinear[] = "linear";
	const char xPlanar[] = "planar";
	const char xThreeD[] = "threeD";
	const char xBondRotationalSearch[] = "bondRotationalSearch";
	const char xSeed[] = "seed";

	const char xMoleculeTemplate[] = "moleculeTemplate";
	const char xAtomTemplate[] = "atomTemplate";
	const char xNumber[] = "number";
	const char xFormat[] = "format";
	const char xDegrees[] = "degrees";
	const char xType[] = "type";

	const char xAgmlFile[] = "agmlFile";
	const char xDirectory[] = "directory";
	const char xFile[] = "file";
	const char xFreezingIterations[] = "freezingIterations";

	const char xSource[] = "source";
	const char xPopulation[] = "population";

	const char xKelvin[] = "kelvin";
	const char xAcceptedPerturbations[] = "acceptedPerturbations";
	const char xPercent[] = "percent";

	const char xBoltzmannConstant[] = "boltzmannConstant";
	const char xQuenchingFactor[] = "quenchingFactor";

	const char xTranslationVector[] = "translationVector";
	const char xRotationAngle[] = "rotationAngle";
	const char xRadians[] = "radians";
	const char xStartLength[] = "startLength";
	const char xMinLength[] = "minLength";
	const char xProbability[] = "probability";
	const char xStartDegrees[] = "startDegrees";
	const char xMinDegrees[] = "minDegrees";
	const char xNumberPerIteration[] = "numberPerIteration";
	const char xPerturbationsPerIteration[] = "perturbationsPerIteration";
	const char xIteration[] = "iteration";

	const char xAcceptedPertHistIt[] = "acceptedPertHistIt";

	const char xTotalEnergyCalculations[] = "totalEnergyCalculations";
	const char xElapsedSeconds[] = "elapsedSeconds";
	const char xRunComplete[] = "runComplete";
	const char xStructures[] = "structures";
	const char xStructure[] = "structure";
	const char xIsTransitionState[] = "isTransitionState";
	const char xId[] = "id";
	const char xAtomGroup[] = "atomGroup";
	const char xFrozen[] = "frozen";
	const char xTranslation[] = "translation";
	const char xRadianAngles[] = "radianAngles";
	const char xAtom[] = "atom";
	const char xX[] = "x";
	const char xY[] = "y";
	const char xZ[] = "z";
	const char xBigZ[] = "Z";

	const char xMaxSize[] = "maxSize";
	const char xRmsDistance[] = "rmsDistance";
	const char xOrderBy[] = "orderBy";

	const char pADF[] = "ADF";
	const char pGAMESSUK[] = "GAMESS-UK";
	const char pGAMESSUS[] = "GAMESS-US";
	const char pGaussian[] = "Gaussian";
	const char pFirefly[] = "Firefly";
	const char pJaguar[] = "Jaguar";
	const char pMolpro[] = "Molpro";
	const char pNWChem[] = "NWChem";
	const char pORCA[] = "ORCA";
	const char pBest[] = "best";
	const char pTrue[] = "true";
	const char pFalse[] = "false";
	const char pLennardJones[] = "Lennard Jones";
	const char pCartesian[] = "Cartesian";
	const char pAll[] = "All";

	const char pTemp[] = "temp";
	const char pResults[] = "results";
	const char pStructure[] = "structure";
	const char pStop[] = "stop";

	const char pEnergy[] = "energy";
	const char pFile[] = "file";
	const char pId[] = "id";

	const char ReadingFile[] = "Reading file: %1$s\n";
	const char WritingFile[] = "Writing file: %1$s\n";
	const char ErrorOpeningFile[] = "Error opening file: %1$s\n";
	const char ErrorWritingFile[] = "Error writing file: %1$s\n";

	// Error messages in xsdAttributeUtil.cc
	const char DuplicateAttributes[] = "Duplicate attribute '%1$s' found for element '%2$s' with values '%3$s' and '%4$s'.\n";
	const char UnrecognizedAttribute[] = "Unrecognized attribute '%1$s' found in the element '%2$s'.\n";
	const char MissingAttribute[] = "The attribute '%1$s' is required on the element '%2$s'.\n";
	const char MustNotContainAttributes[] = "The '%1$s' element must not contain any attributes.\n";
	const char AvailableAttributes[] = "Available attributes on the element '%1$s' can be: %2$s.\n";

	// Error messages in xsdElementUtil.cc
	const char MaxOf1ElementExceeded[] = "More than one element '%1$s' can't be inside an '%2$s' element.\n";
	const char UnrecognizedElement[] = "Unrecognized element '%1$s' inside an element '%2$s'.\n";
	const char ElementNumNot1[] = "There must be one '%1$s' element inside each element '%2$s'.\n";
	const char ElementRequiresExactly1Child[] = "The '%1$s' element cannot contain more than one child element.\n";
	const char ElementRequiresNChildren[] = "There must be at least %1$u '%2$s' elements inside the '%3$s' element.\n";
	const char ElementRequires1ChildMin[] = "There must be at least one '%1$s' element inside the '%2$s' element.\n";
	const char MisplacedElement[] = "Unrecognized or misplaced '%1$s' element inside the '%2$s' element.\n";
	const char MaxElementsExceeded[] = "There can't be more than %1$u '%2$s' element(s) inside the '%3$s' element.\n";
	const char ChoiceError[] = "The element '%1$s' must contain one of these elements: %2$s.\n";
	const char ChoiceElementOrder[] = "Elements inside the '%1$s' element must be listed in this order:\n";
	const char pUnlimited[] = "unlimited";

	// Error messages in xsdTypeUtil.cc
	const char UnrecognizedAttributeValue[] = "The value '%1$s' is unrecognized for the attribute '%2$s' on the element '%3$s'. Possible values are %4$s.\n";
	const char UnableToReadInteger[] = "Unable to read integer '%1$s' from attribute '%2$s' in element '%3$s'.\n";
	const char UnableToReadPositiveInteger[] = "Unable to read positive integer '%1$s' from attribute '%2$s' in element '%3$s'.\n";
	const char ZeroNotAllowed[] = "Zero is not allowed for the attribute '%1$s' in the element '%2$s'.\n";
	const char UnableToReadFloat[] = "Unable to read real number '%1$s' from attribute '%2$s' in element '%3$s'.\n";
	const char UnableToReadPositiveFloat[] = "Unable to read positive real number '%1$s' from attribute '%2$s' in element '%3$s'.\n";
	const char NegativeNotAllowed[] = "Negative numbers such as %1$s are not allowed for the attribute '%2$s' in the element '%3$s'.\n";
	const char UnableToReadElementText[] = "The element '%1$s' requires closing and opening tags with text in between (<%1$s>Text</%1$s>).\n";
	const char ErrorReadingAtomicNumber[] = "Error reading an atomic number '%1$s' on the attribute '%2$s' on the element '%3$s'.\n";
	const char ErrorReadingAtomicNumber2[] = "Error reading the atomic number: '%1$s'.\n";
	const char ErrorAtomicNumOverMax[] = "An atomic number '%1$s' in the element '%2$s' is %3$u, which is greater than the maximum of %4$u.\n";
	const char ErrorAtomicNumOverMax2[] = "An atomic number %1$u is greater than the maximum of %2$u.\n";
	const char ErrorEmptyPath[] = "There is an empty directory or file for the '%1$s' attribute on the '%2$s' element.\n";
	const char RangeError[] = "The '%1$s' attribute on the '%2$s' element must be in the range [%3$s,%4$s].\n";
	const char UnableToReadTime[] = "Unable to read time '%1$s' from attribute '%2$s' in element '%3$s'.\n";

	// Error messages in input.cc
	const char ElementNotFound[] = "The '%1$s' element was not found.\n";
	const char OneRootElement[] = "There can be only one root element.";
	const char TwoElementsWithSameName[] = "Two elements '%1$s' have the same name '%2$s'.\n";

	// Error messages in action.cc
	const char ConstraintNameMisMatch[] = "On the %1$s element with %2$s '%3$s', the %4$s %1$s '%5$s' have not been defined.\n";

	// Error messages in constraints.cc
	const char ErrorZ1Z2[] = "In the element '%1$s', '%2$s' and '%3$s' must both be included or both must be excluded.\n";
	const char ErrorOneGeneralMin[] = "A second element '%1$s' was found without '%2$s' and '%3$s'.  There can be only one general minimum distance constraint.\n";
	const char ErrorDuplicateMinDist[] = "The minimum distance('%1$s') between %2$s=%3$u and %4$s=%5$u has already been set.\n";

	// Error messages in structuresTemplate.cc
	const char ConstraintNotDefined[] = "On the element %1$s, the %2$s '%3$s' have not been defined.\n";
	const char ResultsShouldBeEmpty[] = "The results directory (%1$s) should be empty at the start of a run.\n";
	const char EmptyStructureTemplate[] = "The '%1$s' element must contain one or more '%2$s' or '%3$s' elements.\n";
	const char SeededStructureDoesntMatchTemplate[] = "Seeded structure %u is not compatible with the structuresTemplate.\n";
	const char SeededStructureDoesntMatchConstraints[] = "Seeded structure %u does not meet the constraints.\n";

	// Error messages in atomGroupTemplate.cc
	const char ErrorEmptyMoleculeTemplate[] = "Please specify at least one atom in the format 'ATOMIC_NUMBER_OR_SYMBOL x y z' between <%1$s></%1$s> tags.\n";
	const char MissingChildElements3[] = "The element '%1$s' must contain at least one of these elements: '%2$s', '%3$s', or '%4$s'.\n";

	// Error messages in perturbations.cc
	const char ProbabilityMustTotalOne[] = "In the element '%1$s', the total probability of each perturbation type must total 1.0.\n";

	// Error messages in externalEnergyXml.cc
	const char ErrorMissingEnergyDir[] = "In the '%1$s' element, please specify either a '%2$s' or a '%3$s'.\n";
	const char WallTimeError[] = "Error reading wall time: '%1$s'\n";

	const char UnableToExecuteCommand[] = "Unable to execute command: '%1$s'.\n";

	// Batch messages
	const char ProcessingBatch[] = "Processing batch...";
	const char BatchComplete[] = "Batch complete!";
	const char BatchAlreadyCompleted[] = "This batch has already been completed.";

	// Simulated Annealing messages
	const char AllRunsCompleted[] = "All runs have been completed.";
}

#endif
