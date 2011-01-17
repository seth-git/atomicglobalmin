////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "helper.h"


void printHelpMenu()
{
	ifstream infile("helperHelp.txt");
	const int MAX_LINE_LENGTH = 500;
	char fileLine[MAX_LINE_LENGTH];
	while (infile.getline(fileLine, MAX_LINE_LENGTH))
		cout << fileLine << endl;
	cout << endl;
	infile.close();
}

int getChoiceInRange(int low, int high)
{
	string answerString;
	int answerInt;
	while (true) {
		cin >> answerString;
		answerInt = atoi(answerString.c_str());
		if (low != -1)
			if (answerInt < low) {
				cout << "Please enter a number greater or equal to " << low << ", or press Ctrl-C to quit." << endl;
				continue;
			}
		if (high != -1)
			if (answerInt > high) {
				cout << "Please enter a number less than or equal to " << high << ", or press Ctrl-C to quit." << endl;
				continue;
			}
		return answerInt;
	}
}

void enterSingleMolecule(Input &input, bool askNumber)
{
	char line[500];
	vector<string> lines;
	int atomic_number;
	int answer;
	Point3D point;
	
	cout << "Enter the cartesian coordinates of a molecule(or set of atoms) with one atom per line in the format below.  Enter 'done' when finished." << endl;
	cout << "atomic_number x y z" << endl;

	while (fgets(line, sizeof line, stdin) != NULL )
	{
		char *newline = strchr(line, '\n'); /* search for newline character */
		if (newline != NULL)
			*newline = '\0'; /* overwrite trailing newline */
		if (strcmp(line, "done") == 0)
			break;
		if (strcmp(line, "") == 0)
			continue;
		if (sscanf(line, "%d %lf %lf %lf", &atomic_number, &point.x, &point.y, &point.z) != 4)
			cout << "Error reading atom.  Please enter it again." << endl;
		lines.push_back(line);
	}
	if (askNumber) {
		cout << "How many of these molecules are there?" << endl;
		answer = getChoiceInRange(1,-1);
	}
	++input.m_iNumStructureTypes;
}

void wizzard(void)
{
	Input input;
	int answerInt;
	cout << "Welcome to the pso input file creation wizzard!" << endl << endl;
	cout << "You may press Ctrl-C at any time to stop the wizzard." << endl << endl;
	cout << "Please choose from the following options:" << endl;
	cout << "1. Perform a bond rotational search on a single molecule" << endl;
	cout << "2. Perform a global minimum search via translation and rotation of molecules/atoms" << endl;
	answerInt = getChoiceInRange(1,2);
	switch (answerInt) {
	case 1:
		enterSingleMolecule(input, true);
		break;
	case 2:
		break;
	}
}

int main(int argc, char *argv[])
{
	vector<const char*> recognizedOptions;
	vector<int> argumentsPerOption;
	vector<const char*> optionMessages;
	int numOptionArguments;
	char** optionArguments;
	
	if (!Init::initProgram(1)) {
		return 0;
	}
	
	recognizedOptions.push_back("-h");
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back("--help");
	argumentsPerOption.push_back(0);
	optionMessages.push_back("");
	recognizedOptions.push_back("-t");
	argumentsPerOption.push_back(2);
	optionMessages.push_back("When using the -t option, please specify a resume or optimization file followed by an output file to which results will be written.");
	recognizedOptions.push_back("-c");
	argumentsPerOption.push_back(3);
	optionMessages.push_back("When using the -c option, please specify a resume or optimization file, followed by a directory where input files will be written, followed by a file prefix.");
	recognizedOptions.push_back("-p");
	argumentsPerOption.push_back(3);
	optionMessages.push_back("When using the -p option, please specify a resume or optimization file, followed by a directory where input files will be written, followed by a file prefix.");
	recognizedOptions.push_back("-o");
	argumentsPerOption.push_back(4);
	optionMessages.push_back("When using the -o option, please specify a resume file, followed by the optimization file to be created, followed by the number of structures to transfer, followed by the number of structures to optimize at a time.");
	recognizedOptions.push_back("-a");
	argumentsPerOption.push_back(2);
	optionMessages.push_back("When using the -a option, please specify an optimization file, followed by the number of additional structures to transfer from the resume file.");
	recognizedOptions.push_back("-or");
	argumentsPerOption.push_back(3);
	optionMessages.push_back("When using the -or option, please specify an input file, followed by the optimization file to be created, followed by the number of structures to optimize at a time.");
	recognizedOptions.push_back("-ro");
	argumentsPerOption.push_back(3);
	optionMessages.push_back("When using the -ro option, please specify an input file, followed by the optimization file to be created, followed by the number of structures to optimize at a time.");
	recognizedOptions.push_back("-b");
	argumentsPerOption.push_back(1);
	optionMessages.push_back("");
	recognizedOptions.push_back("-m");
	argumentsPerOption.push_back(4);
	optionMessages.push_back("When using the -m option, please specify an input file, followed by the number of degrees to rotate bond angles, followed by an optimization file, followed by the number of structures to optimize at a time.");
	recognizedOptions.push_back("-u");
	argumentsPerOption.push_back(1);
	optionMessages.push_back("When using the -u option, please specify an input file.");
//	recognizedOptions.push_back("-w");
//	argumentsPerOption.push_back(0);
//	optionMessages.push_back("");
	
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
		if ((ArgumentParser::getNumOptions() == 0) || ArgumentParser::optionPresent("-h") || ArgumentParser::optionPresent("--help")) {
			printHelpMenu();
			throw "";
		}
		if (ArgumentParser::optionPresent("-w")) { // # Run the wizzard
			wizzard();
		} else if (ArgumentParser::optionPresent("-t")) { // # Make an output file from a resume file that's readable
			if (ArgumentParser::getNumOptions() > 1) {
				cout << "No other options may be specified with the -t option." << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments("-t", numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			tempOutputFileName = optionArguments[1];
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (!(input.m_bResumeFileRead || input.m_bOptimizationFileRead)) {
				cout << "The input file you have specified is not a resume or an optimization file." << endl;
				throw "";
			} else {
				fout.open(tempOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
				if (!fout.is_open())
				{
					cout << "Unable to open the output file: " << tempOutputFileName << endl;
					throw "";
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
		} else if (ArgumentParser::optionPresent("-b")) { // # Check to make sure bonds were recognized correctly
			if (ArgumentParser::getNumOptions() > 1) {
				cout << "No other options may be specified with the -b option." << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments("-b", numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			cout << "Reading File: " << inputFileName << endl;
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
		} else if (ArgumentParser::optionPresent("-m")) { // # Check to make sure bonds were recognized correctly
			if (ArgumentParser::getNumOptions() > 1) {
				cout << "No other options may be specified with the -m option." << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments("-m", numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			FLOAT angle = atof(optionArguments[1])/180*PIE;
			tempOutputFileName = optionArguments[2];
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				delete moleculeSets[i];
			moleculeSets.clear();
			if (input.m_tempelateMoleculeSet.performBondRotations(angle, moleculeSets)) {
				sscanf(optionArguments[3], "%d", &input.m_iStructuresToOptimizeAtATime);
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
		} else if (ArgumentParser::optionPresent("-c") || ArgumentParser::optionPresent("-p")) { // # Check to make sure bonds were recognized correctly
			bool cOption = ArgumentParser::optionPresent("-c");
			if (ArgumentParser::getNumOptions() > 1) {
				if (cOption)
					cout << "No other options may be specified with the -c option." << endl;
				else
					cout << "No other options may be specified with the -p option." << endl;
				throw "";
			}
			if (cOption)
				ArgumentParser::getOptionArguments("-c", numOptionArguments, &optionArguments);
			else
				ArgumentParser::getOptionArguments("-p", numOptionArguments, &optionArguments);
			vector<MoleculeSet*> *moleculeSetsToUse;
			inputFileName = optionArguments[0];
			outputDirectory = optionArguments[1];
			Input::checkDirectoryOrFileName(outputDirectory);
			if (stat(outputDirectory.c_str(), &fileStatistics) != 0) { // If the file doesn't exist
				cout << "The directory " << outputDirectory << " does not exist.  Do you wish to create it? ";
				cin >> answer;
				if (strncmp(answer.c_str(),"yes",3) != 0) {
					cout << "Directory not created..." << endl << endl;
					throw "";
				} else {
					snprintf(commandLine, 500, "mkdir %s", outputDirectory.c_str());
					if (system(commandLine) == 0)
						cout << "Created directory: " << outputDirectory << endl;
					else 
						throw "";
				}
			}
	
	
			comPrefix = optionArguments[2];
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (!Energy::init(input, 0))
				throw "";
			if (!(input.m_bResumeFileRead || input.m_bOptimizationFileRead)) {
				cout << "The input file you have specified is not a resume or an optimization file." << endl;
				throw "";
			}
			
			if (cOption)
				moleculeSetsToUse = &bestNMoleculeSets;
			else
				moleculeSetsToUse = &moleculeSets;
			
			if (moleculeSetsToUse->size() > 100) {
				cout << "There are " << moleculeSetsToUse->size() << " structures in this file." << endl;
				cout << "Enter the number of structures you wish to create " << input.m_pSelectedEnergyProgram->m_sName << " input files for or type 'all': ";
				cin >> answer;
				if (strncmp(answer.c_str(),"all",3) == 0)
					numFiles = moleculeSetsToUse->size();
				else
					numFiles = atoi(answer.c_str());
				if (numFiles <= 0) {
					cout << "Files not created..." << endl << endl;
					throw "";
				}
			} else
				numFiles = moleculeSetsToUse->size();
	
			cout << "Creating " << numFiles << " input files..." << endl;
			for (i = 0; i < numFiles; ++i) {
				(*moleculeSetsToUse)[i]->setInputEnergyFile(outputDirectory.c_str(), comPrefix.c_str(), i+1, input.m_pSelectedEnergyProgram->m_sInputFileExtension.c_str());
				Energy::createInputFile(*(*moleculeSetsToUse)[i], i+1, true);
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
		} else if (ArgumentParser::optionPresent("-o")) { // # Create an optimization file
			if (ArgumentParser::getNumOptions() > 1) {
				cout << "No other options may be specified with the -o option." << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments("-o", numOptionArguments, &optionArguments);
			int numToTransfer;
			inputFileName = optionArguments[0];
			tempOutputFileName = optionArguments[1];
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (!input.m_bResumeFileRead && !input.m_bOptimizationFileRead) {
				cout << "The input file you have specified is not a resume or an optimization file." << endl << endl;
				throw "";
			}
			sscanf(optionArguments[2], "%d", &numToTransfer);
			if ((numToTransfer <= 0) ||
			(numToTransfer > (signed int)bestNMoleculeSets.size())) {
				cout << "Please specify a valid number of structures to transfer (between 1 and "
				<< bestNMoleculeSets.size() << ")." << endl << endl;
				throw "";
			}
			if (input.m_iNumberOfLogFilesToSave < numToTransfer) {
				cout << "Changing the number of saved optimized structures to " << numToTransfer << "." << endl;
				input.m_iNumberOfBestStructuresToSave = numToTransfer;
				input.m_iNumberOfLogFilesToSave = numToTransfer;
			}
			sscanf(optionArguments[3], "%d", &input.m_iStructuresToOptimizeAtATime);
			if ((input.m_iStructuresToOptimizeAtATime <= 0) ||
			(input.m_iStructuresToOptimizeAtATime > (signed int)bestNMoleculeSets.size())) {
				cout << "Please specify a valid number of structures to optimize at a time (between 1 and "
				<< bestNMoleculeSets.size() << ")." << endl << endl;
				throw "";
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
				cout << "Please move or delete any files in the .log/energy files directory: " << input.m_sSaveLogFilesInDirectory << endl;
			// Clean up
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				delete moleculeSets[i];
			for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
				delete bestNMoleculeSets[i];
			moleculeSets.clear();
			bestNMoleculeSets.clear();
		} else if (ArgumentParser::optionPresent("-or") || ArgumentParser::optionPresent("-ro")) { // # Create an optimization file with random structures
			bool order1 = ArgumentParser::optionPresent("-or");
			if (ArgumentParser::getNumOptions() > 1) {
				if (order1)
					cout << "No other options may be specified with the -or option." << endl;
				else
					cout << "No other options may be specified with the -ro option." << endl;
				throw "";
			}
			if (order1)
				ArgumentParser::getOptionArguments("-or", numOptionArguments, &optionArguments);
			else
				ArgumentParser::getOptionArguments("-ro", numOptionArguments, &optionArguments);
			inputFileName = optionArguments[0];
			tempOutputFileName = optionArguments[1];
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (input.m_bResumeFileRead || input.m_bOptimizationFileRead) {
				cout << "The input file you have specified is a resume or optimization file, and not an input file. Please specify an input file." << endl << endl;
				throw "";
			}
			if (Init::initializePopulation(input, moleculeSets)) {
				input.m_iNumberOfBestStructuresToSave = moleculeSets.size();
				input.m_iNumberOfLogFilesToSave = moleculeSets.size();
				sscanf(optionArguments[2], "%d", &input.m_iStructuresToOptimizeAtATime);
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
		} else if (ArgumentParser::optionPresent("-a")) { // # Create an optimization file
			if (ArgumentParser::getNumOptions() > 1) {
				cout << "No other options may be specified with the -a option." << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments("-a", numOptionArguments, &optionArguments);
			int numToTransfer;
			inputFileName = optionArguments[0];
			sscanf(optionArguments[1], "%d", &numToTransfer);
			cout << "Reading File: " << inputFileName << endl;
			if (!input.open(inputFileName, true, false, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				throw "";
			if (!input.m_bOptimizationFileRead) {
				cout << "The input file you have specified is not an optimization file." << endl << endl;
				throw "";
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
				throw "";
			if (!resume.m_bResumeFileRead) {
				cout << "This is not a resume file: " << resumeFileName << endl << endl;
				throw "";
			}
			
			if ((numToTransfer <= 0) ||
			(numToTransfer > (signed int)(bestNMoleculeSetsTemp.size() - (input.m_iNumEnergyEvaluations + moleculeSets.size())))) {
				cout << "Please specify a valid number of structures to transfer (between 1 and "
				<< (bestNMoleculeSetsTemp.size() - (input.m_iNumEnergyEvaluations + moleculeSets.size())) << ")."
				<< endl << endl;
				throw "";
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
		} else if (ArgumentParser::optionPresent("-u")) { // # Update output file from individual output files (used with .pso -i option)
			if (ArgumentParser::getNumOptions() > 1) {
				cout << "No other options may be specified with the -u option." << endl;
				throw "";
			}
			ArgumentParser::getOptionArguments("-u", numOptionArguments, &optionArguments);
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
				cout << "Unrecognized argument or option: " << argument << endl;
				argument = ArgumentParser::nextUnrecognized();
			}
		}
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	ArgumentParser::cleanUp();
	EnergyProgram::cleanUp();
	return 0;
}

