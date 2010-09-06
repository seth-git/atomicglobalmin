////////////////////////////////////////////////////////////////////////////////
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "init.h"

bool Init::initLinearMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions)
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

bool Init::initPlanarMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions)
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

bool Init::init3DMoleculeSet(MoleculeSet &moleculeSet, Point3D &boxDimensions)
{
	if (!moleculeSet.initPositionsAndAngles(boxDimensions, INITIALIZATION_TRIES)) {
		cout << "Failed to initialize 3D molecule set after " << INITIALIZATION_TRIES << " tries." << endl;
		return false;
	}
	return true;
}

bool Init::init3DMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Point3D &boxDimensions, FLOAT maxAtomDist)
{
	if (!moleculeSet.initPositionsAndAnglesWithMaxDist(boxDimensions, maxAtomDist, INITIALIZATION_TRIES)) {
		cout << "Failed to initialize 3D molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
		return false;
	}
	return true;
}

bool Init::init3DNonFragMoleculeSetWithMaxDist(MoleculeSet &moleculeSet, Point3D &boxDimensions, FLOAT maxAtomDist)
{
	if (!moleculeSet.initNonFragmentedSructure(boxDimensions, maxAtomDist, INITIALIZATION_TRIES)) {
		cout << "Failed to initialize 3D non-fragmented molecule set with maximum distance constraint after " << INITIALIZATION_TRIES << " tries." << endl;
		return false;
	}
	return true;
}

bool Init::initializePopulation(Input &input, vector<MoleculeSet*> &moleculeSets)
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

bool Init::initProgram(int mpiRank)
{
	char fileName[500];
	
	if (!Atom::initAtomicMasses()) // Read the file containing the atomic masses of atoms
		return false;
	if (!Bond::readBondDistanceFile("bondLengths.txt"))
		return false;
	Molecule::initRandoms(mpiRank);
	
	if ((getcwd(fileName, sizeof(fileName)) == NULL) && (errno != ERANGE))
		return false;
	Input::s_program_directory = fileName;
	
	return true;
}

