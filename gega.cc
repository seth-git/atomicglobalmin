////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages an atom.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "gega.h"

Gega::Gega ()
{
}

Gega::~Gega ()
{
}

void Gega::tournamentMateSelection(vector<MoleculeSet*> &population, vector<MoleculeSet*> &matingPool)
{
	int *prgTimesSelected;
	int i;
	int potentialMate1, potentialMate2;
	int count;
	
	prgTimesSelected = new int[population.size()];
	for (i = 0; i < (signed int)population.size(); ++i)
		prgTimesSelected[i] = 0;
	
	for (i = 1; i <= (signed int)population.size() * 2; ++i) {
		potentialMate1 = Molecule::randomInt(0, population.size()-1);
		while (prgTimesSelected[potentialMate1] >= 4) {
			++potentialMate1;
			if (potentialMate1 >= (signed int)population.size())
				potentialMate1 = 0;
		}
		potentialMate2 = Molecule::randomInt(0,population.size()-1);
		count = 0;
		while ((prgTimesSelected[potentialMate2] >= 4) || (potentialMate2 == potentialMate1)) {
			++potentialMate2;
			if (potentialMate2 >= (signed int)population.size())
				potentialMate2 = 0;

			// Note: it sometimes happens that one structure doesn't get picked for a tournament until the end.
			// If this happens, it has to be in a tournament by itself (it gets a free ride).
			++count;
			if (count >= (signed int)population.size()) {
				potentialMate2 = potentialMate1;
				break;
			}
		}
		if (population[potentialMate1]->getEnergy() >= population[potentialMate2]->getEnergy())
			matingPool.push_back(population[potentialMate1]);
		else
			matingPool.push_back(population[potentialMate2]);
		++prgTimesSelected[potentialMate1];
		++prgTimesSelected[potentialMate2];
	}
	
	delete[] prgTimesSelected;
}

void Gega::performMatings(vector<MoleculeSet*> &matingPool, vector<MoleculeSet*> &offSpring, int iNumStructureTypes,
                           int iNumStructuresOfEachType[], Point3D &boxDimensions)
{
	bool *prgSelected;
	int i;
	int mate1, mate2;
	int populationSize = (signed int)matingPool.size() / 2;
	MoleculeSet *pChild;
//	cout << "Starting matings" << endl;
	
	prgSelected = new bool[matingPool.size()];
	for (i = 0; i < (signed int)matingPool.size(); ++i)
		prgSelected[i] = false;
	
	for (i = 1; i <= populationSize; ++i) {
		mate1 = Molecule::randomInt(0,(signed int)matingPool.size()-1);
		while (prgSelected[mate1]) {
			++mate1;
			if (mate1 >= (signed int)matingPool.size())
				mate1 = 0;
		}
		mate2 = Molecule::randomInt(0,(signed int)matingPool.size()-1);
		while (prgSelected[mate2] || (mate2 == mate1)) {
			++mate2;
			if (mate2 >= (signed int)matingPool.size())
				mate2 = 0;
		}
		pChild = new MoleculeSet();
		pChild->makeChildAverage(*matingPool[mate1], *matingPool[mate2], iNumStructureTypes,
		                         iNumStructuresOfEachType, boxDimensions);
		offSpring.push_back(pChild);
		prgSelected[mate1] = true;
		prgSelected[mate2] = true;
	}
	delete[] prgSelected;
//	cout << "Finished matings" << endl;
}

void Gega::generationReplacement(vector<MoleculeSet*> &population, vector<MoleculeSet*> &offSpring,
                                 vector<MoleculeSet*> &newPopulation)
{
	int numParentStructuresToSave = 200;
	int numChildStructuresToSave;
	int i;
	
	if (numParentStructuresToSave > (signed int)population.size())
		numParentStructuresToSave = population.size();
	numChildStructuresToSave = population.size() - numParentStructuresToSave;
	
	Input::sortMoleculeSets(offSpring,0,offSpring.size()-1);
	for (i = 0; i < numChildStructuresToSave; ++i)
		newPopulation.push_back(offSpring[i]);
	
	// Delete the offspring we're not saving
	for (i = numChildStructuresToSave; i < (signed int)offSpring.size(); ++i)
		delete offSpring[i];
	offSpring.clear();
	
	Input::sortMoleculeSets(population,0,population.size()-1);
	for (i = 0; i < numParentStructuresToSave; ++i)
		newPopulation.push_back(population[i]);
	
	// Delete the parents we're not saving
	for (i = numParentStructuresToSave; i < (signed int)population.size(); ++i)
		delete population[i];
	population.clear();
}


