////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing a molecule or a set of adams that
//    belong in a group.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "main.h"

bool initLinearMoleculeSet(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, bool useLocalOptimization, bool readInOptimizedCoordinates)
{
	bool success;
	Point3D shiftAmount;
	int initializationTries = INITIALIZATION_TRIES;
	
	do {
		success = moleculeSet.initPositionsAndAngles(boxDimensions, initializationTries);
		if (!success) {
			cout << "Failed to initialize linear molecule set after " << initializationTries << " tries." << endl;
			cout << "Initialization box size is probably too small: " << boxDimensions.x << "x"
			                                                          << boxDimensions.y << "x"
			                                                          << boxDimensions.z << "x" << endl;
			return false;
		}
		energy.calculateEnergy(moleculeSet,false, useLocalOptimization, readInOptimizedCoordinates);
		if (!energy.getConverged())
			initializationTries = 1000000000; // Infinite tries
		else
			break;
	} while (true);
	moleculeSet.setEnergy(energy.getEnergy());
	
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

bool initPlanarMoleculeSet(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, bool useLocalOptimization, bool readInOptimizedCoordinates)
{
	bool success;
	Point3D shiftAmount;
	int initializationTries = INITIALIZATION_TRIES;
	
	do {
		success = moleculeSet.initPositionsAndAngles(boxDimensions, initializationTries);
		if (!success) {
			cout << "Failed to initialize planar molecule set after " << initializationTries << " tries." << endl;
			cout << "Initialization box size is probably too small: " << boxDimensions.x << "x"
			                                                          << boxDimensions.y << "x"
			                                                          << boxDimensions.z << "x" << endl;
			return false;
		}
		energy.calculateEnergy(moleculeSet,false, useLocalOptimization, readInOptimizedCoordinates);
		if (!energy.getConverged())
			initializationTries = 1000000000; // Infinite tries
		else
			break;
	} while (true);
	moleculeSet.setEnergy(energy.getEnergy());

	// Especially for particle swarm optimization, we need to start in the middle of the box
	// rather than on one side, so shift the atoms, so they're in the middle of the box.
	// This code assumes that boxDimensions.x = boxDimensions.y and that boxDimensions.z is 0.
	shiftAmount.x = 0;
	shiftAmount.y = 0;
	shiftAmount.z = boxDimensions.x / 2;
	moleculeSet.moveMoleculeSet(shiftAmount);
	
	return true;
}

bool init3DMoleculeSet(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, bool useLocalOptimization, bool readInOptimizedCoordinates)
{
	bool success;
	do {
		success = moleculeSet.initPositionsAndAngles(boxDimensions, INITIALIZATION_TRIES);
		if (!success) {
			cout << "Failed to initialize 3D molecule set after " << INITIALIZATION_TRIES << " tries." << endl;
			return false;
		}
		energy.calculateEnergy(moleculeSet,false,useLocalOptimization, readInOptimizedCoordinates);
	} while (!energy.getConverged());
	moleculeSet.setEnergy(energy.getEnergy());
	return true;
}

bool init3DMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, FLOAT maxAtomDist, bool useLocalOptimization, bool readInOptimizedCoordinates)
{
	bool success;
	do {
		success = moleculeSet.initPositionsAndAnglesWithMaxDist(boxDimensions, maxAtomDist, INITIALIZATION_TRIES);
		if (!success) {
			cout << "Failed to initialize 3D molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
			return false;
		}
		energy.calculateEnergy(moleculeSet,false,useLocalOptimization, readInOptimizedCoordinates);
	} while (!energy.getConverged());
	moleculeSet.setEnergy(energy.getEnergy());
	return true;
}

bool init3DNonFragMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions, FLOAT maxAtomDist, bool useLocalOptimization, bool readInOptimizedCoordinates)
{
	bool success;
	do {
		success = moleculeSet.initNonFragmentedSructure(boxDimensions, maxAtomDist, INITIALIZATION_TRIES);
		if (!success) {
			cout << "Failed to initialize 3D non-fragmented molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
			return false;
		}
		energy.calculateEnergy(moleculeSet,false,useLocalOptimization, readInOptimizedCoordinates);
	} while (!energy.getConverged());
	moleculeSet.setEnergy(energy.getEnergy());
	return true;
}

MoleculeSet *transformMoleculeSet(MoleculeSet &moleculeSet, Energy &energy, Point3D &boxDimensions,
                                  FLOAT deltaForCoordinates, FLOAT deltaForAnglesInRad, FLOAT temperature,
                                  FLOAT boltzmanConstant, bool usePreviousWaveFunction, bool bPerformNonFragmentedSearch,
                                  bool bPerformBasinHopping, FLOAT maxDist)
{
//	static const FLOAT boltzmanConstant = 1.380662e-23;
	static const FLOAT hartreeToJewels = 2622950;
	static const FLOAT e = 2.71828182846;
	FLOAT deltaEnergy, p;
	bool transformationSucceeded;
	
	moleculeSet.setTransitionAccepted(false);
	MoleculeSet* newMoleculeSet = new MoleculeSet();
	newMoleculeSet->copy(moleculeSet);
	
	if (bPerformNonFragmentedSearch) {
		transformationSucceeded = newMoleculeSet->performTransformationNonFrag(boxDimensions,
		                                                deltaForCoordinates, deltaForAnglesInRad, maxDist);
	} else {
		transformationSucceeded = newMoleculeSet->performTransformation(boxDimensions,
		                                                                deltaForCoordinates, deltaForAnglesInRad);
//		transformationSucceeded = newMoleculeSet->performMultipleTransformations(boxDimensions,
//	    	                                                            deltaForCoordinates, deltaForAnglesInRad);
	}
	
	if (!transformationSucceeded)
	{
		delete newMoleculeSet;
		return NULL;
	}
	
	if (usePreviousWaveFunction)
		energy.backupWaveFunctionFile(moleculeSet);
	
	energy.calculateEnergy(*newMoleculeSet,usePreviousWaveFunction,bPerformBasinHopping,false);
	if (!energy.getConverged())
	{
		if (usePreviousWaveFunction)
			energy.restoreWaveFunctionFile(moleculeSet);
		delete newMoleculeSet;
		return NULL;
	}
	newMoleculeSet->setEnergy(energy.getEnergy());
	
	deltaEnergy = newMoleculeSet->getEnergy() - moleculeSet.getEnergy();
	if (deltaEnergy <= 0)
		newMoleculeSet->setTransitionAccepted(true);
	else
	{
		p = pow(e,-(deltaEnergy*hartreeToJewels)/(boltzmanConstant * temperature));
		if (Molecule::randomFloat(0,1) <= p)
			newMoleculeSet->setTransitionAccepted(true);
		else
		{
			if (usePreviousWaveFunction)
				energy.restoreWaveFunctionFile(moleculeSet);
			delete newMoleculeSet;
			return NULL;
		}
	}
	return newMoleculeSet;
}

void swarmMoleculeSet(MoleculeSet &moleculeSet, MoleculeSet &minDistMoleculeSet, MoleculeSet &populationBestSet,
                      MoleculeSet &individualBestSet, Energy &energy, FLOAT coordInertia,
                      FLOAT coordIndividualMinimumAttraction, FLOAT coordPopulationMinimumAttraction,
                      FLOAT coordMaxVelocity, FLOAT angleInertia, FLOAT angleIndividualMinimumAttraction,
                      FLOAT anglePopulationMinimumAttraction, FLOAT angleMaxVelocity, Point3D &boxDimensions,
                      bool enforceMinDistOnACopy, bool usePreviousWaveFunction, FLOAT attractionRepulsion,
                      FLOAT fIndividualBestUpdateDist, bool useLocalOptimization)
{
//	FLOAT dist = 0.2 * ((moleculeSet.getNumberOfAtoms() * moleculeSet.getNumberOfAtoms()) - moleculeSet.getNumberOfAtoms()) / 2;
	moleculeSet.performPSO(populationBestSet, individualBestSet, coordInertia, coordIndividualMinimumAttraction,
	                       coordPopulationMinimumAttraction, coordMaxVelocity, angleInertia,
	                       angleIndividualMinimumAttraction, anglePopulationMinimumAttraction, angleMaxVelocity,
	                       boxDimensions, !enforceMinDistOnACopy, attractionRepulsion);
	if (enforceMinDistOnACopy) {
		if (attractionRepulsion > 0) {
			minDistMoleculeSet.copy(moleculeSet);
			minDistMoleculeSet.enforceMinDistConstraints(boxDimensions);

			energy.calculateEnergy(minDistMoleculeSet, false, useLocalOptimization, false);
			if (!energy.getConverged())
				minDistMoleculeSet.setEnergy(0);
			else
				minDistMoleculeSet.setEnergy(energy.getEnergy());
			if (minDistMoleculeSet.getEnergy() < individualBestSet.getEnergy())
				if (!minDistMoleculeSet.withinDistance(populationBestSet,fIndividualBestUpdateDist))
					individualBestSet.copy(minDistMoleculeSet);
		}
	} else {
		if (attractionRepulsion > 0) {
			energy.calculateEnergy(moleculeSet, usePreviousWaveFunction, useLocalOptimization, false);
			if (!energy.getConverged())
				moleculeSet.setEnergy(0);
			else
				moleculeSet.setEnergy(energy.getEnergy());
			if (moleculeSet.getEnergy() < individualBestSet.getEnergy())
				if (!moleculeSet.withinDistance(populationBestSet,fIndividualBestUpdateDist))
					individualBestSet.copy(moleculeSet);
		}
	}
}

void * performTask(void * nodeData)
{
	NodeData_t *data = (NodeData_t *)nodeData;
	MoleculeSet *newMoleculeSet;
	MoleculeSet tempMoleculeSet;
	MoleculeSet *tempMoleculeSetPtr;
	MoleculeSet *swarmBestMoleculeSetPtr;
	bool success = true;
	bool ours;
	
	for (int i = 0; i < (signed int)(data->moleculeSets)->size(); ++i)
	{
		if (g_bErrorToStopFor)
			break;
		if ((*data->moleculeSets)[i]->needsRun())
		{
			ours=0;
			pthread_mutex_lock(data->mutex);
			if ((*data->moleculeSets)[i]->needsRun())
			{
				(*data->moleculeSets)[i]->setRunning();
				ours=1;
			}
			pthread_mutex_unlock(data->mutex);
			
			if (ours)
			{
				switch (data->task)
				{
					case INIT_LINEAR_TASK:
						success = initLinearMoleculeSet(*(*data->moleculeSets)[i], *data->energy, data->boxDimensions,
								data->useLocalOptimization, data->readInOptimizedCoordinates);
						if (!success) {
							g_bErrorToStopFor = true;
							return 0;
						}
					break;
					case INIT_PLANAR_TASK:
						success = initPlanarMoleculeSet(*(*data->moleculeSets)[i], *data->energy, data->boxDimensions,
								data->useLocalOptimization, data->readInOptimizedCoordinates);
						if (!success) {
							g_bErrorToStopFor = true;
							return 0;
						}
					break;
					case INIT_3D_TASK:
						success = init3DMoleculeSet(*(*data->moleculeSets)[i], *data->energy, data->boxDimensions,
								data->useLocalOptimization, data->readInOptimizedCoordinates);
						if (!success) {
							g_bErrorToStopFor = true;
							return 0;
						}
					break;
					case INIT_3D_WITH_MAX_DIST_TASK:
						success = init3DMoleculeSetWithMaxDist(*(*data->moleculeSets)[i], *data->energy,
						                                       data->boxDimensions, data->maxAtomDistance,
										       data->useLocalOptimization, data->readInOptimizedCoordinates);
						if (!success) {
							g_bErrorToStopFor = true;
							return 0;
						}
					break;
					case INIT_3D_NONFRAG_WITH_MAX_DIST_TASK:
						success = init3DNonFragMoleculeSetWithMaxDist(*(*data->moleculeSets)[i], *data->energy,
						                                              data->boxDimensions, data->maxAtomDistance,
											      data->useLocalOptimization, data->readInOptimizedCoordinates);
						if (!success) {
							g_bErrorToStopFor = true;
							return 0;
						}
					break;
					case TRANSFORM_TASK:
						newMoleculeSet = transformMoleculeSet(*(*data->moleculeSets)[i],
							*data->energy,data->boxDimensions, data->deltaForCoordinates,
 							data->deltaForAnglesInRad,data->temperature, data->fBoltzmanConstant,
						    data->usePreviousWaveFunction, data->bPerformNonFragmentedSearch, data->bPerformBasinHopping,
						    data->maxAtomDistance);
						if (newMoleculeSet != NULL) // if the transition was accepted
						{
							delete (*data->moleculeSets)[i];
							(*data->moleculeSets)[i] = newMoleculeSet;
						}
					break;
					case SWARM_TASK:
						if (data->moleculeSetsMinDistEnforced->size() > 0)
							tempMoleculeSetPtr = (*data->moleculeSetsMinDistEnforced)[i];
						else
							tempMoleculeSetPtr = &tempMoleculeSet; // a moleculeSet with nothing in it
						if (data->usePopulationBestAndNotLocalBest)
							swarmBestMoleculeSetPtr = data->populationBestMoleculeSet;
						else
							swarmBestMoleculeSetPtr = (*data->localBestMoleculeSets)[i];
						swarmMoleculeSet(*(*data->moleculeSets)[i], *tempMoleculeSetPtr,
						                 *swarmBestMoleculeSetPtr, *(*data->bestIndividualMoleculeSets)[i],
						                 *data->energy, data->fCoordInertia, data->fCoordIndividualMinimumAttraction,
						                 data->fCoordPopulationMinimumAttraction, data->fCoordMaximumVelocity,
						                 data->fAngleInertia, data->fAngleIndividualMinimumAttraction,
						                 data->fAnglePopulationMinimumAttraction, data->fAngleMaximumVelocity,
						                 data->boxDimensions, data->bEnforceMinDistOnCopy, data->usePreviousWaveFunction,
						                 data->fAttractionRepulsion, data->fIndividualBestUpdateDist,
								 data->useLocalOptimization);
					break;
					case CALCULATE_ENERGY_TASK:
						data->energy->calculateEnergy(*(*data->moleculeSets)[i],false,data->useLocalOptimization, data->readInOptimizedCoordinates);
						if (!data->energy->getConverged())
							(*data->moleculeSets)[i]->setEnergy(0);
						else
							(*data->moleculeSets)[i]->setEnergy(data->energy->getEnergy());
					break;
				}
				(*data->moleculeSets)[i]->setFinished();
			}
		}
	}
	return 0;
}

void performAllTasks(NodeData_t &nodeDataTemplate, Energy **energyObjects, int numEnergyObjectsOrNodes)
{
	int i;
	pthread_t * threads;
	threads = (pthread_t*) calloc(numEnergyObjectsOrNodes,sizeof(pthread_t));
	NodeData_t * nodeData;
	nodeData = (NodeData_t*) calloc(numEnergyObjectsOrNodes,sizeof(NodeData_t));
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, 0);
	
	for (i = 0; i < (signed int)nodeDataTemplate.moleculeSets->size(); ++i)
		(*nodeDataTemplate.moleculeSets)[i]->setNeedsRun();
	
	for (i = 0; i < numEnergyObjectsOrNodes; ++i)
	{
		nodeData[i] = nodeDataTemplate;
		nodeData[i].energy = energyObjects[i];
		nodeData[i].mutex = &mutex;
//		performTask((void *) &nodeData[i]); // Use this statement to not use threads (uncomment out the pthread_join's too)
		pthread_create(	&threads[i], 0, &performTask, (void *) &nodeData[i]);
	}
	for (i = 0; i < numEnergyObjectsOrNodes; ++i)
	{
		pthread_join( threads[i], 0 );
	}
	free(threads);
	free(nodeData);
	// Couldn't get registers: No such process.
	
	if (!g_bErrorToStopFor)
		for (i = 0; i < (signed int)nodeDataTemplate.moleculeSets->size(); ++i)
			if (!(*nodeDataTemplate.moleculeSets)[i]->isFinished())
			{
				cout << "The threads said they finished, but they didn't." << endl;
				exit(0);
			}
}

void initializePopulation(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets,
                          NodeData_t &nodeData, Energy** energyObjects, bool useLocalOptimization,
                          bool readInOptimizedCoordinates)
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
	int iLinearSructures, iPlanarStructures, i3DStructures, i3DStructuresWithMaxDist, i3DNonFragStructuresWithMaxDist;
	
	nodeData.usePreviousWaveFunction = false;
	nodeData.useLocalOptimization = useLocalOptimization;
	nodeData.usePreviousWaveFunction = readInOptimizedCoordinates;
	
	if (input.m_bInitInPairs) {
		iLinearSructures = input.m_iLinearSructures / 2;
		iPlanarStructures = input.m_iPlanarStructures / 2;
		i3DStructures = input.m_i3DStructures / 2;
		i3DStructuresWithMaxDist = input.m_i3DStructuresWithMaxDist / 2;
		i3DNonFragStructuresWithMaxDist = input.m_i3DNonFragStructuresWithMaxDist / 2;
	} else {
		iLinearSructures = input.m_iLinearSructures;
		iPlanarStructures = input.m_iPlanarStructures;
		i3DStructures = input.m_i3DStructures;
		i3DStructuresWithMaxDist = input.m_i3DStructuresWithMaxDist;
		i3DNonFragStructuresWithMaxDist = input.m_i3DNonFragStructuresWithMaxDist;
	}
	
	// Create the initial population based on the template molecule set
	if (iLinearSructures > 0) {
		for (i = 0; i < iLinearSructures; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			linearMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet); // Since we're using pointers to newMoleculeSet, it can be in two lists
		}
		nodeData.task = INIT_LINEAR_TASK;
		nodeData.boxDimensions.x = input.m_boxDimensions.x;
		nodeData.boxDimensions.y = input.m_fLinearBoxHeight;
		nodeData.boxDimensions.z = input.m_fLinearBoxHeight;
		nodeData.moleculeSets = &linearMoleculeSets;
		performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
	}
	
	if (!g_bErrorToStopFor && (iPlanarStructures > 0)) {
		for (i = 0; i < iPlanarStructures; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			planarMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
		nodeData.task = INIT_PLANAR_TASK;
		nodeData.boxDimensions.x = input.m_boxDimensions.x;
		nodeData.boxDimensions.y = input.m_boxDimensions.y;
		nodeData.boxDimensions.z = 0;
		nodeData.moleculeSets = &planarMoleculeSets;
		performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
	}
	
	if (!g_bErrorToStopFor && (i3DStructures > 0)) {
		for (i = 0; i < i3DStructures; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			threeDMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
		nodeData.task = INIT_3D_TASK;
		nodeData.boxDimensions = input.m_boxDimensions;
	 	nodeData.moleculeSets = &threeDMoleculeSets;
		performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
	}
	
	if (!g_bErrorToStopFor && (i3DStructuresWithMaxDist > 0)) {
		for (i = 0; i < i3DStructuresWithMaxDist; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			threeDWithMaxDistMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
		nodeData.task = INIT_3D_WITH_MAX_DIST_TASK;
		nodeData.boxDimensions = input.m_boxDimensions;
		nodeData.moleculeSets = &threeDWithMaxDistMoleculeSets;
		nodeData.maxAtomDistance = input.m_fMaxAtomDistance;
		performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
	}
	
	if (!g_bErrorToStopFor && (i3DNonFragStructuresWithMaxDist > 0)) {
		for (i = 0; i < i3DNonFragStructuresWithMaxDist; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(input.m_tempelateMoleculeSet);
			newMoleculeSet->setId(++moleculeSetId);
			threeDNonFragWithMaxDistMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
		nodeData.task = INIT_3D_NONFRAG_WITH_MAX_DIST_TASK;
		nodeData.boxDimensions = input.m_boxDimensions;
		nodeData.moleculeSets = &threeDNonFragWithMaxDistMoleculeSets;
		nodeData.maxAtomDistance = input.m_fMaxAtomDistance;
		performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
	}
	
	if (!g_bErrorToStopFor && input.m_bInitInPairs) {
		int iHalfTotalPopulationSize = (signed int)moleculeSets.size();
		for (i = 0; i < iHalfTotalPopulationSize; ++i)
		{
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(*moleculeSets[i]);
			newMoleculeSet->setId(++moleculeSetId);
			newMoleculeSet->offSetCoordinatesAndAnglesSlightly(input.m_boxDimensions);
			nearCopyMoleculeSets.push_back(newMoleculeSet);
			moleculeSets.push_back(newMoleculeSet);
		}
		nodeData.task = CALCULATE_ENERGY_TASK;
		nodeData.boxDimensions = input.m_boxDimensions;
		nodeData.moleculeSets = &nearCopyMoleculeSets;
		nodeData.maxAtomDistance = input.m_fMaxAtomDistance;
		performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
	}
	
	if (!g_bErrorToStopFor)
		Gega::saveBestN(moleculeSets, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
		                input.m_fMinDistnaceBetweenSameMoleculeSets);
	for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i)
		energyObjects[i]->resetTimesCalculatedEnergy();
}

void simulatedAnnealing(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets, bool usingSeeding)
{
	vector<MoleculeSet*> emptyMoleculeSets; // This is only created here so we have something to pass in for
	                                         // bestIndividualMoleculeSets which is used in PSO, not simulated annealing.
	int i;
	long iTotalAcceptedTransitions, iTotalTransitions; // totals for the past input.m_iNumIterationsBeforeDecreasingTemp iterations
	FLOAT fAcceptanceRatio, fPercentAcceptedTransitions;
	FLOAT fSQRTQuenchingFactor;
	string tempResumeFileName = input.m_sResumeFileName + ".temp";
	char commandString[500];
	// Variables to keep track of how long this took
	time_t seconds;
	time_t minutes;
	time_t hours;
	time_t days;
	// Variables used in threads
	NodeData_t nodeData; // This node data structure will be coppied after it is passed to performAllTasks
	Energy** energyObjects;
	int iTotalEnergyCalculations;
	int iTotalConverged;
	FLOAT fPercentConverged;
	ofstream fout;
	
	energyObjects = new Energy*[input.m_srgNodeNames.size()];
	for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i)
		energyObjects[i] = new Energy(&input, i+1, input.m_srgNodeNames[i], input.m_sEnergyFunction);
	
	g_bErrorToStopFor = false;
	if (!input.m_bResumeFileRead) {
		seconds = time (NULL);
		fout.open(input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			g_bErrorToStopFor = true;
		} else {
			input.printToFile(fout);
			fout << endl << endl;
			fout << "Output from program:" << endl;
			if (!usingSeeding) {
				cout << "Initializing the population..." << endl;
				fout << "Initializing the population..." << endl;
				initializePopulation(input, moleculeSets, bestNMoleculeSets, nodeData, energyObjects,
				                     input.m_bPerformBasinHopping,false);
				input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
			} else {
				cout << "Using structures from previous run(s)..." << endl;
				fout << "Using structures from previous run(s)..." << endl;
			}
			
			input.m_iAcceptedTransitionsIndex = -1;
			iTotalAcceptedTransitions = 0;
			iTotalTransitions = 0;
		}
	} else {
		seconds = time (NULL) - (input.m_tTimeStampEnd - input.m_tTimeStampStart);
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			g_bErrorToStopFor = true;
		} else
			fout << "Resuming after program execution was stopped..." << endl;
		deleteEnergyFiles(moleculeSets, energyObjects, input.m_srgNodeNames.size(), input.m_bUsePrevWaveFunction);
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
	
	if (!g_bErrorToStopFor) {
		// Perform Simulated Annealing
		if (input.m_bPerformBasinHopping)
			cout << "Performing Basin Hopping..." << endl;
		else
			cout << "Simulating Annealing..." << endl;
		nodeData.task = TRANSFORM_TASK;
		nodeData.bPerformNonFragmentedSearch = input.m_bPerformNonFragSearch;
		nodeData.bPerformBasinHopping = input.m_bPerformBasinHopping;
		nodeData.maxAtomDistance = input.m_fMaxAtomDistance;
		nodeData.boxDimensions = input.m_boxDimensions;
		nodeData.moleculeSets = &moleculeSets;
		nodeData.deltaForCoordinates = input.m_fStartCoordinatePerturbation;
		nodeData.deltaForAnglesInRad = input.m_fStartAnglePerturbation;
		nodeData.temperature = input.m_fStartingTemperature;
		nodeData.fBoltzmanConstant = input.m_fBoltzmanConstant;
		fSQRTQuenchingFactor = sqrt(input.m_fQuenchingFactor);
		sprintf(commandString,"mv %s %s", tempResumeFileName.c_str(), input.m_sResumeFileName.c_str());
		
		do {
			++input.m_iIteration;
			nodeData.usePreviousWaveFunction = (input.m_bUsePrevWaveFunction && (input.m_iIteration % 2 == 1));
//			cout << "usePreviousWaveFunction = " << nodeData.usePreviousWaveFunction << endl;
			
			if (input.m_bDecreasingTemp)
			{
				input.m_fStartingTemperature *= input.m_fQuenchingFactor;
				input.m_fStartCoordinatePerturbation *= fSQRTQuenchingFactor;
				if (input.m_fStartCoordinatePerturbation < input.m_fMinCoordinatePerturbation)
					input.m_fStartCoordinatePerturbation = input.m_fMinCoordinatePerturbation;
				input.m_fStartAnglePerturbation *= fSQRTQuenchingFactor;
				if (input.m_fStartAnglePerturbation < input.m_fMinAnglePerturbation)
					input.m_fStartAnglePerturbation = input.m_fMinAnglePerturbation;
				nodeData.temperature = input.m_fStartingTemperature;
				nodeData.deltaForCoordinates = input.m_fStartCoordinatePerturbation;
				nodeData.deltaForAnglesInRad = input.m_fStartAnglePerturbation;
			}
			performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
			input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
			
			++input.m_iAcceptedTransitionsIndex;
			if (input.m_iAcceptedTransitionsIndex >= input.m_iNumIterationsBeforeDecreasingTemp)
				input.m_iAcceptedTransitionsIndex = 0;
			if (input.m_iIteration > input.m_iNumIterationsBeforeDecreasingTemp) {
				iTotalAcceptedTransitions -= input.m_prgAcceptedTransitions[input.m_iAcceptedTransitionsIndex];
				iTotalTransitions -= moleculeSets.size();
			}
			
			// Count the number of accepted transitions
			input.m_prgAcceptedTransitions[input.m_iAcceptedTransitionsIndex] = 0;
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				if (moleculeSets[i]->getTransitionAccepted())
					++input.m_prgAcceptedTransitions[input.m_iAcceptedTransitionsIndex];
			iTotalAcceptedTransitions += input.m_prgAcceptedTransitions[input.m_iAcceptedTransitionsIndex];
			iTotalTransitions += moleculeSets.size();
			
			fPercentAcceptedTransitions = (FLOAT)input.m_prgAcceptedTransitions[input.m_iAcceptedTransitionsIndex] /
			                              (FLOAT)moleculeSets.size();
			fAcceptanceRatio = (FLOAT)iTotalAcceptedTransitions / (FLOAT)iTotalTransitions;
			
			iTotalEnergyCalculations = 0;
            iTotalConverged = 0;
            for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i) {
                iTotalEnergyCalculations += energyObjects[i]->getTimesCalculatedEnergy();
                iTotalConverged += energyObjects[i]->getTimesConverged();
				energyObjects[i]->resetTimesCalculatedEnergy();
            }
            fPercentConverged = (FLOAT) iTotalConverged / (FLOAT) iTotalEnergyCalculations * 100;
			
			Gega::saveBestN(moleculeSets,bestNMoleculeSets,input.m_iNumberOfBestStructuresToSave,
			                input.m_fMinDistnaceBetweenSameMoleculeSets);
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				fout << "It: " << input.m_iIteration;
				fout << setiosflags(ios::fixed) << setprecision(8)
				     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy();
				fout << setiosflags(ios::fixed) << setprecision(1)
				     << " Temp: " << input.m_fStartingTemperature;
				fout << setiosflags(ios::fixed) << setprecision(4)
				     << " Coord, Angle Pert: " << input.m_fStartCoordinatePerturbation << ", "
				     << input.m_fStartAnglePerturbation * RAD_TO_DEG;
				fout << setiosflags(ios::fixed) << setprecision(1)
				     << " Accepted Trans: " << (fAcceptanceRatio*100) << "%"
				     << setiosflags(ios::fixed) << setprecision(1)
				     << " Converged: " << fPercentConverged << "%" << endl;
				
				cout << "It: " << input.m_iIteration;
				cout << setiosflags(ios::fixed) << setprecision(8)
				     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy();
				cout << setiosflags(ios::fixed) << setprecision(1)
				     << " Temp: " << input.m_fStartingTemperature;
				cout << setiosflags(ios::fixed) << setprecision(4)
				     << " Coord, Angle Pert: " << input.m_fStartCoordinatePerturbation << ", "
				     << input.m_fStartAnglePerturbation * RAD_TO_DEG;
				cout << setiosflags(ios::fixed) << setprecision(1)
				     << " Accepted Trans: " << (fAcceptanceRatio*100) << "%"
				     << " Converged: " << fPercentConverged << "%" << endl;
			}
			// Print the structures
//			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
//			{
//				fout << "Structure: " << i+1 << endl;
//				moleculeSets[i]->print(fout);
//				fout << "Energy = ";
//				fout << setiosflags(ios::fixed) << setprecision(8) << moleculeSets[i]->getEnergy() << endl << endl;
//			}
			
			if ((fPercentConverged < MIN_CONVERGENCE_PERCENT) && ((signed int)moleculeSets.size() >= 10)) {
				g_bErrorToStopFor = true;
				cout << "The convergence pecentage is below " << setprecision(2) << MIN_CONVERGENCE_PERCENT << "%.  Something may be wrong.  Exiting..." << endl;
				break;
			}
			
			if (!input.m_bDecreasingTemp)
			{
				if (!input.m_bPerformBasinHopping)
				{
					if ((input.m_iIteration >= input.m_iNumIterationsBeforeDecreasingTemp) &&
						(fAcceptanceRatio <= input.m_fAcceptanceRatio))
					{
						input.m_bDecreasingTemp = true;
						fout << "Decreasing Temperature..." << endl;
					}
				}
			}
			else
			{
				if (fPercentAcceptedTransitions < input.m_fMinAcceptedTransitions)
					++input.m_iGenerationsSinceMinAcceptedTransitionsReached;
				else
					input.m_iGenerationsSinceMinAcceptedTransitionsReached = 0;
			}
			
			if ((input.m_sResumeFileName.length() > 0) && ((input.m_iIteration%input.m_iResumeFileNumIterations) == 0))
			{
				input.writeResumeFile(tempResumeFileName, moleculeSets, bestNMoleculeSets, emptyMoleculeSets,
				                      seconds, time (NULL));
				if (system(commandString))
				{
					cout << "Error copying the temporary resume file(" << tempResumeFileName
					     << ") to the real resume file." << endl;
					break;
				}
			}
			
			if (input.m_iIteration >= input.m_iMaxIterations)
				break;
			if (input.m_bDecreasingTemp && (input.m_fStartingTemperature < input.m_fMinTemperatureToStop) &&
			    (input.m_iGenerationsSinceMinAcceptedTransitionsReached >= 50))
				break;
		} while (true);
		
		if (!g_bErrorToStopFor)
		{
			cout << "Finished after " << input.m_iIteration << " iterations at a temperature of " << input.m_fStartingTemperature << "." << endl;
			if (input.m_sEnergyFunction.compare("Lennard Jones") == 0) {
				cout << "Optimizing best structure..." << endl;
				fout << "Optimizing best structure..." << endl;
				energyObjects[0]->calculateEnergy(*bestNMoleculeSets[0],false,true,true);
				bestNMoleculeSets[0]->setEnergy(energyObjects[0]->getEnergy());
			}
			
			fout << "Best " << bestNMoleculeSets.size() << " structures:" << endl;
			cout << "Best " << bestNMoleculeSets.size() << " structures:" << endl;
			for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			{
				fout << "Structure #" << (i+1) << ": " << endl;
				bestNMoleculeSets[i]->print(fout);
				fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: " << bestNMoleculeSets[i]->getEnergy() << endl;
				cout << setiosflags(ios::fixed) << setprecision(8) << "Energy: " << bestNMoleculeSets[i]->getEnergy() << endl;
			}
		}
	}
	
   	// Clean up
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		delete moleculeSets[i];
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	moleculeSets.clear();
	bestNMoleculeSets.clear();
	for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i)
		delete energyObjects[i];
	delete[] energyObjects;
	
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
}

void particleSwarmOptimization(Input &input, vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestNMoleculeSets,
                               vector<MoleculeSet*> &bestIndividualMoleculeSets, bool usingSeeding)
{
	vector<MoleculeSet*> moleculeSetsMinDistEnforced; // a version of the population (moleculeSets) in which the min. distance constraint is enforced
	vector<MoleculeSet*> localBestMoleculeSets;
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
	FLOAT totCoordinateVelocity;
	FLOAT maxAngularVelocity;
	FLOAT totAngularVelocity;
	// Variables used in threads
	NodeData_t nodeData; // This node data structure will be coppied after it is passed to performAllTasks
	Energy** energyObjects;
	int iTotalEnergyCalculations;
	int iTotalConverged;
	FLOAT fPercentConverged;
	ofstream fout;
	FLOAT fDecreaseCoordInertiaConstant = 0;
	FLOAT fDecreaseAngleInertiaConstant = 0;
	FLOAT visibility = 1;
	FLOAT **particleDistanceMatrix;
	FLOAT diversity = 0;
	FLOAT fCubeDiagonal = sqrt(3 * input.m_boxDimensions.x * input.m_boxDimensions.x);
	int iSwitchToAttractionReplaceBest;
	FLOAT tempFloat;
	
	if (input.m_iReachEndInertiaAtIteration != 0) {
		fDecreaseCoordInertiaConstant = (input.m_fEndCoordInertia - input.m_fStartCoordInertia) /
			(FLOAT)input.m_iReachEndInertiaAtIteration;
		fDecreaseAngleInertiaConstant = (input.m_fEndAngleInertia - input.m_fStartAngleInertia) /
			(FLOAT)input.m_iReachEndInertiaAtIteration;
	}
	
	energyObjects = new Energy*[input.m_srgNodeNames.size()];
	for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i)
		energyObjects[i] = new Energy(&input, i+1, input.m_srgNodeNames[i], input.m_sEnergyFunction);
	
	g_bErrorToStopFor = false;
	if (!input.m_bResumeFileRead) {
		seconds = time (NULL);
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			g_bErrorToStopFor = true;
		} else {
			input.printToFile(fout);
			fout << endl << endl;
			fout << "Output from program:" << endl;
			if (!usingSeeding) {
				cout << "Initializing the population..." << endl;
				fout << "Initializing the population..." << endl;
				initializePopulation(input, moleculeSets, bestNMoleculeSets, nodeData, energyObjects,
				                     input.m_bUseLocalOptimization, false);
				input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
				for (i = 0; i < (signed int)moleculeSets.size(); ++i)
					moleculeSets[i]->initVelocities(0.2,10*DEG_TO_RAD);
			} else {
				cout << "Using structures from previous run(s)..." << endl;
				fout << "Using structures from previous run(s)..." << endl;
			}
		}
		
		for (i = 0; i < (signed int)moleculeSets.size(); ++i) {
			newMoleculeSet = new MoleculeSet();
			newMoleculeSet->copy(*moleculeSets[i]);
			bestIndividualMoleculeSets.push_back(newMoleculeSet);
		}
		
		if (input.m_fLocalMinDist != 0)
			input.m_fStartVisibilityDistance = input.m_fLocalMinDist;
	} else {
		seconds = time (NULL) - (input.m_tTimeStampEnd - input.m_tTimeStampStart);
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			g_bErrorToStopFor = true;
		} else
			fout << "Resuming after program execution was stopped..." << endl;
		deleteEnergyFiles(moleculeSets, energyObjects, input.m_srgNodeNames.size(), input.m_bUsePrevWaveFunction);
	}
	
	particleDistanceMatrix = new FLOAT*[moleculeSets.size()];
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		particleDistanceMatrix[i] = new FLOAT[moleculeSets.size()];
	
	if (input.m_bEnforceMinDistOnCopy)
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			moleculeSetsMinDistEnforced.push_back(new MoleculeSet(i+1));
	
	if (input.m_fLocalMinDist != 0)
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			localBestMoleculeSets.push_back(new MoleculeSet(i+1));
	
	if (!g_bErrorToStopFor) {
		cout << "Performing Particle Swarm Optimization..." << endl;
		nodeData.task = SWARM_TASK;
		nodeData.boxDimensions = input.m_boxDimensions;
		nodeData.moleculeSets = &moleculeSets;
		nodeData.moleculeSetsMinDistEnforced = &moleculeSetsMinDistEnforced;
		nodeData.bestIndividualMoleculeSets = &bestIndividualMoleculeSets;
		nodeData.usePopulationBestAndNotLocalBest = (input.m_fLocalMinDist == 0);
		nodeData.localBestMoleculeSets = &localBestMoleculeSets;
		nodeData.fCoordInertia = input.m_fStartCoordInertia;
		nodeData.fCoordIndividualMinimumAttraction = input.m_fCoordIndividualMinimumAttraction;
		nodeData.fCoordPopulationMinimumAttraction = input.m_fCoordPopulationMinimumAttraction;
		nodeData.fCoordMaximumVelocity = input.m_fCoordMaximumVelocity;
		nodeData.fAngleInertia = input.m_fStartAngleInertia;
		nodeData.fAngleIndividualMinimumAttraction = input.m_fAngleIndividualMinimumAttraction * DEG_TO_RAD;
		nodeData.fAnglePopulationMinimumAttraction = input.m_fAnglePopulationMinimumAttraction * DEG_TO_RAD;
		nodeData.fAngleMaximumVelocity = input.m_fAngleMaximumVelocity * DEG_TO_RAD;
		nodeData.bEnforceMinDistOnCopy = input.m_bEnforceMinDistOnCopy;
		nodeData.fAttractionRepulsion = input.m_fAttractionRepulsion;
		nodeData.fIndividualBestUpdateDist = input.m_fIndividualBestUpdateDist;
		nodeData.useLocalOptimization = input.m_bUseLocalOptimization;
		
		sprintf(commandString,"mv %s %s", tempResumeFileName.c_str(), input.m_sResumeFileName.c_str());
		do {
			++input.m_iIteration;
			nodeData.usePreviousWaveFunction = (input.m_bUsePrevWaveFunction && (input.m_iIteration % 2 == 1));
			
			getPopulationDistanceMatrix(moleculeSets, particleDistanceMatrix);
			diversity = calculateDiversity((signed int)moleculeSets.size(),particleDistanceMatrix,fCubeDiagonal);
			if (input.m_fSwitchToRepulsionWhenDiversityIs > 0) {
				if ((input.m_fAttractionRepulsion > 0) &&
				    (diversity <= input.m_fSwitchToRepulsionWhenDiversityIs) &&
				    (input.m_iNumIterationsBestEnergyHasntChanged >= input.m_iSwitchToRepulsionWhenNoProgress)) {
					cout << "Switching to repulsion phase..." << endl;
					fout << "Switching to repulsion phase..." << endl;
					input.m_fAttractionRepulsion = -1;
					if (input.m_fLocalMinDist != 0) {
					    cout << setiosflags(ios::fixed) << setprecision(3) << "Resetting visibility distance from "
						     << input.m_fLocalMinDist << " to " << input.m_fStartVisibilityDistance << endl;
					    fout << setiosflags(ios::fixed) << setprecision(3) << "Resetting visibility distance from "
						     << input.m_fLocalMinDist << " to " << input.m_fStartVisibilityDistance << endl;
						input.m_fLocalMinDist = input.m_fStartVisibilityDistance;
					}
				}
				if ((input.m_fAttractionRepulsion < 0) && (diversity >= input.m_fSwitchToAttractionWhenDiversityIs)) {
					cout << "Switching to attraction phase..." << endl;
					fout << "Switching to attraction phase..." << endl;
					input.m_fAttractionRepulsion = 1;
					
					// Reset the best solution seen by each particle to solutions in bestNMoleculeSets
					iSwitchToAttractionReplaceBest = input.m_iSwitchToAttractionReplaceBest;
					if (iSwitchToAttractionReplaceBest > (signed int)bestNMoleculeSets.size())
						iSwitchToAttractionReplaceBest = (signed int)bestNMoleculeSets.size();
					if (iSwitchToAttractionReplaceBest > (signed int)moleculeSets.size())
						iSwitchToAttractionReplaceBest = (signed int)moleculeSets.size();
					for (i = 0; i < iSwitchToAttractionReplaceBest; ++i)
						bestIndividualMoleculeSets[i]->copy(*bestNMoleculeSets[i]);
					for (i = iSwitchToAttractionReplaceBest; i < (signed int)moleculeSets.size(); ++i) {
						bestIndividualMoleculeSets[i]->copy(*moleculeSets[i]);
						bestIndividualMoleculeSets[i]->setEnergy(1234567);
					}
				}
			}
			
			if (input.m_fLocalMinDist != 0) {
				visibility = findLocalBestMoleculeSets(moleculeSets, bestIndividualMoleculeSets,
				                           localBestMoleculeSets, particleDistanceMatrix, input.m_fLocalMinDist);
				input.m_fLocalMinDist += input.m_fLocalMinDistIncrease;
			} else { // else if automatic 100% visibility
				nodeData.populationBestMoleculeSet = bestNMoleculeSets[0];
			}
			
			if (input.m_iReachEndInertiaAtIteration != 0) {
				if (input.m_iIteration < input.m_iReachEndInertiaAtIteration) {
					nodeData.fCoordInertia = fDecreaseCoordInertiaConstant * input.m_iIteration
					                       + input.m_fStartCoordInertia;
					nodeData.fAngleInertia = fDecreaseAngleInertiaConstant * input.m_iIteration
					                       + input.m_fStartAngleInertia;
				} else {
					nodeData.fCoordInertia = input.m_fEndCoordInertia;
					nodeData.fAngleInertia = input.m_fEndAngleInertia;
				}
			}
			
//			cout << setiosflags(ios::fixed) << setprecision(4)
//			     << "Inertias: " << nodeData.fCoordInertia << ", " << nodeData.fAngleInertia << endl;
			
//			cout << "Marker 1." << endl;
			nodeData.fAttractionRepulsion = input.m_fAttractionRepulsion;
			performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
//			cout << "Marker 2." << endl;
//	It: 165 Best Energy: -110.24506622 Coord Vel Max,Avg: 0.075, 0.016 Angle Vel Max,Avg: 0.000, 0.000 Converged: 100.0%
			// Get velocity statistics
			if (input.m_fAttractionRepulsion > 0)
				input.m_iNumEnergyEvaluations += (signed int)moleculeSets.size();
			
			maxCoordinateVelocity = 0;
			totCoordinateVelocity = 0;
			maxAngularVelocity = 0;
			totAngularVelocity = 0;
			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
				moleculeSets[i]->getPSOVelocityStats(maxCoordinateVelocity, totCoordinateVelocity,
				                                     maxAngularVelocity, totAngularVelocity);
			
			totCoordinateVelocity /= moleculeSets[0]->getNumberOfMolecules() * moleculeSets.size();
			totAngularVelocity /= moleculeSets[0]->getNumberOfMolecules() * 3 * moleculeSets.size();
			
			
			// Print the structures
/*			for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			{
				fout << "Structure: " << i+1 << endl;
				moleculeSets[i]->print(fout);
				fout << "Energy = ";
				fout << setiosflags(ios::fixed) << setprecision(8) << moleculeSets[i]->getEnergy() << endl << endl;
			}*/
            
            iTotalEnergyCalculations = 0;
            iTotalConverged = 0;
            for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i) {
                iTotalEnergyCalculations += energyObjects[i]->getTimesCalculatedEnergy();
                iTotalConverged += energyObjects[i]->getTimesConverged();
				energyObjects[i]->resetTimesCalculatedEnergy();
            }
            fPercentConverged = (FLOAT) iTotalConverged / (FLOAT) iTotalEnergyCalculations * 100;
			
			if (input.m_fAttractionRepulsion > 0) {
				if (bestNMoleculeSets.size() > 0)
					tempFloat = bestNMoleculeSets[0]->getEnergy();
				else
					tempFloat = 0;
				if (input.m_bEnforceMinDistOnCopy)
					Gega::saveBestN(moleculeSetsMinDistEnforced, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
					                input.m_fMinDistnaceBetweenSameMoleculeSets);
				else
					Gega::saveBestN(moleculeSets, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
					                input.m_fMinDistnaceBetweenSameMoleculeSets);
				if (abs(tempFloat - bestNMoleculeSets[0]->getEnergy()) < 0.00000001)
					++input.m_iNumIterationsBestEnergyHasntChanged;
				else
					input.m_iNumIterationsBestEnergyHasntChanged = 0;
			}
			
			fout << "It: " << input.m_iIteration;
			fout << setiosflags(ios::fixed) << setprecision(8)
			     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy() << setprecision(3)
				 << " Coord Vel Max,Avg: " << maxCoordinateVelocity << ", " << totCoordinateVelocity
				 << " Angle Vel Max,Avg: " << (maxAngularVelocity * RAD_TO_DEG) << ", "
				 << (totAngularVelocity * RAD_TO_DEG)
			     << setiosflags(ios::fixed) << setprecision(3)
			     << " Div: " << diversity
			     << setiosflags(ios::fixed) << setprecision(1)
			     << " Vis: " << (visibility * 100) << "%" << endl;

//			     << " Converged: " << fPercentConverged << "%" << endl;
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				cout << "It: " << input.m_iIteration;
				cout << setiosflags(ios::fixed) << setprecision(8)
				     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy() << setprecision(3)
    				 << " Coord Vel Max,Avg: " << maxCoordinateVelocity << ", " << totCoordinateVelocity
    				 << " Angle Vel Max,Avg: " << (maxAngularVelocity * RAD_TO_DEG) << ", "
    				 << (totAngularVelocity * RAD_TO_DEG)
				     << setiosflags(ios::fixed) << setprecision(3)
				     << " Div: " << diversity
				     << setiosflags(ios::fixed) << setprecision(1)
				     << " Vis: " << (visibility * 100) << "%" << endl;
//				     << " Converged: " << fPercentConverged << "%" << endl;
			}
			
			if ((fPercentConverged < MIN_CONVERGENCE_PERCENT) && ((signed int)moleculeSets.size() >= 10)) {
				g_bErrorToStopFor = true;
				cout << "The convergence pecentage is below " << setprecision(2) << MIN_CONVERGENCE_PERCENT << "%.  Something may be wrong.  Exiting..." << endl;
				break;
			}
			
			if ((input.m_sResumeFileName.length() > 0) && ((input.m_iIteration%input.m_iResumeFileNumIterations) == 0))
			{
				input.writeResumeFile(tempResumeFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets, seconds, time (NULL));
				if (system(commandString))
				{
					cout << "Error copying the temporary resume file(" << tempResumeFileName
					     << ") to the real resume file." << endl;
					break;
				}
			}
			
			if (input.m_iIteration >= input.m_iMaxIterations)
				break;
		} while (true);
		
		if (!g_bErrorToStopFor) {
			cout << "Finished after " << input.m_iIteration << " iterations." << endl;
			fout << "Finished after " << input.m_iIteration << " iterations." << endl;
			if (input.m_sEnergyFunction.compare("Lennard Jones") == 0) {
				cout << "Optimizing best structure..." << endl;
				fout << "Optimizing best structure..." << endl;
				energyObjects[0]->calculateEnergy(*bestNMoleculeSets[0],false,true,true);
				bestNMoleculeSets[0]->setEnergy(energyObjects[0]->getEnergy());
				cout << "Done." << endl;
			}
			fout << "Best " << bestNMoleculeSets.size() << " structures:" << endl;
			cout << "Best " << bestNMoleculeSets.size() << " structures:" << endl;
			for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			{
				fout << "Structure #" << (i+1) << ": " << endl;
				bestNMoleculeSets[i]->print(fout);
				fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: "
				     << bestNMoleculeSets[i]->getEnergy() << endl;
				cout << setiosflags(ios::fixed) << setprecision(8) << "Energy: "
				     << bestNMoleculeSets[i]->getEnergy() << endl;
			}
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
	if (input.m_fLocalMinDist != 0)
		for (i = 0; i < (signed int)moleculeSets.size(); ++i)
			delete localBestMoleculeSets[i];
	moleculeSets.clear();
	bestIndividualMoleculeSets.clear();
	moleculeSetsMinDistEnforced.clear();
	localBestMoleculeSets.clear();

	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	bestNMoleculeSets.clear();
	
	for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i)
		delete energyObjects[i];
	delete[] energyObjects;
	
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

FLOAT getConnectivityDistance(vector<MoleculeSet*> &population, FLOAT desiredPercentConnectivity)
{
	FLOAT *distances;
	FLOAT distance;
	int i, j;
	int count, totalComparisons;
	
	if (desiredPercentConnectivity >= 1)
		return 1e100;
	if (desiredPercentConnectivity <= 0)
		return 0;
	
	totalComparisons = (signed int)((population.size()-1) * population.size() / 2);
	distances = new FLOAT[totalComparisons];
	
	count = 0;
	for (i = 0; i < (signed int)population.size()-1; ++i)
		for (j = i+1; j < (signed int)population.size(); ++j)
			distances[count++] = population[i]->getDistance(*population[j]);
	
	quickSort(distances,0,totalComparisons-1);
	count = (int)(desiredPercentConnectivity*totalComparisons);
	if (count > totalComparisons)
		count = totalComparisons;
	if (count < 0)
		count = 0;
	distance = distances[count];
	delete[] distances;
	
	return distance;
}

FLOAT findLocalBestMoleculeSets(vector<MoleculeSet*> &population, vector<MoleculeSet*> bestIndividualMoleculeSets,
                               vector<MoleculeSet*> &localBestMoleculeSets, FLOAT **distanceMatrix, FLOAT distance)
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
				if (bestIndividualMoleculeSets[j]->getEnergy() <
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

void gega(Input &input, vector<MoleculeSet*> &population, vector<MoleculeSet*> &bestNMoleculeSets, bool usingSeeding)
{
	vector<MoleculeSet*> emptyMoleculeSets; // This is only created here so we have something to pass in for
	                                        // bestIndividualMoleculeSets which is used in PSO.
	vector<MoleculeSet*> matingPool;
	vector<MoleculeSet*> offSpring;
	vector<MoleculeSet*> population2;
	vector<MoleculeSet*> *pPopulationA;
	vector<MoleculeSet*> *pPopulationB;
	int i;
	string tempResumeFileName = input.m_sResumeFileName + ".temp";
	char commandString[500];
	// Variables to keep track of how long this took
	time_t seconds;
	time_t minutes;
	time_t hours;
	time_t days;
	// Variables used in threads
	NodeData_t nodeData; // This node data structure will be coppied after it is passed to performAllTasks
	Energy** energyObjects;
    int iTotalEnergyCalculations;
    int iTotalConverged;
    FLOAT fPercentConverged;
	ofstream fout;
	
	energyObjects = new Energy*[input.m_srgNodeNames.size()];
	for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i)
		energyObjects[i] = new Energy(&input, i+1, input.m_srgNodeNames[i], input.m_sEnergyFunction);
	
	g_bErrorToStopFor = false;
	if (!input.m_bResumeFileRead) {
		seconds = time (NULL);
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			g_bErrorToStopFor = true;
		} else if (!usingSeeding) {
			input.printToFile(fout);
			fout << endl << endl;
			fout << "Output from program:" << endl;
			if (!usingSeeding) {
				cout << "Initializing the population..." << endl;
				fout << "Initializing the population..." << endl;
				initializePopulation(input, population, bestNMoleculeSets, nodeData, energyObjects,
				                     false, false);
				input.m_iNumEnergyEvaluations += (signed int)population.size();
			} else {
				cout << "Using structures from previous run(s)..." << endl;
				fout << "Using structures from previous run(s)..." << endl;
			}
		}
	} else {
		seconds = time (NULL) - (input.m_tTimeStampEnd - input.m_tTimeStampStart);
		fout.open (input.m_sOutputFileName.c_str(), ofstream::out | ofstream::app); // Append to the existing file
		if (!fout.is_open())
		{
			cout << "Unable to open the output file: " << input.m_sOutputFileName << endl;
			g_bErrorToStopFor = true;
		} else
			fout << "Resuming after program execution was stopped..." << endl;
		deleteEnergyFiles(population, energyObjects, input.m_srgNodeNames.size(), input.m_bUsePrevWaveFunction);
	}
	
	if (!g_bErrorToStopFor) {
		cout << "Performing Genetic Algorithm..." << endl;
		nodeData.task = CALCULATE_ENERGY_TASK;
		nodeData.moleculeSets = &offSpring;
		pPopulationA = &population;
		pPopulationB = &population2;
		
		sprintf(commandString,"mv %s %s", tempResumeFileName.c_str(), input.m_sResumeFileName.c_str());
		do {
			++input.m_iIteration;
			Gega::tournamentMateSelection(*pPopulationA, matingPool);
			Gega::performMatings(matingPool, offSpring, input.m_iNumStructureTypes,
			                     input.m_iNumStructuresOfEachType, input.m_boxDimensions);
			
			matingPool.clear();
			performAllTasks(nodeData, energyObjects, input.m_srgNodeNames.size());
			input.m_iNumEnergyEvaluations += (signed int)nodeData.moleculeSets->size();
			
            iTotalEnergyCalculations = 0;
            iTotalConverged = 0;
            for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i) {
                iTotalEnergyCalculations += energyObjects[i]->getTimesCalculatedEnergy();
                iTotalConverged += energyObjects[i]->getTimesConverged();
				energyObjects[i]->resetTimesCalculatedEnergy();
            }
            fPercentConverged = (FLOAT) iTotalConverged / (FLOAT) iTotalEnergyCalculations * 100;
			
			Gega::generationReplacement(*pPopulationA, offSpring, *pPopulationB);
			Gega::saveBestN(*pPopulationB, bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
			                input.m_fMinDistnaceBetweenSameMoleculeSets);
			
			fout << "It: " << input.m_iIteration;
			fout << setiosflags(ios::fixed) << setprecision(8)
			     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy()
			     << setprecision(1)
			     << " Converged: " << fPercentConverged << "%" << endl;
			if (input.m_iIteration % input.m_iPrintSummaryInfoEveryNIterations == 0)
			{
				cout << "It: " << input.m_iIteration;
				cout << setiosflags(ios::fixed) << setprecision(8)
				     << " Best Energy: " << bestNMoleculeSets[0]->getEnergy()
				     << setprecision(1)
				     << " Converged: " << fPercentConverged << "%" << endl;
			}
			
			if ((fPercentConverged < MIN_CONVERGENCE_PERCENT) && ((signed int)population.size() >= 10)) {
				g_bErrorToStopFor = true;
				cout << "The convergence pecentage is below " << setprecision(2) << MIN_CONVERGENCE_PERCENT << "%.  Something may be wrong.  Exiting..." << endl;
				break;
			}
			
			if ((input.m_sResumeFileName.length() > 0) && ((input.m_iIteration % 20) == 0))
			{
				input.writeResumeFile(tempResumeFileName, *pPopulationB, bestNMoleculeSets, emptyMoleculeSets,
				                      seconds, time (NULL));
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
		} while (true);
		
		if (!g_bErrorToStopFor) {
			cout << "Finished after " << input.m_iIteration << " iterations." << endl;
			
			fout << "Best " << bestNMoleculeSets.size() << " structures:" << endl;
			cout << "Best " << bestNMoleculeSets.size() << " structures:" << endl;
			for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			{
				fout << "Structure #" << (i+1) << ": " << endl;
				bestNMoleculeSets[i]->print(fout);
				fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: "
				     << bestNMoleculeSets[i]->getEnergy() << endl;
				cout << setiosflags(ios::fixed) << setprecision(8) << "Energy: "
				     << bestNMoleculeSets[i]->getEnergy() << endl;
			}
		}
	}
	
   	// Clean up
	for (i = 0; i < (signed int)population.size(); ++i)
		delete population[i];
	for (i = 0; i < (signed int)population2.size(); ++i)
		delete population2[i];
	for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
		delete bestNMoleculeSets[i];
	population.clear();
	population2.clear();
	bestNMoleculeSets.clear();
	for (i = 0; i < (signed int)input.m_srgNodeNames.size(); ++i)
		delete energyObjects[i];
	delete[] energyObjects;
	
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
}

void deleteEnergyFiles(vector<MoleculeSet*> &moleculeSets, Energy** energyObjects, int numEnergyObjects,
                       bool usePreviousWaveFunction)
{
	if (usePreviousWaveFunction)
		for (int i = 0; i < (signed int)moleculeSets.size(); ++i)
			energyObjects[0]->deleteEnergyFiles(moleculeSets[i]);
	else
		for (int i = 0; i < (signed int)numEnergyObjects; ++i)
			energyObjects[i]->deleteEnergyFiles(NULL);
}


int main(int argc, char *argv[])
{
	string inputFileName;
	string tempOutputFileName;
	ofstream fout;
	int i, j;
	bool usingSeeding;
	Input input;
	Input inputSeeded;
	int numSeedFiles;
	vector<MoleculeSet*> moleculeSets;
	vector<MoleculeSet*> bestNMoleculeSets;
	vector<MoleculeSet*> bestIndividualMoleculeSets; // The best solution found for each individual (used in particle swarm optimization)
	vector<MoleculeSet*> *moleculeSetsSeeded;
	vector<MoleculeSet*> *bestNMoleculeSetsSeeded;
	vector<MoleculeSet*> *bestIndividualMoleculeSetsSeeded; // The best solution found for each individual (used in particle swarm optimization)
	
	Molecule::initRandoms();
	
	if (strncmp(argv[1],"-t", 2) == 0) { // # Make an output file from a resume file that's readable
		if (argc != 4) {
			cout << "When using the -t option, please specify a resume file followed by an output file to which results will be written." << endl;
			return 0;
		}
		inputFileName = argv[argc - 2];
		tempOutputFileName = argv[argc - 1];
		cout << "Reading Input File: " << inputFileName << endl;
		if (!input.open(inputFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
			return 0;
		if (!input.m_bResumeFileRead) {
			cout << "The input file you have specified is not a resume file." << endl;
			return 0;
		} else {
			fout.open(tempOutputFileName.c_str(), ofstream::out); // Erase the existing file, if there is one
			if (!fout.is_open())
			{
				cout << "Unable to open the output file: " << tempOutputFileName << endl;
				return 0;
			}
			input.printToFile(fout);
			fout << endl << endl << "Best " << bestNMoleculeSets.size() << " structures from iteration "
			     << input.m_iIteration << ":" << endl;
			for (i = 0; i < (signed int)bestNMoleculeSets.size(); ++i)
			{
				fout << "Structure #" << (i+1) << ": " << endl;
				bestNMoleculeSets[i]->print(fout);
				fout << setiosflags(ios::fixed) << setprecision(8) << "Energy: "
				     << bestNMoleculeSets[i]->getEnergy() << endl;
			}
			fout.close();
			cout << "Output written to: " << tempOutputFileName << endl << endl;
		}
	} else {
		if (argc < 2) {
			cout << "Please specify an input file." << endl << endl;
			return 0;
		}
		
		usingSeeding = (strncmp(argv[1],"-s", 2) == 0);
		if (usingSeeding) {
			if (argc < 4) {
				cout << "After '-s', please specify one or more resume files followed by an input file." << endl << endl;
				return 0;
			}
			bool errorToStopFor = false;
			numSeedFiles = argc-3;
			moleculeSetsSeeded = new vector<MoleculeSet*>[numSeedFiles];
			bestNMoleculeSetsSeeded = new vector<MoleculeSet*>[numSeedFiles];
			bestIndividualMoleculeSetsSeeded = new vector<MoleculeSet*>[numSeedFiles];
			
			for (i = 0; i < numSeedFiles; ++i) {
				inputFileName = argv[i+2];
				cout << "Reading Seed File: " << inputFileName << endl;
				if (!inputSeeded.open(inputFileName, moleculeSetsSeeded[i], bestNMoleculeSetsSeeded[i],
				    bestIndividualMoleculeSetsSeeded[i]))
					errorToStopFor = true;
				else if (!inputSeeded.m_bResumeFileRead) {
					cout << "Error: this program was expecting the seed file \"" << inputFileName
					     << "\" to be a resume file and it's not." << endl << endl;
					errorToStopFor = true;
				}
			}
			
			if (!errorToStopFor) {
				inputFileName = argv[argc-1];
				cout << "Reading Input File: " << inputFileName << endl;
				if (!input.open(inputFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
					errorToStopFor = true;
				else if (input.m_bResumeFileRead) {
					cout << "Error: this program was expecting this file \"" << inputFileName
					     << "\" to not be a resume file and it is." << endl << endl;
					errorToStopFor = true;
				}
			}
			
			for (i = 0; i < numSeedFiles; ++i) {
				if (errorToStopFor)
					break;
				
				if (!input.m_tempelateMoleculeSet.haveSameAtomsAndMolecules(*moleculeSetsSeeded[i][0])) {
					cout << "Error: The chemical structures in \"" << input.m_sInputFileName <<
					        "\" and \"" << argv[i+2] << "\" are not compatible." << endl << endl;
					errorToStopFor = true;
					break;
				}
				if ((input.m_iAlgorithmToDo == SIMULATED_ANNEALING) && input.m_bPerformNonFragSearch) {
					for (j = 0; j < (signed int)bestNMoleculeSetsSeeded[i].size(); ++j)
						if (bestNMoleculeSetsSeeded[i][j]->isFragmented(input.m_fMaxAtomDistance)) {
							cout << "You said to perform a non-fragmented search, but structure #" << (j+1)
							     << " in the list of best structures in \"" << argv[i+2]
							     << "\" is fragmented." << endl << endl;
							errorToStopFor = true;
							break;
						}
					if (errorToStopFor)
						break;
				}
				
				Gega::saveBestN(bestNMoleculeSetsSeeded[i], bestNMoleculeSets, input.m_iNumberOfBestStructuresToSave,
					            input.m_fMinDistnaceBetweenSameMoleculeSets);
				Gega::saveBestN(bestNMoleculeSetsSeeded[i], moleculeSets, input.m_iTotalPopulationSize,
					            input.m_fMinDistnaceBetweenSameMoleculeSets);
			}
			
			for (i = 0; i < numSeedFiles; ++i) {
				for (j = 0; j < (signed int)moleculeSetsSeeded[i].size(); ++j)
					delete moleculeSetsSeeded[i][j];
				moleculeSetsSeeded[i].clear();
				for (j = 0; j < (signed int)bestNMoleculeSetsSeeded[i].size(); ++j)
					delete bestNMoleculeSetsSeeded[i][j];
				bestNMoleculeSetsSeeded[i].clear();
				for (j = 0; j < (signed int)bestIndividualMoleculeSetsSeeded[i].size(); ++j)
					delete bestIndividualMoleculeSetsSeeded[i][j];
				bestIndividualMoleculeSetsSeeded[i].clear();
			}
			delete[] moleculeSetsSeeded;
			delete[] bestNMoleculeSetsSeeded;
			delete[] bestIndividualMoleculeSetsSeeded;
			
			if (errorToStopFor)
				return 0;
			
			if ((signed int)moleculeSets.size() < input.m_iTotalPopulationSize) {
				cout << setiosflags(ios::fixed) << setprecision(2)
					 << "The seed file(s) provided " << moleculeSets.size()
				     << " structures each different by an RMS distance of "
				     << input.m_fMinDistnaceBetweenSameMoleculeSets << "," << endl
				     << "but the input file \""
				     << input.m_sInputFileName << "\" requires " << input.m_iTotalPopulationSize << " structures."
				     << endl << endl;
				return 0;
			}
		} else {
			inputFileName = argv[argc-1];
			cout << "Reading Input File: " << inputFileName << endl;
			if (!input.open(inputFileName, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets))
				return 0;
		}
		
		if (input.m_iAlgorithmToDo == SIMULATED_ANNEALING)
			simulatedAnnealing(input, moleculeSets, bestNMoleculeSets, usingSeeding);
		else if (input.m_iAlgorithmToDo == PARTICLE_SWARM_OPTIMIZATION)
			particleSwarmOptimization(input, moleculeSets, bestNMoleculeSets, bestIndividualMoleculeSets, usingSeeding);
		else if (input.m_iAlgorithmToDo == GENETIC_ALGORITHM)
			gega(input, moleculeSets, bestNMoleculeSets, usingSeeding);
	}
	return 0;
}
