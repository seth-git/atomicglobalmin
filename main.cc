////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include <mpi.h>

bool simulatedAnnealing(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets, string &seedFiles, bool bIndependentRun)
{
	vector<MoleculeSet*> emptyMoleculeSets; // This is only created here so we have something to pass in for
	                                        // bestIndividualMoleculeSets which is used in PSO, not simulated annealing.
	vector<MoleculeSet*> optimizedMoleculeSets;
	MoleculeSet *pMoleculeSet;
	int i;
	long iTotalAcceptedTransitions = 0;
	long iTotalTransitions = 0; // totals for the past input.m_iNumIterationsBeforeDecreasingTemp iterations
	FLOAT fAcceptanceRatio; // percentage of accepted transitions
	FLOAT fSQRTQuenchingFactor;
	string tempResumeFileName = input.m_sResumeFileName + ".temp";
	char commandString[500];
	// Variables to keep track of how long this took
	time_t seconds;
	time_t minutes;
	time_t hours;
	time_t days;

	ofstream fout;
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
	bool finished = false;
	
	int iNumTransitions;

	if (input.m_bPerformBasinHopping)
		energyCalculationType = OPTIMIZE_BUT_DONT_READ;
	else if (input.m_bTransitionStateSearch)
		energyCalculationType = TRANSITION_STATE_SEARCH;
	else
		energyCalculationType = SINGLE_POINT_ENERGY_CALCULATION;
	
	
	try {
		if (!input.m_bResumeFileRead || (bIndependentRun && (seedFiles.length() > 0))) {
			seconds = time (NULL);

			fout.open(input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
			
			if (!fout.is_open())
			{
				cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
				throw "Unable to open the output file.";
			}
			if (input.m_bResumeFileRead) {
				input.m_bResumeFileRead = !input.m_bResumeFileRead;
				input.printToFile(fout);
				input.m_bResumeFileRead = !input.m_bResumeFileRead;
			} else {
				input.printToFile(fout);
			}
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
				if (!Init::initializePopulation(input, moleculeSets))
					throw "Unable to initialize the population.";
			}
			if (!Mpi::calculateEnergies(energyCalculationType, input, moleculeSets, optimizedMoleculeSets))
				throw "Not all calculations finished.";
			if (Mpi::s_timeToFinish)
				throw "Time to finish, cleaning up.";
			input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
			if (input.m_bTransitionStateSearch) {
				if (optimizedMoleculeSets.size() > 0)
					Input::saveBestN(optimizedMoleculeSets, bestNMoleculeSets,
							input.m_iNumberOfBestStructuresToSave,
							input.m_fMinDistnaceBetweenSameMoleculeSets,
							input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
				for (i = 0; i < (signed int)optimizedMoleculeSets.size(); ++i)
					delete optimizedMoleculeSets[i];
				optimizedMoleculeSets.clear();
			} else {
				Input::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
					input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
					input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
			}
			
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
		} else {
			seconds = time (NULL) - input.m_tElapsedSeconds;
			fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
			if (!fout.is_open())
			{
				cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
				throw "Unable to open the output file.";
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
		
		if ((input.m_fStartingTemperature < 0) || (input.m_fDesiredAcceptedTransitions < 0)) {
			cout <<  "This resume file was created using the -i option.  It stores the list of best structures, but it can not be resumed." << endl;
			throw "This resume file was created using the -i option.  It stores the list of best structures, but it can not be resumed.";
		}
		
		// Perform Simulated Annealing
		if (input.m_bTestMode)
			if (!input.printTestFileHeader(0,*moleculeSets[0]))
				throw "";
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
			if (!Mpi::calculateEnergies(energyCalculationType, input, moleculeSetsTransformed, optimizedMoleculeSets))
				throw "Not all calculations finished.";
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
						while (percentToCutOff * (positiveDeltaEnergies.size()) <
						       probabilitySum(positiveDeltaEnergies,input.m_fBoltzmanConstant,lowTemp))
							lowTemp *= 0.5;
						
						myFloat = -1;
						for (i = 0; i < (signed int)positiveDeltaEnergies.size(); ++i)
							if (positiveDeltaEnergies[i] > myFloat)
								myFloat = positiveDeltaEnergies[i];

						highTemp = -myFloat / (log(percentToCutOff) * input.m_fBoltzmanConstant);
						while (percentToCutOff * (positiveDeltaEnergies.size()) >
						       probabilitySum(positiveDeltaEnergies,input.m_fBoltzmanConstant,highTemp))
							highTemp *= 2;
						i = 0;
						while (true) {
							++i;
							newTemp = (lowTemp + highTemp) * 0.5;
							if (i > 1000)
								break;
							if (percentToCutOff * (positiveDeltaEnergies.size()) >
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
					Input::saveBestN(optimizedMoleculeSets, bestNMoleculeSets,
						input.m_iNumberOfBestStructuresToSave,
						input.m_fMinDistnaceBetweenSameMoleculeSets,
						input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
				for (i = 0; i < (signed int)optimizedMoleculeSets.size(); ++i)
					delete optimizedMoleculeSets[i];
				optimizedMoleculeSets.clear();
			} else {
				Input::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
					input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
				        input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
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
				// Note: if you modify the above print statement, also modify Input::compileIndependentRunData

//				fout << setiosflags(ios::fixed) << setprecision(1)
//				     << " Converged: " << (Mpi::s_percentageOfSuccessfulCalculations * 100) << "%" << endl;
				
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
//				cout << " Converged: " << (Mpi::s_percentageOfSuccessfulCalculations * 100) << "%" << endl;
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
				if (!input.printTestFileGeometry(input.m_iIteration, *moleculeSets[0]))
					throw "";
			if (Mpi::s_timeToFinish)
				throw "Time to finish, cleaning up.";
		} while (true);
		
		cout << "Finished after " << input.m_iIteration << " iterations at a temperature of " << input.m_fStartingTemperature << "." << endl;
		if ((input.m_iEnergyFunction == LENNARD_JONES) && !input.m_bTransitionStateSearch) {
			cout << "Optimizing best structure..." << endl;
			fout << "Optimizing best structure..." << endl;
			emptyMoleculeSets.push_back(bestNMoleculeSets[0]);
			Mpi::calculateEnergies(OPTIMIZE_AND_READ, input, emptyMoleculeSets, optimizedMoleculeSets);
		
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
		finished = true;
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	
   	// Clean up
	for (i = 0; i < (signed int)moleculeSetsTransformed.size(); ++i)
		delete moleculeSetsTransformed[i];
	moleculeSetsTransformed.clear();
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		delete moleculeSets[i];
	moleculeSets.clear();
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();
	
	fout.close();
	if (input.m_bTestMode)
		input.printTestFileFooter();
	return finished;
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

	FLOAT fCoordInertia = 0;
	FLOAT fAngleInertia = 0;
	
	ofstream fout;
	FLOAT fDecreaseCoordInertiaConstant = 0;
	FLOAT fDecreaseAngleInertiaConstant = 0;
	FLOAT visibility = 0;
	FLOAT **particleDistanceMatrix = NULL;
	FLOAT diversity = 0;
	FLOAT fCubeDiagonal = sqrt(3 * input.m_boxDimensions.x * input.m_boxDimensions.x);
	int iSwitchToAttractionReplaceBest;
	FLOAT tempFloat;
	int energyCalculationType;
	int iMoleculesWithMultipleAtoms;
	int iRandomMolecule = -1;
	
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
	
	try {
		if (!input.m_bResumeFileRead) {
			seconds = time (NULL);
			fout.open (input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
			if (!fout.is_open())
			{
				cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
				throw "Unable to open the output file.";
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
					if (!Init::initializePopulation(input, moleculeSets))
						throw "Unable to initialize the popultion.";
				}
				if (!Mpi::calculateEnergies(energyCalculationType, input, moleculeSets, optimizedMoleculeSets))
					throw "Not all calculations finished.";
				if (Mpi::s_timeToFinish)
					throw "Time to finish, cleaning up.";
				input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
				Input::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
						input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
						input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->initVelocities(input.m_fCoordMaximumVelocity,input.m_fAngleMaximumVelocity);
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
				throw "Unable to open the output file.";
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
		
		// Perform particle swarm optimization
		if (input.m_bTestMode) {
			iRandomMolecule = Molecule::randomInt(0,moleculeSets.size()-1);
			if (!input.printTestFileHeader(0, *moleculeSets[iRandomMolecule]))
				throw "";
		}
		
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
					if (!Mpi::calculateEnergies(energyCalculationType, input, moleculeSetsMinDistEnforced, optimizedMoleculeSets))
						throw "Not all calculations finished.";
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
					if (!Mpi::calculateEnergies(energyCalculationType, input, moleculeSets, optimizedMoleculeSets))
						throw "Not all calculations finished.";
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
					Input::saveBestN(moleculeSetsMinDistEnforced, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
							input.m_fMinDistnaceBetweenSameMoleculeSets, input.m_iNumberOfLogFilesToSave,
							input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
				else
					Input::saveBestN(moleculeSets, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
							input.m_fMinDistnaceBetweenSameMoleculeSets, input.m_iNumberOfLogFilesToSave,
							input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
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
//			     << " Converged: " << (Mpi::s_percentageOfSuccessfulCalculations * 100) << "%" << endl;
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
//				     << " Converged: " << (Mpi::s_percentageOfSuccessfulCalculations * 100) << "%" << endl;
			}
			
			if (input.m_iFreezeUntilIteration == input.m_iIteration) {
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->unFreezeAll(input.m_fCoordMaximumVelocity,input.m_fAngleMaximumVelocity);
				cout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
				fout << "Removing frozen status from the coordinates of seeded atoms..." << endl;
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
				if (!input.printTestFileGeometry(input.m_iIteration, *moleculeSets[iRandomMolecule]))
					throw "";
			if (Mpi::s_timeToFinish)
				throw "Time to finish, cleaning up.";
		} while (true);
		
		cout << "Finished after " << input.m_iIteration << " iterations." << endl;
		fout << "Finished after " << input.m_iIteration << " iterations." << endl;
		if (input.m_iEnergyFunction == LENNARD_JONES) {
			vector<MoleculeSet*> tempMoleculeSets;
			cout << "Optimizing best structure..." << endl;
			fout << "Optimizing best structure..." << endl;
			tempMoleculeSets.push_back(bestNMoleculeSets[0]);
			Mpi::calculateEnergies(OPTIMIZE_AND_READ, input, tempMoleculeSets, optimizedMoleculeSets);
		
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
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}

	if (particleDistanceMatrix != NULL) {
   		// Clean up
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete[] particleDistanceMatrix[i];
		delete[] particleDistanceMatrix;
	}
	
	for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
		delete moleculeSets[i];
		delete bestIndividualMoleculeSets[i];
	}
	moleculeSets.clear();
	bestIndividualMoleculeSets.clear();
	if (input.m_bEnforceMinDistOnCopy)
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete moleculeSetsMinDistEnforced[i];
	moleculeSetsMinDistEnforced.clear();
	for (i = 0; i < (signed int)localBestMoleculeSets.size(); ++i)
			delete localBestMoleculeSets[i];
	localBestMoleculeSets.clear();

	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();
	
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
	vector<MoleculeSet*> *pPopulationA = NULL;
	vector<MoleculeSet*> *pPopulationB = NULL;
	vector<MoleculeSet*> optimizedMoleculeSets;
	int i;
	string tempResumeFileName = input.m_sResumeFileName + ".temp";
	char commandString[500];
	// Variables to keep track of how long this took
	time_t seconds;
	time_t minutes;
	time_t hours;
	time_t days;
	ofstream fout;
	int energyCalculationType;
	FLOAT **particleDistanceMatrix = NULL;
	FLOAT diversity = 0;
	FLOAT fCubeDiagonal = sqrt(3 * input.m_boxDimensions.x * input.m_boxDimensions.x);
	
	if (input.m_bUseLocalOptimization)
		energyCalculationType = OPTIMIZE_BUT_DONT_READ;
	else
		energyCalculationType = SINGLE_POINT_ENERGY_CALCULATION;

	try {	
		if (!input.m_bResumeFileRead) {
			seconds = time (NULL);
			fout.open (input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
			if (!fout.is_open())
			{
				cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
				throw "Unable to open the output file.";
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
					if (!Init::initializePopulation(input, population))
						throw "Unable to initialize the population.";
				}
				if (!Mpi::calculateEnergies(energyCalculationType, input, population, optimizedMoleculeSets))
					throw "Not all calculations finished.";
				if (Mpi::s_timeToFinish)
					throw "Time to finish, cleaning up.";
				input.m_iNumEnergyEvaluations += (signed int)population.size();
				Input::saveBestN(population,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
						input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
						input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
			}
			
			cout << "Performing Genetic Algorithm..." << endl;
			fout << "Performing Genetic Algorithm..." << endl;
		} else {
			seconds = time (NULL) - input.m_tElapsedSeconds;
			fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
			if (!fout.is_open())
			{
				cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
				throw "Unable to open the output file.";
			}
			fout << "Resuming after program execution was stopped..." << endl;
		}
		
		particleDistanceMatrix = new FLOAT*[population.size()];
		for (i = 0; i < (signed int)population.size(); ++i)
			particleDistanceMatrix[i] = new FLOAT[population.size()];

		// Perform the genetic algorithm
		pPopulationA = &population;
		pPopulationB = &population2;
		if (input.m_bTestMode)
			if (!input.printTestFileHeader(0,*bestNMoleculeSets[0]))
				throw "";
		
		sprintf(commandString,"mv %s %s", tempResumeFileName.c_str(), input.m_sResumeFileName.c_str());
		do {
			++input.m_iIteration;
			Gega::tournamentMateSelection(*pPopulationA, matingPool);
			Gega::performMatings(matingPool, offSpring, input.m_iNumStructureTypes,
					     input.m_iNumStructuresOfEachType, input.m_boxDimensions);
			
			matingPool.clear();
			if (!Mpi::calculateEnergies(energyCalculationType, input, offSpring, optimizedMoleculeSets))
				throw "Not all calculations finished.";
			input.m_iNumEnergyEvaluations += (signed int)offSpring.size();
			
			Input::saveBestN(offSpring, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
					input.m_fMinDistnaceBetweenSameMoleculeSets, input.m_iNumberOfLogFilesToSave,
					input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
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
				if (!input.printTestFileGeometry(input.m_iIteration, *bestNMoleculeSets[0]))
					throw "";
			if (Mpi::s_timeToFinish)
				throw "Time to finish, cleaning up.";
		} while (true);
		
		cout << "Finished after " << input.m_iIteration << " iterations." << endl;
		fout << "Finished after " << input.m_iIteration << " iterations." << endl;
		if (input.m_iEnergyFunction == LENNARD_JONES) {
			vector<MoleculeSet*> tempMoleculeSets;
			cout << "Optimizing best structure..." << endl;
			fout << "Optimizing best structure..." << endl;
			tempMoleculeSets.push_back(bestNMoleculeSets[0]);
			Mpi::calculateEnergies(OPTIMIZE_AND_READ, input, tempMoleculeSets, optimizedMoleculeSets);
		
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
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	
   	// Clean up
	if (particleDistanceMatrix != NULL) {
		for (i = 0; i < (signed int)pPopulationA->size(); ++i)
			delete[] particleDistanceMatrix[i];
		delete[] particleDistanceMatrix;
	}
	
	for (i = 0; i < (signed int)population.size(); ++i)
		delete population[i];
	for (i = 0; i < (signed int)population2.size(); ++i)
		delete population2[i];
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	population.clear();
	population2.clear();
	bestNMoleculeSets.clear();
	
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
	
	try {
		if (moleculeSets.size() == 0) {
			cout << "There are no structures left to optimize." << endl;
			throw "There are no structures left to optimize.";
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
			if (!Mpi::calculateEnergies(energyCalculationType, input, population, optimizedMoleculeSets))
				throw "Not all calculations finished.";
			input.m_iNumEnergyEvaluations += (signed int)population.size();
			
			// Update the list of optimized best structures
			Input::saveBestN(optimizedMoleculeSets, bestNMoleculeSets, (signed int)(optimizedMoleculeSets.size() +
					bestNMoleculeSets.size()), input.m_fMinDistnaceBetweenSameMoleculeSets,
					input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory.c_str(), Energy::s_checkPointFileName.c_str());
			
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
				throw "Error copying the temporary optimization file to the real optimization file.";
			}
		
			// Write the output file
			cout << setiosflags(ios::fixed) << setprecision(8);
			cout << "Best Energy: " << bestNMoleculeSets[0]->getEnergy() << " Best Structures: " << bestNMoleculeSets.size() << " Structures left to optimize: " << moleculeSets.size() << endl;
			if (Mpi::s_timeToFinish)
				throw "Time to finish, cleaning up.";
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
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	
	// Clean up
	for (i = 0; i < (signed int)population.size(); ++i)
		delete population[i];
	population.clear();
	
	for (i = 0; i < (signed int)optimizedMoleculeSets.size(); ++i)
		delete optimizedMoleculeSets[i];
	optimizedMoleculeSets.clear();
	
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		delete moleculeSets[i];
	moleculeSets.clear();
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();
}

bool setWallTime(void)
{
	int numOptionArguments;
	char** optionArguments;
	int hours, minutes, seconds;
	
	if (ArgumentParser::optionPresent("-walltime")) {
		int numPartsScanned;
		
		ArgumentParser::getOptionArguments("-walltime", numOptionArguments, &optionArguments);
		numPartsScanned = sscanf(optionArguments[0], "%d:%d:%d", &hours, &minutes, &seconds);
		if (numPartsScanned != 3) {
			cout << "Please specify a wall time in the format HH:MM:SS." << endl;
			return false;
		}
	} else {
		hours = 100000; // about 11 years
		minutes = 0;
		seconds = 0;
	}
	minutes += hours*60;
	seconds += minutes*60;
	Mpi::s_wallTime = time(NULL) + seconds; 
	return true;
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

bool initCommandLineArguments(int argc, char *argv[])
{
	vector<const char*> recognizedOptions;
	vector<int> argumentsPerOption;
	vector<const char*> optionMessages;
	
	recognizedOptions.push_back("-h");
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back("--help");
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back("-s");
	argumentsPerOption.push_back(-1);
	optionMessages.push_back("When using the -s option, please specify one or more resume/optimiation files,\nfollowed by the number of iterations to perform freezing, followed by an input file.");
	recognizedOptions.push_back("-i");
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back("-m");
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back("-test");
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back("-walltime");
	argumentsPerOption.push_back(1);
	optionMessages.push_back("Please specify a wall time in the format DD:HH:MM.");
	
	ArgumentParser::init(&recognizedOptions, &argumentsPerOption, &optionMessages);
	if (!ArgumentParser::parse(argc, argv))
		return false;
	return true;
}

int master(int rank)
{
	int numOptionArguments;
	char** optionArguments;
	
	string inputFileName;
	string seedFileName;
	string outputDirectory;
	string comPrefix;
	string tempOutputFileName;
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
	MoleculeSet* pMoleculeSet;
	struct stat fileStatistics;
	string answer;
	char commandLine[500];
	DIR* directory;
	struct dirent* dirp;
	bool error = false;
	int nMPIProcesses;
	bool masterDistributingTasks = ArgumentParser::optionPresent("-m");
	bool bIndependentRunSetupPreviouslyDone;
	
	MPI_Comm_size(MPI_COMM_WORLD, &nMPIProcesses);
	
	try {
		if ((ArgumentParser::getNumOptions() == 0) || ArgumentParser::optionPresent("-h") || ArgumentParser::optionPresent("--help")) {
			printHelpMenu();
			throw "Help menu";
		}
		if (rank == 0) {
			inputFileName = ArgumentParser::s_argv[ArgumentParser::s_argc-1];
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, true, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets)) {
				cout << "The last argument must be the input, resume, or optimization file." << endl;
				throw "The last argument must be the input, resume, or optimization file.";
			}
			if (ArgumentParser::optionPresent("-i")) {
				if (masterDistributingTasks) {
					cout << "The -i and -m options may not be used together." << endl;
					throw "The -i and -m options may not be used together.";
				}
				if (input.m_iAlgorithmToDo != SIMULATED_ANNEALING) {
					cout << "The -i option only works with simulated annealing type algorithms." << endl;
					throw "The -i option only works with simulated annealing type algorithms.";
				}
				if (input.m_bResumeFileRead || input.m_bOptimizationFileRead) {
					cout << "The -i option only works with input files, and not with resume or optimization files." << endl;
					throw "The -i option only works with input files, and not with resume or optimization files.";
				}
				if (nMPIProcesses != input.m_iTotalPopulationSize) {
					cout << "When using the -i option, the population size(" << input.m_iTotalPopulationSize
	                                            << ") must equal the number of MPI processes(" << nMPIProcesses << ")." << endl;
					throw "When using the -i option, the population size must equal the number of MPI processes.";
				}
			}
			answer = input.m_sPathToEnergyFiles + "/stop";
			Mpi::setQuitFlag(answer);
		}
		if ((rank == 0) && ArgumentParser::optionPresent("-s")) { // # Do seeding
			MoleculeSet* pBestMoleculeSetSeeded = NULL;
			int* iNumStructuresOfEachTypeBest = NULL;
			try {
				if (input.m_bResumeFileRead || input.m_bOptimizationFileRead) {
					cout << "Seeding is only allowed when reading an input file. ";
					if (input.m_bResumeFileRead)
						cout << inputFileName << " is a resume file and not an input file." << endl;
					else
						cout << inputFileName << " is an optimization file and not an input file." << endl;
					throw "Seeding is only allowed when reading an input file (not a resume or optimization file).";
				}
				ArgumentParser::getOptionArguments("-s", numOptionArguments, &optionArguments);
				--numOptionArguments; // The last argument should be the input file
				
				if (numOptionArguments < 2) {
					cout << "With the -s option, please specify at least one seed file and the number of iterations to perform freezing." << endl;
					throw "With the -s option, please specify at least one seed file and the number of iterations to perform freezing.";
				}
				
				if (input.m_bResumeFileRead || input.m_bOptimizationFileRead) {
					cout << "When using the -s option, the last argument '" << inputFileName << "' must be an input file and not a resume or optimization file." << endl << endl;
					throw "-s option error";
				}
				
				input.m_iFreezeUntilIteration = atoi(optionArguments[numOptionArguments-1]);
				if (input.m_iFreezeUntilIteration < 0) {
					cout << "The number of iterations to perform freezing should come just before the input file and cannot be negative." << endl;
					throw "The number of iterations to perform freezing should come just before the input file and cannot be negative.";
				}
				
				numSeedFiles = numOptionArguments-1;
				for (i = 0; i < numSeedFiles; ++i) {
					seedFileName = optionArguments[i];
					seedFiles += seedFileName;
					if (i != (numSeedFiles-1))
						seedFiles += ", ";
					cout << "Reading Seed File: " << seedFileName << endl;
					if (!inputSeeded.open(seedFileName, false, false, moleculeSetsTemp, bestNMoleculeSetsTemp, bestIndividualMoleculeSetsTemp))
						throw "Couldn't open seed file.";
					if (!(inputSeeded.m_bResumeFileRead || inputSeeded.m_bOptimizationFileRead)) {
						cout << "Error: this program was expecting the seed file \"" << seedFileName
						     << "\" to be a resume or optimization file, but it's not." << endl << endl;
						throw "A seed file is an input file.";
					}
					if (!input.seedCompatible(inputSeeded)) {
						cout << "The structures in the file " << seedFileName
						     << " aren't compatible with the structures in the file " << inputFileName << "." << endl;
						throw "Seed file incompatability.";
					}
					
					for (j = 0; j < (signed int)bestNMoleculeSetsTemp.size(); ++j)
						if (bestNMoleculeSetsTemp[j]->isFragmented(input.m_fMaxAtomDistance)) {
							cout << "Seed files must contain structures that are not fragmented (maximum distance between molecules is enforced)." << endl;
							cout << "Structure #" << (j+1)
							     << " in the list of best structures in \"" << seedFileName
							     << "\" is fragmented." << endl << endl;
							throw "Fragmentation seed error";
						}
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
							delete pMoleculeSet;
							throw "Failed to initialize 3D non-fragmented molecule set with maximum distance constraint.";
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

				Input::saveBestN(moleculeSetsSeeded, moleculeSets, input.m_iTotalPopulationSize,
						input.m_fMinDistnaceBetweenSameMoleculeSets, 0, NULL, NULL);
				while ((signed int)moleculeSets.size() < input.m_iTotalPopulationSize) {
					pMoleculeSet = new MoleculeSet();
						
					if (!pMoleculeSet->initFromSeed(*pBestMoleculeSetSeeded, input.m_iNumStructureTypes,
									iNumStructuresOfEachTypeBest, input.m_iNumStructuresOfEachType,
									input.m_cartesianPoints, input.m_atomicNumbers, input.m_boxDimensions,
									input.m_fMaxAtomDistance, INITIALIZATION_TRIES)) {
						cout << "Failed to initialize 3D non-fragmented molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
						delete pMoleculeSet;
						throw "Failed to initialize 3D non-fragmented molecule set with maximum distance constraint.";
					}
					moleculeSets.push_back(pMoleculeSet);
				}
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->setId(i+1);
				if (input.m_iFreezeUntilIteration > 0)
					for (i = 0; i < (signed int)moleculeSets.size(); ++i)
						if (moleculeSets[i]->getNumberOfMoleculesFrozen() == moleculeSets[i]->getNumberOfMolecules()) {
							cout << "Seed files must contain at least one fewer unit/structure/molecule of one type in order to perform freezing." << endl;
							throw "Seed files must contain at least one fewer unit/structure/molecule of one type in order to perform freezing.";
						}
			} catch (const char* message) {
				error = true;
				if (PRINT_CATCH_MESSAGES)
					cerr << "Caught message: " << message << endl;
			}
			// Clean up
			for (i = 0; i < (signed int)moleculeSetsTemp.size(); ++i)
				delete moleculeSetsTemp[i];
			moleculeSetsTemp.clear();
			for (i = 0; i < (signed int)bestNMoleculeSetsTemp.size(); ++i)
				delete bestNMoleculeSetsTemp[i];
			bestNMoleculeSetsTemp.clear();
			for (i = 0; i < (signed int)bestIndividualMoleculeSetsTemp.size(); ++i)
				delete bestIndividualMoleculeSetsTemp[i];
			bestIndividualMoleculeSetsTemp.clear();
			if (pBestMoleculeSetSeeded != NULL)
				delete pBestMoleculeSetSeeded;
			if (iNumStructuresOfEachTypeBest != NULL)
				delete[] iNumStructuresOfEachTypeBest;
			if (error)
				throw "Inner throw";
		}
		if (ArgumentParser::optionPresent("-i")) {
			if (rank == 0) {
				vector<string> inputFileNames;
				if (!input.setupForIndependentRun(inputFileNames, moleculeSets, bIndependentRunSetupPreviouslyDone))
					throw "";
				
				// Delete any seeded structures
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					delete moleculeSets[i];
				moleculeSets.clear();
				
				for (i = 1; i < nMPIProcesses; ++i) {
			    		MPI_Send((void*) inputFileNames[i].c_str(),             /* message buffer */
					         inputFileNames[i].length()+1,                 /* buffer size */
					         MPI_BYTE,           /* data item is an character or byte */
					         i,              /* destination process rank */
					         WORKTAG,           /* user chosen message tag */
					         MPI_COMM_WORLD);   /* default communicator */
				}
				inputFileName = inputFileNames[0];
			} else {
				MPI_Status status;
				char messageBuffer[500];
				MPI_Recv(messageBuffer, sizeof(messageBuffer), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				if (status.MPI_TAG == DIETAG)
					throw "";
				inputFileName = messageBuffer;
			}
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, true, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "Error opening input file with the -i option on.";
		}
		string temp;
		if (input.m_bTransitionStateSearch && (input.m_iEnergyFunction == LENNARD_JONES)) {
			cout << "Transition state searches are not implemented with the Lennard Jones Potential." << endl;
			cout << "You may perform the search, but no transition states wll be found. Do you wish to continue? ";
			cin >> temp;
			if (strncmp(temp.c_str(),"yes",3) != 0)
				throw "";
		}
		if (!Energy::init(input, rank))
			throw "Couldn't initialize energy calculations.";
		if (ArgumentParser::optionPresent("-test")) {
			if (input.m_bOptimizationFileRead) {
				cout << "Test mode is not allowed with optimization files." << endl;
				throw "Test mode is not allowed with optimization files.";
			} else
				input.m_bTestMode = true;
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
						throw "Couldn't delete log files directory.";
				} else {
					cout << "Directory " << input.m_sSaveLogFilesInDirectory << " was not deleted.  Exiting..." << endl;
					throw "User didn't delete the log files directory.";
				}
				closedir(directory);
			}
			snprintf(commandLine, 500, "mkdir %s", input.m_sSaveLogFilesInDirectory.c_str());
			cout << "Creating directory: " << input.m_sSaveLogFilesInDirectory << endl;
			if (system(commandLine) != 0)
				throw "Couldn't create log files directory.";
		}
		const char* argument = ArgumentParser::nextUnrecognized();
		bool foundUnrecognizedOptions = false;
		string tempString;
		string::size_type pos;
		while (argument != NULL) {
			tempString = argument;
			pos = tempString.find(".inp");
			if (pos == string::npos) // If we didn't find it
				pos = tempString.find(".res");
			if (pos == string::npos) // If we didn't find it
				pos = tempString.find(".opt");
			if (pos == string::npos) { // If we didn't find it
				cout << "Unrecognized argument or option: " << argument << endl;
				foundUnrecognizedOptions = true;
			}
			argument = ArgumentParser::nextUnrecognized();
		}
		if (foundUnrecognizedOptions)
			throw "Unrecognized options";
		
		if (input.m_bOptimizationFileRead) {
			if (!Mpi::masterSetup(input.m_iStructuresToOptimizeAtATime, false,masterDistributingTasks, rank))
				throw "Couldn't create scratch directory";
			optimizeBestStructures(input, moleculeSets, bestNMoleculeSets);
		} else {
			switch (input.m_iAlgorithmToDo) {
				case SIMULATED_ANNEALING:
				case PARTICLE_SWARM_OPTIMIZATION:
				case GENETIC_ALGORITHM:
					if (!Mpi::masterSetup(input.m_iTotalPopulationSize, ArgumentParser::optionPresent("-i"), masterDistributingTasks, rank))
						throw "Couldn't create scratch directory";
					break;
				default:
					Mpi::end(rank);
			}
			bool finished;
			switch (input.m_iAlgorithmToDo) {
				case SIMULATED_ANNEALING:
					finished = simulatedAnnealing(input, moleculeSets, bestNMoleculeSets, seedFiles, ArgumentParser::optionPresent("-i"));
					
					if (finished && ArgumentParser::optionPresent("-i") && (rank == 0)) {
						inputFileName = ArgumentParser::s_argv[ArgumentParser::s_argc-1];
						cout << "Reading File: " << inputFileName << endl;
						if (!input.open(inputFileName, true, true, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
							throw "";
						input.compileIndependentRunData(true);
					}
					break;
				case PARTICLE_SWARM_OPTIMIZATION:
					particleSwarmOptimization(input, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets, seedFiles);
					break;
				case GENETIC_ALGORITHM:
					gega(input, moleculeSets, bestNMoleculeSets, seedFiles);
					break;
			}
					}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
		Mpi::end(rank);
	}
	if (Mpi::s_timeToFinish)
		cout << "Quitting " << (Mpi::s_wallTime - time(NULL)) << " seconds before the wall time..." << endl;
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		delete moleculeSets[i];
	moleculeSets.clear();
	for (i = 0; i < (signed int)bestIndividualMoleculeSets.size(); ++i)
		delete bestIndividualMoleculeSets[i];
	bestIndividualMoleculeSets.clear();
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();
	return 0;
}

int main(int argc, char *argv[])
{
	int myrank;

	/* Initialize MPI */

	MPI_Init(&argc, &argv);
	
	try {
		/* Find out my identity in the default communicator */
		MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
		if (!Init::initProgram(myrank))
			throw "";
		
		if (!initCommandLineArguments(argc, argv))
			throw "";
		if (!setWallTime())
			throw "";
		if ((myrank == 0) || ArgumentParser::optionPresent("-i"))
			master(myrank);
		else
			Mpi::slave(myrank, ArgumentParser::optionPresent("-m"));
	} catch(const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	ArgumentParser::cleanUp();

	/* Shut down MPI */

	MPI_Finalize();
	return 0;
}

