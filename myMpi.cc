
////////////////////////////////////////////////////////////////////////////////
// Purpose: This is a class that computes the energy value for a molecule set.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "myMpi.h"

Mpi::Mpi()
{
}

Mpi::~Mpi ()
{
}  

time_t	Mpi::s_wallTime;
bool	Mpi::s_timeToFinish = false;
time_t	Mpi::s_longestiCalculationTime = 0;
double	Mpi::s_percentageOfSuccessfulCalculations;
bool    Mpi::s_independent = false; // seperate simulated annealing runs
bool    Mpi::s_masterDistributingTasks = false; // one extra node sends out tasks to the other slave nodes
string	Mpi::s_EndIfFileExists = "";
MPI_Request Mpi::s_mpiRequest;

bool Mpi::calculateEnergies (int energyCalculationType, vector<MoleculeSet*> &population, vector<MoleculeSet*> &optimizedPopulation)
{
	int i;
	MoleculeSet *pMoleculeSet = NULL;
	bool success = false;
	EnergyProgram* pEnergyProgram = Energy::getEnergyProgram();

	for (i = 0; i < (signed int)optimizedPopulation.size(); ++i)
		delete optimizedPopulation[i];
	optimizedPopulation.clear();

	if (pEnergyProgram->m_bUsesMPI) {
		Energy::createInputFiles(population);
		success = master(energyCalculationType, population, optimizedPopulation);
		
		return success;
	}
	
	if (pEnergyProgram->m_iProgramID == LENNARD_JONES) {
		switch (energyCalculationType) {
			case OPTIMIZE_BUT_DONT_READ:
				for (i = 0; i < (signed int)population.size(); ++i) {
					pMoleculeSet = new MoleculeSet();
					pMoleculeSet->copy(*population[i]);
					pMoleculeSet->performLennardJonesOptimization(LENNARD_JONES_EPSILON, LENNARD_JONES_SIGMA);
					pMoleculeSet->computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
					population[i]->setEnergy(pMoleculeSet->getEnergy());
					delete pMoleculeSet;
				}
			break;
			case OPTIMIZE_AND_READ:
				for (i = 0; i < (signed int)population.size(); ++i) {
					pMoleculeSet = new MoleculeSet();
					pMoleculeSet->copy(*population[i]);
					pMoleculeSet->performLennardJonesOptimization(LENNARD_JONES_EPSILON, LENNARD_JONES_SIGMA);
					pMoleculeSet->computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
					population[i]->setEnergy(pMoleculeSet->getEnergy());
					optimizedPopulation.push_back(pMoleculeSet);
				}
			break;
			case SINGLE_POINT_ENERGY_CALCULATION:
			case TRANSITION_STATE_SEARCH:
			default:
				for (i = 0; i < (signed int)population.size(); ++i)
					population[i]->computeLennardJonesEnergy(LENNARD_JONES_EPSILON,LENNARD_JONES_SIGMA);
			break;
		}
		if (s_wallTime-time(NULL) < 90)
			s_timeToFinish = true;
		if (!s_timeToFinish && (s_EndIfFileExists.length() > 0) && MoleculeSet::fileExists(s_EndIfFileExists.c_str()))
			s_timeToFinish = true;
		s_percentageOfSuccessfulCalculations = 1.0;
		return true;
	} else {
		cout << "The energy program '" << pEnergyProgram->m_sName << "' needs to be implemented in myMpi.cc and energy.cc." << endl;
		return false;
	}
	return false;
}

bool Mpi::masterSetup(int populationSize, bool independent, bool masterDistributingTasks, int rank)
{
	int nWorkers;
	int nSlaves;
	int ranki;
	char messageBuffer[1000];
	
	s_masterDistributingTasks = masterDistributingTasks;
		
	MPI_Comm_size(MPI_COMM_WORLD, &nWorkers);
	nSlaves = nWorkers - 1;

	s_independent = independent;
	if ((nSlaves == 0) || independent) {
		if (!Energy::createScratchDir())
			return false;
	} else {
		if (!masterDistributingTasks)
			if (!Energy::createScratchDir())
				return false;
		Energy::createMPIInitMessage(messageBuffer, sizeof(messageBuffer));

		int bufferSize = strlen(messageBuffer)+1;
		if (PRINT_MPI_MESSAGES)
			printf("Prepared to send buffer: %s, size = %d\n", messageBuffer, bufferSize);

		// Check for too many slaves
		if (nSlaves > populationSize) {
			if (PRINT_MPI_MESSAGES)
				printf("The number of slaves %d is greater than the popultation size %d.\n", nSlaves, populationSize);
			for (ranki = populationSize; ranki < nSlaves; ++ranki) {
				if (PRINT_MPI_MESSAGES)
					printf("Putting extra slave %d to sleep.\n", ranki);
				MPI_Send(0, 0, MPI_INT, ranki, DIETAG, MPI_COMM_WORLD);
			}
			nSlaves = populationSize;
		}
		// Tell the taskers what directories we're working in
		for (ranki = 1; ranki <= nSlaves; ++ranki) {
			if (PRINT_MPI_MESSAGES)
				printf("Sending buffer to %d.\n", ranki);
			MPI_Send(messageBuffer,             /* message buffer */
				 bufferSize,                 /* buffer size */
				 MPI_BYTE,           /* data item is an character or byte */
				 ranki,              /* destination process rank */
				 WORKTAG,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
		}
	}
	
	if ((rank > 0) && independent) {
		int returned = MPI_Irecv(0, 0, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &s_mpiRequest); // prepare to receive a kill signal
		if (PRINT_MPI_MESSAGES)
			printf("MPI_Irecv returned (%d) for process with rank %d.\n", returned, rank);
	}
	return true;
}

bool Mpi::setQuitFlag(string &endIfFileExists)
{
	s_EndIfFileExists = endIfFileExists;
	if ((s_EndIfFileExists.length() > 0) && MoleculeSet::fileExists(s_EndIfFileExists.c_str())) {
		char command[500];
		snprintf(command, sizeof(command), "rm -f %s", s_EndIfFileExists.c_str());
		if (system(command) == -1)
			return false;
	}
	return true;
}

bool Mpi::master(int energyCalculationType, vector<MoleculeSet*> &population, vector<MoleculeSet*> &optimizedPopulation)
{
	int nWorkers;
	int nSlaves;
	int i;
	MPI_Status status;
	int numJobsAssigned = 0;
	int numJobsDone = 0;
	int jobDone = 0;
	bool *jobStatus = new bool[population.size()];
	time_t *jobStartTime = new time_t[population.size()];
	time_t totalTime;
	int converged = 0;
	int rank;
	bool error = false;
	
	s_percentageOfSuccessfulCalculations = 1;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	for (i = 0; i < (int)population.size(); ++i)
		jobStatus[i] = false;
	
	MPI_Comm_size(MPI_COMM_WORLD, &nWorkers);
	nSlaves = nWorkers - 1;
	if (nSlaves > (int)population.size())
		nSlaves = (int)population.size();
	if (s_masterDistributingTasks) {
		for (i = 1; i <= nSlaves; ++i) {
			jobStartTime[numJobsAssigned] = time(NULL);
			if (PRINT_MPI_MESSAGES)
				cout << "Sending slave " << i << " task " << (numJobsAssigned+1) << " at time " << jobStartTime[numJobsAssigned] << "." << endl;
			++numJobsAssigned;
			MPI_Send(&numJobsAssigned,  /* message buffer */
				 1,                 /* one data item */
				 MPI_INT,           /* data item is an integer */
				 i,                 /* rank of slave */
				 WORKTAG,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
		}
	} else if (!s_independent) {
		int size = population.size();
		if (PRINT_MPI_MESSAGES && (nSlaves > 0))
			cout << "Sending slaves next round of calculations." << endl;
		for (i = 1; i <= nSlaves; ++i) {
			MPI_Send(&size,  /* message buffer */
				 1,                 /* one data item */
				 MPI_INT,           /* data item is an integer */
				 i,                 /* rank of slave */
				 WORKTAG,           /* user chosen message tag */
				 MPI_COMM_WORLD);   /* default communicator */
		}
	}

	try {
		if (s_independent || (nSlaves == 0) || s_masterDistributingTasks) {
			while (numJobsDone < (int)population.size()) {
				if (s_independent || (nSlaves == 0)) {
					if (PRINT_MPI_MESSAGES)
						printf("Master with rank %d doing task %d.\n", rank, numJobsAssigned+1);
					jobStartTime[numJobsAssigned] = time(NULL);
					++numJobsAssigned;
					Energy::doEnergyCalculation(numJobsAssigned);
					jobDone = numJobsAssigned;
				} else  { // if (s_masterDistributingTasks)
					/* Receive results from a slave */
					MPI_Recv(&jobDone,          /* message buffer */
						 1,                 /* one data item */
						 MPI_INT,           /* of type double real */
						 MPI_ANY_SOURCE,    /* receive from any sender */
						 MPI_ANY_TAG,       /* any type of message */
						 MPI_COMM_WORLD,    /* default communicator */
						 &status);          /* info about the received message */
					if (PRINT_MPI_MESSAGES)
						printf("Slave %d finished task %d.\n", status.MPI_SOURCE, jobDone);
					if (status.MPI_TAG == DIETAG) {
						throw "Slave reported an error.";
					}
				}
				--jobDone;
				jobStatus[jobDone] = true;
				++numJobsDone;
				
				totalTime = time(NULL) - jobStartTime[jobDone];
				if (totalTime > s_longestiCalculationTime)
					s_longestiCalculationTime = totalTime;
				if (!s_timeToFinish && ((s_wallTime-time(NULL)) < s_longestiCalculationTime)) {
					s_timeToFinish = true;
					if (PRINT_MPI_MESSAGES) {
						cout << "Time: " << time(NULL) << ", walltime: " << s_wallTime
						     << ", the longest job took: " << s_longestiCalculationTime
						     << ", we better finish." << endl;
						if (s_masterDistributingTasks)
							printf("Sending slaves the kill signal.\n");
					}
					if (numJobsAssigned < (signed int)population.size())
						throw "times up!";
				}
				
				if (s_masterDistributingTasks && (numJobsAssigned < (int)population.size())) {
					jobStartTime[numJobsAssigned] = time(NULL);
					++numJobsAssigned;
					if (PRINT_MPI_MESSAGES)
						printf("Sending slave %d task %d.\n", status.MPI_SOURCE, numJobsAssigned);
					MPI_Send(&numJobsAssigned,  /* message buffer */
						 1,                 /* one data item */
						 MPI_INT,           /* data item is an integer */
						 status.MPI_SOURCE, /* to who we just received from */
						 WORKTAG,           /* user chosen message tag */
						 MPI_COMM_WORLD);   /* default communicator */
				}
				if (!readOutputFile(energyCalculationType, population, optimizedPopulation, jobDone, converged)) {
					error = true;
					throw "Couldn't read log file";
				}
			}
			if (s_timeToFinish)
				end(rank);
		} else { // if (!s_masterDistributingTasks)
			int message;
			for (i = 0; i < (int)population.size(); i += nWorkers) {
				jobStartTime[i] = time(NULL);
				Energy::doEnergyCalculation(i+1);
				if (PRINT_MPI_MESSAGES)
					printf("Master with rank %d finished task %d.\n", rank, (i+1));
				
				totalTime = time(NULL) - jobStartTime[i];
				if (totalTime > s_longestiCalculationTime)
					s_longestiCalculationTime = totalTime;
				if (!s_timeToFinish && ((s_wallTime-time(NULL)) < s_longestiCalculationTime)) {
					s_timeToFinish = true;
					if (PRINT_MPI_MESSAGES) {
						cout << "Time: " << time(NULL) << ", walltime: " << s_wallTime
						     << ", the longest job took: " << s_longestiCalculationTime
						     << ", we better finish." << endl;
						printf("Sending slaves the kill signal.\n");
					}
					throw "times up!";
				}
			}
			for (i = 1; i <= nSlaves; ++i) {
				/* Receive results from a slave */
				MPI_Recv(&message,          /* message buffer */
					 1,                 /* one data item */
					 MPI_INT,           /* of type double real */
					 i,                 /* receive from sender i */
					 MPI_ANY_TAG,       /* any type of message */
					 MPI_COMM_WORLD,    /* default communicator */
					 &status);          /* info about the received message */
				if (status.MPI_TAG == DIETAG) {
					if (PRINT_MPI_MESSAGES)
						printf("Slave %d quit without being told.", status.MPI_SOURCE);
					throw "Slave quit without being told.";
				}
				if (PRINT_MPI_MESSAGES)
					printf("Slave %d finished all assigned tasks.\n", status.MPI_SOURCE);
			}
			numJobsDone = population.size();
			for (i = 0; i < (int)population.size(); ++i)
				if (!readOutputFile(energyCalculationType, population, optimizedPopulation, i, converged)) {
					error = true;
					throw "Couldn't read log file";
				}
		}
		s_percentageOfSuccessfulCalculations = (FLOAT)converged / (FLOAT)population.size();
		if (!s_timeToFinish) {
			if ((s_percentageOfSuccessfulCalculations < MIN_SUCCESSFULL_ENERGY_CALCULATION_PERCENTAGE) && (population.size() > 1)) {
				error = true;
				if (rank == 0)
					cout << "The pecentage of successful energy calculations is " << (100.0*s_percentageOfSuccessfulCalculations) << "% which is below the limit of "
					     << setprecision(2) << (100.0*MIN_SUCCESSFULL_ENERGY_CALCULATION_PERCENTAGE) << "%.  Something may be wrong.  Exiting..." << endl;
				else
					cout << "The pecentage of successful energy calculations is " << (100.0*s_percentageOfSuccessfulCalculations) << "% which is below the limit of "
					     << setprecision(2) << (100.0*MIN_SUCCESSFULL_ENERGY_CALCULATION_PERCENTAGE) << "%.  Something may be wrong.  Process with rank " << rank << " exiting..." << endl;
				end(rank);
			} else if (rank == 0) {
				if ((s_EndIfFileExists.length() > 0) && MoleculeSet::fileExists(s_EndIfFileExists.c_str())) {
					s_timeToFinish = true;
					cout << "Recieved the stop signal." << endl;
					end(rank);
				}
			} else if (s_independent) {
				int messageReceived = 0;
				int returned = MPI_Test(&s_mpiRequest, &messageReceived, &status);
				if (PRINT_MPI_MESSAGES)
					printf("MPI_Test returned %d and messageReceived %d for process with rank %d and status %d (die=%d).\n", returned, messageReceived, rank, status.MPI_TAG, DIETAG);
				if (messageReceived && (status.MPI_TAG == DIETAG)) {
					s_timeToFinish = true;
					end(rank);
				}
			}
		} 
	} catch (const char* message) {
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
		end(rank);
	}
	delete[] jobStatus;
	delete[] jobStartTime;
	return (!error && (numJobsDone == (signed int)population.size()));
}

bool Mpi::readOutputFile(int energyCalculationType, vector<MoleculeSet*> &population, vector<MoleculeSet*> &optimizedPopulation, int fileIndex, int &converged)
{	
	MoleculeSet* pMoleculeSet = NULL;
	int gaussianReturned;
	FLOAT energy;
	bool success = true;
	EnergyProgram* pEnergyProgram = Energy::getEnergyProgram();
	
	try {
		// Read the output file
		if ((energyCalculationType == TRANSITION_STATE_SEARCH) || (energyCalculationType == OPTIMIZE_AND_READ)) {
			pMoleculeSet = new MoleculeSet();
			pMoleculeSet->copy(*population[fileIndex]);
		}
		switch (pEnergyProgram->m_iProgramID) {
			case GAUSSIAN:
				gaussianReturned = Energy::readGaussianOutputFile(population[fileIndex]->getOutputEnergyFile(0), energy, pMoleculeSet);
				if (!(gaussianReturned & OPENED_FILE)) {
					cerr << "Could not open file  '" << population[fileIndex]->getOutputEnergyFile(0) << "'.  Exiting... " <<endl;
					converged = 0;
					throw "";
				}
				if ((gaussianReturned & READ_ENERGY) && (gaussianReturned & OBTAINED_GEOMETRY))
					++converged;
				else
					cerr << "Opened this file, but could not read it: '" << population[fileIndex]->getOutputEnergyFile(0) << "'." <<endl;
				break;
			default:
				
				success = false;
				break;
		}
		if (((energyCalculationType == TRANSITION_STATE_SEARCH) && pMoleculeSet->getIsTransitionState()) ||
		    (energyCalculationType == OPTIMIZE_AND_READ)) {
			optimizedPopulation.push_back(pMoleculeSet);
//			if ((energyCalculationType == TRANSITION_STATE_SEARCH) && pMoleculeSet->getIsTransitionState()) {
//				cout << "Found transition state:" << endl;
//				population[fileIndex]->printToScreen();
//			}
			pMoleculeSet = NULL;
		}
		population[fileIndex]->setEnergy(energy);
	} catch (const char* message) {
		success = false;
		if (PRINT_CATCH_MESSAGES)
			cerr << "Caught message: " << message << endl;
	}
	if (pMoleculeSet != NULL)
		delete pMoleculeSet;
	return success;
}

void Mpi::end(int rank)
{
	int nWorkers;
	int nSlaves;
	MPI_Comm_size(MPI_COMM_WORLD, &nWorkers);
	nSlaves = nWorkers - 1;
	
	if (rank == 0) {
		if (s_independent || !s_masterDistributingTasks || (nSlaves == 0))
			Energy::deleteScratchDir();
		for (int ranki = 1; ranki <= nSlaves; ++ranki) {
			if (PRINT_MPI_MESSAGES)
				cout << "Sending slave " << ranki << " the kill signal at time " << time(NULL) << "." << endl;
			MPI_Send(0, 0, MPI_INT, ranki, DIETAG, MPI_COMM_WORLD);
		}
	} else {
		Energy::deleteScratchDir();
	}
}

void Mpi::slave(int rank, bool masterDistributingTasks)
{
	int nWorkers;
	MPI_Status status;
	char messageBuffer[1000];
	int taskNumber = 0;
	int populationSize = 0;
		
	time_t startTime;
	time_t endTime;
	
	MPI_Comm_size(MPI_COMM_WORLD, &nWorkers);
	MPI_Recv(messageBuffer, sizeof(messageBuffer), MPI_BYTE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	
	if (status.MPI_TAG == DIETAG)
		return;
	if (PRINT_MPI_MESSAGES)
		cout << "mpiSlave " << rank << " received initial message: " << messageBuffer << " at time " << time(NULL) << endl;
	if (!Energy::init(messageBuffer, rank))
		return;
	if (!Energy::createScratchDir())
		return;
	
	if (masterDistributingTasks) {
		while (true) {
			MPI_Recv(&taskNumber, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // Get a new taskNumber to do
			if (status.MPI_TAG == DIETAG) {
				if (PRINT_MPI_MESSAGES)
					cout << "Slave " << rank << " received the kill signal at time " << time(NULL) << endl;
				break;
			}
			if (PRINT_MPI_MESSAGES)
				printf("Slave %d received task %d.\n", rank, taskNumber);
			Energy::doEnergyCalculation(taskNumber);
			MPI_Send(&taskNumber, 1, MPI_INT, 0, WORKTAG, MPI_COMM_WORLD); // Tell the master we did taskNumber
		}
	} else {
		try {
			while (true) {
				MPI_Recv(&populationSize, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // Get a new taskNumber to do
				if (status.MPI_TAG == DIETAG) {
					if (PRINT_MPI_MESSAGES)
						cout << "Slave " << rank << " received the kill signal at time " << time(NULL) << endl;
					break;
				}
				if (PRINT_MPI_MESSAGES)
					printf("Slave %d received tasks.\n", rank);
				for (taskNumber = rank+1; taskNumber <= populationSize; taskNumber += nWorkers) {
					startTime = time(NULL);
					Energy::doEnergyCalculation(taskNumber);
					if (PRINT_MPI_MESSAGES)
						printf("Slave %d finished task %d.\n", rank, taskNumber);
				
					endTime = time(NULL) - startTime;
					if (endTime > s_longestiCalculationTime)
						s_longestiCalculationTime = endTime;
					if (!s_timeToFinish && ((s_wallTime-time(NULL)) < s_longestiCalculationTime)) {
						s_timeToFinish = true;
						if (PRINT_MPI_MESSAGES) {
							cout << "Time: " << time(NULL) << ", walltime: " << s_wallTime
							     << ", the longest job took: " << s_longestiCalculationTime
							     << ", this slave better finish." << endl;
						}
						MPI_Send(&populationSize, 1, MPI_INT, 0, DIETAG, MPI_COMM_WORLD); // Tell the master we are done early
						throw "times up!";
					}
				}
				MPI_Send(&populationSize, 1, MPI_INT, 0, WORKTAG, MPI_COMM_WORLD); // Tell the master we are done
			}
		} catch (const char* message) {
			if (PRINT_CATCH_MESSAGES)
				cerr << "Caught message: " << message << endl;
		}
	}
	Energy::deleteScratchDir();
}

