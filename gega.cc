////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages an atom.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
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
	
	sortMoleculeSets(offSpring,0,offSpring.size()-1);
	for (i = 0; i < numChildStructuresToSave; ++i)
		newPopulation.push_back(offSpring[i]);
	
	// Delete the offspring we're not saving
	for (i = numChildStructuresToSave; i < (signed int)offSpring.size(); ++i)
		delete offSpring[i];
	offSpring.clear();
	
	sortMoleculeSets(population,0,population.size()-1);
	for (i = 0; i < numParentStructuresToSave; ++i)
		newPopulation.push_back(population[i]);
	
	// Delete the parents we're not saving
	for (i = numParentStructuresToSave; i < (signed int)population.size(); ++i)
		delete population[i];
	population.clear();
}

void Gega::sortMoleculeSets(vector<MoleculeSet*> &moleculeSets, int lo, int hi)
{
	int left, right;
	FLOAT median;
	MoleculeSet* temp;
	
	if( hi > lo ) // if at least 2 elements, then
	{
		left=lo; right=hi;
		median= moleculeSets[(lo+hi)/2]->getEnergy();  // just an estimate!
		
		while(right >= left) // partition moleculeSets[lo..hi]
		// moleculeSets[lo..left-1] <= median and moleculeSets[right+1..hi] >= median
		{
			while(moleculeSets[left]->getEnergy() < median)
				left++;
			
			while(moleculeSets[right]->getEnergy() > median)
				right--;
			
			if(left > right)
				break;
			//swap
			temp=moleculeSets[left];
			moleculeSets[left]=moleculeSets[right];
			moleculeSets[right]=temp;
			left++;
			right--;
		}
		sortMoleculeSets(moleculeSets, lo, right);
		sortMoleculeSets(moleculeSets, left,  hi);
	}
}

void Gega::saveBestN(vector<MoleculeSet*> &moleculeSets, vector<MoleculeSet*> &bestN, int n,
                     FLOAT fMinDistnaceBetweenSameMoleculeSets, int iNumEnergyFilesToSave, string &sLogFilesDirectory)
{
	int i, ii, iii, indexToInsert;
	MoleculeSet* temp;
	char fileName[500];
	char commandLine[500];
	vector<MoleculeSet*> sortedMoleculeSets;
	
	for (i = 0; i < (signed int)moleculeSets.size(); ++i)
		sortedMoleculeSets.push_back(moleculeSets[i]);
	
	sortMoleculeSets(sortedMoleculeSets, 0, sortedMoleculeSets.size()-1);
	
	for (i = 0; i < (signed int)sortedMoleculeSets.size(); ++i)
	{
		// Find if/where to insert sortedMoleculeSets[i] in bestN
		indexToInsert = (signed int)bestN.size();
		while ((indexToInsert >= 1) && (sortedMoleculeSets[i]->getEnergy() < bestN[indexToInsert-1]->getEnergy()))
			--indexToInsert;
		if ((indexToInsert < (signed int)bestN.size()) || ((signed int)bestN.size() < n))
		{
			// Make sure there aren't any stuctures with lower energy that are "the same" as sortedMoleculeSets[i]
			for (ii = indexToInsert-1; ii >= 0; --ii)
				if (bestN[ii]->withinDistance(*sortedMoleculeSets[i],fMinDistnaceBetweenSameMoleculeSets))
				{
					indexToInsert = -1;
					break;
				}
			if (indexToInsert == -1)
				continue; // don't insert 
			
			// Insert
			// First shift the MoleculeSets at indexToInsert to the right
			bestN.push_back(NULL);
			for (ii = ((signed int) bestN.size())-1 ; ii > indexToInsert; --ii) {
				bestN[ii] = bestN[ii-1];
				
				// Copy the energy file
				if ((ii+1) <= iNumEnergyFilesToSave) {
					snprintf(fileName, 500, "%s/best%d.log", sLogFilesDirectory.c_str(), ii+1);
					snprintf(commandLine, 500, "mv %s %s", bestN[ii]->getEnergyFile(), fileName);
					system(commandLine);
					bestN[ii]->setEnergyFile(fileName);
				}
			}

			// Insert a copy of sortedMoleculeSets[i]
			temp = new MoleculeSet();
			temp->copy(*sortedMoleculeSets[i]);
			bestN[indexToInsert] = temp;
			
			// Copy the energy file
			if (indexToInsert < iNumEnergyFilesToSave) {
				snprintf(fileName, 500, "%s/best%d.log", sLogFilesDirectory.c_str(), (indexToInsert+1));
				snprintf(commandLine, 500, "mv %s %s", bestN[indexToInsert]->getEnergyFile(), fileName);
				system(commandLine);
				bestN[indexToInsert]->setEnergyFile(fileName);
			}
			
			// Remove any structures higher in energy than sortedMoleculeSets[i] that are the same as sortedMoleculeSets[i]
			for (ii = indexToInsert+1; ii < (signed int)bestN.size(); ++ii)
				while (bestN[ii]->withinDistance(*bestN[indexToInsert],
				                       fMinDistnaceBetweenSameMoleculeSets))
				{
					delete bestN[ii];
					for (iii = ii; iii < (signed int)bestN.size()-1; ++iii) {
						bestN[iii] = bestN[iii+1];
				
						// Copy the energy file
						if ((iii+1) <= iNumEnergyFilesToSave) {
							snprintf(fileName, 500, "%s/best%d.log", sLogFilesDirectory.c_str(), iii+1);
							snprintf(commandLine, 500, "mv %s %s", bestN[iii]->getEnergyFile(), fileName);
							system(commandLine);
							bestN[iii]->setEnergyFile(fileName);
						}
					}
					bestN.pop_back();
					if (ii >= (signed int)bestN.size())
						break;
				}
			
			// Make sure that bestN isn't bigger than n
			// This could happen if bestN == n and then we added one to the list which had better energy than
			// one in the original bestN
			if ((signed int)bestN.size() > n)
			{
				if ((signed int)bestN.size() <= iNumEnergyFilesToSave) {
					snprintf(commandLine, 500, "rm %s", bestN[(signed int)bestN.size()-1]->getEnergyFile());
					system(commandLine);
				}
				delete bestN[(signed int)bestN.size()-1];
				bestN.pop_back();
			}
		}
	}
}


