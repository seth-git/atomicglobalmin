////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include <mpi.h>

void simulatedAnnealing(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets, string &seedFiles, bool bIndependentRun)
{
	const Strings* messages = Strings::instance();
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
				cout << messages->m_sCantWriteToFile << ": " << input.m_sOutputFileName << endl;
				throw messages->m_sCantOpenOutputFile.c_str();
			}
			if (input.m_bResumeFileRead) {
				input.m_bResumeFileRead = !input.m_bResumeFileRead;
				input.printToFile(fout);
				input.m_bResumeFileRead = !input.m_bResumeFileRead;
			} else {
				input.printToFile(fout);
			}
			fout << endl << endl;
			fout << messages->m_sProgramOutput << ":" << endl;
			if (seedFiles.length() > 0) {
				cout << messages->m_sSeedingPopulationFromFile << " " << seedFiles << "..." << endl;
				fout << messages->m_sSeedingPopulationFromFile << " " << seedFiles << "..." << endl;
				if (input.m_iFreezeUntilIteration > 0) {
					cout << messages->m_sFreezingSeededMoleculesFor << " " << input.m_iFreezeUntilIteration << " " << messages->m_sIterations << "..." << endl;
					fout << messages->m_sFreezingSeededMoleculesFor << " " << input.m_iFreezeUntilIteration << " " << messages->m_sIterations << "..." << endl;
				} else {
					for (i = 0; i < (signed int)moleculeSets.size(); ++i)
						moleculeSets[i]->unFreezeAll(-1,-1);
				}
			} else {
				cout << messages->m_sInitializingPopulation << "..." << endl;
				fout << messages->m_sInitializingPopulation << "..." << endl;
				if (!Init::initializePopulation(input, moleculeSets))
					throw messages->m_sCantInitializePopulation.c_str();
			}
			if (!Mpi::calculateEnergies(energyCalculationType, moleculeSets, optimizedMoleculeSets))
				throw messages->m_sNotAllCalcFinished.c_str();
			if (Mpi::s_timeToFinish)
				throw messages->m_sTimeToFinishAndCleanUp.c_str();
			input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
			if (input.m_bTransitionStateSearch) {
				if (optimizedMoleculeSets.size() > 0)
					Input::saveBestN(optimizedMoleculeSets, bestNMoleculeSets,
							input.m_iNumberOfBestStructuresToSave,
							input.m_fMinDistnaceBetweenSameMoleculeSets,
							input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory.c_str());
				for (i = 0; i < (signed int)optimizedMoleculeSets.size(); ++i)
					delete optimizedMoleculeSets[i];
				optimizedMoleculeSets.clear();
			} else {
				Input::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
					input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
					input.m_sSaveLogFilesInDirectory.c_str());
			}
			
			iTotalAcceptedTransitions = 0;
			iTotalTransitions = 0;
			if (input.m_bPerformBasinHopping && (input.m_fQuenchingFactor == 1.0)) {
				cout << messages->m_sPerformingBasinHopping << "..." << endl;
				fout << messages->m_sPerformingBasinHopping << "..." << endl;
			} else if (input.m_bTransitionStateSearch) {
				cout << messages->m_sSearchingForTransitionStates << "..." << endl;
				fout << messages->m_sSearchingForTransitionStates << "..." << endl;
			} else {
				cout << messages->m_sPerformingSimulatingAnnealing << "..." << endl;
				fout << messages->m_sPerformingSimulatingAnnealing << "..." << endl;
			}
		} else {
			seconds = time (NULL) - input.m_tElapsedSeconds;
			fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
			if (!fout.is_open())
			{
				cout << messages->m_sCantAppendToFile << ": " << input.m_sOutputFileName << endl;
				throw messages->m_sCantOpenOutputFile.c_str();
			} else
				fout << messages->m_sResumingProgram << "..." << endl;
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
			cout << messages->m_sNonResumableFile << endl;
			cout << messages->m_sUseInpFileInstead << endl;
			throw messages->m_sNonResumableFile.c_str();
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
			if (!Mpi::calculateEnergies(energyCalculationType, moleculeSetsTransformed, optimizedMoleculeSets))
				throw messages->m_sNotAllCalcFinished.c_str();
			if (Mpi::s_timeToFinish)
				throw messages->m_sTimeToFinishAndCleanUp.c_str();
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
						fout << messages->m_sSettingTemperature << " " << Atom::printFloat(newTemp) << "..." << endl;
						cout << messages->m_sSettingTemperature << " " << Atom::printFloat(newTemp) << "..." << endl;
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
				} else {
					delete moleculeSetsTransformed[i];
				}
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
						input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory.c_str());
				for (i = 0; i < (signed int)optimizedMoleculeSets.size(); ++i)
					delete optimizedMoleculeSets[i];
				optimizedMoleculeSets.clear();
			} else {
				Input::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
					input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
				        input.m_sSaveLogFilesInDirectory.c_str());
			}
			
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				fout << messages->m_sAbbreviationIteration << ": " << input.m_iIteration;
				if (input.m_bTransitionStateSearch)
					fout << " " << messages->m_sAbbreviationNoTransitionStates << ": " << bestNMoleculeSets.size();
				fout << setiosflags(ios::fixed) << setprecision(8);
				if (bestNMoleculeSets.size() > 0)
					fout << " " << messages->m_sAbbreviationBestEnergy << ": " << bestNMoleculeSets[0]->getEnergy();
				else
					fout << " " << messages->m_sAbbreviationBestEnergy << ": -";
				if (!input.m_bTransitionStateSearch) {
					fout << setiosflags(ios::fixed) << setprecision(1)
					     << " " << messages->m_sAbbreviationTemperature << ": " << input.m_fStartingTemperature;
					fout << " " << messages->m_sAbbreviationNumberOfPerterbations << ": " << iNumTransitions;
					fout << setiosflags(ios::fixed) << setprecision(4)
					     << " " << messages->m_sAbbreviationCoordinateAnglePerterbations << ": " << input.m_fStartCoordinatePerturbation << ", "
					     << input.m_fStartAnglePerturbation * RAD_TO_DEG;
					fout << setiosflags(ios::fixed) << setprecision(1)
					     << " " << messages->m_sAbbreviationAcceptedPerterbationPercentage << ": " << (fAcceptanceRatio*100) << "%";
				}
				fout << endl;
				// Note: if you modify the above print statement, also modify Input::compileIndependentRunData

//				fout << setiosflags(ios::fixed) << setprecision(1)
//				     << " Converged: " << (Mpi::s_percentageOfSuccessfulCalculations * 100) << "%" << endl;
				
				cout << messages->m_sAbbreviationIteration << ": " << input.m_iIteration;
				if (input.m_bTransitionStateSearch)
					cout << " " << messages->m_sAbbreviationNoTransitionStates << ": " << bestNMoleculeSets.size();
				cout << setiosflags(ios::fixed) << setprecision(8);
				if (bestNMoleculeSets.size() > 0)
					cout << " " << messages->m_sAbbreviationBestEnergy << ": " << bestNMoleculeSets[0]->getEnergy();
				else
					cout << " " << messages->m_sAbbreviationBestEnergy << ": -";
				if (!input.m_bTransitionStateSearch) {
					cout << setiosflags(ios::fixed) << setprecision(1)
					     << " " << messages->m_sAbbreviationTemperature << ": " << input.m_fStartingTemperature;
					cout << " " << messages->m_sAbbreviationNumberOfPerterbations << ": " << iNumTransitions;
					cout << setiosflags(ios::fixed) << setprecision(4)
					     << " " << messages->m_sAbbreviationCoordinateAnglePerterbations << ": " << input.m_fStartCoordinatePerturbation << ", "
					     << input.m_fStartAnglePerturbation * RAD_TO_DEG;
					cout << setiosflags(ios::fixed) << setprecision(1)
					     << " " << messages->m_sAbbreviationAcceptedPerterbationPercentage << ": " << (fAcceptanceRatio*100) << "%";
				}
				cout << endl;
//				cout << " Converged: " << (Mpi::s_percentageOfSuccessfulCalculations * 100) << "%" << endl;
			}
			
			if (input.m_iFreezeUntilIteration == input.m_iIteration) {
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->unFreezeAll(-1,-1);
				cout << messages->m_sUnFreezingSeededMolecules << "..." << endl;
				fout << messages->m_sUnFreezingSeededMolecules << "..." << endl;
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
						fout << messages->m_sDecreasingTemp << "..." << endl;
						cout << messages->m_sDecreasingTemp << "..." << endl;
					}
				}
			}
			
			if (input.m_iIteration >= input.m_iMaxIterations)
				input.m_bRunComplete = true;
			if (input.m_bDecreasingTemp && (input.m_fStartingTemperature < input.m_fMinTemperatureToStop) &&
			    (fAcceptanceRatio < input.m_fMinAcceptedTransitions))
				input.m_bRunComplete = true;
			
			if ((input.m_sResumeFileName.length() > 0) &&
			    ((input.m_iIteration % input.m_iResumeFileNumIterations) == 0))
			{
				input.writeResumeFile(tempResumeFileName, moleculeSets, bestNMoleculeSets, emptyMoleculeSets,
						      time (NULL) - seconds, true);
				if (system(commandString)) {	
					cout << messages->m_sErrorUpdatingResume << ": " << commandString << endl;
					throw messages->m_sErrorUpdatingResume.c_str();
				}
			}
			
			if (input.m_bTestMode)
				if (!input.printTestFileGeometry(input.m_iIteration, *moleculeSets[0]))
					throw messages->m_sErrorPrintingTestFileGeometry.c_str();
			if (Mpi::s_timeToFinish && !input.m_bRunComplete)
				throw messages->m_sTimeToFinishAndCleanUp.c_str();
		} while (!input.m_bRunComplete);
		
		cout << messages->m_sEndTemperatureAndIteration1 << " " << input.m_iIteration << " " << messages->m_sEndTemperatureAndIteration2 << " " << input.m_fStartingTemperature << "." << endl;
		if ((input.m_pSelectedEnergyProgram->m_sPathToExecutable.length() == 0) && !input.m_bTransitionStateSearch) {
			cout << messages->m_sOptimizingBestStructure << "..." << endl;
			fout << messages->m_sOptimizingBestStructure << "..." << endl;
			emptyMoleculeSets.push_back(bestNMoleculeSets[0]);
			Mpi::calculateEnergies(OPTIMIZE_AND_READ, emptyMoleculeSets, optimizedMoleculeSets);
		
			bestNMoleculeSets[0]->copy(*optimizedMoleculeSets[0]);
			delete optimizedMoleculeSets[0];
			optimizedMoleculeSets.clear();
			emptyMoleculeSets.clear();
		}
		
		fout << endl << messages->m_sBestStructure << ":" << endl;
		cout << endl << messages->m_sBestStructure << ":" << endl;
		bestNMoleculeSets[0]->printToScreen();
		bestNMoleculeSets[0]->print(fout);
		cout << setiosflags(ios::fixed) << setprecision(8) << messages->m_sEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
		fout << setiosflags(ios::fixed) << setprecision(8) << messages->m_sEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << endl << endl;

		seconds = time (NULL) - seconds;
		days = seconds / (24*60*60);
		seconds = seconds - days * (24*60*60);
		hours = seconds / (60*60);
		seconds = seconds - hours * (60*60);
		minutes = seconds / 60;
		seconds = seconds - minutes * 60;
		fout << messages->m_sEnergyCalculations << ": " << input.m_iNumEnergyEvaluations << endl;
		cout << messages->m_sEnergyCalculations << ": " << input.m_iNumEnergyEvaluations << endl;
		fout << messages->m_sFinishTime1 << " " << days << " " << messages->m_sFinishTime2 << ", " << hours << " " << messages->m_sFinishTime3 << ", "
		     << minutes << " " << messages->m_sFinishTime4 << ", " << seconds << " " << messages->m_sFinishTime5 << endl;
		cout << messages->m_sFinishTime1 << " " << days << " " << messages->m_sFinishTime2 << ", " << hours << " " << messages->m_sFinishTime3 << ", "
		     << minutes << " " << messages->m_sFinishTime4 << ", " << seconds << " " << messages->m_sFinishTime5 << endl;
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << messages->m_sCaughtMessage << ": " << message << endl;
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

	if (fout.is_open())
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
	const Strings* messages = Strings::instance();
	vector<MoleculeSet*> moleculeSetsMinDistEnforced; // a version of the population (moleculeSets) in which the min. distance constraint is enforced
	vector<MoleculeSet*> localBestMoleculeSets;
	vector<MoleculeSet*> optimizedMoleculeSets;
	MoleculeSet* newMoleculeSet;
	int i;
	string tempResumeFileName = input.m_sResumeFileName + ".temp";
	char commandString[500];
	char line[500];
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
				cout << messages->m_sCantWriteToFile << ": " << input.m_sOutputFileName << endl;
				throw messages->m_sCantOpenOutputFile.c_str();
			} else {
				input.printToFile(fout);
				fout << endl << endl;
				fout << messages->m_sProgramOutput << ":" << endl;
				if (seedFiles.length() > 0) {
					cout << messages->m_sSeedingPopulationFromFile << " " << seedFiles << "..." << endl;
					fout << messages->m_sSeedingPopulationFromFile << " " << seedFiles << "..." << endl;
					if (input.m_iFreezeUntilIteration > 0) {
						cout << messages->m_sFreezingSeededMoleculesFor << " " << input.m_iFreezeUntilIteration << " " << messages->m_sIterations << "..." << endl;
						fout << messages->m_sFreezingSeededMoleculesFor << " " << input.m_iFreezeUntilIteration << " " << messages->m_sIterations << "..." << endl;
					} else {
						for (i = 0; i < (signed int)moleculeSets.size(); ++i)
							moleculeSets[i]->unFreezeAll(input.m_fCoordMaximumVelocity,input.m_fAngleMaximumVelocity);
					}
				} else {
					cout << messages->m_sInitializingPopulation << "..." << endl;
					fout << messages->m_sInitializingPopulation << "..." << endl;
					if (!Init::initializePopulation(input, moleculeSets))
						throw messages->m_sCantInitializePopulation.c_str();
				}
				if (!Mpi::calculateEnergies(energyCalculationType, moleculeSets, optimizedMoleculeSets))
					throw messages->m_sNotAllCalcFinished;
				if (Mpi::s_timeToFinish)
					throw messages->m_sTimeToFinishAndCleanUp.c_str();
				input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
				Input::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
						input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
						input.m_sSaveLogFilesInDirectory.c_str());
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->initVelocities(input.m_fCoordMaximumVelocity,input.m_fAngleMaximumVelocity);
			}
			
			for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
				newMoleculeSet = new MoleculeSet();
				newMoleculeSet->copy(*moleculeSets[i]);
				bestIndividualMoleculeSets.push_back(newMoleculeSet);
				bestIndividualMoleculeSets[i]->centerInBox(input.m_boxDimensions);
			}
			
			cout << messages->m_sRunningParticleSwarmOptimization << "..." << endl;
			fout << messages->m_sRunningParticleSwarmOptimization << "..." << endl;
		} else {
			seconds = time (NULL) - input.m_tElapsedSeconds;
			fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
			if (!fout.is_open())
			{
				cout << messages->m_sCantAppendToFile << ": " << input.m_sOutputFileName << endl;
				throw messages->m_sCantOpenOutputFile.c_str();
			} else 
				fout << messages->m_sResumingProgram << "..." << endl;
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
					cout << messages->m_sSwitchingToRepulsion << "..." << endl;
					fout << messages->m_sSwitchingToRepulsion << "..." << endl;
					input.m_fAttractionRepulsion = -1;
					printf(messages->m_sResettingVisibility.c_str(), input.m_fVisibilityDistance, input.m_fStartVisibilityDistance);
					sprintf(line, messages->m_sResettingVisibility.c_str(), input.m_fVisibilityDistance, input.m_fStartVisibilityDistance);
					fout << line;
//					cout << setiosflags(ios::fixed) << setprecision(3) << messages->m_sResettingVisibility1 << " "
//					     << input.m_fVisibilityDistance << " " << messages->m_sResettingVisibility2 << " " << input.m_fStartVisibilityDistance << endl;
//					fout << setiosflags(ios::fixed) << setprecision(3) << messages->m_sResettingVisibility1 << " "
//					     << input.m_fVisibilityDistance << " " << messages->m_sResettingVisibility2 << " " << input.m_fStartVisibilityDistance << endl;
					input.m_fVisibilityDistance = input.m_fStartVisibilityDistance;
				}
				if ((input.m_fAttractionRepulsion < 0) && (diversity >= input.m_fSwitchToAttractionWhenDiversityIs)) {
					cout << messages->m_sSwitchingToAttraction << "..." << endl;
					fout << messages->m_sSwitchingToAttraction << "..." << endl;
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
					if (!Mpi::calculateEnergies(energyCalculationType, moleculeSetsMinDistEnforced, optimizedMoleculeSets))
						throw messages->m_sNotAllCalcFinished.c_str();
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
					if (!Mpi::calculateEnergies(energyCalculationType, moleculeSets, optimizedMoleculeSets))
						throw messages->m_sNotAllCalcFinished.c_str();
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
							input.m_sSaveLogFilesInDirectory.c_str());
				else
					Input::saveBestN(moleculeSets, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
							input.m_fMinDistnaceBetweenSameMoleculeSets, input.m_iNumberOfLogFilesToSave,
							input.m_sSaveLogFilesInDirectory.c_str());
				if (abs(tempFloat - bestNMoleculeSets[0]->getEnergy()) < 0.001)
					++input.m_iNumIterationsBestEnergyHasntChanged;
				else
					input.m_iNumIterationsBestEnergyHasntChanged = 0;
			}
			  
			fout << messages->m_sAbbreviationIteration << ": " << input.m_iIteration;
			fout << setiosflags(ios::fixed) << setprecision(8)
			     << " " << messages->m_sAbbreviationBestEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << setprecision(3)
				 << " " << messages->m_sAbbreviationCoordinateVelocity << ": " << maxCoordinateVelocity << ", " << avgCoordinateVelocity
				 << " " << messages->m_sAbbreviationAngleVelocity << ": " << (maxAngularVelocity * RAD_TO_DEG) << ", "
				 << (avgAngularVelocity * RAD_TO_DEG)
			     << setiosflags(ios::fixed) << setprecision(3)
			     << " " << messages->m_sAbbreviationDiversity << ": " << diversity
			     << setiosflags(ios::fixed) << setprecision(1)
			     << " " << messages->m_sAbbreviationVisibility << ": " << (visibility * 100) << "%" << endl;
//			     << " Converged: " << (Mpi::s_percentageOfSuccessfulCalculations * 100) << "%" << endl;
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				cout << messages->m_sAbbreviationIteration << ": " << input.m_iIteration;
				cout << setiosflags(ios::fixed) << setprecision(8)
				     << " " << messages->m_sAbbreviationBestEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << setprecision(3)
				     << " " << messages->m_sAbbreviationCoordinateVelocity << ": " << avgCoordinateVelocity
				     << " " << messages->m_sAbbreviationAngleVelocity << ": " << (maxAngularVelocity * RAD_TO_DEG) << ", "
				     << (avgAngularVelocity * RAD_TO_DEG)
				     << setiosflags(ios::fixed) << setprecision(3)
				     << " " << messages->m_sAbbreviationDiversity << ": " << diversity
				     << setiosflags(ios::fixed) << setprecision(1)
				     << " " << messages->m_sAbbreviationVisibility << ": " << (visibility * 100) << "%" << endl;
//				     << " Converged: " << (Mpi::s_percentageOfSuccessfulCalculations * 100) << "%" << endl;
			}
			
			if (input.m_iFreezeUntilIteration == input.m_iIteration) {
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->unFreezeAll(input.m_fCoordMaximumVelocity,input.m_fAngleMaximumVelocity);
				cout << messages->m_sUnFreezingSeededMolecules << "..." << endl;
				fout << messages->m_sUnFreezingSeededMolecules << "..." << endl;
			}
			
			if (input.m_iIteration >= input.m_iMaxIterations)
				input.m_bRunComplete = true;
			
			if ((input.m_sResumeFileName.length() > 0) && ((input.m_iIteration%input.m_iResumeFileNumIterations) == 0))
			{
				input.writeResumeFile(tempResumeFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets, time (NULL) - seconds, true);
				if (system(commandString))
				{
					cout << messages->m_sErrorUpdatingResume << ": " << commandString << endl;
					throw messages->m_sErrorUpdatingResume.c_str();
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
						     << messages->m_sSettingRMSVisibilityDist << ": " << input.m_fVisibilityDistance << endl;
						fout << setiosflags(ios::fixed) << setprecision(3)
						     << messages->m_sSettingRMSVisibilityDist << ": " << input.m_fVisibilityDistance << endl;
					}
				}
			} else
				input.m_fVisibilityDistance += input.m_fVisibilityDistanceIncrease;
			
			if (input.m_bTestMode)
				if (!input.printTestFileGeometry(input.m_iIteration, *moleculeSets[iRandomMolecule]))
					throw messages->m_sErrorPrintingTestFileGeometry.c_str();
			if (Mpi::s_timeToFinish && !input.m_bRunComplete)
				throw messages->m_sTimeToFinishAndCleanUp.c_str();
		} while (!input.m_bRunComplete);
		
		cout << messages->m_sEndTemperatureAndIteration1 << " " << input.m_iIteration << " " << messages->m_sIterations << "." << endl;
		fout << messages->m_sEndTemperatureAndIteration1 << " " << input.m_iIteration << " " << messages->m_sIterations << "." << endl;
		if (input.m_pSelectedEnergyProgram->m_sPathToExecutable.length() == 0) {
			vector<MoleculeSet*> tempMoleculeSets;
			cout << messages->m_sOptimizingBestStructure << "..." << endl;
			fout << messages->m_sOptimizingBestStructure << "..." << endl;
			tempMoleculeSets.push_back(bestNMoleculeSets[0]);
			Mpi::calculateEnergies(OPTIMIZE_AND_READ, tempMoleculeSets, optimizedMoleculeSets);
		
			bestNMoleculeSets[0]->copy(*optimizedMoleculeSets[0]);
			delete optimizedMoleculeSets[0];
			tempMoleculeSets.clear();
			optimizedMoleculeSets.clear();
		}
		
		fout << endl << messages->m_sBestStructure << ":" << endl;
		cout << endl << messages->m_sBestStructure << ":" << endl;
		bestNMoleculeSets[0]->printToScreen();
		bestNMoleculeSets[0]->print(fout);
		cout << setiosflags(ios::fixed) << setprecision(8) << messages->m_sEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
		fout << setiosflags(ios::fixed) << setprecision(8) << messages->m_sEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << endl << endl;

		seconds = time (NULL) - seconds;
		days = seconds / (24*60*60);
		seconds = seconds - days * (24*60*60);
		hours = seconds / (60*60);
		seconds = seconds - hours * (60*60);
		minutes = seconds / 60;
		seconds = seconds - minutes * 60;
		fout << messages->m_sEnergyCalculations << ": " << input.m_iNumEnergyEvaluations << endl;
		cout << messages->m_sEnergyCalculations << ": " << input.m_iNumEnergyEvaluations << endl;
		fout << messages->m_sFinishTime1 << " " << days << " " << messages->m_sFinishTime2 << ", " << hours << " " << messages->m_sFinishTime3 << ", "
		     << minutes << " " << messages->m_sFinishTime4 << ", " << seconds << " " << messages->m_sFinishTime5 << endl;
		cout << messages->m_sFinishTime1 << " " << days << " " << messages->m_sFinishTime2 << ", " << hours << " " << messages->m_sFinishTime3 << ", "
		     << minutes << " " << messages->m_sFinishTime4 << ", " << seconds << " " << messages->m_sFinishTime5 << endl;
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << messages->m_sCaughtMessage << ": " << message << endl;
	}

	if (particleDistanceMatrix != NULL) {
   		// Clean up
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete[] particleDistanceMatrix[i];
		delete[] particleDistanceMatrix;
	}
	
	for (i = 0; i < (signed int)moleculeSetsMinDistEnforced.size(); ++i)
		delete moleculeSetsMinDistEnforced[i];
	moleculeSetsMinDistEnforced.clear();
	
	for (i = 0; i < (signed int)localBestMoleculeSets.size(); ++i)
		delete localBestMoleculeSets[i];
	localBestMoleculeSets.clear();

	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();

	for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
		delete moleculeSets[i];
		delete bestIndividualMoleculeSets[i];
	}
	moleculeSets.clear();
	bestIndividualMoleculeSets.clear();

	if (fout.is_open())
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
	const Strings* messages = Strings::instance();
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
				cout << messages->m_sCantWriteToFile << ": " << input.m_sOutputFileName << endl;
				throw messages->m_sCantOpenOutputFile.c_str();
			} else {
				input.printToFile(fout);
				fout << endl << endl;
				fout << messages->m_sProgramOutput << ":" << endl;
				if (seedFiles.length() > 0) {
					cout << messages->m_sSeedingPopulationFromFile << " " << seedFiles << "..." << endl;
					fout << messages->m_sSeedingPopulationFromFile << " " << seedFiles << "..." << endl;
					if (input.m_iFreezeUntilIteration > 0) {
						cout << messages->m_sFreezingNotImplementedWGA << "..." << endl;
						fout << messages->m_sFreezingNotImplementedWGA << "..." << endl;
						input.m_iFreezeUntilIteration = 0;
					}
					for (i = 0; i < (signed int)population.size(); ++i)
						population[i]->unFreezeAll(-1,-1);
				} else {
					cout << messages->m_sInitializingPopulation << "..." << endl;
					fout << messages->m_sInitializingPopulation << "..." << endl;
					if (!Init::initializePopulation(input, population))
						throw messages->m_sCantInitializePopulation.c_str();
				}
				if (!Mpi::calculateEnergies(energyCalculationType, population, optimizedMoleculeSets))
					throw messages->m_sNotAllCalcFinished.c_str();
				if (Mpi::s_timeToFinish)
					throw messages->m_sTimeToFinishAndCleanUp.c_str();
				input.m_iNumEnergyEvaluations += (signed int)population.size();
				Input::saveBestN(population,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
						input.m_fMinDistnaceBetweenSameMoleculeSets,input.m_iNumberOfLogFilesToSave,
						input.m_sSaveLogFilesInDirectory.c_str());
			}
			
			cout << messages->m_sRunningGeneticAlgorithm << "..." << endl;
			fout << messages->m_sRunningGeneticAlgorithm << "..." << endl;
		} else {
			seconds = time (NULL) - input.m_tElapsedSeconds;
			fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
			if (!fout.is_open())
			{
				cout << messages->m_sCantAppendToFile << ": " << input.m_sOutputFileName << endl;
				throw messages->m_sCantOpenOutputFile.c_str();
			}
			fout << messages->m_sResumingProgram << "..." << endl;
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
			if (!Mpi::calculateEnergies(energyCalculationType, offSpring, optimizedMoleculeSets))
				throw messages->m_sNotAllCalcFinished.c_str();
			input.m_iNumEnergyEvaluations += (signed int)offSpring.size();
			
			Input::saveBestN(offSpring, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
					input.m_fMinDistnaceBetweenSameMoleculeSets, input.m_iNumberOfLogFilesToSave,
					input.m_sSaveLogFilesInDirectory.c_str());
			Gega::generationReplacement(*pPopulationA, offSpring, *pPopulationB);
			
			getPopulationDistanceMatrix(*pPopulationB, particleDistanceMatrix);
			diversity = calculateDiversity((signed int)pPopulationB->size(),particleDistanceMatrix,fCubeDiagonal);
			
			fout << messages->m_sAbbreviationIteration << ": " << input.m_iIteration;
			fout << setiosflags(ios::fixed) << setprecision(8)
			     << " " << messages->m_sAbbreviationBestEnergy << ": " << bestNMoleculeSets[0]->getEnergy()
			     << " " << messages->m_sAbbreviationDiversity << ": " << diversity << endl;
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				cout << messages->m_sAbbreviationIteration << ": " << input.m_iIteration;
				cout << setiosflags(ios::fixed) << setprecision(8)
				     << " " << messages->m_sAbbreviationBestEnergy << ": " << bestNMoleculeSets[0]->getEnergy()
				     << " " << messages->m_sAbbreviationDiversity << ": " << diversity << endl;
			}
			
			if (input.m_iFreezeUntilIteration == input.m_iIteration) {
				for (i = 0; i < (signed int)pPopulationB->size(); ++i)
					(*pPopulationB)[i]->unFreezeAll(-1,-1);
				cout << messages->m_sUnFreezingSeededMolecules << "..." << endl;
				fout << messages->m_sUnFreezingSeededMolecules << "..." << endl;
			}
			
			if (input.m_iIteration >= input.m_iMaxIterations)
				input.m_bRunComplete = true;
			
			if ((input.m_sResumeFileName.length() > 0) && ((input.m_iIteration%input.m_iResumeFileNumIterations) == 0))
			{
				input.writeResumeFile(tempResumeFileName, *pPopulationB, bestNMoleculeSets, emptyMoleculeSets, time (NULL) - seconds, true);
				if (system(commandString))
				{
					cout << messages->m_sErrorUpdatingResume << ": " << commandString << endl;
					throw messages->m_sErrorUpdatingResume.c_str();
				}
			}
			
			if (pPopulationA == &population) {
				pPopulationA = &population2;
				pPopulationB = &population;
			} else {
				pPopulationA = &population;
				pPopulationB = &population2;
			}
			
			if (input.m_bTestMode)
				if (!input.printTestFileGeometry(input.m_iIteration, *bestNMoleculeSets[0]))
					throw messages->m_sErrorPrintingTestFileGeometry.c_str();
			if (Mpi::s_timeToFinish && !input.m_bRunComplete)
				throw messages->m_sTimeToFinishAndCleanUp.c_str();
		} while (!input.m_bRunComplete);
		
		cout << messages->m_sEndTemperatureAndIteration1 << " " << input.m_iIteration << " " << messages->m_sIterations << "." << endl;
		fout << messages->m_sEndTemperatureAndIteration1 << " " << input.m_iIteration << " " << messages->m_sIterations << "." << endl;
		if (input.m_pSelectedEnergyProgram->m_sPathToExecutable.length() == 0) {
			vector<MoleculeSet*> tempMoleculeSets;
			cout << messages->m_sOptimizingBestStructure << "..." << endl;
			fout << messages->m_sOptimizingBestStructure << "..." << endl;
			tempMoleculeSets.push_back(bestNMoleculeSets[0]);
			Mpi::calculateEnergies(OPTIMIZE_AND_READ, tempMoleculeSets, optimizedMoleculeSets);
		
			bestNMoleculeSets[0]->copy(*optimizedMoleculeSets[0]);
			delete optimizedMoleculeSets[0];
			tempMoleculeSets.clear();
			optimizedMoleculeSets.clear();
		}
		
		fout << endl << messages->m_sBestStructure << ":" << endl;
		cout << endl << messages->m_sBestStructure << ":" << endl;
		bestNMoleculeSets[0]->printToScreen();
		bestNMoleculeSets[0]->print(fout);
		cout << setiosflags(ios::fixed) << setprecision(8) << messages->m_sEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << endl << endl;
		fout << setiosflags(ios::fixed) << setprecision(8) << messages->m_sEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << endl << endl;

		seconds = time (NULL) - seconds;
		days = seconds / (24*60*60);
		seconds = seconds - days * (24*60*60);
		hours = seconds / (60*60);
		seconds = seconds - hours * (60*60);
		minutes = seconds / 60;
		seconds = seconds - minutes * 60;
		fout << messages->m_sEnergyCalculations << ": " << input.m_iNumEnergyEvaluations << endl;
		cout << messages->m_sEnergyCalculations << ": " << input.m_iNumEnergyEvaluations << endl;
		fout << messages->m_sFinishTime1 << " " << days << " " << messages->m_sFinishTime2 << ", " << hours << " " << messages->m_sFinishTime3 << ", "
		     << minutes << " " << messages->m_sFinishTime4 << ", " << seconds << " " << messages->m_sFinishTime5 << endl;
		cout << messages->m_sFinishTime1 << " " << days << " " << messages->m_sFinishTime2 << ", " << hours << " " << messages->m_sFinishTime3 << ", "
		     << minutes << " " << messages->m_sFinishTime4 << ", " << seconds << " " << messages->m_sFinishTime5 << endl;
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << messages->m_sCaughtMessage << ": " << message << endl;
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
	
	if (fout.is_open())
		fout.close();
	if (input.m_bTestMode)
		input.printTestFileFooter();
}

void optimizeBestStructures(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets)
{
	const Strings* messages = Strings::instance();
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
			cout << messages->m_sNoStructuresRemaining << endl;
			throw messages->m_sNoStructuresRemaining.c_str();
		}
		cout << messages->m_sPerformingCalculations << "..." << endl;
		while (moleculeSets.size() > 0) {
			if (iStructuresToOptimizeAtATime > (signed int)moleculeSets.size())
				iStructuresToOptimizeAtATime = moleculeSets.size();
			
			// Perform Energy calculations
			for (i = 0; i < iStructuresToOptimizeAtATime; ++i) {
				newMoleculeSet = new MoleculeSet();
				newMoleculeSet->copy(*moleculeSets[i]);
				population.push_back(newMoleculeSet);
			}
			if (!Mpi::calculateEnergies(energyCalculationType, population, optimizedMoleculeSets))
				throw messages->m_sNotAllCalcFinished.c_str();
			input.m_iNumEnergyEvaluations += (signed int)population.size();
			
			// Update the list of optimized best structures
			Input::saveBestN(optimizedMoleculeSets, bestNMoleculeSets, (signed int)(optimizedMoleculeSets.size() +
					bestNMoleculeSets.size()), input.m_fMinDistnaceBetweenSameMoleculeSets,
					input.m_iNumberOfLogFilesToSave, input.m_sSaveLogFilesInDirectory.c_str());
			
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
				cout << messages->m_sErrorUpdatingOptimization << ": " << commandString << endl;
				throw messages->m_sErrorUpdatingOptimization.c_str();
			}
		
			// Write the output file
			cout << setiosflags(ios::fixed) << setprecision(8);
			cout << messages->m_sAbbreviationBestEnergy << ": " << bestNMoleculeSets[0]->getEnergy() << " " << messages->m_sAbbreviationBestStructures << ": " << bestNMoleculeSets.size() << " " << messages->m_sStructuresRemaining << ": " << moleculeSets.size() << endl;
			if (Mpi::s_timeToFinish)
				throw messages->m_sTimeToFinishAndCleanUp.c_str();
		}

		seconds = time (NULL) - seconds;
		days = seconds / (24*60*60);
		seconds = seconds - days * (24*60*60);
		hours = seconds / (60*60);
		seconds = seconds - hours * (60*60);
		minutes = seconds / 60;
		seconds = seconds - minutes * 60;
		cout << messages->m_sEnergyCalculations << ": " << input.m_iNumEnergyEvaluations << endl;
		cout << messages->m_sFinishTime1 << " " << days << " " << messages->m_sFinishTime2 << ", " << hours << " " << messages->m_sFinishTime3 << ", "
		     << minutes << " " << messages->m_sFinishTime4 << ", " << seconds << " " << messages->m_sFinishTime5 << endl;
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << messages->m_sCaughtMessage << ": " << message << endl;		
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
	const Strings* messages = Strings::instance();
	int numOptionArguments;
	char** optionArguments;
	int hours, minutes, seconds;
	
	if (ArgumentParser::optionPresent(messages->m_sOptionWalltime.c_str())) {
		int numPartsScanned;
		
		ArgumentParser::getOptionArguments(messages->m_sOptionWalltime.c_str() , numOptionArguments, &optionArguments);
		numPartsScanned = sscanf(optionArguments[0], "%d:%d:%d", &hours, &minutes, &seconds);
		if (numPartsScanned != 3) {
			cout << messages->m_sOptionWalltimeMessage << endl;
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

bool initCommandLineArguments(int argc, char *argv[])
{
	const Strings* messages = Strings::instance();
	vector<const char*> recognizedOptions;
	vector<int> argumentsPerOption;
	vector<const char*> optionMessages;
	
	recognizedOptions.push_back(messages->m_sOptionH.c_str());
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back(messages->m_sOptionH2.c_str());
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back(messages->m_sOptionS.c_str());
	argumentsPerOption.push_back(-1);
	optionMessages.push_back(messages->m_sOptionSMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionI.c_str());
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back(messages->m_sOptionM.c_str());
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back(messages->m_sOptionTest.c_str());
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back(messages->m_sOptionWalltime.c_str());
	argumentsPerOption.push_back(1);
	optionMessages.push_back(messages->m_sOptionWalltimeMessage.c_str());
	
	ArgumentParser::init(&recognizedOptions, &argumentsPerOption, &optionMessages);
	if (!ArgumentParser::parse(argc, argv))
		return false;
	return true;
}

int master(int rank)
{
	const Strings* messages = Strings::instance();
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
	bool masterDistributingTasks = ArgumentParser::optionPresent(messages->m_sOptionM.c_str());
	bool bIndependentRunSetupPreviouslyDone;
	
	MPI_Comm_size(MPI_COMM_WORLD, &nMPIProcesses);
	
	try {
		if ((ArgumentParser::getNumOptions() == 0) || ArgumentParser::optionPresent(messages->m_sOptionH.c_str()) || ArgumentParser::optionPresent(messages->m_sOptionH2.c_str())) {
			Strings::instance()->printOptions();
			throw messages->m_sHelpMenu.c_str();
		}
		if (rank == 0) {
			inputFileName = ArgumentParser::s_argv[ArgumentParser::s_argc-1];
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, true, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets)) {
				cout << messages->m_sLastArgMustBeInput << endl;
				throw messages->m_sLastArgMustBeInput.c_str();
			}
			if (ArgumentParser::optionPresent(messages->m_sOptionI.c_str())) {
				if (masterDistributingTasks) {
					cout << messages->m_sCantUseIMOptionsTogether << endl;
					throw messages->m_sCantUseIMOptionsTogether.c_str();
				}
				if (input.m_iAlgorithmToDo != SIMULATED_ANNEALING) {
					cout << messages->m_sIOptOnlyForSimulatedAnnealing << endl;
					throw messages->m_sIOptOnlyForSimulatedAnnealing.c_str();
				}
				if (input.m_bResumeFileRead || input.m_bOptimizationFileRead) {
					cout << messages->m_sIOptOnlyWInput << endl;
					throw messages->m_sIOptOnlyWInput.c_str();
				}
				if (nMPIProcesses != input.m_iTotalPopulationSize) {
					cout << messages->m_sIOptPopSizeEqualNoMPIProcs1 << "(" << input.m_iTotalPopulationSize
					     << ") " << messages->m_sIOptPopSizeEqualNoMPIProcs2 << "(" << nMPIProcesses << ")." << endl;
					throw messages->m_sIOptPopSizeEqualNoMPIProcs.c_str();
				}
			}
			answer = input.m_sPathToEnergyFiles + "/stop";
			Mpi::setQuitFlag(answer);
		}
		if ((rank == 0) && ArgumentParser::optionPresent(messages->m_sOptionS.c_str())) { // # Do seeding
			MoleculeSet* pBestMoleculeSetSeeded = NULL;
			int* iNumStructuresOfEachTypeBest = NULL;
			try {
				if (input.m_bResumeFileRead || input.m_bOptimizationFileRead) {
					cout << messages->m_sOptionSMessage << endl;
					throw messages->m_sOptionSMessage.c_str();
				}
				ArgumentParser::getOptionArguments(messages->m_sOptionS.c_str(), numOptionArguments, &optionArguments);
				--numOptionArguments; // The last argument should be the input file
				
				if (numOptionArguments < 2) {
					cout << messages->m_sOptionSMessage << endl;
					throw messages->m_sOptionSMessage.c_str();
				}
				
				input.m_iFreezeUntilIteration = atoi(optionArguments[numOptionArguments-1]);
				if (input.m_iFreezeUntilIteration < 0) {
					cout << messages->m_sFreezingIterationsCantBeNegative << endl;
					throw messages->m_sFreezingIterationsCantBeNegative.c_str();
				}
				
				numSeedFiles = numOptionArguments-1;
				for (i = 0; i < numSeedFiles; ++i) {
					seedFileName = optionArguments[i];
					seedFiles += seedFileName;
					if (i != (numSeedFiles-1))
						seedFiles += ", ";
					cout << messages->m_sReadingSeedFile << ": " << seedFileName << endl;
					if (!inputSeeded.open(seedFileName, false, false, moleculeSetsTemp, bestNMoleculeSetsTemp, bestIndividualMoleculeSetsTemp))
						throw messages->m_sCantReadFile.c_str();
					if (!(inputSeeded.m_bResumeFileRead || inputSeeded.m_bOptimizationFileRead)) {
						cout << messages->m_sSeedFileNotInputFile << ": " << seedFileName << endl << endl;
						throw messages->m_sSeedFileNotInputFile.c_str();
					}
					if (!input.seedCompatible(inputSeeded)) {
						cout << messages->m_sSeedFileIncompatible1 << " " << seedFileName << " "
						     << messages->m_sSeedFileIncompatible2 << " " << inputFileName << "." << endl;
						throw messages->m_sSeedFileIncompatible.c_str();
					}
					
					for (j = 0; j < (signed int)bestNMoleculeSetsTemp.size(); ++j)
						if (bestNMoleculeSetsTemp[j]->isFragmented(input.m_fMaxAtomDistance)) {
							cout << messages->m_sSeedFileContainsFragmented << endl;
							printf(messages->m_sStructureIsFragmented.c_str(), j+1, seedFileName.c_str());
							cout << endl << endl;
							throw messages->m_sSeedFileContainsFragmented.c_str();
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
							cout << messages->m_sFailedToInit3dNonFragStructure1 << " " << INITIALIZATION_TRIES << " " << messages->m_sFailedToInit3dNonFragStructure2 << endl;
							delete pMoleculeSet;
							throw messages->m_sFailedToInit3dNonFragStructure.c_str();
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
						input.m_fMinDistnaceBetweenSameMoleculeSets, 0, NULL);
				while ((signed int)moleculeSets.size() < input.m_iTotalPopulationSize) {
					pMoleculeSet = new MoleculeSet();
						
					if (!pMoleculeSet->initFromSeed(*pBestMoleculeSetSeeded, input.m_iNumStructureTypes,
									iNumStructuresOfEachTypeBest, input.m_iNumStructuresOfEachType,
									input.m_cartesianPoints, input.m_atomicNumbers, input.m_boxDimensions,
									input.m_fMaxAtomDistance, INITIALIZATION_TRIES)) {
						cout << messages->m_sFailedToInit3dNonFragStructure1 << " " << INITIALIZATION_TRIES << " " << messages->m_sFailedToInit3dNonFragStructure2 << endl;
						delete pMoleculeSet;
						throw messages->m_sFailedToInit3dNonFragStructure.c_str();
					}
					moleculeSets.push_back(pMoleculeSet);
				}
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->setId(i+1);
				if (input.m_iFreezeUntilIteration > 0)
					for (i = 0; i < (signed int)moleculeSets.size(); ++i)
						if (moleculeSets[i]->getNumberOfMoleculesFrozen() == moleculeSets[i]->getNumberOfMolecules()) {
							cout << messages->m_sFreezingRequiresOneLessMolecule << endl;
							throw messages->m_sFreezingRequiresOneLessMolecule.c_str();
						}
			} catch (const char* message) {
				error = true;
				if (PRINT_CATCH_MESSAGES)
					cerr << messages->m_sCaughtMessage << ": " << message << endl;
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
		if (ArgumentParser::optionPresent(messages->m_sOptionI.c_str())) {
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
					         WORK_TAG,           /* user chosen message tag */
					         MPI_COMM_WORLD);   /* default communicator */
				}
				inputFileName = inputFileNames[0];
			} else {
				MPI_Status status;
				char messageBuffer[500];
				MPI_Recv(messageBuffer, sizeof(messageBuffer), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				if (status.MPI_TAG != WORK_TAG)
					throw "Failed to receive work tag in main.cc";
				inputFileName = messageBuffer;
			}
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, true, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw messages->m_sErrorOpeningInputWIOpt;
		}
		string temp;
		if (input.m_bTransitionStateSearch && (input.m_pSelectedEnergyProgram->m_iProgramID == LENNARD_JONES)) {
			cout << messages->m_sNoTransSearchWLennardJones << endl;
			cout << messages->m_sNoTransFoundContinueAnyway << " ";
			cin >> temp;
			if (temp != messages->m_sYes)
				throw "";
		}
		if (!Energy::init(input, rank))
			throw messages->m_sFailedToInitEnergyCalc.c_str();
		if (ArgumentParser::optionPresent(messages->m_sOptionTest.c_str())) {
			if (input.m_bOptimizationFileRead) {
				cout << messages->m_sNoTestWithOptFiles << endl;
				throw messages->m_sNoTestWithOptFiles.c_str();
			} else
				input.m_bTestMode = true;
		}
		if ((input.m_iNumberOfLogFilesToSave > 0) && (input.m_pSelectedEnergyProgram->m_iNumOutputFileTypes > 0) &&
		    ((input.m_bOptimizationFileRead && (bestNMoleculeSets.size() == 0)) ||
		     (!input.m_bOptimizationFileRead && !input.m_bResumeFileRead) ||
		     (stat(input.m_sSaveLogFilesInDirectory.c_str(), &fileStatistics) != 0))) {  // if the directory doesn't exist
			// Delete the old log files
			directory = opendir(input.m_sSaveLogFilesInDirectory.c_str());
			if (directory) { // If we were able to open the directory
				i = 0;
				while ((dirp = readdir(directory)) != NULL ) {
					if ((0 == strcmp(".", dirp->d_name)) || (0 == strcmp( "..",dirp->d_name)))
						continue;
					++i;
				}
				answer = "";
				if (i > 0) {
					cout << messages->m_sBestNDirectoryNotEmpty1 << " " << input.m_sSaveLogFilesInDirectory << ". " << messages->m_sBestNDirectoryNotEmpty2;
					cin >> answer;
				}
				if ((i == 0) || (answer == messages->m_sYes)) {
					cout << messages->m_sDeletingDirectory << ": " << input.m_sSaveLogFilesInDirectory << endl;
					snprintf(commandLine, 500, "rm -rf %s", input.m_sSaveLogFilesInDirectory.c_str());
					if (system(commandLine) != 0) {
						cout << messages->m_sCouldntDeleteFilesInDir << endl;
						throw messages->m_sCouldntDeleteFilesInDir.c_str();
					}
				} else {
					cout << messages->m_sDirectoryNotDeleted << ": " << input.m_sSaveLogFilesInDirectory << " " << messages->m_sExiting << "..." << endl;
					throw messages->m_sDirectoryNotDeleted.c_str();
				}
				closedir(directory);
			}
			snprintf(commandLine, 500, "mkdir %s", input.m_sSaveLogFilesInDirectory.c_str());
			cout << messages->m_sCreatingDirectory << ": " << input.m_sSaveLogFilesInDirectory << endl;
			if (system(commandLine) != 0) {
				cout << messages->m_sCouldntCreateDirectory << endl;
				throw messages->m_sCouldntCreateDirectory.c_str();
			}
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
				cout << messages->m_sUnrecognizedArgumentOrOption << ": " << argument << endl;
				foundUnrecognizedOptions = true;
			}
			argument = ArgumentParser::nextUnrecognized();
		}
		if (foundUnrecognizedOptions)
			throw messages->m_sUnrecognizedArgumentOrOption.c_str();

		if (input.m_bOptimizationFileRead) {
			if (!Mpi::masterSetup(input.m_iStructuresToOptimizeAtATime, false,masterDistributingTasks, rank))
				throw messages->m_sCouldntCreateScratchDirectory.c_str();
			optimizeBestStructures(input, moleculeSets, bestNMoleculeSets);
			Mpi::end(rank);
		} else {
			if (!Mpi::masterSetup(input.m_iTotalPopulationSize, ArgumentParser::optionPresent(messages->m_sOptionI.c_str()), masterDistributingTasks, rank))
				throw messages->m_sCouldntCreateScratchDirectory.c_str();
			if (input.m_bResumeFileRead && input.m_bRunComplete) {
				cout << messages->m_sRunComplete << endl;
			} else {
				switch (input.m_iAlgorithmToDo) {
				case SIMULATED_ANNEALING:
					simulatedAnnealing(input, moleculeSets, bestNMoleculeSets, seedFiles, ArgumentParser::optionPresent(messages->m_sOptionI.c_str()));
					break;
				case PARTICLE_SWARM_OPTIMIZATION:
					particleSwarmOptimization(input, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets, seedFiles);
					break;
				case GENETIC_ALGORITHM:
					gega(input, moleculeSets, bestNMoleculeSets, seedFiles);
					break;
				}
			}
			Mpi::end(rank);
			if ((input.m_iAlgorithmToDo == SIMULATED_ANNEALING) && (rank == 0) &&
			    ArgumentParser::optionPresent(messages->m_sOptionI.c_str()) && (input.m_bRunComplete || Mpi::s_timeToFinish)) {
				inputFileName = ArgumentParser::s_argv[ArgumentParser::s_argc-1];
				cout << messages->m_sReadingFile << ": " << inputFileName << endl;
				if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
					throw "Failed to reopen input file name before calling compileIndependentRunData.";
				input.compileIndependentRunData(true);
			}
		}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << messages->m_sCaughtMessage << ": " << message << endl;
		Mpi::end(rank);
	}
	if (Mpi::s_timeToFinish)
		cout << messages->m_sTimeBeforeWallTime1 << " " << (Mpi::s_wallTime - time(NULL)) << " " << messages->m_sTimeBeforeWallTime2 << "..." << endl;
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
		if (!Init::initProgram())
			throw "Failed to init program.";
		
		if (!initCommandLineArguments(argc, argv))
			throw "Failed to read command line args.";
		const Strings* messages = Strings::instance();
		if (ArgumentParser::optionPresent(messages->m_sOptionI.c_str()) && Strings::s_sDefaultLanguageCode != "en") {
			cout << "The -i option is not yet implemented with non-English languages." << endl;
			throw "";
		}
		if (!setWallTime())
			throw "Failed to set wall time";
		if ((myrank == 0) || ArgumentParser::optionPresent(messages->m_sOptionI.c_str()))
			master(myrank);
		else {
			Mpi::slave(myrank, ArgumentParser::optionPresent(messages->m_sOptionM.c_str()));
			Mpi::end(myrank);
		}
	} catch(const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	ArgumentParser::cleanUp();
	EnergyProgram::cleanUp();

	/* Shut down MPI */
	MPI_Finalize();
	return 0;
}

