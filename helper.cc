////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "helper.h"

int main(int argc, char *argv[])
{
	const Strings* messages = Strings::instance();
	vector<const char*> recognizedOptions;
	vector<int> argumentsPerOption;
	vector<const char*> optionMessages;
	int numOptionArguments;
	char** optionArguments;
	
	if (!Init::initProgram(1)) {
		return 0;
	}
	
	recognizedOptions.push_back(messages->m_sOptionHelpH.c_str());
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back(messages->m_sOptionHelpH2.c_str());
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back(messages->m_sOptionHelpT.c_str());
	argumentsPerOption.push_back(2);
	optionMessages.push_back(messages->m_sOptionHelpTMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionHelpC.c_str());
	argumentsPerOption.push_back(3);
	optionMessages.push_back(messages->m_sOptionHelpCMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionHelpP.c_str());
	argumentsPerOption.push_back(3);
	optionMessages.push_back(messages->m_sOptionHelpPMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionHelpO.c_str());
	argumentsPerOption.push_back(4);
	optionMessages.push_back(messages->m_sOptionHelpOMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionHelpA.c_str());
	argumentsPerOption.push_back(2);
	optionMessages.push_back(messages->m_sOptionHelpAMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionHelpOR.c_str());
	argumentsPerOption.push_back(3);
	optionMessages.push_back(messages->m_sOptionHelpORMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionHelpRO.c_str());
	argumentsPerOption.push_back(3);
	optionMessages.push_back(messages->m_sOptionHelpROMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionHelpB.c_str());
	argumentsPerOption.push_back(1);
	optionMessages.push_back("");
	recognizedOptions.push_back(messages->m_sOptionHelpM.c_str());
	argumentsPerOption.push_back(4);
	optionMessages.push_back(messages->m_sOptionHelpMMessage.c_str());
	recognizedOptions.push_back(messages->m_sOptionHelpU.c_str());
	argumentsPerOption.push_back(1);
	optionMessages.push_back(messages->m_sOptionHelpUMessage.c_str());
	
	ArgumentParser::init(&recognizedOptions, &argumentsPerOption, &optionMessages);
	if (!ArgumentParser::parse(argc, argv))
		return 0;

	string inputFileName;
	string outputDirectory;
	string comPrefix;
	string tempOutputFileName;
	string sId;
	string emptyString;
	ofstream fout;
	int i, j;
	string seedFiles;
	Input input;
	Input resume;
	string resumeFileName;
	Input inputSeeded;
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
	int numFiles;
	
	try {
		if ((ArgumentParser::getNumOptions() == 0) || ArgumentParser::optionPresent(messages->m_sOptionHelpH.c_str()) || ArgumentParser::optionPresent(messages->m_sOptionHelpH2.c_str())) {
			Strings::instance()->printHelperOptions();
			throw "";
		}
		if (ArgumentParser::optionPresent(messages->m_sOptionHelpT.c_str())) { // # Make an output file from a resume file that's readable
			if (ArgumentParser::getNumOptions() > 1) {
				printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpT.c_str());
				cout << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments(messages->m_sOptionHelpT.c_str(), numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			tempOutputFileName = optionArguments[1];
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (!(input.m_bResumeFileRead || input.m_bOptimizationFileRead)) {
				cout << messages->m_sNotResumeOrOptFile << endl;
				throw "";
			} else {
				fout.open(tempOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
				if (!fout.is_open())
				{
					cout << messages->m_sCantOpenOutputFile << ": " << tempOutputFileName << endl;
					throw "";
				}
				input.printInputParamsToFile(fout);
				fout << endl << endl << messages->m_sBest << " " << bestNMoleculeSets.size() << " " << messages->m_sStructuresFromIteration << " "
				     << input.m_iIteration << ":" << endl;
				for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
				{
					fout << messages->m_sStructureNumber << (i+1) << ": " << endl;
					bestNMoleculeSets[i]->print(fout);
					fout << setiosflags(ios::fixed) << setprecision(8) << messages->m_sEnergy << ": "
					<< bestNMoleculeSets[i]->getEnergy() << endl << endl;
				}
				fout << endl << endl << messages->m_sPopulationOf << " " << moleculeSets.size() << " " << messages->m_sStructuresFromIteration << " "
				     << input.m_iIteration << ":" << endl;
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				{
					fout << messages->m_sStructureNumber << (i+1) << ": " << endl;
					moleculeSets[i]->print(fout);
					fout << setiosflags(ios::fixed) << setprecision(8) << messages->m_sEnergy << ": "
					<< moleculeSets[i]->getEnergy() << endl << endl;
				}
				fout.close();
				cout << messages->m_sOutputWrittenTo << ": " << tempOutputFileName << endl;
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
		} else if (ArgumentParser::optionPresent(messages->m_sOptionHelpB.c_str())) { // # Check to make sure bonds were recognized correctly
			if (ArgumentParser::getNumOptions() > 1) {
				printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpB.c_str());
				cout << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments(messages->m_sOptionHelpB.c_str(), numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
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
		} else if (ArgumentParser::optionPresent(messages->m_sOptionHelpM.c_str())) { // # Check to make sure bonds were recognized correctly
			if (ArgumentParser::getNumOptions() > 1) {
				printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpM.c_str());
				cout << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments(messages->m_sOptionHelpM.c_str(), numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			FLOAT angle = atof(optionArguments[1])/180*PIE;
			tempOutputFileName = optionArguments[2];
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				delete moleculeSets[i];
			moleculeSets.clear();
			if (input.m_tempelateMoleculeSet.performBondRotations(angle, moleculeSets)) {
				sscanf(optionArguments[3], "%d", &input.m_iStructuresToOptimizeAtATime);
				if ((input.m_iStructuresToOptimizeAtATime <= 0) ||
				(input.m_iStructuresToOptimizeAtATime > (signed int)moleculeSets.size())) {
					printf(messages->m_sEnterNumStructuresToOptimizeAt1Time.c_str(), moleculeSets.size());
					cout << endl << endl;
				}
				printf(messages->m_sCreatingOptimizationFile.c_str(), tempOutputFileName.c_str());
				cout << endl;
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
		} else if (ArgumentParser::optionPresent(messages->m_sOptionHelpC.c_str()) || ArgumentParser::optionPresent(messages->m_sOptionHelpP.c_str())) { // # Check to make sure bonds were recognized correctly
			bool cOption = ArgumentParser::optionPresent(messages->m_sOptionHelpC.c_str());
			if (ArgumentParser::getNumOptions() > 1) {
				if (cOption)
					printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpC.c_str());
				else
					printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpP.c_str());
				cout << endl;
				throw "";
			}
			if (cOption)
				ArgumentParser::getOptionArguments(messages->m_sOptionHelpC.c_str(), numOptionArguments, &optionArguments);
			else
				ArgumentParser::getOptionArguments(messages->m_sOptionHelpP.c_str(), numOptionArguments, &optionArguments);
			vector<MoleculeSet*> *moleculeSetsToUse;
			inputFileName = optionArguments[0];
			outputDirectory = optionArguments[1];
			Input::checkDirectoryOrFileName(outputDirectory);
			if (stat(outputDirectory.c_str(), &fileStatistics) != 0) { // If the file doesn't exist
				while (true) {
					printf(messages->m_sDirectoryDoesntExistCreateIt.c_str(), outputDirectory.c_str());
					printf(" ");
					cin >> answer;
					if (messages->m_sYes == answer) {
						snprintf(commandLine, 500, "mkdir %s", outputDirectory.c_str());
						if (system(commandLine) == 0) {
							printf(messages->m_sCreatedDirectory.c_str(), outputDirectory.c_str());
							cout << endl;
							break;
						} else {
							printf(messages->m_sErrorCreatingDirectory.c_str(), outputDirectory.c_str());
							cout << endl;
							throw "";
						}
					} else if (messages->m_sNo == answer) {
						cout << messages->m_sDirectoryNotCreated << endl << endl;
						throw "";
					} else {
						printf(messages->m_sAnswerYesOrNo.c_str(), messages->m_sYes.c_str(), messages->m_sNo.c_str());
						cout << endl << endl;
					}
				}
			}
			
			comPrefix = optionArguments[2];
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (!Energy::init(input, 0))
				throw "";
			if (!(input.m_bResumeFileRead || input.m_bOptimizationFileRead)) {
				cout << messages->m_sNotResumeOrOptFile << endl;
				throw "";
			}
			
			if (cOption)
				moleculeSetsToUse = &bestNMoleculeSets;
			else
				moleculeSetsToUse = &moleculeSets;
			
			if (moleculeSetsToUse->size() > 100) {
				printf(messages->m_sNumStructuresInFile.c_str(), moleculeSetsToUse->size());
				cout << endl;
				printf(messages->m_sEnterNumInputFiles.c_str(), input.m_pSelectedEnergyProgram->m_sName.c_str(), messages->m_sAll.c_str());
				printf(" ");
				cin >> answer;
				if (answer == messages->m_sAll)
					numFiles = moleculeSetsToUse->size();
				else
					numFiles = atoi(answer.c_str());
				if (numFiles <= 0) {
					cout << messages->m_sFilesNotCreated.c_str() << endl << endl;
					throw "";
				}
			} else
				numFiles = moleculeSetsToUse->size();
			printf(messages->m_sCreateingNInputFiles.c_str(), numFiles);
			cout << endl;
			for (i = 0; i < numFiles; ++i) {
				(*moleculeSetsToUse)[i]->setInputEnergyFile(outputDirectory.c_str(), comPrefix.c_str(), i+1, input.m_pSelectedEnergyProgram->m_sInputFileExtension.c_str());
				Energy::createInputFile(*(*moleculeSetsToUse)[i], i+1, false, true);
			}
			cout << messages->m_sDone << endl;
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
		} else if (ArgumentParser::optionPresent(messages->m_sOptionHelpO.c_str())) { // # Create an optimization file
			if (ArgumentParser::getNumOptions() > 1) {
				printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpO.c_str());
				cout << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments(messages->m_sOptionHelpO.c_str(), numOptionArguments, &optionArguments);
			int numToTransfer;
			inputFileName = optionArguments[0];
			tempOutputFileName = optionArguments[1];
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (!input.m_bResumeFileRead && !input.m_bOptimizationFileRead) {
				cout << messages->m_sNotResumeOrOptFile << endl << endl;
				throw "";
			}
			sscanf(optionArguments[2], "%d", &numToTransfer);
			if ((numToTransfer <= 0) ||
			(numToTransfer > (signed int)bestNMoleculeSets.size())) {
				printf(messages->m_sEnterNumStructuresToTransfer.c_str(), bestNMoleculeSets.size());
				cout << endl << endl;
				throw "";
			}
			if (input.m_iNumberOfLogFilesToSave < numToTransfer && input.m_pSelectedEnergyProgram->m_iProgramID != LENNARD_JONES) {
				printf(messages->m_sChangingNumSavedOptimizedStructures.c_str(), numToTransfer);
				cout << endl;
				input.m_iNumberOfBestStructuresToSave = numToTransfer;
				input.m_iNumberOfLogFilesToSave = numToTransfer;
			}
			sscanf(optionArguments[3], "%d", &input.m_iStructuresToOptimizeAtATime);
			if ((input.m_iStructuresToOptimizeAtATime <= 0) ||
			    (input.m_iStructuresToOptimizeAtATime > (signed int)bestNMoleculeSets.size())) {
				printf(messages->m_sEnterNumStructuresToOptimizeAt1Time.c_str(), bestNMoleculeSets.size());
				cout << endl << endl;
				throw "";
			}
			while (numToTransfer < (signed int)bestNMoleculeSets.size()) {
				delete bestNMoleculeSets[bestNMoleculeSets.size()-1];
				bestNMoleculeSets.pop_back();
			}
			input.m_iNumEnergyEvaluations = 0;
			input.writeResumeFile(tempOutputFileName, bestNMoleculeSets, emptyMoleculeSets,
					emptyMoleculeSets, 0, false);
			printf(messages->m_sCreatedOptFile.c_str(), tempOutputFileName.c_str());
			cout << endl;
			if (input.m_iNumberOfLogFilesToSave > 0) {
				printf(messages->m_sPleaseCleanDirectory.c_str(), input.m_sSaveLogFilesInDirectory.c_str());
				cout << endl;
			}
			// Clean up
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				delete moleculeSets[i];
			for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
				delete bestNMoleculeSets[i];
			moleculeSets.clear();
			bestNMoleculeSets.clear();
		} else if (ArgumentParser::optionPresent(messages->m_sOptionHelpOR.c_str()) || ArgumentParser::optionPresent(messages->m_sOptionHelpRO.c_str())) { // # Create an optimization file with random structures
			bool order1 = ArgumentParser::optionPresent(messages->m_sOptionHelpOR.c_str());
			if (ArgumentParser::getNumOptions() > 1) {
				if (order1)
					printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpOR.c_str());
				else
					printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpRO.c_str());
				cout << endl;
				throw "";
			}
			if (order1)
				ArgumentParser::getOptionArguments(messages->m_sOptionHelpOR.c_str(), numOptionArguments, &optionArguments);
			else
				ArgumentParser::getOptionArguments(messages->m_sOptionHelpRO.c_str(), numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			tempOutputFileName = optionArguments[1];
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (input.m_bResumeFileRead || input.m_bOptimizationFileRead) {
				cout << messages->m_sPleaseNoResOrOptFile << endl << endl;
				throw "";
			}
			if (Init::initializePopulation(input, moleculeSets)) {
				input.m_iNumberOfBestStructuresToSave = moleculeSets.size();
				input.m_iNumberOfLogFilesToSave = moleculeSets.size();
				sscanf(optionArguments[2], "%d", &input.m_iStructuresToOptimizeAtATime);
				if ((input.m_iStructuresToOptimizeAtATime <= 0) ||
				(input.m_iStructuresToOptimizeAtATime > (signed int)moleculeSets.size())) {
					printf(messages->m_sEnterNumStructuresToOptimizeAt1Time.c_str(), moleculeSets.size());
					cout << endl << endl;
				} else {
					input.m_iNumEnergyEvaluations = 0;
					input.writeResumeFile(tempOutputFileName, moleculeSets, emptyMoleculeSets,
						emptyMoleculeSets, 0, false);
					printf(messages->m_sCreatedOptFile.c_str(), tempOutputFileName.c_str());
					cout << endl;
				}
			}
			// Clean up
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				delete moleculeSets[i];
			moleculeSets.clear();
		} else if (ArgumentParser::optionPresent(messages->m_sOptionHelpA.c_str())) { // # Create an optimization file
			if (ArgumentParser::getNumOptions() > 1) {
				printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpA.c_str());
				cout << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments(messages->m_sOptionHelpA.c_str(), numOptionArguments, &optionArguments);
			int numToTransfer;
			inputFileName = optionArguments[0];
			sscanf(optionArguments[1], "%d", &numToTransfer);
			cout << messages->m_sReadingFile << ": " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (!input.m_bOptimizationFileRead) {
				cout << messages->m_sFileIsNotOptFile << endl << endl;
				throw "";
			}
			while (true) {
				printf(messages->m_sAddFromThisResFile.c_str(), input.m_sResumeFileName.c_str());
				printf(" ");
				cin >> answer;
				if (answer == messages->m_sYes) {
					resumeFileName = input.m_sResumeFileName;
					break;
				} else if (answer == messages->m_sNo) {
					cout << messages->m_sEnterResumeFile << " ";
					cin >> resumeFileName;
					break;
				} else {
					printf(messages->m_sAnswerYesOrNo.c_str(), messages->m_sYes.c_str(), messages->m_sNo.c_str());
					cout << endl << endl;
				}
			}
			if (!resume.open(resumeFileName, false, false, moleculeSetsTemp, bestNMoleculeSetsTemp, bestIndividualMoleculeSetsTemp))
				throw "";
			if (!resume.m_bResumeFileRead) {
				printf(messages->m_sNotAResumeFile.c_str(), resumeFileName.c_str());
				cout << endl << endl;
				throw "";
			}
			
			if ((bestNMoleculeSetsTemp.size() - (input.m_iNumEnergyEvaluations + moleculeSets.size())) == 0) {
				printf(messages->m_sNoStructuresLeftToTransfer.c_str(), resumeFileName.c_str());
				cout << endl << endl;
				throw "";
			}
			
			if ((numToTransfer <= 0) ||
			(numToTransfer > (signed int)(bestNMoleculeSetsTemp.size() - (input.m_iNumEnergyEvaluations + moleculeSets.size())))) {
				printf(messages->m_sEnterNumStructuresToTransfer.c_str(), (bestNMoleculeSetsTemp.size() - ((unsigned int)input.m_iNumEnergyEvaluations + moleculeSets.size())));
				cout << endl << endl;
				throw "";
			}
			
			j = input.m_iNumEnergyEvaluations + moleculeSets.size() + numToTransfer;
			for (i = input.m_iNumEnergyEvaluations + moleculeSets.size(); i < j; ++i) {
				pMoleculeSet = new MoleculeSet();
				pMoleculeSet->copy(*bestNMoleculeSetsTemp[i]);
				moleculeSets.push_back(pMoleculeSet);
			}
			if (input.m_iNumberOfLogFilesToSave < j) {
				printf(messages->m_sChangingNumSavedOptimizedStructures.c_str(), j);
				cout << endl;
				input.m_iNumberOfBestStructuresToSave = j;
				input.m_iNumberOfLogFilesToSave = j;
			}
			input.writeResumeFile(inputFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets,
					input.m_tElapsedSeconds, false);
			printf(messages->m_sTransferedNStructuresToOptFile.c_str(), numToTransfer, inputFileName.c_str());
			cout << endl;
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
		} else if (ArgumentParser::optionPresent(messages->m_sOptionHelpU.c_str())) { // # Update output file from individual output files (used with .pso -i option)
			if (ArgumentParser::getNumOptions() > 1) {
				printf(messages->m_sMultipleOptionsError.c_str(), messages->m_sOptionHelpU.c_str());
				cout << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments(messages->m_sOptionHelpU.c_str(), numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			input.compileIndependentRunData(true);
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
		} else {
			const char* argument = ArgumentParser::nextUnrecognized();
			while (argument != NULL) {
				printf(messages->m_sUnrecognizedArgOrOption.c_str(), argument);
				cout << endl;
				argument = ArgumentParser::nextUnrecognized();
			}
		}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << messages->m_sCaughtMessage << ": " << message << endl;
	}
	ArgumentParser::cleanUp();
	EnergyProgram::cleanUp();
	return 0;
}

