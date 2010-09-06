////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "main.h"

bool initLinearMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions)
{
	Point3D shiftAmount;
	
	if (!moleculeSet.initPositionsAndAngles(boxDimensions, INITIALIZATION_TRIES)) {
		cout << "Failed to initialize linear molecule set after " << INITIALIZATION_TRIES << " tries." << endl;
		cout << "Initialization box size is probably too small: " << boxDimensions.x << "x"
		                                                          << boxDimensions.y << "x"
		                                                          << boxDimensions.z << "x" << endl;
		return false;
	}
	
	// Especially for particle swarm optimization, we need to start in the middle of the box
	// rather than in the corner, so shift the atoms, so they're in the middle of the box.
	// This code assumes that boxDimensions.x is longer than boxDimensions.y and boxDimensions.z and that
	// boxDimensions.y and boxDimensions.z are small (and equal to one another).
	shiftAmount.x = 0;
	shiftAmount.y = (boxDimensions.x / 2) - (boxDimensions.y / 2);
	shiftAmount.z = shiftAmount.y;
	moleculeSet.moveMoleculeSet(shiftAmount);
	return true;
}

bool initPlanarMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions)
{
	Point3D shiftAmount;
	
	if (!moleculeSet.initPositionsAndAngles(boxDimensions, INITIALIZATION_TRIES)) {
		cout << "Failed to initialize planar molecule set after " << INITIALIZATION_TRIES << " tries." << endl;
		cout << "Initialization box size is probably too small: " << boxDimensions.x << "x"
		                                                          << boxDimensions.y << "x"
		                                                          << boxDimensions.z << "x" << endl;
		return false;
	}

	// Especially for particle swarm optimization, we need to start in the middle of the box
	// rather than on one side, so shift the atoms, so they're in the middle of the box.
	// This code assumes that boxDimensions.x = boxDimensions.y and that boxDimensions.z is 0.
	shiftAmount.x = 0;
	shiftAmount.y = 0;
	shiftAmount.z = boxDimensions.x / 2;
	moleculeSet.moveMoleculeSet(shiftAmount);
	
	return true;
}

bool init3DMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions)
{
	if (!moleculeSet.initPositionsAndAngles(boxDimensions, INITIALIZATION_TRIES)) {
		cout << "Failed to initialize 3D molecule set after " << INITIALIZATION_TRIES << " tries." << endl;
		return false;
	}
	return true;
}

bool init3DMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Point3D &boxDimensions, FLOAT maxAtomDist)
{
	if (!moleculeSet.initPositionsAndAnglesWithMaxDist(boxDimensions, maxAtomDist, INITIALIZATION_TRIES)) {
		cout << "Failed to initialize 3D molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
		return false;
	}
	return true;
}

bool init3DNonFragMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Point3D &boxDimensions, FLOAT maxAtomDist)
{
	if (!moleculeSet.initNonFragmentedSructure(boxDimensions, maxAtomDist, INITIALIZATION_TRIES)) {
		cout << "Failed to initialize 3D non-fragmented molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
		return false;
	}
	return true;
}

bool initializePopulation(Input &input, vector<MoleculeSet*> &moleculeSets)
{
	vector<MoleculeSet*> linearMoleculeSets;
	vector<MoleculeSet*> planarMoleculeSets;
	vector<MoleculeSet*> threeDMoleculeSets;
	vector<MoleculeSet*> threeDWithMaxDistMoleculeSets;
	vector<MoleculeSet*> threeDNonFragWithMaxDistMoleculeSets;
	vector<MoleculeSet*> nearCopyMoleculeSets;
	MoleculeSet* newMoleculeSet;
	int i;
	int moleculeSetId = 0;
	Point3D boxDimensions;
	
	// Create the initial population based on the template molecule set
	if (input.m_iLinearSructures > 0) {
		boxDimensions.x = input.m_boxDimensions.x;
		boxDimensions.y = input.m_fLinearBoxHeight;
		boxDimensions.z = input.m_fLinearBoxHeight;
		for (i = 0; i < input.m_iLinearSructures; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			if (!initLinearMoleculeSet(*newMoleculeSet, boxDimensions)) {
				delete newMoleculeSet;
				return false;
			}
			linearMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet); // Since we're using pointers to newMoleculeSet, it can be in two lists
		}
	}
	
	if (input.m_iPlanarStructures > 0) {
		boxDimensions.x = input.m_boxDimensions.x;
		boxDimensions.y = input.m_boxDimensions.y;
		boxDimensions.z = 0;
		for (i = 0; i < input.m_iPlanarStructures; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			if (!initPlanarMoleculeSet(*newMoleculeSet, boxDimensions)) {
				delete newMoleculeSet;
				return false;
			}
			planarMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
	}
	
	if (input.m_i3DStructures > 0) {
		for (i = 0; i < input.m_i3DStructures; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			if (!init3DMoleculeSet(*newMoleculeSet, input.m_boxDimensions)) {
				delete newMoleculeSet;
				return false;
			}
			threeDMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
	}
	
	if (input.m_i3DStructuresWithMaxDist > 0) {
		for (i = 0; i < input.m_i3DStructuresWithMaxDist; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			if (!init3DMoleculeSetWithMaxDist(*newMoleculeSet, input.m_boxDimensions, input.m_fMaxAtomDistance)) {
				delete newMoleculeSet;
				return false;
			}
			threeDWithMaxDistMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
	}
	
	if (input.m_i3DNonFragStructuresWithMaxDist > 0) {
		for (i = 0; i < input.m_i3DNonFragStructuresWithMaxDist; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			if (!init3DNonFragMoleculeSetWithMaxDist(*newMoleculeSet, input.m_boxDimensions, input.m_fMaxAtomDistance)) {
				delete newMoleculeSet;
				return false;
			}
			threeDNonFragWithMaxDistMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
	}
	// Print the structures
/*	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
	{
		cout << "Structure: " << i+1 << endl;
		moleculeSets[i]->printToScreen();
		cout << "Energy = ";
		cout << setiosflags(ios::fixed) << setprecision(8) << moleculeSets[i]->getEnergy() << endl << endl;
	}*/
	return true;
}

void simulatedAnnealing(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets, string &seedFiles)
{
	vector<MoleculeSet*> emptyMoleculeSets; // This is only created here so we have something to pass in for
	                                        // bestIndividualMoleculeSets which is used in PSO, not simulated annealing.
	vector<MoleculeSet*> optimizedMoleculeSets;
	MoleculeSet *pMoleculeSet;
	int i;
	long iTotalAcceptedTransitions, iTotalTransitions; // totals for the past input.m_iNumIterationsBeforeDecreasingTemp iterations
	FLOAT fAcceptanceRatio; // percentage of accepted transitions
	FLOAT fSQRTQuenchingFactor;
	string tempResumeFileName = input.m_sResumeFileName + ".temp";
	char commandString[500];
	// Variables to keep track of how long this took
	time_t seconds;
	time_t minutes;
	time_t hours;
	time_t days;

	FLOAT fPercentConverged;
	ofstream fout;
	bool bErrorToStopFor = false;
	int energyCalculationType;
	bool success;
	vector<MoleculeSet*> moleculeSetsTransformed;
	int tempAccepted = 0;
	
	FLOAT deltaEnergy, p;
	
	// Variables for setting the scaling factor
	int numIterationsToSetTemp;
	int setTemperatureEvery;
	vector<FLOAT> positiveDeltaEnergies;
	int numWithNegativeDeltaEnergy = 0;
	FLOAT percentToCutOff = 0;
	FLOAT lowTemp, highTemp;
	FLOAT newTemp = 0;
	FLOAT myFloat;
	
	int iNumTransitions;

	if (input.m_bPerformBasinHopping)
		energyCalculationType = OPTIMIZE_BUT_DONT_READ;
	else if (input.m_bTransitionStateSearch)
		energyCalculationType = TRANSITION_STATE_SEARCH;
	else
		energyCalculationType = SINGLE_POINT_ENERGY_CALCULATION;
	
	
	if (!input.m_bResumeFileRead) {
		seconds = time (NULL);
		fout.open(input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
		
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			bErrorToStopFor = true;
		} else {
			input.printToFile(fout);
			fout << endl << endl;
			fout << "Output from program:" << endl;
			if (seedFiles.length() > 0) {
				cout << "Using seeded population from " << seedFiles << "..." << endl;
				fout << "Using seeded population from " << seedFiles << "..." << endl;
				if (input.m_iFreezeUntilIteration > 0) {
					cout << "Assigning frozen status to the coordinates of seeded atoms for " << input.m_iFreezeUntilIteration << " iterations..." << endl;
					fout << "Assigning frozen status to the coordinates of seeded atoms for " << input.m_iFreezeUntilIteration << " iterations..." << endl;
				} else {
					for (i = 0; i < (signed int)moleculeSets.size(); ++i)
						moleculeSets[i]->unFreezeAll(-1,-1);
				}
			} else {
				cout << "Initializing the population..." << endl;
				fout << "Initializing the population..." << endl;
				if (!initializePopulation(input, moleculeSets))
					bErrorToStopFor = true;
			}
			if (!bErrorToStopFor) {
				Energy::calculateEnergies(energyCalculationType, input, moleculeSets, optimizedMoleculeSets);
				input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
				if (input.m_bTransitionStateSearch) {
					if (optimizedMoleculeSets.size() > 0)
						Gega::saveBestN(optimizedMoleculeSets, bestNMoleculeSets,
								input.m_iNumberOfBestStructuresToSave,
								input.m_fMinDistnaceBetweenSameMoleculeSets,
								input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory);
					for (i = 0; i < (signed int)optimizedMoleculeSets.size(); ++i)
						delete optimizedMoleculeSets[i];
					optimizedMoleculeSets.clear();
				} else {
					Gega::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
						input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
                                                input.m_sSaveLogFilesInDirectory);
				}
				if (input.m_bTestMode)
					input.printTestFileHeader(0,*moleculeSets[0]);
			}
			
			input.m_iAcceptedTransitionsIndex = -1;
			iTotalAcceptedTransitions = 0;
			iTotalTransitions = 0;
			if (input.m_bPerformBasinHopping && (input.m_fQuenchingFactor == 1.0)) {
				cout << "Performing Basin Hopping..." << endl;
				fout << "Performing Basin Hopping..." << endl;
			} else if (input.m_bTransitionStateSearch) {
				cout << "Searching for Transition States..." << endl;
				fout << "Searching for Transition States..." << endl;
			} else {
				cout << "Simulating Annealing..." << endl;
				fout << "Simulating Annealing..." << endl;
			}
		}
	} else {
		seconds = time (NULL) - input.m_tElapsedSeconds;
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			bErrorToStopFor = true;
		} else
			fout << "Resuming after program execution was stopped..." << endl;
		iTotalAcceptedTransitions = 0;
		if (input.m_iIteration < input.m_iNumIterationsBeforeDecreasingTemp) {
			iTotalTransitions = input.m_iIteration * moleculeSets.size();
			for (i = 0; i < input.m_iIteration; ++i)
				iTotalAcceptedTransitions += input.m_prgAcceptedTransitions[i];
		} else {
			iTotalTransitions = input.m_iNumIterationsBeforeDecreasingTemp * moleculeSets.size();
			for (i = 0; i < input.m_iNumIterationsBeforeDecreasingTemp; ++i)
				iTotalAcceptedTransitions += input.m_prgAcceptedTransitions[i];
		}
	}
	
	numIterationsToSetTemp = 2000/(signed int)moleculeSets.size();
	if (numIterationsToSetTemp < 200)
		numIterationsToSetTemp = 200;
	setTemperatureEvery = 200 / (signed int)moleculeSets.size();
	if (setTemperatureEvery < 20)
		setTemperatureEvery = 20;
	
	if (!bErrorToStopFor) {
		// Perform Simulated Annealing
		fSQRTQuenchingFactor = sqrt(input.m_fQuenchingFactor);
		sprintf(commandString,"mv %s %s", tempResumeFileName.c_str(), input.m_sResumeFileName.c_str());
		
		do {
			++input.m_iIteration;
			if (input.m_bDecreasingTemp)
			{
				input.m_fStartingTemperature *= input.m_fQuenchingFactor;
				input.m_fNumPerterbations *= fSQRTQuenchingFactor;
				input.m_fStartCoordinatePerturbation *= fSQRTQuenchingFactor;
				if (input.m_fStartCoordinatePerturbation < input.m_fMinCoordinatePerturbation)
					input.m_fStartCoordinatePerturbation = input.m_fMinCoordinatePerturbation;
				input.m_fStartAnglePerturbation *= fSQRTQuenchingFactor;
				if (input.m_fStartAnglePerturbation < input.m_fMinAnglePerturbation)
					input.m_fStartAnglePerturbation = input.m_fMinAnglePerturbation;
			}
			
			// Perform perturbations
			iNumTransitions = (int)input.m_fNumPerterbations;
			if (input.m_iIteration <= input.m_iFreezeUntilIteration)
				iNumTransitions = 1;
			if (iNumTransitions < 1)
				iNumTransitions = 1;
			for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
				pMoleculeSet = new MoleculeSet();
				pMoleculeSet->copy(*moleculeSets[i]);
				do {
					if (input.m_bPerformNonFragSearch)
						success = pMoleculeSet->performTransformationsNonFrag(input.m_boxDimensions,
						                  input.m_fStartCoordinatePerturbation, input.m_fStartAnglePerturbation,
						                  input.m_fMaxAtomDistance, iNumTransitions);
					else
						success = pMoleculeSet->performTransformations(input.m_boxDimensions,
						                  input.m_fStartCoordinatePerturbation, input.m_fStartAnglePerturbation, iNumTransitions);
				} while (!success);
				moleculeSetsTransformed.push_back(pMoleculeSet);
			}
			fPercentConverged = Energy::calculateEnergies(energyCalculationType, input, moleculeSetsTransformed, optimizedMoleculeSets);
			input.m_iNumEnergyEvaluations += moleculeSets.size();
			
			if (!input.m_bTransitionStateSearch && (input.m_fDesiredAcceptedTransitions > 0) &&
			    (input.m_iIteration <= (numIterationsToSetTemp+input.m_iFreezeUntilIteration))) {
				for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
					deltaEnergy = moleculeSetsTransformed[i]->getEnergy() - moleculeSets[i]->getEnergy();
					if (deltaEnergy > 0)
						positiveDeltaEnergies.push_back(deltaEnergy);
					else
						++numWithNegativeDeltaEnergy;
				}

				if ((input.m_iIteration % setTemperatureEvery == 0) || (input.m_fStartingTemperature == 0)) {
					percentToCutOff = (input.m_fDesiredAcceptedTransitions *
					                   (positiveDeltaEnergies.size() + numWithNegativeDeltaEnergy)
					                  -numWithNegativeDeltaEnergy) / (FLOAT)positiveDeltaEnergies.size();
					if (percentToCutOff < 0) {
						newTemp = 100; // something small
					} else if (positiveDeltaEnergies.size() == 0) {
						// It doesn't matter in this case
						newTemp = -1.0 / (log(percentToCutOff) * input.m_fStartingTemperature);
					} else if (positiveDeltaEnergies.size() == 1) {
						newTemp = -positiveDeltaEnergies[0] /
					                   (log(percentToCutOff) * input.m_fStartingTemperature);
					} else {
						myFloat = 1e30;
						for (i = 0; i < (signed int)positiveDeltaEnergies.size(); ++i)
							if (positiveDeltaEnergies[i] < myFloat)
								myFloat = positiveDeltaEnergies[i];
						lowTemp = -myFloat / (log(percentToCutOff) * input.m_fBoltzmanConstant);
						while (percentToCutOff*positiveDeltaEnergies.size() <
						       probabilitySum(positiveDeltaEnergies,input.m_fBoltzmanConstant,lowTemp))
							lowTemp *= 0.5;
						
						myFloat = -1;
						for (i = 0; i < (signed int)positiveDeltaEnergies.size(); ++i)
							if (positiveDeltaEnergies[i] > myFloat)
								myFloat = positiveDeltaEnergies[i];

						highTemp = -myFloat / (log(percentToCutOff) * input.m_fBoltzmanConstant);
						while (percentToCutOff*positiveDeltaEnergies.size() >
						        probabilitySum(positiveDeltaEnergies,input.m_fBoltzmanConstant,highTemp))
							highTemp *= 2;
						i = 0;
						while (true) {
							++i;
							newTemp = (lowTemp + highTemp) * 0.5;
							if (i > 1000)
								break;
							if (percentToCutOff*positiveDeltaEnergies.size() >
							    probabilitySum(positiveDeltaEnergies, input.m_fBoltzmanConstant, newTemp))
								lowTemp = newTemp;
							else
								highTemp = newTemp;
						}
					}
					positiveDeltaEnergies.clear();
					numWithNegativeDeltaEnergy = 0;
					if ((input.m_iIteration == (numIterationsToSetTemp+input.m_iFreezeUntilIteration)) ||
					    (input.m_fStartingTemperature == 0)) {
						fout << "Setting the temperature to " << Atom::printFloat(newTemp) << "..." << endl;
						cout << "Setting the temperature to " << Atom::printFloat(newTemp) << "..." << endl;
					}
					input.m_fStartingTemperature = newTemp;
				}
			}
			
			tempAccepted = 0;
			for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
				deltaEnergy = moleculeSetsTransformed[i]->getEnergy() - moleculeSets[i]->getEnergy();
				success = false;
				if (input.m_bTransitionStateSearch || (deltaEnergy <= 0))
					success = true;
				else {
					p = exp(-deltaEnergy / (input.m_fBoltzmanConstant * input.m_fStartingTemperature));
					if (Molecule::randomFloat(0, 1) <= p)
						success = true;
				}
				if (success) {
					++tempAccepted;
					delete moleculeSets[i];
					moleculeSets[i] = moleculeSetsTransformed[i];
				} else
					delete moleculeSetsTransformed[i];
			}
			moleculeSetsTransformed.clear();
			
			// Count the number of accepted transitions
			++input.m_iAcceptedTransitionsIndex;
			if (input.m_iAcceptedTransitionsIndex >= input.m_iNumIterationsBeforeDecreasingTemp)
				input.m_iAcceptedTransitionsIndex = 0;
			if (input.m_iIteration > input.m_iNumIterationsBeforeDecreasingTemp) {
				iTotalAcceptedTransitions -= input.m_prgAcceptedTransitions[input.m_iAcceptedTransitionsIndex];
				iTotalTransitions -= moleculeSets.size();
			}
			input.m_prgAcceptedTransitions[input.m_iAcceptedTransitionsIndex] = tempAccepted;
			iTotalAcceptedTransitions += input.m_prgAcceptedTransitions[input.m_iAcceptedTransitionsIndex];
			iTotalTransitions += moleculeSets.size();
			
			fAcceptanceRatio = (FLOAT)iTotalAcceptedTransitions / (FLOAT)iTotalTransitions;

			if (input.m_bTransitionStateSearch) {
				if (optimizedMoleculeSets.size() > 0)
					Gega::saveBestN(optimizedMoleculeSets, bestNMoleculeSets,
						input.m_iNumberOfBestStructuresToSave,
						input.m_fMinDistnaceBetweenSameMoleculeSets,
						input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory);
				for (i = 0; i < (signed int)optimizedMoleculeSets.size(); ++i)
					delete optimizedMoleculeSets[i];
				optimizedMoleculeSets.clear();
			} else {
				Gega::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
					input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
					input.m_sSaveLogFilesInDirectory);
			}
			
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				fout << "It: " << input.m_iIteration;
				if (input.m_bTransitionStateSearch)
					fout << " Number of Transition States Found: " << bestNMoleculeSets.size();
				fout << setiosflags(ios::fixed) << setprecision(8);
				if (bestNMoleculeSets.size() > 0)
					fout << " Best Energy: " << bestNMoleculeSets[0]->getEnergy();
				else
					fout << " Best Energy: -";
				if (!input.m_bTransitionStateSearch) {
					fout << setiosflags(ios::fixed) << setprecision(1)
					     << " Temp: " << input.m_fStartingTemperature;
					fout << " Num Pert: " << iNumTransitions;
					fout << setiosflags(ios::fixed) << setprecision(4)
					     << " Coord, Angle Pert: " << input.m_fStartCoordinatePerturbation << ", "
					     << input.m_fStartAnglePerturbation * RAD_TO_DEG;
					fout << setiosflags(ios::fixed) << setprecision(1)
					     << " Accepted Pert: " << (fAcceptanceRatio*100) << "%";
				}
				fout << endl;
//				fout << setiosflags(ios::fixed) << setprecision(1)
//				     << " Converged: " << (fPercentConverged * 100) << "%" << endl;
				
				cout << "It: " << input.m_iIteration;
				if (input.m_bTransitionStateSearch)
					cout << " # Transition States Found: " << bestNMoleculeSets.size();
				cout << setiosflags(ios::fixed) << setprecision(8);
				if (bestNMoleculeSets.size() > 0)
					cout << " Best Energy: " << bestNMoleculeSets[0]->getEnergy();
				else
					cout << " Best Energy: -";
				if (!input.m_bTransitionStateSearch) {
					cout << setiosflags(ios::fixed) << setprecision(1)
					     << " Temp: " << input.m_fStartingTemperature;
					cout << " Num Pert: " << iNumTransitions;
					cout << setiosflags(ios::fixed) << setprecision(4)
					     << " Coord, Angle Pert: " << input.m_fStartCoordinatePerturbation << ", "
					     << input.m_fStartAnglePerturbation * RAD_TO_DEG;
					cout << setiosflags(ios::fixed) << setprecision(1)
					     << " Accepted Pert: " << (fAcceptanceRatio*100) << "%";
				}
				cout << endl;
//				cout << " Converged: " << (fPercentConverged * 100) << "%" << endl;
			}
			
			if (input.m_iFreezeUntilIteration == input.m_iIteration) {
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->unFreezeAll(-1,-1);
				cout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
				fout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
				if (input.m_fDesiredAcceptedTransitions > 0)
					input.m_fStartingTemperature = 0;
			}
			// Print the structures
/*			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			{
				fout << "Structure: " << i+1 << endl;
				moleculeSets[i]->print(fout);
				fout << "Energy = ";
				fout << setiosflags(ios::fixed) << setprecision(8) << moleculeSets[i]->getEnergy() << endl << endl;
			}*/
			
			if (((fPercentConverged*100) < MIN_CONVERGENCE_PERCENT) && ((signed int)moleculeSets.size() >= 10)) {
				cout << "The pecentage of successful energy calculations performed is below " << setprecision(2) << MIN_CONVERGENCE_PERCENT << "%.  Something may be wrong.  Exiting..." << endl;
				exit(0);
			}
			
			if (!input.m_bDecreasingTemp)
			{
				if ((input.m_fQuenchingFactor != 1.0) && !input.m_bTransitionStateSearch)
				{
					if ((input.m_iIteration >= input.m_iNumIterationsBeforeDecreasingTemp) &&
					    (fAcceptanceRatio <= input.m_fAcceptanceRatio))
					{
						input.m_bDecreasingTemp = true;
						fout << "Decreasing Temperature..." << endl;
						cout << "Decreasing Temperature..." << endl;
					}
				}
			}
			
			if ((input.m_sResumeFileName.length() > 0) &&
			    ((input.m_iIteration % input.m_iResumeFileNumIterations) == 0))
			{
				input.writeResumeFile(tempResumeFileName, moleculeSets, bestNMoleculeSets, emptyMoleculeSets,
				                      time (NULL) - seconds, true);
				if (system(commandString)) {	
					cout << "Error copying the temporary resume file(" << tempResumeFileName
					     << ") to the real resume file." << endl;
					break;
				}
			}
			
			if (input.m_iIteration >= input.m_iMaxIterations)
				break;
			if (input.m_bDecreasingTemp && (input.m_fStartingTemperature < input.m_fMinTemperatureToStop) &&
			    (fAcceptanceRatio < input.m_fMinAcceptedTransitions))
				break;
			
			if (input.m_bTestMode)
				input.printTestFileGeometry(input.m_iIteration, *moleculeSets[0]);
		} while (true);
		
		if (!bErrorToStopFor)
		{ 
			cout << "Finished after " << input.m_iIteration << " iterations at a temperature of " << input.m_fStartingTemperature << "." << endl;
			if ((input.m_iEnergyFunction == LENNARD_JONES) && !input.m_bTransitionStateSearch) {
				cout << "Optimizing best structure..." << endl;
				fout << "Optimizing best structure..." << endl;
				emptyMoleculeSets.push_back(bestNMoleculeSets[0]);
				fPercentConverged = Energy::calculateEnergies(OPTIMIZE_AND_READ, input, emptyMoleculeSets, optimizedMoleculeSets);
			
				bestNMoleculeSets[0]->copy(*optimizedMoleculeSets[0]);
				delete optimizedMoleculeSets[0];
				optimizedMoleculeSets.clear();
				emptyMoleculeSets.clear();
			}
			
			fout << endl << "Best structure:" << endl;
			cout << endl << "Best structure:" << endl;
			bestNMoleculeSets[0]->printToScreen();
			bestNMoleculeSets[0]->print(fout);
			cout << setiosflags(ios::fixed) << setprecision(8) << "Energy: " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
			fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
	
			seconds = time (NULL) - seconds;
			days = seconds / (24*60*60);
			seconds = seconds - days * (24*60*60);
			hours = seconds / (60*60);
			seconds = seconds - hours * (60*60);
			minutes = seconds / 60;
			seconds = seconds - minutes * 60;
			fout << "Number of times the energy was calculated: " << input.m_iNumEnergyEvaluations << endl;
			cout << "Number of times the energy was calculated: " << input.m_iNumEnergyEvaluations << endl;
			fout << "Finished in " << days << " days, " << hours << " hours, "
			     << minutes << " minutes, " << seconds << " seconds." << endl;
			cout << "Finished in " << days << " days, " << hours << " hours, "
			     << minutes << " minutes, " << seconds << " seconds." << endl;
		}
	}
	
   	// Clean up
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		delete moleculeSets[i];
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	moleculeSets.clear();
	bestNMoleculeSets.clear();
	
	fout.close();
	if (input.m_bTestMode)
		input.printTestFileFooter();
}

FLOAT probabilitySum(vector<FLOAT> &deltaEnergies, FLOAT scalingFactor, FLOAT temperature)
{
	FLOAT p = 0;
	for (int i = 0; i < (signed int)deltaEnergies.size(); ++i)
		p += exp(-deltaEnergies[i]/(scalingFactor*temperature));
	return p;
}


void particleSwarmOptimization(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets,
                               vector<MoleculeSet*> &bestIndividualMoleculeSets, string &seedFiles)
{
	vector<MoleculeSet*> moleculeSetsMinDistEnforced; // a version of the population (moleculeSets) in which the min. distance constraint is enforced
	vector<MoleculeSet*> localBestMoleculeSets;
	vector<MoleculeSet*> optimizedMoleculeSets;
	MoleculeSet* newMoleculeSet;
	int i;
	string tempResumeFileName = input.m_sResumeFileName + ".temp";
	char commandString[500];
	// Variables to keep track of how long this took
	time_t seconds;
	time_t minutes;
	time_t hours;
	time_t days;
	// Velocity Statistics
	FLOAT maxCoordinateVelocity;
	FLOAT avgCoordinateVelocity;
	FLOAT maxAngularVelocity;
	FLOAT avgAngularVelocity;

	FLOAT fCoordInertia;
	FLOAT fAngleInertia;
	
	FLOAT fPercentConverged = 1;
	ofstream fout;
	FLOAT fDecreaseCoordInertiaConstant = 0;
	FLOAT fDecreaseAngleInertiaConstant = 0;
	FLOAT visibility = 0;
	FLOAT **particleDistanceMatrix;
	FLOAT diversity = 0;
	FLOAT fCubeDiagonal = sqrt(3 * input.m_boxDimensions.x * input.m_boxDimensions.x);
	int iSwitchToAttractionReplaceBest;
	FLOAT tempFloat;
	bool bErrorToStopFor = false;
	int energyCalculationType;
	int iMoleculesWithMultipleAtoms;
	int iRandomMolecule;
	
	if (input.m_bUseLocalOptimization)
		energyCalculationType = OPTIMIZE_BUT_DONT_READ;
	else
		energyCalculationType = SINGLE_POINT_ENERGY_CALCULATION;
	
	if (input.m_iReachEndInertiaAtIteration != 0) {
		fDecreaseCoordInertiaConstant = (input.m_fEndCoordInertia - input.m_fStartCoordInertia) /
			(FLOAT)input.m_iReachEndInertiaAtIteration;
		fDecreaseAngleInertiaConstant = (input.m_fEndAngleInertia - input.m_fStartAngleInertia) /
			(FLOAT)input.m_iReachEndInertiaAtIteration;
	}
	
	if (!input.m_bResumeFileRead) {
		seconds = time (NULL);
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			bErrorToStopFor = true;
		} else {
			input.printToFile(fout);
			fout << endl << endl;
			fout << "Output from program:" << endl;
			if (seedFiles.length() > 0) {
				cout << "Using seeded population from " << seedFiles << "..." << endl;
				fout << "Using seeded population from " << seedFiles << "..." << endl;
				if (input.m_iFreezeUntilIteration > 0) {
					cout << "Assigning frozen status to the coordinates of seeded atoms for " << input.m_iFreezeUntilIteration << " iterations..." << endl;
					fout << "Assigning frozen status to the coordinates of seeded atoms for " << input.m_iFreezeUntilIteration << " iterations..." << endl;
				} else {
					for (i = 0; i < (signed int)moleculeSets.size(); ++i)
						moleculeSets[i]->unFreezeAll(input.m_fCoordMaximumVelocity,input.m_fAngleMaximumVelocity);
				}
			} else {
				cout << "Initializing the population..." << endl;
				fout << "Initializing the population..." << endl;
				if (!initializePopulation(input, moleculeSets))
					bErrorToStopFor = true;
			}
			if (!bErrorToStopFor) {
				Energy::calculateEnergies(energyCalculationType, input, moleculeSets, optimizedMoleculeSets);
				input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
				Gega::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
				                input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
				                input.m_sSaveLogFilesInDirectory);
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->initVelocities(input.m_fCoordMaximumVelocity,input.m_fAngleMaximumVelocity);
				if (input.m_bTestMode) {
					iRandomMolecule = Molecule::randomInt(0,moleculeSets.size()-1);
					input.printTestFileHeader(0, *moleculeSets[iRandomMolecule]);
				}
			}
		}
		
		for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(*moleculeSets[i]);
			bestIndividualMoleculeSets.push_back(newMoleculeSet);
			bestIndividualMoleculeSets[i]->centerInBox(input.m_boxDimensions);
		}
		
		cout << "Performing Particle Swarm Optimization..." << endl;
		fout << "Performing Particle Swarm Optimization..." << endl;
	} else {
		seconds = time (NULL) - input.m_tElapsedSeconds;
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			bErrorToStopFor = true;
		} else
			fout << "Resuming after program execution was stopped..." << endl;
//		deleteEnergyFiles(moleculeSets, energyObjects, input.m_srgNodeNames.size(), input.m_bUsePrevWaveFunction);
	}
	
	particleDistanceMatrix = new FLOAT*[moleculeSets.size()];
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		particleDistanceMatrix[i] = new FLOAT[moleculeSets.size()];
	
	if (input.m_bEnforceMinDistOnCopy)
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			moleculeSetsMinDistEnforced.push_back(new MoleculeSet(i+1));
	
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		localBestMoleculeSets.push_back(new MoleculeSet(i+1));
	
	iMoleculesWithMultipleAtoms = moleculeSets[0]->getNumberOfMoleculesWithMultipleAtoms();
	if (iMoleculesWithMultipleAtoms == 0)
		iMoleculesWithMultipleAtoms = 1;
	
	if (!bErrorToStopFor) {
		// Perform particle swarm optimization
		
		sprintf(commandString,"mv %s %s", tempResumeFileName.c_str(), input.m_sResumeFileName.c_str());
		do {
			++input.m_iIteration;
			
			getPopulationDistanceMatrix(moleculeSets, particleDistanceMatrix);
			diversity = calculateDiversity((signed int)moleculeSets.size(),particleDistanceMatrix,fCubeDiagonal);
			if (input.m_fSwitchToRepulsionWhenDiversityIs > 0) {
				if ((input.m_fAttractionRepulsion > 0) &&
				    (diversity <= input.m_fSwitchToRepulsionWhenDiversityIs) && (visibility > 0.75) &&
				    (input.m_iNumIterationsBestEnergyHasntChanged >= input.m_iSwitchToRepulsionWhenNoProgress)) {
					cout << "Switching to repulsion phase..." << endl;
					fout << "Switching to repulsion phase..." << endl;
					input.m_fAttractionRepulsion = -1;
					cout << setiosflags(ios::fixed) << setprecision(3) << "Resetting visibility distance from "
					     << input.m_fVisibilityDistance << " to " << input.m_fStartVisibilityDistance << endl;
					fout << setiosflags(ios::fixed) << setprecision(3) << "Resetting visibility distance from "
					     << input.m_fVisibilityDistance << " to " << input.m_fStartVisibilityDistance << endl;
					input.m_fVisibilityDistance = input.m_fStartVisibilityDistance;
				}
				if ((input.m_fAttractionRepulsion < 0) && (diversity >= input.m_fSwitchToAttractionWhenDiversityIs)) {
					cout << "Switching to attraction phase..." << endl;
					fout << "Switching to attraction phase..." << endl;
					input.m_fAttractionRepulsion = 1;
					
					// Reset the best solution seen by each particle to solutions in bestNMoleculeSets
					iSwitchToAttractionReplaceBest = input.m_iTotalPopulationSize;
					if (iSwitchToAttractionReplaceBest > (signed int)bestNMoleculeSets.size())
						iSwitchToAttractionReplaceBest = (signed int)bestNMoleculeSets.size();
					if (iSwitchToAttractionReplaceBest > (signed int)moleculeSets.size())
						iSwitchToAttractionReplaceBest = (signed int)moleculeSets.size();
					for (i = 0; i < iSwitchToAttractionReplaceBest; ++i) {
						bestIndividualMoleculeSets[i]->copy(*bestNMoleculeSets[i]);
						bestIndividualMoleculeSets[i]->centerInBox(input.m_boxDimensions);
					}
					for (i = iSwitchToAttractionReplaceBest; i < (signed int)moleculeSets.size(); ++i) {
						bestIndividualMoleculeSets[i]->copy(*moleculeSets[i]);
						bestIndividualMoleculeSets[i]->setEnergy(1234567);
						bestIndividualMoleculeSets[i]->centerInBox(input.m_boxDimensions);
					}
					input.m_iNumIterationsBestEnergyHasntChanged = 0;
				}
			}
			
			visibility = findLocalBestMoleculeSets(moleculeSets, bestIndividualMoleculeSets,
			                           localBestMoleculeSets, particleDistanceMatrix, input.m_fVisibilityDistance, input.m_boxDimensions);
			
			if (input.m_iReachEndInertiaAtIteration != 0) {
				if (input.m_iIteration < input.m_iReachEndInertiaAtIteration) {
					fCoordInertia = fDecreaseCoordInertiaConstant * input.m_iIteration
					                       + input.m_fStartCoordInertia;
					fAngleInertia = fDecreaseAngleInertiaConstant * input.m_iIteration
					                       + input.m_fStartAngleInertia;
				} else {
					fCoordInertia = input.m_fEndCoordInertia;
					fAngleInertia = input.m_fEndAngleInertia;
				}
			}
			
			// Do the particle swarm
			for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
				moleculeSets[i]->performPSO(*localBestMoleculeSets[i], *bestIndividualMoleculeSets[i], fCoordInertia,
				                            input.m_fCoordIndividualMinimumAttraction, input.m_fCoordPopulationMinimumAttraction,
				                            input.m_fCoordMaximumVelocity, fAngleInertia, input.m_fAngleIndividualMinimumAttraction,
				                            input.m_fAnglePopulationMinimumAttraction, input.m_fAngleMaximumVelocity,
				                            input.m_boxDimensions, !input.m_bEnforceMinDistOnCopy, input.m_fAttractionRepulsion);
			}
			
			
			if (input.m_fAttractionRepulsion > 0) {
				if (input.m_bEnforceMinDistOnCopy) {
					for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
						moleculeSetsMinDistEnforced[i]->copy(*moleculeSets[i]);
						moleculeSetsMinDistEnforced[i]->enforceMinDistConstraints(input.m_boxDimensions);
					}
					fPercentConverged = Energy::calculateEnergies(energyCalculationType, input, moleculeSetsMinDistEnforced, optimizedMoleculeSets);
					for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
						moleculeSets[i]->setEnergy(moleculeSetsMinDistEnforced[i]->getEnergy());
						if (moleculeSetsMinDistEnforced[i]->getEnergy() < bestIndividualMoleculeSets[i]->getEnergy()) {
							if (!moleculeSetsMinDistEnforced[i]->withinDistance(*localBestMoleculeSets[i],
							                                                    input.m_fIndividualBestUpdateDist)) {
								bestIndividualMoleculeSets[i]->copy(*moleculeSetsMinDistEnforced[i]);
								bestIndividualMoleculeSets[i]->centerInBox(input.m_boxDimensions);
							}
						}
					}
				} else {
					fPercentConverged = Energy::calculateEnergies(energyCalculationType, input, moleculeSets, optimizedMoleculeSets);
					for (i = 0; i < (signed int)moleculeSets.size(); ++i)
						if (moleculeSets[i]->getEnergy() < bestIndividualMoleculeSets[i]->getEnergy()) {
							if (!moleculeSets[i]->withinDistance(*localBestMoleculeSets[i],
							                                     input.m_fIndividualBestUpdateDist)) {
								bestIndividualMoleculeSets[i]->copy(*moleculeSets[i]);
								bestIndividualMoleculeSets[i]->centerInBox(input.m_boxDimensions);
							}
						}
				}
			}
			
			// Get velocity statistics
			if (input.m_fAttractionRepulsion > 0)
				input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
			
			maxCoordinateVelocity = 0;
			avgCoordinateVelocity = 0;
			maxAngularVelocity = 0;
			avgAngularVelocity = 0;
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				moleculeSets[i]->getPSOVelocityStats(maxCoordinateVelocity, avgCoordinateVelocity,
				                                     maxAngularVelocity, avgAngularVelocity);
			
			avgCoordinateVelocity /= moleculeSets[0]->getNumberOfMolecules() * moleculeSets.size();
			avgAngularVelocity /= iMoleculesWithMultipleAtoms * 3 * moleculeSets.size();
			
			
			// Print the structures
/*			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			{
				fout << "Structure: " << i+1 << endl;
				moleculeSets[i]->print(fout);
				fout << "Energy = ";
				fout << setiosflags(ios::fixed) << setprecision(8) << moleculeSets[i]->getEnergy() << endl << endl;
			}*/
            		
			if (input.m_fAttractionRepulsion > 0) {
				if (bestNMoleculeSets.size() > 0)
					tempFloat = bestNMoleculeSets[0]->getEnergy();
				else
					tempFloat = 0;
				if (input.m_bEnforceMinDistOnCopy)
					Gega::saveBestN(moleculeSetsMinDistEnforced, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
					                input.m_fMinDistnaceBetweenSameMoleculeSets, input.m_iNumberOfLogFilesToSave,
				                        input.m_sSaveLogFilesInDirectory);
				else
					Gega::saveBestN(moleculeSets, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
					                input.m_fMinDistnaceBetweenSameMoleculeSets, input.m_iNumberOfLogFilesToSave,
				                        input.m_sSaveLogFilesInDirectory);
				if (abs(tempFloat - bestNMoleculeSets[0]->getEnergy()) < 0.001)
					++input.m_iNumIterationsBestEnergyHasntChanged;
				else
					input.m_iNumIterationsBestEnergyHasntChanged = 0;
			}
			
			fout << "It: " << input.m_iIteration;
			fout << setiosflags(ios::fixed) << setprecision(8)
			     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy() << setprecision(3)
				 << " Coord Vel Max,Avg: " << maxCoordinateVelocity << ", " << avgCoordinateVelocity
				 << " Angle Vel Max,Avg: " << (maxAngularVelocity * RAD_TO_DEG) << ", "
				 << (avgAngularVelocity * RAD_TO_DEG)
			     << setiosflags(ios::fixed) << setprecision(3)
			     << " Div: " << diversity
			     << setiosflags(ios::fixed) << setprecision(1)
			     << " Vis: " << (visibility * 100) << "%" << endl;
//			     << " Converged: " << (fPercentConverged * 100) << "%" << endl;
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				cout << "It: " << input.m_iIteration;
				cout << setiosflags(ios::fixed) << setprecision(8)
				     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy() << setprecision(3)
    				 << " Coord Vel Max,Avg: " << maxCoordinateVelocity << ", " << avgCoordinateVelocity
    				 << " Angle Vel Max,Avg: " << (maxAngularVelocity * RAD_TO_DEG) << ", "
    				 << (avgAngularVelocity * RAD_TO_DEG)
				     << setiosflags(ios::fixed) << setprecision(3)
				     << " Div: " << diversity
				     << setiosflags(ios::fixed) << setprecision(1)
				     << " Vis: " << (visibility * 100) << "%" << endl;
//				     << " Converged: " << (fPercentConverged * 100) << "%" << endl;
			}
			
			if (input.m_iFreezeUntilIteration == input.m_iIteration) {
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->unFreezeAll(input.m_fCoordMaximumVelocity,input.m_fAngleMaximumVelocity);
				cout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
				fout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
			}
			
			if (((fPercentConverged*100) < MIN_CONVERGENCE_PERCENT) && ((signed int)moleculeSets.size() >= 10)) {
				cout << "The pecentage of successful energy calculations performed is below " << setprecision(2) << MIN_CONVERGENCE_PERCENT << "%.  Something may be wrong.  Exiting..." << endl;
				exit(0);
			}
			
			if ((input.m_sResumeFileName.length() > 0) && ((input.m_iIteration%input.m_iResumeFileNumIterations) == 0))
			{
				input.writeResumeFile(tempResumeFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets, time (NULL) - seconds, true);
				if (system(commandString))
				{
					cout << "Error copying the temporary resume file(" << tempResumeFileName
					     << ") to the real resume file." << endl;
					break;
				}
			}
			
			if (input.m_bStartingVisibilityAuto && (input.m_fStartVisibilityDistance == -1)) {
				if (input.m_fCoordMaximumVelocity > 0)
					tempFloat = input.m_fCoordMaximumVelocity * 0.5;
				else
					tempFloat = 0.1;
				if (input.m_iIteration > input.m_iFreezeUntilIteration) {
					if ((input.m_iIteration >= 200) ||
					    (avgCoordinateVelocity <= 0.005) ||
					    ((input.m_iNumIterationsBestEnergyHasntChanged >= 70) &&
					     (avgCoordinateVelocity < tempFloat))) { 
						input.m_fStartVisibilityDistance = getVisibilityDistance(moleculeSets, particleDistanceMatrix, 0.0001) * 0.8;
						input.m_fVisibilityDistance = input.m_fStartVisibilityDistance;
						cout << setiosflags(ios::fixed) << setprecision(3)
						     << "Setting RMS visibility distance to: " << input.m_fVisibilityDistance << endl;
						fout << setiosflags(ios::fixed) << setprecision(3)
						     << "Setting RMS visibility distance to: " << input.m_fVisibilityDistance << endl;
					}
				}
			} else
				input.m_fVisibilityDistance += input.m_fVisibilityDistanceIncrease;
			
			if (input.m_iIteration >= input.m_iMaxIterations)
				break;
			
			if (input.m_bTestMode)
				input.printTestFileGeometry(input.m_iIteration, *moleculeSets[iRandomMolecule]);
		} while (true);
		
		if (!bErrorToStopFor) { 
			cout << "Finished after " << input.m_iIteration << " iterations." << endl;
			fout << "Finished after " << input.m_iIteration << " iterations." << endl;
			if (input.m_iEnergyFunction == LENNARD_JONES) {
				vector<MoleculeSet*> tempMoleculeSets;
				cout << "Optimizing best structure..." << endl;
				fout << "Optimizing best structure..." << endl;
				tempMoleculeSets.push_back(bestNMoleculeSets[0]);
				Energy::calculateEnergies(OPTIMIZE_AND_READ, input, tempMoleculeSets, optimizedMoleculeSets);
			
				bestNMoleculeSets[0]->copy(*optimizedMoleculeSets[0]);
				delete optimizedMoleculeSets[0];
				tempMoleculeSets.clear();
				optimizedMoleculeSets.clear();
			}
			
			fout << endl << "Best structure:" << endl;
			cout << endl << "Best structure:" << endl;
			bestNMoleculeSets[0]->printToScreen();
			bestNMoleculeSets[0]->print(fout);
			cout << setiosflags(ios::fixed) << setprecision(8) << "Energy: " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
			fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
		}
	}
   	// Clean up
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		delete[] particleDistanceMatrix[i];
	delete[] particleDistanceMatrix;
	
	for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
		delete moleculeSets[i];
		delete bestIndividualMoleculeSets[i];
	}
	if (input.m_bEnforceMinDistOnCopy)
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSetsMinDistEnforced[i];
	for (i = 0; i < (signed int)localBestMoleculeSets.size(); ++i)
			delete localBestMoleculeSets[i];
	moleculeSets.clear();
	bestIndividualMoleculeSets.clear();
	moleculeSetsMinDistEnforced.clear();
	localBestMoleculeSets.clear();

	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();
	
	seconds = time (NULL) - seconds;
	days = seconds / (24*60*60);
	seconds = seconds - days * (24*60*60);
	hours = seconds / (60*60);
	seconds = seconds - hours * (60*60);
	minutes = seconds / 60;
	seconds = seconds - minutes * 60;
	fout << "Number of times the energy was calculated: " << input.m_iNumEnergyEvaluations << endl;
	cout << "Number of times the energy was calculated: " << input.m_iNumEnergyEvaluations << endl;
	fout << "Finished in " << days << " days, " << hours << " hours, "
	     << minutes << " minutes, " << seconds << " seconds." << endl;
	cout << "Finished in " << days << " days, " << hours << " hours, "
	     << minutes << " minutes, " << seconds << " seconds." << endl;
	
	fout.close();
	if (input.m_bTestMode)
		input.printTestFileFooter();
}

void quickSort(FLOAT *array, int lo, int hi)
{
	int left, right;
	FLOAT median;
	FLOAT temp;
	
	if( hi > lo ) // if at least 2 elements, then
	{
		left=lo; right=hi;
		
		// We don't know what the median is, so we'll just pick something
		median = array[lo];
		
		while(right >= left)
		{
			while(array[left] < median)
				left++;
			while(array[right] > median)
				right--;
			
			if(left > right)
				break;
			
			// Swap
			temp=array[left];
			array[left]=array[right];
			array[right]=temp;
			
			left++;
			right--;
		}
		
		quickSort(array, lo, right);
		quickSort(array, left, hi);
	}
}

FLOAT getVisibilityDistance(vector<MoleculeSet*> &population, FLOAT **distanceMatrix, FLOAT desiredPercentVisibility)
{
	FLOAT *distances;
	FLOAT distance;
	int i, j;
	int count, totalComparisons;

	if (desiredPercentVisibility >= 1)
		return 1e100;
	if (desiredPercentVisibility <= 0)
		return 0;

	totalComparisons = (signed int)((population.size()-1) * population.size() / 2);
	distances = new FLOAT[totalComparisons];

	count = 0;
	for (i = 0; i < (signed int)population.size()-1; ++i)
		for (j = i+1; j < (signed int)population.size(); ++j)
			distances[count++] = distanceMatrix[i][j];

	quickSort(distances,0,totalComparisons-1);
	count = (int)(desiredPercentVisibility*totalComparisons);
	if (count >= totalComparisons)
		count = totalComparisons-1;
	if (count < 0)
		count = 0;
	distance = distances[count];
	delete[] distances;

	return distance;
}


FLOAT findLocalBestMoleculeSets(vector<MoleculeSet*> &population, vector<MoleculeSet*> bestIndividualMoleculeSets,
                               vector<MoleculeSet*> &localBestMoleculeSets, FLOAT **distanceMatrix, FLOAT distance, Point3D &boxDimensions)
{
	int i, j;
	int indexOfLocalBest;
	int withinDist, totalComparisons;
	
	withinDist = 0;
	totalComparisons = 0;
	for (i = 0; i < (signed int)population.size()-1; ++i)
		for (j = i+1; j < (signed int)population.size(); ++j) {
			if (distanceMatrix[i][j] <= distance)
				++withinDist;
			++totalComparisons;
		}
	
	for (i = 0; i < (signed int)population.size(); ++i) {
		indexOfLocalBest = i;
		for (j = 0; j < (signed int)population.size(); ++j)
			if ((i != j) && (distanceMatrix[i][j] <= distance))
				if (bestIndividualMoleculeSets[j]->getEnergy() <=
				    bestIndividualMoleculeSets[indexOfLocalBest]->getEnergy()) {
					indexOfLocalBest = j;
				}
		localBestMoleculeSets[i]->copy(*bestIndividualMoleculeSets[indexOfLocalBest]);
	}
	return (FLOAT)withinDist / (FLOAT)totalComparisons;
}

void getPopulationDistanceMatrix(vector<MoleculeSet*> &population, FLOAT **distanceMatrix)
{
	int i, j;
	FLOAT distance;
	
	for (i = 0; i < (signed int)population.size()-1; ++i)
		for (j = i+1; j < (signed int)population.size(); ++j) {
			distance = population[i]->getDistance(*population[j]);
			distanceMatrix[i][j] = distance;
			distanceMatrix[j][i] = distance;
		}
	for (i = 0; i < (signed int)population.size(); ++i)
		distanceMatrix[i][i] = 0;
}

FLOAT calculateDiversity(int populationSize, FLOAT **distanceMatrix, FLOAT cubeDiagonal)
{
	int i, j;
	FLOAT average = 0;
	int totalUniqueDistances = (populationSize-1) * populationSize / 2;
	
	for (i = 0; i < populationSize-1; ++i)
		for (j = i+1; j < populationSize; ++j)
			average += distanceMatrix[i][j];
	average /= totalUniqueDistances;
	average /= cubeDiagonal;
	return average;
}


void gega(Input &input, vector<MoleculeSet*> &population, vector<MoleculeSet*> &bestNMoleculeSets, string &seedFiles)
{
	vector<MoleculeSet*> emptyMoleculeSets; // This is only created here so we have something to pass in for
	                                        // bestIndividualMoleculeSets which is used in PSO.
	vector<MoleculeSet*> matingPool;
	vector<MoleculeSet*> offSpring;
	vector<MoleculeSet*> population2;
	vector<MoleculeSet*> *pPopulationA;
	vector<MoleculeSet*> *pPopulationB;
	vector<MoleculeSet*> optimizedMoleculeSets;
	int i;
	string tempResumeFileName = input.m_sResumeFileName + ".temp";
	char commandString[500];
	// Variables to keep track of how long this took
	time_t seconds;
	time_t minutes;
	time_t hours;
	time_t days;
	// Variables used in threads
	FLOAT fPercentConverged = 1;
	ofstream fout;
	bool bErrorToStopFor = false;
	int energyCalculationType;
	FLOAT **particleDistanceMatrix;
	FLOAT diversity = 0;
	FLOAT fCubeDiagonal = sqrt(3 * input.m_boxDimensions.x * input.m_boxDimensions.x);
	
	if (input.m_bUseLocalOptimization)
		energyCalculationType = OPTIMIZE_BUT_DONT_READ;
	else
		energyCalculationType = SINGLE_POINT_ENERGY_CALCULATION;
	
	if (!input.m_bResumeFileRead) {
		seconds = time (NULL);
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			bErrorToStopFor = true;
		} else {
			input.printToFile(fout);
			fout << endl << endl;
			fout << "Output from program:" << endl;
			if (seedFiles.length() > 0) {
				cout << "Using seeded population from " << seedFiles << "..." << endl;
				fout << "Using seeded population from " << seedFiles << "..." << endl;
				if (input.m_iFreezeUntilIteration > 0) {
					cout << "Warning: Freezing is not implemented with the genetic algorithm..." << endl;
					fout << "Warning: Freezing is not implemented with the genetic algorithm..." << endl;
					input.m_iFreezeUntilIteration = 0;
				}
				for (i = 0; i < (signed int)population.size(); ++i)
					population[i]->unFreezeAll(-1,-1);
			} else {
				cout << "Initializing the population..." << endl;
				fout << "Initializing the population..." << endl;
				if (!initializePopulation(input, population))
					bErrorToStopFor = true;
			}
			if (!bErrorToStopFor) {
				Energy::calculateEnergies(energyCalculationType, input, population, optimizedMoleculeSets);
				input.m_iNumEnergyEvaluations += (signed int)population.size();
				Gega::saveBestN(population,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
				                input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
				                input.m_sSaveLogFilesInDirectory);
				if (input.m_bTestMode)
					input.printTestFileHeader(0,*bestNMoleculeSets[0]);
			}
		}
		
		cout << "Performing Genetic Algorithm..." << endl;
		fout << "Performing Genetic Algorithm..." << endl;
	} else {
		seconds = time (NULL) - input.m_tElapsedSeconds;
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			bErrorToStopFor = true;
		} else
			fout << "Resuming after program execution was stopped..." << endl;
	}
	
	particleDistanceMatrix = new FLOAT*[population.size()];
	for (i = 0; i < (signed int)population.size(); ++i)
		particleDistanceMatrix[i] = new FLOAT[population.size()];

	if (!bErrorToStopFor) {
		pPopulationA = &population;
		pPopulationB = &population2;
		
		sprintf(commandString,"mv %s %s", tempResumeFileName.c_str(), input.m_sResumeFileName.c_str());
		do {
			++input.m_iIteration;
			Gega::tournamentMateSelection(*pPopulationA, matingPool);
			Gega::performMatings(matingPool, offSpring, input.m_iNumStructureTypes,
			                     input.m_iNumStructuresOfEachType, input.m_boxDimensions);
			
			matingPool.clear();
			fPercentConverged = Energy::calculateEnergies(energyCalculationType, input, offSpring, optimizedMoleculeSets);
			input.m_iNumEnergyEvaluations += (signed int)offSpring.size();
			
			Gega::saveBestN(offSpring, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
			                input.m_fMinDistnaceBetweenSameMoleculeSets, input.m_iNumberOfLogFilesToSave,
			                input.m_sSaveLogFilesInDirectory);
			Gega::generationReplacement(*pPopulationA, offSpring, *pPopulationB);
			
			getPopulationDistanceMatrix(*pPopulationB, particleDistanceMatrix);
			diversity = calculateDiversity((signed int)pPopulationB->size(),particleDistanceMatrix,fCubeDiagonal);
			
			fout << "It: " << input.m_iIteration;
			fout << setiosflags(ios::fixed) << setprecision(8)
			     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy()
			     << " Diversity: " << diversity << endl;
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				cout << "It: " << input.m_iIteration;
				cout << setiosflags(ios::fixed) << setprecision(8)
				     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy()
				     << " Diversity: " << diversity << endl;
			}
			
			if (input.m_iFreezeUntilIteration == input.m_iIteration) {
				for (i = 0; i < (signed int)pPopulationB->size(); ++i)
					(*pPopulationB)[i]->unFreezeAll(-1,-1);
				cout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
				fout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
			}
			
			if (((fPercentConverged*100) < MIN_CONVERGENCE_PERCENT) && ((signed int)population.size() >= 10)) {
				cout << "The pecentage of successful energy calculations performed is below " << setprecision(2) << MIN_CONVERGENCE_PERCENT << "%.  Something may be wrong.  Exiting..." << endl;
				exit(0);
			}
			
			if ((input.m_sResumeFileName.length() > 0) && ((input.m_iIteration%input.m_iResumeFileNumIterations) == 0))
			{
				input.writeResumeFile(tempResumeFileName, *pPopulationB, bestNMoleculeSets, emptyMoleculeSets, time (NULL) - seconds, true);
				if (system(commandString))
				{
					cout << "Error copying the temporary resume file(" << tempResumeFileName
					     << ") to the real resume file." << endl;
					break;
				}
			}
			
			if (pPopulationA == &population) {
				pPopulationA = &population2;
				pPopulationB = &population;
			} else {
				pPopulationA = &population;
				pPopulationB = &population2;
			}
			
			if (input.m_iIteration >= input.m_iMaxIterations)
				break;
			if (input.m_bTestMode)
				input.printTestFileGeometry(input.m_iIteration, *bestNMoleculeSets[0]);
		} while (true);
		
		if (!bErrorToStopFor) { 
			cout << "Finished after " << input.m_iIteration << " iterations." << endl;
			fout << "Finished after " << input.m_iIteration << " iterations." << endl;
			if (input.m_iEnergyFunction == LENNARD_JONES) {
				vector<MoleculeSet*> tempMoleculeSets;
				cout << "Optimizing best structure..." << endl;
				fout << "Optimizing best structure..." << endl;
				tempMoleculeSets.push_back(bestNMoleculeSets[0]);
				Energy::calculateEnergies(OPTIMIZE_AND_READ, input, tempMoleculeSets, optimizedMoleculeSets);
			
				bestNMoleculeSets[0]->copy(*optimizedMoleculeSets[0]);
				delete optimizedMoleculeSets[0];
				tempMoleculeSets.clear();
				optimizedMoleculeSets.clear();
			}
			
			fout << endl << "Best structure:" << endl;
			cout << endl << "Best structure:" << endl;
			bestNMoleculeSets[0]->printToScreen();
			bestNMoleculeSets[0]->print(fout);
			cout << setiosflags(ios::fixed) << setprecision(8) << "Energy: " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
			fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
		}
	}
	
   	// Clean up
	for (i = 0; i < (signed int)pPopulationA->size(); ++i)
		delete[] particleDistanceMatrix[i];
	delete[] particleDistanceMatrix;
	
	for (i = 0; i < (signed int)population.size(); ++i)
		delete population[i];
	for (i = 0; i < (signed int)population2.size(); ++i)
		delete population2[i];
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	population.clear();
	population2.clear();
	bestNMoleculeSets.clear();
	
	seconds = time (NULL) - seconds;
	days = seconds / (24*60*60);
	seconds = seconds - days * (24*60*60);
	hours = seconds / (60*60);
	seconds = seconds - hours * (60*60);
	minutes = seconds / 60;
	seconds = seconds - minutes * 60;
	fout << "Number of times the energy was calculated: " << input.m_iNumEnergyEvaluations << endl;
	cout << "Number of times the energy was calculated: " << input.m_iNumEnergyEvaluations << endl;
	fout << "Finished in " << days << " days, " << hours << " hours, "
	     << minutes << " minutes, " << seconds << " seconds." << endl;
	cout << "Finished in " << days << " days, " << hours << " hours, "
	     << minutes << " minutes, " << seconds << " seconds." << endl;
	
	fout.close();
	if (input.m_bTestMode)
		input.printTestFileFooter();
}

void optimizeBestStructures(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets)
{
	vector<MoleculeSet*> population;
	vector<MoleculeSet*> optimizedMoleculeSets;
	vector<MoleculeSet*> tempMoleculeSets;
	vector<MoleculeSet*> bestIndividualMoleculeSets;
	MoleculeSet* newMoleculeSet;
	FLOAT fPercentConverged;
	int i;
	int iStructuresToOptimizeAtATime;
	int energyCalculationType;
	// Variables to keep track of how long this took
	time_t seconds;
	time_t minutes;
	time_t hours;
	time_t days;
	string tempResumeFileName = input.m_sInputFileName + ".temp";
	char commandString[500];

	sprintf(commandString,"mv %s %s", tempResumeFileName.c_str(), input.m_sInputFileName.c_str());
	
	cout << setiosflags(ios::fixed) << setprecision(1);
	iStructuresToOptimizeAtATime = input.m_iStructuresToOptimizeAtATime;
	
	if (input.m_bTransitionStateSearch)
		energyCalculationType = TRANSITION_STATE_SEARCH;
	else
		energyCalculationType = OPTIMIZE_AND_READ;
	
	seconds = time (NULL) - input.m_tElapsedSeconds;
	
	if (moleculeSets.size() == 0) {
		cout << "There are no structures left to optimize." << endl;
		return;
	}
	
	cout << "Optimizing Structures..." << endl;
	while (moleculeSets.size() > 0) {
		if (iStructuresToOptimizeAtATime > (signed int)moleculeSets.size())
			iStructuresToOptimizeAtATime = moleculeSets.size();
		
		// Perform Energy calculations
		for (i = 0; i < iStructuresToOptimizeAtATime; ++i) {
			newMoleculeSet = new MoleculeSet();
		        newMoleculeSet->copy(*moleculeSets[i]);
		        population.push_back(newMoleculeSet);
		}
		fPercentConverged = Energy::calculateEnergies(energyCalculationType, input, population, optimizedMoleculeSets);
		if (((fPercentConverged*100) < MIN_CONVERGENCE_PERCENT) && ((signed int)moleculeSets.size() >= 10)) {
			cout << "The pecentage of successful energy calculations performed is below " << setprecision(2) << MIN_CONVERGENCE_PERCENT << "%.  Something may be wrong.  Exiting..." << endl;
			exit(0);
		}
		input.m_iNumEnergyEvaluations += (signed int)population.size();
		
		// Update the list of optimized best structures
		Gega::saveBestN(optimizedMoleculeSets, bestNMoleculeSets, (signed int)(optimizedMoleculeSets.size() +
		                bestNMoleculeSets.size()), input.m_fMinDistnaceBetweenSameMoleculeSets,
		                input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory);
		
		// Remove the optimized structures from moleculeSets
		for (i = 0; i < iStructuresToOptimizeAtATime; ++i)
			delete moleculeSets[i];
		for (i = iStructuresToOptimizeAtATime; i < (signed int)moleculeSets.size(); ++i)
			tempMoleculeSets.push_back(moleculeSets[i]);
		moleculeSets.clear();
		for (i = 0; i < (signed int)tempMoleculeSets.size(); ++i)
			moleculeSets.push_back(tempMoleculeSets[i]);
		tempMoleculeSets.clear();
		
		// Clean up
		for (i = 0; i < (signed int)population.size(); ++i)
			delete population[i];
		population.clear();
		for (i = 0; i < (signed int)optimizedMoleculeSets.size(); ++i)
			delete optimizedMoleculeSets[i];
		optimizedMoleculeSets.clear();
		
		input.writeResumeFile(tempResumeFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets,
		                      time (NULL) - seconds, false);
		if (system(commandString)) {	
			cout << "Error copying the temporary optimization file(" << tempResumeFileName
			     << ") to the real optimization file." << endl;
			break;
		}
		
		// Write the output file
		cout << setiosflags(ios::fixed) << setprecision(8);
		cout << "Best Energy: " << bestNMoleculeSets[0]->getEnergy() << " Best Structures: " << bestNMoleculeSets.size() << " Structures left to optimize: " << moleculeSets.size() << endl;
	}
	seconds = time (NULL) - seconds;
	days = seconds / (24*60*60);
	seconds = seconds - days * (24*60*60);
	hours = seconds / (60*60);
	seconds = seconds - hours * (60*60);
	minutes = seconds / 60;
	seconds = seconds - minutes * 60;
	cout << "Number of times the energy was calculated: " << input.m_iNumEnergyEvaluations << endl;
	cout << "Finished in " << days << " days, " << hours << " hours, "
	     << minutes << " minutes, " << seconds << " seconds." << endl;
}

void printHelpMenu()
{
	ifstream infile("help.txt");
	const int MAX_LINE_LENGTH = 500;
	char fileLine[MAX_LINE_LENGTH];
	while (infile.getline(fileLine, MAX_LINE_LENGTH))
		cout << fileLine << endl;
	cout << endl;
	infile.close();
}

int main(int argc, char *argv[])
{
	string inputFileName;
	string outputDirectory;
	string comPrefix;
	string tempOutputFileName;
	char numberString[25];
	string sId;
	string emptyString;
	ofstream fout;
	int i, j, k;
	string seedFiles;
	Input input;
	Input resume;
	string resumeFileName;
	Input inputSeeded;
	int numSeedFiles;
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	vector<MoleculeSet*> bestIndividualMoleculeSets; // The best solution found for each individual (used in particle swarm optimization)
	vector<MoleculeSet*> moleculeSetsSeeded;
	vector<MoleculeSet*> moleculeSetsTemp;
	vector<MoleculeSet*> bestNMoleculeSetsTemp;
	vector<MoleculeSet*> bestIndividualMoleculeSetsTemp; // The best solution found for each individual (used in particle swarm optimization)
	vector<MoleculeSet*> emptyMoleculeSets;
	MoleculeSet* pBestMoleculeSetSeeded = NULL;
	int* iNumStructuresOfEachTypeBest = NULL;
	MoleculeSet* pMoleculeSet;
	struct stat fileStatistics;
	string answer;
	char commandLine[500];
	int numFiles;
	DIR* directory;
	struct dirent* dirp;
	
	Molecule::initRandoms();
	
	if ((argc < 2) || (strncmp(argv[1],"-h", 2) == 0) || (strncmp(argv[1],"--help", 6) == 0)) {
		printHelpMenu();
		return 0;
	}
	if (strncmp(argv[1],"-t", 3) == 0) { // # Make an output file from a resume file that's readable
		if (argc != 4) {
			cout << "When using the -t option, please specify a resume or optimization file followed by an output file to which results will be written." << endl;
			return 0;
		}
		inputFileName = argv[argc - 2];
		tempOutputFileName = argv[argc - 1];
		cout << "Reading File: " << inputFileName << endl;
		if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
			return 0;
		if (!(input.m_bResumeFileRead || input.m_bOptimizationFileRead)) {
			cout << "The input file you have specified is not a resume or an optimization file." << endl;
			return 0;
		} else {
			fout.open(tempOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
			if (!fout.is_open())
			{
				cout << "Unable to open the output file: " << tempOutputFileName << endl;
				return 0;
			}
			input.printInputParamsToFile(fout);
			fout << endl << endl << "Best " << bestNMoleculeSets.size() << " structures from iteration "
			     << input.m_iIteration << ":" << endl;
			for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			{
				fout << "Structure #" << (i+1) << ": " << endl;
				bestNMoleculeSets[i]->print(fout);
				fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: "
				     << bestNMoleculeSets[i]->getEnergy() << endl << endl;
			}
			fout << endl << endl << "Population of " << moleculeSets.size() << " structures from iteration "
			     << input.m_iIteration << ":" << endl;
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			{
				fout << "Structure #" << (i+1) << ": " << endl;
				moleculeSets[i]->print(fout);
				fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: "
				     << moleculeSets[i]->getEnergy() << endl << endl;
			}
			fout.close();
			cout << "Output written to: " << tempOutputFileName << endl;
		}
   		// Clean up
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			delete bestNMoleculeSets[i];
		for (i = 0; i < (signed int)bestIndividualMoleculeSets.size(); ++i)
			delete bestIndividualMoleculeSets[i];
		moleculeSets.clear();
		bestNMoleculeSets.clear();
		bestIndividualMoleculeSets.clear();
	} else if (strncmp(argv[1],"-b", 3) == 0) { // # Check to make sure bonds were recognized correctly
		inputFileName = argv[2];
		cout << "Reading File: " << inputFileName << endl;
		if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
			return 0;
		input.printBondInfo();
   		// Clean up
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			delete bestNMoleculeSets[i];
		for (i = 0; i < (signed int)bestIndividualMoleculeSets.size(); ++i)
			delete bestIndividualMoleculeSets[i];
		moleculeSets.clear();
		bestNMoleculeSets.clear();
		bestIndividualMoleculeSets.clear();
	} else if (strncmp(argv[1],"-m", 3) == 0) { // # Check to make sure bonds were recognized correctly
		if (argc != 6) {
			cout << "When using the -m option, please specify an input file, followed by the number of degrees to rotate bond angles, followed by an optimization file, followed by the number of structures to optimize at a time." << endl;
			return 0;
		}
		inputFileName = argv[2];
		FLOAT angle = atof(argv[3])/180*PIE;
		tempOutputFileName = argv[4];
		cout << "Reading File: " << inputFileName << endl;
		if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
			return 0;
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		moleculeSets.clear();
		if (input.m_tempelateMoleculeSet.performBondRotations(angle, moleculeSets)) {
			sscanf(argv[5], "%d", &input.m_iStructuresToOptimizeAtATime);
			if ((input.m_iStructuresToOptimizeAtATime <= 0) ||
			    (input.m_iStructuresToOptimizeAtATime > (signed int)moleculeSets.size())) {
				cout << "Please specify a valid number of structures to optimize at a time (between 1 and "
				     << moleculeSets.size() << ")." << endl << endl;
			}
			cout << "Creating optimization file: " << tempOutputFileName << endl;
			input.writeResumeFile(tempOutputFileName, moleculeSets, emptyMoleculeSets,
				emptyMoleculeSets, 0, false);
			input.printBondInfo();
		}
   		// Clean up
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			delete bestNMoleculeSets[i];
		for (i = 0; i < (signed int)bestIndividualMoleculeSets.size(); ++i)
			delete bestIndividualMoleculeSets[i];
		moleculeSets.clear();
		bestNMoleculeSets.clear();
		bestIndividualMoleculeSets.clear();
	} else if ((strncmp(argv[1],"-c", 3) == 0) || (strncmp(argv[1],"-p", 3) == 0)) { // # Create .com files from an optimization or resume file
		vector<MoleculeSet*> *moleculeSetsToUse;
		if (argc != 5) {
			cout << "When using the -c or -p options, please specify a resume or optimization file, followed by a directory where .com files will be written, followed by a prefix for the .com files." << endl;
			return 0;
		}
		inputFileName = argv[2];
		outputDirectory = argv[3];
		Input::checkDirectoryOrFileName(outputDirectory);
		if (stat(outputDirectory.c_str(), &fileStatistics) != 0) { // If the file doesn't exist
			cout << "The directory " << outputDirectory << " does not exist.  Do you wish to create it? ";
			cin >> answer;
			if (strncmp(answer.c_str(),"yes",3) != 0) {
				cout << "Directory not created..." << endl << endl;
				return 0;
			} else {
				snprintf(commandLine, 500, "mkdir %s", outputDirectory.c_str());
				if (system(commandLine) == 0)
					cout << "Created directory: " << outputDirectory << endl;
				else 
					return 0;
			}
		}


		comPrefix = argv[4];
		cout << "Reading File: " << inputFileName << endl;
		if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
			return 0;
		if (!(input.m_bResumeFileRead || input.m_bOptimizationFileRead)) {
			cout << "The input file you have specified is not a resume or an optimization file." << endl;
			return 0;
		}
		
		if (strncmp(argv[1],"-c", 3) == 0)
			moleculeSetsToUse = &bestNMoleculeSets;
		else
			moleculeSetsToUse = &moleculeSets;
		
		if (moleculeSetsToUse->size() > 100) {
			cout << "There are " << moleculeSetsToUse->size() << " structures in this file." << endl;
			cout << "Enter the number of structures you wish to create .com files for or type 'all': ";
			cin >> answer;
			if (strncmp(answer.c_str(),"all",3) == 0)
				numFiles = moleculeSetsToUse->size();
			else
				numFiles = atoi(answer.c_str());
			if (numFiles <= 0) {
				cout << "Files not created..." << endl << endl;
				return 0;
			}
		} else
			numFiles = moleculeSetsToUse->size();

		cout << "Creating " << numFiles << " .com files..." << endl;
		for (i = 0; i < numFiles; ++i) {
			sprintf(numberString, "%d", (i+1));
			sId = numberString;
			tempOutputFileName = outputDirectory + "/" + comPrefix + sId + ".com";
			Energy::createGaussianInputFile(tempOutputFileName, emptyString, sId, input, *(*moleculeSetsToUse)[i], true);
		}
		cout << "Done!" << endl;
   		// Clean up
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			delete bestNMoleculeSets[i];
		for (i = 0; i < (signed int)bestIndividualMoleculeSets.size(); ++i)
			delete bestIndividualMoleculeSets[i];
		moleculeSets.clear();
		bestNMoleculeSets.clear();
		bestIndividualMoleculeSets.clear();
	} else if (strncmp(argv[1],"-o", 3) == 0) { // # Create an optimization file
		int numToTransfer;
		if (argc != 6) {
			cout << "When using the -o option, please specify a resume file, followed by the optimization file to be created, followed by the number of structures to transfer, followed by the number of structures to optimize at a time." << endl;
			return 0;
		}
		inputFileName = argv[2];
		tempOutputFileName = argv[3];
		cout << "Reading File: " << inputFileName << endl;
		if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
			return 0;
		if (!input.m_bResumeFileRead && !input.m_bOptimizationFileRead) {
			cout << "The input file you have specified is not a resume or an optimization file." << endl << endl;
			return 0;
		}
		sscanf(argv[4], "%d", &numToTransfer);
		if ((numToTransfer <= 0) ||
		    (numToTransfer > (signed int)bestNMoleculeSets.size())) {
			cout << "Please specify a valid number of structures to transfer (between 1 and "
			     << bestNMoleculeSets.size() << ")." << endl << endl;
			return 0;
		}
		if (input.m_iNumberOfLogFilesToSave < numToTransfer) {
			cout << "Changing the number of saved optimized structures to " << numToTransfer << "." << endl;
			input.m_iNumberOfBestStructuresToSave = numToTransfer;
			input.m_iNumberOfLogFilesToSave = numToTransfer;
		}
		sscanf(argv[5], "%d", &input.m_iStructuresToOptimizeAtATime);
		if ((input.m_iStructuresToOptimizeAtATime <= 0) ||
		    (input.m_iStructuresToOptimizeAtATime > (signed int)bestNMoleculeSets.size())) {
			cout << "Please specify a valid number of structures to optimize at a time (between 1 and "
			     << bestNMoleculeSets.size() << ")." << endl << endl;
			return 0;
		}
		while (numToTransfer < (signed int)bestNMoleculeSets.size()) {
			delete bestNMoleculeSets[bestNMoleculeSets.size()-1];
			bestNMoleculeSets.pop_back();
		}
		input.m_iNumEnergyEvaluations = 0;
		input.writeResumeFile(tempOutputFileName, bestNMoleculeSets, emptyMoleculeSets,
		                      emptyMoleculeSets, 0, false);
		cout << "Created Optimization File: " << tempOutputFileName << endl;
		if (input.m_iNumberOfLogFilesToSave > 0)
			cout << "Please move or delete any files in the directory: " << input.m_sSaveLogFilesInDirectory << endl;
		// Clean up
                for (i = 0; i < (signed int)moleculeSets.size(); ++i)
                        delete moleculeSets[i];
                for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
                        delete bestNMoleculeSets[i];
		moleculeSets.clear();
		bestNMoleculeSets.clear();
	} else if ((strncmp(argv[1],"-or", 4) || (strncmp(argv[1],"-ro", 4) == 0)) == 0) { // # Create an optimization file with random structures
		if (argc != 5) {
			cout << "When using the -or option, please specify an input file, followed by the optimization file to be created, followed by the number of structures to optimize at a time." << endl;
			return 0;
		}
		inputFileName = argv[2];
		tempOutputFileName = argv[3];
		cout << "Reading File: " << inputFileName << endl;
		if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
			return 0;
		if (input.m_bResumeFileRead || input.m_bOptimizationFileRead) {
			cout << "The input file you have specified is a resume or optimization file, and not an input file. Please specify an input file." << endl << endl;
			return 0;
		}
		if (initializePopulation(input, moleculeSets)) {
			input.m_iNumberOfBestStructuresToSave = moleculeSets.size();
			input.m_iNumberOfLogFilesToSave = moleculeSets.size();
			sscanf(argv[4], "%d", &input.m_iStructuresToOptimizeAtATime);
			if ((input.m_iStructuresToOptimizeAtATime <= 0) ||
			    (input.m_iStructuresToOptimizeAtATime > (signed int)moleculeSets.size())) {
				cout << "Please specify a valid number of structures to optimize at a time (between 1 and "
				     << moleculeSets.size() << ")." << endl << endl;
			} else {
				input.m_iNumEnergyEvaluations = 0;
				input.writeResumeFile(tempOutputFileName, moleculeSets, emptyMoleculeSets,
					emptyMoleculeSets, 0, false);
				cout << "Created Optimization File: " << tempOutputFileName << endl;
			}
		}
   		// Clean up
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		moleculeSets.clear();
	} else if (strncmp(argv[1],"-a", 3) == 0) { // # Add structures to an optimization file
		int numToTransfer;
		if (argc != 4) {
			cout << "When using the -a option, please specify an optimization file, followed by the number of additional structures to transfer from the resume file." << endl;
			return 0;
		}
		inputFileName = argv[2];
		sscanf(argv[3], "%d", &numToTransfer);
		cout << "Reading File: " << inputFileName << endl;
		if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
			return 0;
		if (!input.m_bOptimizationFileRead) {
			cout << "The input file you have specified is not an optimization file." << endl << endl;
			return 0;
		}
		cout << "Do you want to add structure(s) from this resume file: " << input.m_sResumeFileName << "? ";
		cin >> answer;
		if (strncmp(answer.c_str(),"yes",3) == 0)
			resumeFileName = input.m_sResumeFileName;
		else {
			cout << "Enter the resume file you wish to move structures from: ";
			cin >> resumeFileName;
		}
		if (!resume.open(resumeFileName, false, false, moleculeSetsTemp, bestNMoleculeSetsTemp, bestIndividualMoleculeSetsTemp))
			return 0;
		if (!resume.m_bResumeFileRead) {
			cout << "This is not a resume file: " << resumeFileName << endl << endl;
			return 0;
		}
		
		if ((numToTransfer <= 0) ||
		    (numToTransfer > (signed int)(bestNMoleculeSetsTemp.size() - (input.m_iNumEnergyEvaluations + moleculeSets.size())))) {
			cout << "Please specify a valid number of structures to transfer (between 1 and "
			     << (bestNMoleculeSetsTemp.size() - (input.m_iNumEnergyEvaluations + moleculeSets.size())) << ")."
			     << endl << endl;
			return 0;
		}
		
		j = input.m_iNumEnergyEvaluations + moleculeSets.size() + numToTransfer;
		for (i = input.m_iNumEnergyEvaluations + moleculeSets.size(); i < j; ++i) {
			pMoleculeSet = new MoleculeSet();
			pMoleculeSet->copy(*bestNMoleculeSetsTemp[i]);
			moleculeSets.push_back(pMoleculeSet);
		}
		if (input.m_iNumberOfLogFilesToSave < j) {
			cout << "Changing the number of saved optimized structures to " << j << "." << endl;
			input.m_iNumberOfBestStructuresToSave = j;
			input.m_iNumberOfLogFilesToSave = j;
		}
		input.writeResumeFile(inputFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets,
		                      input.m_tElapsedSeconds, false);
		cout << "Transferred " << numToTransfer << " structures to the optimization file: " << inputFileName << endl;
   		// Clean up
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSets[i];
		for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			delete bestNMoleculeSets[i];
		for (i = 0; i < (signed int)bestIndividualMoleculeSets.size(); ++i)
			delete bestIndividualMoleculeSets[i];
		for (i = 0; i < (signed int)moleculeSetsTemp.size(); ++i)
			delete moleculeSetsTemp[i];
		for (i = 0; i < (signed int)bestNMoleculeSetsTemp.size(); ++i)
			delete bestNMoleculeSetsTemp[i];
		for (i = 0; i < (signed int)bestIndividualMoleculeSetsTemp.size(); ++i)
			delete bestIndividualMoleculeSetsTemp[i];
		moleculeSets.clear();
		bestNMoleculeSets.clear();
		bestIndividualMoleculeSets.clear();
		moleculeSetsTemp.clear();
		bestNMoleculeSetsTemp.clear();
		bestIndividualMoleculeSetsTemp.clear();
	} else {
		if (strncmp(argv[1],"-s", 3) == 0) {
			if (argc < 5) {
				cout << "After '-s', please specify an input file, followed by the number of iterations to perform freezing, followed by one or more resume files." << endl << endl;
				return 0;
			}
			
			bool errorToStopFor = false;
			inputFileName = argv[2];
			cout << "Reading Input File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, true, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets)) {
				errorToStopFor = true;
			} else if (input.m_bResumeFileRead) {
				cout << "Error: this program was expecting this file \"" << inputFileName
				     << "\" to not be a resume file, but it is." << endl << endl;
				errorToStopFor = true;
			} else if (input.m_bOptimizationFileRead) {
				cout << "Error: this program was expecting this file \"" << inputFileName
				     << "\" to not be an optimization file, but it is." << endl << endl;
				errorToStopFor = true;
			}
			
			input.m_iFreezeUntilIteration = atoi(argv[3]);
			if (input.m_iFreezeUntilIteration < 0) {
				cout << "The number of iterations to perform freezing should come after the input file and cannot be negative." << endl;
				errorToStopFor = false;
			}
			
			numSeedFiles = argc-4;
			for (i = 0; i < numSeedFiles; ++i) {
				if (errorToStopFor)
					break;
				inputFileName = argv[i+4];
				seedFiles += inputFileName;
				if (i != (numSeedFiles-1))
					seedFiles += ", ";
				cout << "Reading Seed File: " << inputFileName << endl;
				if (!inputSeeded.open(inputFileName, false, false, moleculeSetsTemp, bestNMoleculeSetsTemp,
				    bestIndividualMoleculeSetsTemp)) {
					errorToStopFor = true;
					break;
				}
				if (!(inputSeeded.m_bResumeFileRead || inputSeeded.m_bOptimizationFileRead)) {
					cout << "Error: this program was expecting the seed file \"" << inputFileName
					     << "\" to be a resume or optimization file, but it's not." << endl << endl;
					errorToStopFor = true;
					break;
				}
				if (!input.seedCompatible(inputSeeded)) {
					cout << "The structures in the file " << inputFileName
					     << " aren't compatible with the structures in the file " << argv[2] << "." << endl;
					errorToStopFor = true;
					break;
				}
				
				for (j = 0; j < (signed int)bestNMoleculeSetsTemp.size(); ++j)
					if (bestNMoleculeSetsTemp[j]->isFragmented(input.m_fMaxAtomDistance)) {
						cout << "Seed files must contain structures that are not fragmented (maximum distance between molecules is enforced)." << endl;
						cout << "Structure #" << (j+1)
						     << " in the list of best structures in \"" << inputFileName
						     << "\" is fragmented." << endl << endl;
						errorToStopFor = true;
						break;
					}
				if (errorToStopFor)
					break;
				for (j = 0; j < (signed int)bestNMoleculeSetsTemp.size(); ++j) {
					if ((pBestMoleculeSetSeeded == NULL) ||
					    (pBestMoleculeSetSeeded->getEnergy() > bestNMoleculeSetsTemp[j]->getEnergy())) {
						delete pBestMoleculeSetSeeded;
						pBestMoleculeSetSeeded = new MoleculeSet();
						pBestMoleculeSetSeeded->copy(*bestNMoleculeSetsTemp[j]);
						delete[] iNumStructuresOfEachTypeBest;
						iNumStructuresOfEachTypeBest = new int[input.m_iNumStructureTypes];
						for (k = 0; k < input.m_iNumStructureTypes; ++k)
							iNumStructuresOfEachTypeBest[k] = inputSeeded.m_iNumStructuresOfEachType[k];
					}
					pMoleculeSet = new MoleculeSet();
					if (!pMoleculeSet->initFromSeed(*bestNMoleculeSetsTemp[j], input.m_iNumStructureTypes,
					                                inputSeeded.m_iNumStructuresOfEachType, input.m_iNumStructuresOfEachType,
					                                input.m_cartesianPoints, input.m_atomicNumbers, input.m_boxDimensions,
					                                input.m_fMaxAtomDistance, INITIALIZATION_TRIES)) {
						cout << "Failed to initialize 3D non-fragmented molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
						errorToStopFor = true;
						break;
					}
					moleculeSetsSeeded.push_back(pMoleculeSet);
				}
   				// Clean up
				for (j = 0; j < (signed int)moleculeSetsTemp.size(); ++j)
					delete moleculeSetsTemp[j];
				for (j = 0; j < (signed int)bestNMoleculeSetsTemp.size(); ++j)
					delete bestNMoleculeSetsTemp[j];
				for (j = 0; j < (signed int)bestIndividualMoleculeSetsTemp.size(); ++j)
					delete bestIndividualMoleculeSetsTemp[j];
				moleculeSetsTemp.clear();
				bestNMoleculeSetsTemp.clear();
				bestIndividualMoleculeSetsTemp.clear();
			}

			if (!errorToStopFor) {
				Gega::saveBestN(moleculeSetsSeeded, moleculeSets, input.m_iTotalPopulationSize,
				                input.m_fMinDistnaceBetweenSameMoleculeSets, 0, input.m_sSaveLogFilesInDirectory);
				while ((signed int)moleculeSets.size() < input.m_iTotalPopulationSize) {
					pMoleculeSet = new MoleculeSet();
						
					if (!pMoleculeSet->initFromSeed(*pBestMoleculeSetSeeded, input.m_iNumStructureTypes,
					                                iNumStructuresOfEachTypeBest, input.m_iNumStructuresOfEachType,
					                                input.m_cartesianPoints, input.m_atomicNumbers, input.m_boxDimensions,
					                                input.m_fMaxAtomDistance, INITIALIZATION_TRIES)) {
						cout << "Failed to initialize 3D non-fragmented molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
						errorToStopFor = true;
						break;
					}
					moleculeSets.push_back(pMoleculeSet);
				}
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->setId(i+1);
				if (input.m_iFreezeUntilIteration > 0)
					for (i = 0; i < (signed int)moleculeSets.size(); ++i)
						if (moleculeSets[i]->getNumberOfMoleculesFrozen() == moleculeSets[i]->getNumberOfMolecules()) {
							cout << "Seed files must contain at least one fewer unit/structure of one type in order to perform freezing." << endl;
							errorToStopFor = true;
							break;
						}
			}
			delete pBestMoleculeSetSeeded;
			delete[] iNumStructuresOfEachTypeBest;
			if (errorToStopFor)
				return 0;
		} else {
			if ((strncmp(argv[1],"-test", 6) == 0) && (argc == 3)) {
				input.m_bTestMode = true;
			} else if (argc > 2) {
				cout << "Error: more than the expected number of 2 arguments." << endl << endl;
				return 0;
			}
			inputFileName = argv[argc-1];
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, true, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				return 0;
		}
		if ((input.m_iNumberOfLogFilesToSave > 0) && (input.m_iEnergyFunction != LENNARD_JONES) &&
		    ((input.m_bOptimizationFileRead && (bestNMoleculeSets.size() == 0)) ||
		     (!input.m_bOptimizationFileRead && !input.m_bResumeFileRead) ||
		     (stat(input.m_sSaveLogFilesInDirectory.c_str(), &fileStatistics) != 0))) {  // if the directory doesn't exist
			// Delete the old log files
			directory = opendir(input.m_sSaveLogFilesInDirectory.c_str());
			if (directory) { // If we were able to open the directory
				i = 0;
				while ((dirp = readdir(directory)) != NULL ) {
					if ((0 == strcmp( ".", dirp->d_name)) || (0 == strcmp( "..",dirp->d_name)))
						continue;
					++i;
				}
				answer = "";
				if (i > 0) {
					cout << "There are files in " << input.m_sSaveLogFilesInDirectory << ".  Do you wish to delete these? ";
					cin >> answer;
				}
				if ((i == 0) || (strncmp(answer.c_str(),"yes",3) == 0)) {
					cout << "Deleting directory: " << input.m_sSaveLogFilesInDirectory << endl;
					snprintf(commandLine, 500, "rm -rf %s", input.m_sSaveLogFilesInDirectory.c_str());
					if (system(commandLine) != 0)
						return 0;
				} else {
					cout << "Directory " << input.m_sSaveLogFilesInDirectory << " was not deleted.  Exiting..." << endl;
					return 0;
				}
				closedir(directory);
			}
			snprintf(commandLine, 500, "mkdir %s", input.m_sSaveLogFilesInDirectory.c_str());
			cout << "Creating directory: " << input.m_sSaveLogFilesInDirectory << endl;
			if (system(commandLine) != 0)
				return 0;
		}
		if (input.m_bOptimizationFileRead)
			optimizeBestStructures(input, moleculeSets, bestNMoleculeSets);
		else if (input.m_iAlgorithmToDo == SIMULATED_ANNEALING)
			simulatedAnnealing(input, moleculeSets, bestNMoleculeSets, seedFiles);
		else if (input.m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			particleSwarmOptimization(input, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets, seedFiles);
		else if (input.m_iAlgorithmToDo == GENETIC_ALGORITHM)
			gega(input, moleculeSets, bestNMoleculeSets, seedFiles);
	}
	return 0;
}

