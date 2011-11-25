////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages a set of molecules.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "moleculeSet.h"

MoleculeSet::MoleculeSet()
{
	init(0);
}

MoleculeSet::MoleculeSet(int id)
{
	init(id);
}

void MoleculeSet::init(int id)
{
	m_iStructureId = id;
	m_iNumberOfMolecules = 0;
	m_prgMolecules = NULL;
	m_iNumberOfAtoms = 0;
	m_atoms = NULL;
	
	m_atomDistances = NULL;
	m_centerOfMass.x = 0;
	m_centerOfMass.y = 0;
	m_centerOfMass.z = 0;
	m_fTotalMass = 0;
	m_prgAtomDistancesToCenterOfMass = NULL;
	m_prgAtomToCenterRank = NULL;
	
	m_fEnergy = 0;
	m_iRun = IS_FINISHED;
	m_bIsTransitionState = false;
}

MoleculeSet::~MoleculeSet()
{
	cleanUp();
}

void MoleculeSet::cleanUp()
{
	int i;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		delete[] m_atomDistances[i];
	delete[] m_atomDistances;
	m_atomDistances = NULL;

	delete[] m_prgAtomDistancesToCenterOfMass;
	m_prgAtomDistancesToCenterOfMass = NULL;
	delete[] m_prgAtomToCenterRank;
	m_prgAtomToCenterRank = NULL;
	
	delete[] m_prgMolecules;
	m_prgMolecules = NULL;
	m_iNumberOfMolecules = 0;
	delete[] m_atoms;
	m_atoms = NULL;
	m_iNumberOfAtoms = 0;
	
	for (i = 0; i < (signed int)m_outputEnergyFiles.size(); ++i)
		if (m_outputEnergyFiles[i] != NULL)
			delete m_outputEnergyFiles[i];
	m_outputEnergyFiles.clear();
	
	m_fEnergy = 0;
	m_iRun = IS_FINISHED;
}

void MoleculeSet::setInputEnergyFile(const char *directory, const char* prefix, int number, const char* extension)
{
	char tempString[500];
	m_inputEnergyFile.m_sDirectory = directory;
	m_inputEnergyFile.m_sPrefix = prefix;
	m_inputEnergyFile.m_iNumber = number;
	m_inputEnergyFile.m_sExtension = extension;
	
	if (m_inputEnergyFile.m_sDirectory.length() > 0)
		snprintf(tempString, sizeof(tempString), "%s/%s%d.%s", directory, prefix, number, extension);
	else
		snprintf(tempString, sizeof(tempString), "%s%d.%s", prefix, number, extension);
	m_inputEnergyFile.m_sFullPathName = tempString;
}

const char* MoleculeSet::getInputEnergyFile() {
	if (m_inputEnergyFile.m_sFullPathName.length() > 0)
		return m_inputEnergyFile.m_sFullPathName.c_str();
	else
		return NULL;
}

void MoleculeSet::setOutputEnergyFile(const char *directory, const char* prefix, int number, const char* extension, unsigned int index, bool checkExistence)
{
	char tempString[500];
	EnergyFileInfo* info = new EnergyFileInfo();
	info->m_sDirectory = directory;
	info->m_sPrefix = prefix;
	info->m_iNumber = number;
	info->m_sExtension = extension;
	
	if (info->m_sDirectory.length() > 0)
		snprintf(tempString, sizeof(tempString), "%s/%s%d.%s", directory, prefix, number, extension);
	else
		snprintf(tempString, sizeof(tempString), "%s%d.%s", prefix, number, extension);
	info->m_sFullPathName = tempString;
	
	if (checkExistence && !fileExists(tempString)) {
		delete info;
		info = NULL;
	}

	while (index > m_outputEnergyFiles.size())
		m_outputEnergyFiles.push_back(NULL);
	if (index == m_outputEnergyFiles.size())
		m_outputEnergyFiles.push_back(info);
	else {
		if (m_outputEnergyFiles[index] != NULL)
			delete m_outputEnergyFiles[index];
		m_outputEnergyFiles[index] = info;
	}
}

const char* MoleculeSet::getOutputEnergyFile(unsigned int index) {
	if (index < m_outputEnergyFiles.size()) {
		if (m_outputEnergyFiles[index] == NULL)
			return NULL;
		else
			return m_outputEnergyFiles[index]->m_sFullPathName.c_str();
	} else
		return NULL;
}

void EnergyFileInfo::copy(const EnergyFileInfo &otherEnergyFileInfo)
{
	m_sDirectory = otherEnergyFileInfo.m_sDirectory;
	m_sPrefix = otherEnergyFileInfo.m_sPrefix;
	m_iNumber = otherEnergyFileInfo.m_iNumber;
	m_sExtension = otherEnergyFileInfo.m_sExtension;
	m_sFullPathName = otherEnergyFileInfo.m_sFullPathName;
}

void MoleculeSet::copy(const MoleculeSet &moleculeSet)
{
	EnergyFileInfo* info;
	int i;
	m_iStructureId = moleculeSet.m_iStructureId;
	
	setNumberOfMolecules(moleculeSet.m_iNumberOfMolecules);
	for (i = 0; i < moleculeSet.m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].copy(moleculeSet.m_prgMolecules[i]);
	initAtomIndexes();
	initAtomDistances();
	sortAtomRanks();
	m_fEnergy = moleculeSet.m_fEnergy;
	m_iRun = moleculeSet.m_iRun;
	m_bIsTransitionState = moleculeSet.m_bIsTransitionState;
	
	m_inputEnergyFile.copy(moleculeSet.m_inputEnergyFile);
	for (i = 0; i < (signed int)moleculeSet.m_outputEnergyFiles.size(); ++i) {
		if (moleculeSet.m_outputEnergyFiles[i] != NULL) {
			info = new EnergyFileInfo();
			info->copy(*(moleculeSet.m_outputEnergyFiles[i]));
		} else {
			info = NULL;
		}
		m_outputEnergyFiles.push_back(info);
	}
}

int MoleculeSet::getNumberOfMolecules()
{
	return m_iNumberOfMolecules;
}

int MoleculeSet::getNumberOfMoleculesWithMultipleAtoms()
{
	int total = 0;
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if (m_prgMolecules[i].getNumberOfAtoms() > 1)
			++total;
	return total;
}

void MoleculeSet::setNumberOfMolecules(int iNumberOfMolecules)
{
	if (iNumberOfMolecules <= 0)
	{
		cout << "You cannot have a number of molecules less than or equal to zero..." << endl;
		iNumberOfMolecules = 1;
	}
	cleanUp();
	m_iNumberOfMolecules = iNumberOfMolecules;
	m_prgMolecules = new Molecule[m_iNumberOfMolecules];
}

Molecule* MoleculeSet::getMolecules()
{
	return m_prgMolecules;
}

int MoleculeSet::getNumberOfAtoms()
{
	return m_iNumberOfAtoms;
}

void MoleculeSet::initVelocities(FLOAT coordinateVelocity, FLOAT maxAngleVelocityInRad)
{
	FLOAT angleX;
	FLOAT angleY;
	Point3D coordinateVelocityVector;
	Point3D angleVelocityVector;
	
	if (coordinateVelocity == 0)
		coordinateVelocity = 0.2;
	
	if (maxAngleVelocityInRad == 0)
		maxAngleVelocityInRad = 20/360*PIE_X_2;

	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if (!m_prgMolecules[i].getFrozen())
		{
			angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
			angleY = Molecule::randomFloat(0,PIE_X_2);
			coordinateVelocityVector = getVectorInDirection(angleX, angleY, coordinateVelocity);
			angleVelocityVector.x = Molecule::randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
			angleVelocityVector.y = Molecule::randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
			angleVelocityVector.z = Molecule::randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
			
			m_prgMolecules[i].setVelocities(coordinateVelocityVector, angleVelocityVector);
		}
}

bool MoleculeSet::initPositionsAndAngles(Point3D &boxDimensions, int numTries)
{
	int i, count, count2;
	bool failedToInitialzeMolecule = true;
	bool* moleculesInitialized = new bool[m_iNumberOfMolecules];
	bool* atomsInitialized = new bool[m_iNumberOfAtoms];

	count = 0;
	do {
		++count;
		for (i = 0; i < m_iNumberOfMolecules; ++i)
			moleculesInitialized[i] = false;
		for (i = 0; i < m_iNumberOfAtoms; ++i)
			atomsInitialized[i] = false;
		for (i = 0; i < m_iNumberOfMolecules; ++i)
		{
			count2 = 0;
			while (true) {
				++count2;
				if (m_prgMolecules[i].getNumberOfAtoms() == 1)
					m_prgMolecules[i].setCenterAndAngles(Molecule::randomFloat(0,boxDimensions.x),
						Molecule::randomFloat(0,boxDimensions.y),Molecule::randomFloat(0,boxDimensions.z),0,0,0);
				else
					m_prgMolecules[i].setCenterAndAngles(Molecule::randomFloat(0,boxDimensions.x),
						Molecule::randomFloat(0,boxDimensions.y),Molecule::randomFloat(0,boxDimensions.z),
						Molecule::randomFloat(0,PIE_X_2),Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2),
					        Molecule::randomFloat(0,PIE_X_2));
				m_prgMolecules[i].initRotationMatrix();
				m_prgMolecules[i].localToGlobal();
				m_prgMolecules[i].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances, atomsInitialized);
				
				failedToInitialzeMolecule = (!checkMinDistConstraints(i, moleculesInitialized) ||
				                             !checkBoxConstraints(boxDimensions,i));
				if (failedToInitialzeMolecule) {
					if (count2 > numTries)
						break;
				} else {
					m_prgMolecules[i].markAtomsAsInitialized(atomsInitialized);
					moleculesInitialized[i] = true;
					break;
				}
			}
			if (failedToInitialzeMolecule)
				break;
		}
	} while ((count < numTries) && !failedToInitialzeMolecule);
	
	delete[] moleculesInitialized;
	delete[] atomsInitialized;
	
	if (failedToInitialzeMolecule)
		return false;
	initAtomDistancesToCenterOfMass();
	sortAtomRanks();
	
	return true;
}

bool MoleculeSet::initPositionsAndAnglesWithMaxDist(Point3D &boxDimensions, FLOAT maxAtomDist, int numTries)
{
	int iInitMoleculeSetTries, iInitMoleculeTries;
	bool failedToInitialzeMolecule = false;
	int i;
	FLOAT angleX, angleY;
	Point3D unitVector;
	bool* moleculesInitialized = new bool[m_iNumberOfMolecules];
	bool* atomsInitialized = new bool[m_iNumberOfAtoms];
	int iNumMoleculesInitialized;
	int iMoleculeViolatingMaxDistConstraint; // index of the molecule violating the maximum distance constraint
	int iCurrentMolecule;
	bool checkMaxDistances;
	bool removeMaxDistConstraintViolator;
	
	iInitMoleculeSetTries = 0;
	do {
		++iInitMoleculeSetTries;
		for (i = 0; i < m_iNumberOfMolecules; ++i)
			moleculesInitialized[i] = false;
		for (i = 0; i < m_iNumberOfAtoms; ++i)
			atomsInitialized[i] = false;
		iMoleculeViolatingMaxDistConstraint = -1;
		
		for (iNumMoleculesInitialized = 0; iNumMoleculesInitialized < m_iNumberOfMolecules; ++iNumMoleculesInitialized)
		{
			iCurrentMolecule = Molecule::randomInt(0,m_iNumberOfMolecules-iNumMoleculesInitialized-1);
			for (i = 0; i <= iCurrentMolecule; ++i)
				if (moleculesInitialized[i])
					++iCurrentMolecule;
			
			iInitMoleculeTries = 0;
			do {
				++iInitMoleculeTries;
				checkMaxDistances = false;
				removeMaxDistConstraintViolator = false;
				if (iMoleculeViolatingMaxDistConstraint != -1) {
					// Place this molecule so it satisfies the maximum distance constraint
					angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
					angleY = Molecule::randomFloat(0,PIE_X_2);
					unitVector = getVectorInDirection(angleX, angleY, 1);
					if (m_prgMolecules[iCurrentMolecule].getNumberOfAtoms() > 1) {
						m_prgMolecules[iCurrentMolecule].setAngleX(Molecule::randomFloat(0,PIE_X_2));
						m_prgMolecules[iCurrentMolecule].setAngleY(Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2));
						m_prgMolecules[iCurrentMolecule].setAngleZ(Molecule::randomFloat(0,PIE_X_2));
					}
					placeMoleculeObservingMaxDist(m_prgMolecules[iCurrentMolecule],
					                      m_prgMolecules[iMoleculeViolatingMaxDistConstraint], unitVector, maxAtomDist);
					removeMaxDistConstraintViolator = true;
				} else {
					if (m_prgMolecules[iCurrentMolecule].getNumberOfAtoms() > 1) {
						// Place the molecule anywhere
						m_prgMolecules[iCurrentMolecule].setCenterAndAngles(Molecule::randomFloat(0,boxDimensions.x),
							Molecule::randomFloat(0,boxDimensions.y),Molecule::randomFloat(0,boxDimensions.z),
							Molecule::randomFloat(0,PIE_X_2),Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2),
						        Molecule::randomFloat(0,PIE_X_2));
						m_prgMolecules[iCurrentMolecule].initRotationMatrix();
						m_prgMolecules[iCurrentMolecule].localToGlobal();
					} else {
						if (m_iNumberOfMolecules-iNumMoleculesInitialized >= 2) { // if at least one other molecule must be initalized
							// Place this atom anywhere
							m_prgMolecules[iCurrentMolecule].setCenterAndAngles(Molecule::randomFloat(0,boxDimensions.x),
								Molecule::randomFloat(0,boxDimensions.y),Molecule::randomFloat(0,boxDimensions.z),0,0,0);
							checkMaxDistances = true; // check to see if the max distance constraint is violated
							m_prgMolecules[iCurrentMolecule].initRotationMatrix();
							m_prgMolecules[iCurrentMolecule].localToGlobal();
						} else {
							// Place this atom so it satisfies the maximum distance constraint
							
							// Randomly select another already initialized molecule
							if (iNumMoleculesInitialized == 0) {
								delete[] moleculesInitialized;
								delete[] atomsInitialized;
								return false;
							}
							int iOtherMolecule = Molecule::randomInt(0,iNumMoleculesInitialized-1);
							for (i = 0; i <= iOtherMolecule; ++i)
								if (!moleculesInitialized[i])
									++iOtherMolecule;

							angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
							angleY = Molecule::randomFloat(0,PIE_X_2);
							unitVector = getVectorInDirection(angleX, angleY, 1);
							placeMoleculeObservingMaxDist(m_prgMolecules[iCurrentMolecule], m_prgMolecules[iOtherMolecule],
							                              unitVector, maxAtomDist);
							removeMaxDistConstraintViolator = true;
						}
					}
				}
				m_prgMolecules[iCurrentMolecule].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances, atomsInitialized);
				
				failedToInitialzeMolecule = (!checkMinDistConstraints(iCurrentMolecule, moleculesInitialized) ||
				                             !checkBoxConstraints(boxDimensions,iCurrentMolecule));
				if (!failedToInitialzeMolecule) {
					if (checkMaxDistances && !checkMaxDistConstraints(iCurrentMolecule, moleculesInitialized, maxAtomDist))
						iMoleculeViolatingMaxDistConstraint = iCurrentMolecule;
					if (removeMaxDistConstraintViolator)
						iMoleculeViolatingMaxDistConstraint = -1;
					m_prgMolecules[iCurrentMolecule].markAtomsAsInitialized(atomsInitialized);
					moleculesInitialized[iCurrentMolecule] = true;
				}
//				else
//					cout << "Failed to initialize molecule: " << iCurrentMolecule << endl;
			} while ((iInitMoleculeTries < 50) && failedToInitialzeMolecule);
			if (failedToInitialzeMolecule)
				break;
		}
	} while ((iInitMoleculeSetTries < numTries) && failedToInitialzeMolecule);
	
	delete[] moleculesInitialized;
	delete[] atomsInitialized;
	
	if ((iMoleculeViolatingMaxDistConstraint != -1) || failedToInitialzeMolecule)
		return false;
	initAtomDistancesToCenterOfMass();
	sortAtomRanks();
	
	return true;
}

bool MoleculeSet::initNonFragmentedSructure(Point3D &boxDimensions, FLOAT maxAtomDist, int numTries)
{
	int iInitMoleculeSetTries, iInitMoleculeTries;
	bool failedToInitialzeMolecule = false;
	int i;
	FLOAT angleX, angleY;
	Point3D unitVector, moleculeCenter;
	bool* moleculesInitialized = new bool[m_iNumberOfMolecules];
	bool* atomsInitialized = new bool[m_iNumberOfAtoms];
	int iNumMoleculesInitialized;
	int iCurrentMolecule, iAlreadyInitializedMolecule;
	
	moleculeCenter.w = 1;
	
	iInitMoleculeSetTries = 0;
	do {
		++iInitMoleculeSetTries;
		for (i = 0; i < m_iNumberOfMolecules; ++i)
			moleculesInitialized[i] = false;
		for (i = 0; i < m_iNumberOfAtoms; ++i)
			atomsInitialized[i] = false;
		
		for (iNumMoleculesInitialized = 0; iNumMoleculesInitialized < m_iNumberOfMolecules; ++iNumMoleculesInitialized)
		{
			// Select a random molecule that hasn't been initialized
			iCurrentMolecule = Molecule::randomInt(0,m_iNumberOfMolecules-iNumMoleculesInitialized-1);
			for (i = 0; i <= iCurrentMolecule; ++i)
				if (moleculesInitialized[i])
					++iCurrentMolecule;
			
			iInitMoleculeTries = 0;
			do {
				++iInitMoleculeTries;
				
				if (m_prgMolecules[iCurrentMolecule].getNumberOfAtoms() > 1) {
					m_prgMolecules[iCurrentMolecule].setAngleX(Molecule::randomFloat(0,PIE_X_2));
					m_prgMolecules[iCurrentMolecule].setAngleY(Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2));
					m_prgMolecules[iCurrentMolecule].setAngleZ(Molecule::randomFloat(0,PIE_X_2));
				}
				if (iNumMoleculesInitialized == 0) {
					moleculeCenter.x = boxDimensions.x / 2;
					moleculeCenter.y = boxDimensions.y / 2;
					moleculeCenter.z = boxDimensions.z / 2;
					m_prgMolecules[iCurrentMolecule].setCenter(moleculeCenter);
					m_prgMolecules[iCurrentMolecule].initRotationMatrix();
					m_prgMolecules[iCurrentMolecule].localToGlobal();
				} else {
					angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
					angleY = Molecule::randomFloat(0,PIE_X_2);
					unitVector = getVectorInDirection(angleX, angleY, 1);
					
					// Select a random molecule that has already been initialized
					iAlreadyInitializedMolecule = Molecule::randomInt(0,iNumMoleculesInitialized-1);
					for (i = 0; i <= iAlreadyInitializedMolecule; ++i)
						if (!moleculesInitialized[i])
							++iAlreadyInitializedMolecule;
					
					placeMoleculeObservingMaxDist(m_prgMolecules[iCurrentMolecule],
					                      m_prgMolecules[iAlreadyInitializedMolecule], unitVector, maxAtomDist);
				}
				moleculeCenter = m_prgMolecules[iCurrentMolecule].getCenter();
//				cout << "Trying molecule " << (iNumMoleculesInitialized+1) << "(" << iCurrentMolecule
//				     << ") at (" << moleculeCenter.x << ","
//				     << moleculeCenter.y << "," << moleculeCenter.z << ")" << endl;
				m_prgMolecules[iCurrentMolecule].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances, atomsInitialized);
//				cout << "checkMinDistConstraints = " << checkMinDistConstraints(iCurrentMolecule, moleculesInitialized) << endl;
//				cout << "checkBoxConstraints = " << checkBoxConstraints(boxDimensions,iCurrentMolecule) << endl;
				
				
				failedToInitialzeMolecule = (!checkMinDistConstraints(iCurrentMolecule, moleculesInitialized) ||
				                             !checkBoxConstraints(boxDimensions,iCurrentMolecule));
//				cout << "failedToInitialzeMolecule = " << failedToInitialzeMolecule << endl;
				if (!failedToInitialzeMolecule) {
					m_prgMolecules[iCurrentMolecule].markAtomsAsInitialized(atomsInitialized);
					moleculesInitialized[iCurrentMolecule] = true;
				}
			} while ((iInitMoleculeTries < 50) && failedToInitialzeMolecule);
			if (failedToInitialzeMolecule) {
				cout << "Failed to initialize moleculeSet in non-fragmented structure" << endl;
				break;
			}
		}
	} while ((iInitMoleculeSetTries < numTries) && failedToInitialzeMolecule);
	
	delete[] moleculesInitialized;
	delete[] atomsInitialized;
	
	if (failedToInitialzeMolecule)
		return false;
	initAtomDistancesToCenterOfMass();
	sortAtomRanks();
	
	return true;
}

bool MoleculeSet::initFromSeed(MoleculeSet &seedMoleculeSet, int iNumStructureTypes,
                               int* iNumStructuresOfEachTypeOriginal, int* iNumStructuresOfEachTypeNew, vector<Point3D> *cartesianPoints,
                               vector<int> *atomicNumbers, Point3D &boxDimensions, FLOAT maxAtomDist, int numTries)
{
	int iInitMoleculeSetTries, iInitMoleculeTries;
	bool failedToInitialzeMolecule = false;
	int i, j, k;
	int mCount1, mCount2, aCount1;
	FLOAT angleX, angleY;
	Point3D unitVector, moleculeCenter;
	bool* moleculesInitializedSaved;
	bool* atomsInitializedSaved;
	bool* moleculesInitialized;
	bool* atomsInitialized;
	int iNumMoleculesInitialized, iNumMoleculesInitializedSaved;
	int iCurrentMolecule, iAlreadyInitializedMolecule;
	MoleculeSet tempCopy;
	
	if (!seedMoleculeSet.tryToEnforceBoxConstraints(boxDimensions)) {
		cout << "A seeded structure was found that doesn't fit in the box.  Exiting..." << endl << endl;
		return false;
	}
	
	// Copy the molecules from the seedMoleculeSet to this molecule set
	mCount1 = 0;
	for (i = 0; i < iNumStructureTypes; ++i)
		mCount1 += iNumStructuresOfEachTypeNew[i];
	setNumberOfMolecules(mCount1);
	
	// Count the number of total atoms
        m_iNumberOfAtoms = 0;
	for (i = 0; i < iNumStructureTypes; ++i)
		m_iNumberOfAtoms += iNumStructuresOfEachTypeNew[i] * cartesianPoints[i].size();

	moleculesInitialized = new bool[m_iNumberOfMolecules];
	atomsInitialized = new bool[m_iNumberOfAtoms];
	moleculesInitializedSaved = new bool[m_iNumberOfMolecules];
	atomsInitializedSaved = new bool[m_iNumberOfAtoms];

	iNumMoleculesInitializedSaved = 0;
	mCount1 = 0;
	mCount2 = 0;
	aCount1 = 0;
	for (i = 0; i < iNumStructureTypes; ++i) {
		for (j = 0; j < iNumStructuresOfEachTypeOriginal[i]; j++) {
			m_prgMolecules[mCount1].copy(seedMoleculeSet.m_prgMolecules[mCount2]);
			moleculesInitializedSaved[mCount1] = true;
			m_prgMolecules[mCount1].setFrozen(true);
			++iNumMoleculesInitializedSaved;
			++mCount1;
			++mCount2;
			for (k = 0; k < (signed int)cartesianPoints[i].size(); ++k) {
				atomsInitializedSaved[aCount1] = true;
				++aCount1;
			}
		}
		for (j = iNumStructuresOfEachTypeOriginal[i]; j < iNumStructuresOfEachTypeNew[i]; ++j) {
			m_prgMolecules[mCount1].makeFromCartesian(cartesianPoints[i], atomicNumbers[i]);
			m_prgMolecules[mCount1].initRotationMatrix();
			m_prgMolecules[mCount1].localToGlobal();
			moleculesInitializedSaved[mCount1] = false;
			++mCount1;
			for (k = 0; k < (signed int)cartesianPoints[i].size(); ++k) {
				atomsInitializedSaved[aCount1] = false;
				++aCount1;
			}
		}
	}
	initAtomIndexes();
	initAtomDistances();
	
	// Assign random locations and angles to the molecules that weren't coppied from seedMoleculeSet and
	// observe minimum and maximum distance constraints
	moleculeCenter.w = 1;
	
	iInitMoleculeSetTries = 0;
	do {
		++iInitMoleculeSetTries;
		for (i = 0; i < m_iNumberOfMolecules; ++i)
			moleculesInitialized[i] = moleculesInitializedSaved[i];
		for (i = 0; i < m_iNumberOfAtoms; ++i)
			atomsInitialized[i] = atomsInitializedSaved[i];
		
		for (iNumMoleculesInitialized = iNumMoleculesInitializedSaved;
		     iNumMoleculesInitialized < m_iNumberOfMolecules; ++iNumMoleculesInitialized)
		{
			// Select a random molecule that hasn't been initialized
			iCurrentMolecule = Molecule::randomInt(0,m_iNumberOfMolecules-iNumMoleculesInitialized-1);
			for (i = 0; i <= iCurrentMolecule; ++i)
				if (moleculesInitialized[i])
					++iCurrentMolecule;
			
			iInitMoleculeTries = 0;
			do {
				++iInitMoleculeTries;
				
				if (m_prgMolecules[iCurrentMolecule].getNumberOfAtoms() > 1) {
					m_prgMolecules[iCurrentMolecule].setAngleX(Molecule::randomFloat(0,PIE_X_2));
					m_prgMolecules[iCurrentMolecule].setAngleY(Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2));
					m_prgMolecules[iCurrentMolecule].setAngleZ(Molecule::randomFloat(0,PIE_X_2));
				}
				if (iNumMoleculesInitialized == 0) {
					moleculeCenter.x = boxDimensions.x / 2;
					moleculeCenter.y = boxDimensions.y / 2;
					moleculeCenter.z = boxDimensions.z / 2;
					m_prgMolecules[iCurrentMolecule].setCenter(moleculeCenter);
					m_prgMolecules[iCurrentMolecule].initRotationMatrix();
					m_prgMolecules[iCurrentMolecule].localToGlobal();
				} else {
					angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
					angleY = Molecule::randomFloat(0,PIE_X_2);
					unitVector = getVectorInDirection(angleX, angleY, 1);
					
					// Select a random molecule that has already been initialized
					iAlreadyInitializedMolecule = Molecule::randomInt(0,iNumMoleculesInitialized-1);
					for (i = 0; i <= iAlreadyInitializedMolecule; ++i)
						if (!moleculesInitialized[i])
							++iAlreadyInitializedMolecule;
					
					placeMoleculeObservingMaxDist(m_prgMolecules[iCurrentMolecule],
					                      m_prgMolecules[iAlreadyInitializedMolecule], unitVector, maxAtomDist);
				}
				moleculeCenter = m_prgMolecules[iCurrentMolecule].getCenter();
//				cout << "Trying molecule " << (iNumMoleculesInitialized+1) << "(" << iCurrentMolecule
//				     << ") at (" << moleculeCenter.x << ","
//				     << moleculeCenter.y << "," << moleculeCenter.z << ")" << endl;
				m_prgMolecules[iCurrentMolecule].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances, atomsInitialized);
//				cout << "checkMinDistConstraints = " << checkMinDistConstraints(iCurrentMolecule, moleculesInitialized) << endl;
//				cout << "checkBoxConstraints = " << checkBoxConstraints(boxDimensions,iCurrentMolecule) << endl;
				
				
				failedToInitialzeMolecule = (!checkMinDistConstraints(iCurrentMolecule, moleculesInitialized)
                                                             || isFragmented(maxAtomDist)
				                             || !checkBoxConstraints(boxDimensions,iCurrentMolecule));
//				cout << "failedToInitialzeMolecule = " << failedToInitialzeMolecule << endl;
				if (!failedToInitialzeMolecule) {
					m_prgMolecules[iCurrentMolecule].markAtomsAsInitialized(atomsInitialized);
					moleculesInitialized[iCurrentMolecule] = true;
				}
			} while ((iInitMoleculeTries < 50) && failedToInitialzeMolecule);
			if (failedToInitialzeMolecule) {
				cout << "Failed to initialize seeded moleculeSet in non-fragmented structure" << endl;
				break;
			}
		}
		if (!failedToInitialzeMolecule) {
			centerInBox(boxDimensions);
			if (getNumberOfMoleculesFrozen() < m_iNumberOfMolecules)
				for (i = 1; i <= 20; ++i) {
					tempCopy.copy(*this);
					if (!tempCopy.performTransformationsNonFrag(boxDimensions, 0.3, 0.785, maxAtomDist, 1)) {
						failedToInitialzeMolecule = true;
						break;
					}
				}
		}
	} while ((iInitMoleculeSetTries < numTries) && failedToInitialzeMolecule);
	
	delete[] moleculesInitialized;
	delete[] atomsInitialized;
	delete[] moleculesInitializedSaved;
	delete[] atomsInitializedSaved;
	
	if (failedToInitialzeMolecule)
		return false;
	initAtomDistancesToCenterOfMass();
	sortAtomRanks();
	
	return true;
}

void MoleculeSet::unFreezeAll(FLOAT coordinateVelocity, FLOAT maxAngleVelocityInRad)
{
	FLOAT angleX;
	FLOAT angleY;
	Point3D coordinateVelocityVector;
	Point3D angleVelocityVector;
	
	if (coordinateVelocity == 0)
		coordinateVelocity = 0.2;
	
	if (maxAngleVelocityInRad == 0)
		maxAngleVelocityInRad = 20/360*PIE_X_2;
	
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if (m_prgMolecules[i].getFrozen())
		{
		 	m_prgMolecules[i].setFrozen(false);
			if ((coordinateVelocity > 0) && (maxAngleVelocityInRad > 0)) {
				angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
				angleY = Molecule::randomFloat(0,PIE_X_2);
				coordinateVelocityVector = getVectorInDirection(angleX, angleY, coordinateVelocity);
				angleVelocityVector.x = Molecule::randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
				angleVelocityVector.y = Molecule::randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
				angleVelocityVector.z = Molecule::randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
				
				m_prgMolecules[i].setVelocities(coordinateVelocityVector, angleVelocityVector);
			}
		}
}

int MoleculeSet::getNumberOfMoleculesFrozen()
{
	int answer = 0;
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
                if (m_prgMolecules[i].getFrozen())
			++answer;
	return answer;
}

void MoleculeSet::placeMoleculeObservingMaxDist(Molecule &moleculeToPlace, Molecule &otherMolecule,
                                                Point3D unitVector, FLOAT maxDist)
{
	FLOAT distance;
	Point3D vector;
	Point3D center;
	
	if ((moleculeToPlace.getNumberOfAtoms() == 1) &&
	    (otherMolecule.getNumberOfAtoms() == 1)) {
		if (maxDist == 0) {
			distance = moleculeToPlace.getMinDistance(otherMolecule);
			distance = Molecule::randomFloat(distance,distance+0.01);
		} else
			distance = Molecule::randomFloat(moleculeToPlace.getMinDistance(otherMolecule),maxDist);
		vector.x = unitVector.x * distance;
		vector.y = unitVector.y * distance;
		vector.z = unitVector.z * distance;
		center = otherMolecule.getPositionRelativeToMoleculeAlongVector(vector);
		moleculeToPlace.setCenter(center);
		moleculeToPlace.initRotationMatrix();
		moleculeToPlace.localToGlobal();
	} else {
		Point3D atom1; // atom in moleculeToPlace closest to the otherMolecule (closest to atom2)
		Point3D atom2; // atom in the otherMolecule closest to moleculeToPlace (closest to atom1)
		FLOAT minDistanceBetweenAtoms;
		
		// Find the atoms in the two molecules that are closest to one another
		distance = 10000; // some big number
		vector.x = unitVector.x * distance;
		vector.y = unitVector.y * distance;
		vector.z = unitVector.z * distance;
		center = otherMolecule.getPositionRelativeToMoleculeAlongVector(vector);
		moleculeToPlace.setCenter(center);
		moleculeToPlace.initRotationMatrix();
		moleculeToPlace.localToGlobal();
		moleculeToPlace.getClosestAtoms(otherMolecule, atom1, atom2, minDistanceBetweenAtoms);
		
		// Let R be a line defined by unitVector and atom2;
		Point3D pointOnRClosestToAtom1 = closestPointFromALineToAPoint(atom2, unitVector, atom1);
			
		// Let L be a line defined by unitVector and otherMolecule.getCenter();
		Point3D pointOnLClosestToAtom1 = closestPointFromALineToAPoint(otherMolecule.getCenter(), unitVector, atom1);
		Point3D pointOnLClosestToAtom2 = closestPointFromALineToAPoint(otherMolecule.getCenter(), unitVector, atom2);
		
		FLOAT a = distanceBetweenPoints(pointOnLClosestToAtom1,moleculeToPlace.getCenter());
		FLOAT b = distanceBetweenPoints(pointOnLClosestToAtom2,otherMolecule.getCenter());
		FLOAT x = distanceBetweenPoints(pointOnRClosestToAtom1,atom1);
		FLOAT e;
		if (maxDist == 0)
			maxDist = minDistanceBetweenAtoms + 0.01;
		if (x > maxDist)
			// There is no way to do this problem the way we want to if x > maxDist.
			// So we'll set e to the minimum possible value and then incrument it (see below).
			e = a + b;
		else {
			FLOAT y;
			if (minDistanceBetweenAtoms < x)
				y = Molecule::randomFloat(x,maxDist);
			else
				y = Molecule::randomFloat(minDistanceBetweenAtoms,maxDist);
			e = sqrt((y * y) - (x * x)) + a + b;
		}
		if (isnan(e))
		{
			cout << "e = nan in the function MoleculeSet::placeMoleculeObservingMaxDist!" << endl;
			return;
		}
		
		while (true) {
			vector.x = unitVector.x * e;
			vector.y = unitVector.y * e;
			vector.z = unitVector.z * e;
			center = otherMolecule.getPositionRelativeToMoleculeAlongVector(vector);
			if (isnan(center.x))
			{
				cout << "center.x = nan in the function MoleculeSet::placeMoleculeObservingMaxDist!" << endl;
				return;
			}
			moleculeToPlace.setCenter(center);
			moleculeToPlace.initRotationMatrix();
			moleculeToPlace.localToGlobal();
			moleculeToPlace.initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances);
			if (moleculeToPlace.checkMinDistances(otherMolecule, m_atomDistances))
				break;
			e += 0.1;
		}
	}
}

Point3D MoleculeSet::closestPointFromALineToAPoint(Point3D pointOnLine, Point3D vectorAlongLine, Point3D point)
{
	Point3D closestPoint;
	FLOAT vectorLengthSquared;
	FLOAT u;
	
	vectorLengthSquared =
		vectorAlongLine.x * vectorAlongLine.x +
		vectorAlongLine.y * vectorAlongLine.y +
		vectorAlongLine.z * vectorAlongLine.z;
	
	u = ((point.x - pointOnLine.x) * vectorAlongLine.x + 
		 (point.y - pointOnLine.y) * vectorAlongLine.y + 
		 (point.z - pointOnLine.z) * vectorAlongLine.z) / vectorLengthSquared;
	
	closestPoint.x = pointOnLine.x + u * vectorAlongLine.x;
	closestPoint.y = pointOnLine.y + u * vectorAlongLine.y;
	closestPoint.z = pointOnLine.z + u * vectorAlongLine.z;
	
	return closestPoint;
}

FLOAT MoleculeSet::distanceBetweenPoints(Point3D point1, Point3D point2)
{
	Point3D diff;
	diff.x = point1.x - point2.x;
	diff.y = point1.y - point2.y;
	diff.z = point1.z - point2.z;
	return sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
}

bool MoleculeSet::isFragmented(FLOAT maxDist)
{
	int iMoleculesVisited;
	bool *visitedMolecules;
	int i;
	
	visitedMolecules = new bool[m_iNumberOfMolecules];
	iMoleculesVisited = 1;
	visitedMolecules[0] = true;
	for (i = 1; i < m_iNumberOfMolecules; ++i)
			visitedMolecules[i] = false;
	traverseConnectedMolecules(0, visitedMolecules, iMoleculesVisited, maxDist);
	
	delete[] visitedMolecules;
	return (iMoleculesVisited < m_iNumberOfMolecules);
}

void MoleculeSet::traverseConnectedMolecules(int iMolecule, bool visitedMolecules[], int &iMoleculesVisited, FLOAT maxDist)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if ((iMolecule != i) && !visitedMolecules[i] &&
		    m_prgMolecules[iMolecule].checkMaxDistances(m_prgMolecules[i], m_atomDistances, maxDist)) {
			visitedMolecules[i] = true;
			++iMoleculesVisited;
			traverseConnectedMolecules(i, visitedMolecules, iMoleculesVisited, maxDist);
		}
}

bool MoleculeSet::checkMinDistConstraints()
{
	int i, j;
	
	for (i = 0; i < m_iNumberOfAtoms-1; ++i)
		for (j = i+1; j < m_iNumberOfAtoms; ++j)
			if (m_atomDistances[i][j] < m_atoms[i]->getMinDistance(*m_atoms[j]))
				return false;
	return true;
}

bool MoleculeSet::checkMinDistConstraints(int iMolecule)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if ((iMolecule != i) && (!m_prgMolecules[i].checkMinDistances(m_prgMolecules[iMolecule], m_atomDistances)))
			return false;
	return true;
}

bool MoleculeSet::checkMinDistConstraints(int iMolecule, bool moleculesInitialized[])
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if (moleculesInitialized[i] && (iMolecule != i) &&
			(!m_prgMolecules[i].checkMinDistances(m_prgMolecules[iMolecule], m_atomDistances)))
			return false;
	return true;
}

bool MoleculeSet::checkMaxDistConstraints(int iMolecule, bool moleculesInitialized[], FLOAT maxDist)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if (moleculesInitialized[i] && (iMolecule != i) &&
			(m_prgMolecules[i].checkMaxDistances(m_prgMolecules[iMolecule], m_atomDistances, maxDist)))
			return true;
	return false;
}

bool MoleculeSet::checkMaxDistConstraints(FLOAT maxDist)
{
	int i, j;
	bool constraintSatisfied;
	for (i = 0; i < m_iNumberOfMolecules; ++i) {
		if (m_prgMolecules[i].getNumberOfAtoms() > 1)
			constraintSatisfied = true;
		else {
			constraintSatisfied = false;
			for (j = 0; j < m_iNumberOfMolecules; ++j)
				if ((i != j) && m_prgMolecules[i].checkMaxDistances(m_prgMolecules[j], m_atomDistances, maxDist)) {
					constraintSatisfied = true;
					break;
				}
		}
		if (!constraintSatisfied)
			return false;
	}
	return true;
}

bool MoleculeSet::tryToEnforceBoxConstraints(Point3D &boxDimensions)
{
	Point3D minCoordinates, maxCoordinates;
	Point3D coordinateSpans; // maxs - mins
	Point3D shiftCoordinates;
	const FLOAT SOME_BIG_NUMBER = 1000000;
	int i;
	minCoordinates.x = SOME_BIG_NUMBER;
	minCoordinates.y = SOME_BIG_NUMBER;
	minCoordinates.z = SOME_BIG_NUMBER;
	maxCoordinates.x = -SOME_BIG_NUMBER;
	maxCoordinates.y = -SOME_BIG_NUMBER;
	maxCoordinates.z = -SOME_BIG_NUMBER;
	
	for (i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].getMinimumAndMaximumCoordinates(minCoordinates, maxCoordinates);
    
	if ((minCoordinates.x < 0) || (maxCoordinates.x > boxDimensions.x) ||
	    (minCoordinates.y < 0) || (maxCoordinates.y > boxDimensions.y) ||
	    (minCoordinates.z < 0) || (maxCoordinates.z > boxDimensions.z))
	{
		coordinateSpans.x = maxCoordinates.x - minCoordinates.x;
		coordinateSpans.y = maxCoordinates.y - minCoordinates.y;
		coordinateSpans.z = maxCoordinates.z - minCoordinates.z;
		
		if ((coordinateSpans.x <= boxDimensions.x) &&
		    (coordinateSpans.y <= boxDimensions.y) &&
		    (coordinateSpans.z <= boxDimensions.z))
		{
			// Set the molecules in the center of the box
			shiftCoordinates.x = (boxDimensions.x * 0.5) - ((coordinateSpans.x * 0.5) + minCoordinates.x);
			shiftCoordinates.y = (boxDimensions.y * 0.5) - ((coordinateSpans.y * 0.5) + minCoordinates.y);
			shiftCoordinates.z = (boxDimensions.z * 0.5) - ((coordinateSpans.z * 0.5) + minCoordinates.z);
			for (i = 0; i < m_iNumberOfMolecules; ++i)
				m_prgMolecules[i].moveMolecule(shiftCoordinates);
			
			return true;
		}
		return false;
	}
	
	return true;
}

bool MoleculeSet::tryToEnforceBoxConstraints(Point3D &boxDimensions, int iChangedMolecule)
{
	Point3D minCoordinates, maxCoordinates;
	Point3D coordinateSpans; // maxs - mins
	Point3D shiftCoordinates;
	const FLOAT SOME_BIG_NUMBER = 1000000;
	int i;
	
	// Enforce the constraint that all atoms must be within the boxDimensions
	minCoordinates.x = SOME_BIG_NUMBER;
	minCoordinates.y = SOME_BIG_NUMBER;
	minCoordinates.z = SOME_BIG_NUMBER;
	maxCoordinates.x = -SOME_BIG_NUMBER;
	maxCoordinates.y = -SOME_BIG_NUMBER;
	maxCoordinates.z = -SOME_BIG_NUMBER;
	m_prgMolecules[iChangedMolecule].getMinimumAndMaximumCoordinates(minCoordinates, maxCoordinates);
	
	if ((minCoordinates.x < 0) || (maxCoordinates.x > boxDimensions.x) ||
	    (minCoordinates.y < 0) || (maxCoordinates.y > boxDimensions.y) ||
	    (minCoordinates.z < 0) || (maxCoordinates.z > boxDimensions.z))
	{
		// See if we are able to  move the entire molecule set so it will fit within the box
		for (i = 0; i < m_iNumberOfMolecules; ++i)
			if (i != iChangedMolecule)
				m_prgMolecules[i].getMinimumAndMaximumCoordinates(minCoordinates, maxCoordinates);
		coordinateSpans.x = maxCoordinates.x - minCoordinates.x;
		coordinateSpans.y = maxCoordinates.y - minCoordinates.y;
		coordinateSpans.z = maxCoordinates.z - minCoordinates.z;
		
		if ((coordinateSpans.x <= boxDimensions.x) &&
		    (coordinateSpans.y <= boxDimensions.y) &&
		    (coordinateSpans.z <= boxDimensions.z))
		{
			// Set the molecules in the center of the box
			shiftCoordinates.x = (boxDimensions.x * 0.5) - ((coordinateSpans.x * 0.5) + minCoordinates.x);
			shiftCoordinates.y = (boxDimensions.y * 0.5) - ((coordinateSpans.y * 0.5) + minCoordinates.y);
			shiftCoordinates.z = (boxDimensions.z * 0.5) - ((coordinateSpans.z * 0.5) + minCoordinates.z);
			for (i = 0; i < m_iNumberOfMolecules; ++i)
				m_prgMolecules[i].moveMolecule(shiftCoordinates);
			return true;
		}
		return false;
	}
	
	return true;
}

bool MoleculeSet::checkBoxConstraints(Point3D &boxDimensions, int iMolecule)
{
	Point3D minCoordinates, maxCoordinates;
	const FLOAT SOME_BIG_NUMBER = 1000000;
	
	minCoordinates.x = SOME_BIG_NUMBER;
	minCoordinates.y = SOME_BIG_NUMBER;
	minCoordinates.z = SOME_BIG_NUMBER;
	maxCoordinates.x = -SOME_BIG_NUMBER;
	maxCoordinates.y = -SOME_BIG_NUMBER;
	maxCoordinates.z = -SOME_BIG_NUMBER;
	m_prgMolecules[iMolecule].getMinimumAndMaximumCoordinates(minCoordinates, maxCoordinates);
	
	if ((minCoordinates.x < 0) || (maxCoordinates.x > boxDimensions.x) ||
	    (minCoordinates.y < 0) || (maxCoordinates.y > boxDimensions.y) ||
	    (minCoordinates.z < 0) || (maxCoordinates.z > boxDimensions.z))
	{
		return false;
	}
	
	return true;
}

void MoleculeSet::centerInBox(Point3D &boxDimensions)
{
	Point3D minCoordinates, maxCoordinates;
	Point3D coordinateSpans; // maxs - mins
	Point3D shiftCoordinates;
	const FLOAT SOME_BIG_NUMBER = 1000000;
	int i;
	minCoordinates.x = SOME_BIG_NUMBER;
	minCoordinates.y = SOME_BIG_NUMBER;
	minCoordinates.z = SOME_BIG_NUMBER;
	maxCoordinates.x = -SOME_BIG_NUMBER;
	maxCoordinates.y = -SOME_BIG_NUMBER;
	maxCoordinates.z = -SOME_BIG_NUMBER;
	
	for (i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].getMinimumAndMaximumCoordinates(minCoordinates, maxCoordinates);
    
	coordinateSpans.x = maxCoordinates.x - minCoordinates.x;
	coordinateSpans.y = maxCoordinates.y - minCoordinates.y;
	coordinateSpans.z = maxCoordinates.z - minCoordinates.z;
	
	// Set the molecules in the center of the box
	shiftCoordinates.x = (boxDimensions.x * 0.5) - ((coordinateSpans.x * 0.5) + minCoordinates.x);
	shiftCoordinates.y = (boxDimensions.y * 0.5) - ((coordinateSpans.y * 0.5) + minCoordinates.y);
	shiftCoordinates.z = (boxDimensions.z * 0.5) - ((coordinateSpans.z * 0.5) + minCoordinates.z);
	for (i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].moveMolecule(shiftCoordinates);
}

Point3D MoleculeSet::getVectorInDirection(FLOAT angleX, FLOAT angleY, FLOAT length)
{
	FLOAT matrixX[MATRIX_SIZE][MATRIX_SIZE];
	FLOAT matrixY[MATRIX_SIZE][MATRIX_SIZE];
	FLOAT temp[MATRIX_SIZE][MATRIX_SIZE];
	Point3D vector;
	
	FLOAT sinAngleX = FLOAT(sin(angleX));
	FLOAT sinAngleY = FLOAT(sin(angleY));
	FLOAT cosAngleX = FLOAT(cos(angleX));
	FLOAT cosAngleY = FLOAT(cos(angleY));
	
	vector.x = 0;
	vector.y = 0;
	vector.z = length;
	vector.w = 1;
	
	matrixX[0][0] = 1;
	matrixX[1][0] = 0;
	matrixX[2][0] = 0;
	matrixX[3][0] = 0;
	matrixX[0][1] = 0;
	matrixX[1][1] = cosAngleX;
	matrixX[2][1] = sinAngleX;
	matrixX[3][1] = 0;
	matrixX[0][2] = 0;
	matrixX[1][2] = -sinAngleX;
	matrixX[2][2] = cosAngleX;
	matrixX[3][2] = 0;
	matrixX[0][3] = 0;
	matrixX[1][3] = 0;
	matrixX[2][3] = 0;
	matrixX[3][3] = 1;

	matrixY[0][0] = cosAngleY;
	matrixY[1][0] = 0;
	matrixY[2][0] = sinAngleY;
	matrixY[3][0] = 0;
	matrixY[0][1] = 0;
	matrixY[1][1] = 1;
	matrixY[2][1] = 0;
	matrixY[3][1] = 0;
	matrixY[0][2] = -sinAngleY;
	matrixY[1][2] = 0;
	matrixY[2][2] = cosAngleY;
	matrixY[3][2] = 0;
	matrixY[0][3] = 0;
	matrixY[1][3] = 0;
	matrixY[2][3] = 0;
	matrixY[3][3] = 1;

	Molecule::matrixMultiplyMatrix(matrixY,matrixX,temp);
	Molecule::matrixMultiplyPoint(vector,temp);
	
	return vector;
}

bool MoleculeSet::offSetCoordinatesAndAnglesSlightly(Point3D &boxDimensions)
{
	int i;
	FLOAT deltaForCoordinates = 0.01;
	FLOAT deltaForAnglesInRad = 0.01744; // about 1 deg
	int numTries = 10;
	int count;
	bool success;
	FLOAT angleX;
	FLOAT angleY;
	Point3D shiftCoordinates;
	Point3D rotateAngles;
	
	for (i = 0; i < m_iNumberOfMolecules; ++i)
	{
		// Shift the coordinates
		count = 0;
		success = false;
		do {
			++count;
			angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
			angleY = Molecule::randomFloat(0,PIE_X_2);
			shiftCoordinates = getVectorInDirection(angleX, angleY, deltaForCoordinates);
			m_prgMolecules[i].moveMolecule(shiftCoordinates);
			initAtomDistances();
			if (checkMinDistConstraints(i) && tryToEnforceBoxConstraints(boxDimensions, i))
				success = true;
			else
			{
				shiftCoordinates.x = -shiftCoordinates.x;
				shiftCoordinates.y = -shiftCoordinates.y;
				shiftCoordinates.z = -shiftCoordinates.z;
				m_prgMolecules[i].moveMolecule(shiftCoordinates);
				initAtomDistances();
			}
		} while ((count < numTries) && !success);
		
		// Shift the angles
		if (m_prgMolecules[i].getNumberOfAtoms() > 1) {
			count = 0;
			success = false;
			do {
				++count;
				angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
				angleY = Molecule::randomFloat(0,PIE_X_2);
				rotateAngles = getVectorInDirection(angleX, angleY, deltaForAnglesInRad);
				m_prgMolecules[i].rotateMolecule(rotateAngles);
				initAtomDistances();
				if (checkMinDistConstraints(i) && tryToEnforceBoxConstraints(boxDimensions, i))
					success = true;
				else
				{
					rotateAngles.x = -rotateAngles.x;
					rotateAngles.y = -rotateAngles.y;
					rotateAngles.z = -rotateAngles.z;
					m_prgMolecules[i].rotateMolecule(rotateAngles);
					initAtomDistances();
				}
			} while ((count < numTries) && !success);
		}
	}
	sortAtomRanks();
	return success;
}

bool MoleculeSet::performTransformations(Point3D &boxDimensions, FLOAT deltaForCoordinates, FLOAT deltaForAnglesInRad, int numTransformations)
{
	int i;
	int moleculeToPurturb;
	int thingToPurturb;
	int moleculeSetTry;
	int moleculeTry;
	const int numMoleculeSetTries = 20;
	const int numMoleculeTries = 60;
	bool* moleculesTransformed;
	bool success;
	
	moleculesTransformed = new bool[m_iNumberOfMolecules];

	success = false;
	for (moleculeSetTry = 1; (moleculeSetTry <= numMoleculeSetTries) && !success; ++moleculeSetTry) {
//		cout << "Try " << moleculeSetTry << " on molecule set " << m_iStructureId << endl;
		for (i = 0; i < m_iNumberOfMolecules; ++i)
			moleculesTransformed[i] = false;
		
		success = true;
		for (i = 1; (i <= numTransformations) && success; ++i) {
//			cout << "Transformation #" << i << endl;
			success = false;
			for (moleculeTry = 1; (moleculeTry <= numMoleculeTries) && !success; ++moleculeTry) {
//				cout << "moleculeTry " << moleculeTry << endl;
				do {
					moleculeToPurturb = Molecule::randomInt(0,m_iNumberOfMolecules-1);
				} while (m_prgMolecules[moleculeToPurturb].getFrozen() || moleculesTransformed[moleculeToPurturb]);
				
				if (m_prgMolecules[moleculeToPurturb].getNumberOfAtoms() == 1)
					thingToPurturb = Molecule::randomInt(1,3);
				else
					thingToPurturb = Molecule::randomInt(1,6);
				if (thingToPurturb <= 3) // move the molecule
				{
					Point3D shiftCoordinates;
					FLOAT angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
					FLOAT angleY = Molecule::randomFloat(0,PIE_X_2);
					shiftCoordinates = getVectorInDirection(angleX,angleY, deltaForCoordinates);
					m_prgMolecules[moleculeToPurturb].moveMolecule(shiftCoordinates);
					initAtomDistances();
					if (checkMinDistConstraints(moleculeToPurturb) && tryToEnforceBoxConstraints(boxDimensions, moleculeToPurturb)) {
						success = true;
						moleculesTransformed[moleculeToPurturb] = true;
					} else {
						success = false;
						shiftCoordinates.x = -shiftCoordinates.x;
						shiftCoordinates.y = -shiftCoordinates.y;
						shiftCoordinates.z = -shiftCoordinates.z;
						m_prgMolecules[moleculeToPurturb].moveMolecule(shiftCoordinates);
						initAtomDistances();
	//					cout << "Move failed, reverting... " << endl;
					}
				}
				else // rotate the molecule
				{
					Point3D rotateAngles;
					rotateAngles.x = 0;
					rotateAngles.y = 0;
					rotateAngles.z = 0;
					switch (thingToPurturb)
					{
						case 4:
							rotateAngles.x = ((FLOAT)Molecule::randomInt(0,1)-0.5) * 2 * deltaForAnglesInRad;
//							cout << "Moving angle X on molecule " << (moleculeToPurturb+1) << "." << endl;
							break;
						case 5:
							rotateAngles.y = ((FLOAT)Molecule::randomInt(0,1)-0.5) * 2 * deltaForAnglesInRad;
//							cout << "Moving angle Y on molecule " << (moleculeToPurturb+1) << "." << endl;
							break;
						case 6:
							rotateAngles.z = ((FLOAT)Molecule::randomInt(0,1)-0.5) * 2 * deltaForAnglesInRad;
//							cout << "Moving angle Z on molecule " << (moleculeToPurturb+1) << "." << endl;
							break;
					}
					m_prgMolecules[moleculeToPurturb].rotateMolecule(rotateAngles);
					initAtomDistances();
					if (checkMinDistConstraints(moleculeToPurturb) && tryToEnforceBoxConstraints(boxDimensions, moleculeToPurturb)) {
						success = true;
						moleculesTransformed[moleculeToPurturb] = true;
					} else {
						success = false;
						rotateAngles.x = -rotateAngles.x;
						rotateAngles.y = -rotateAngles.y;
						rotateAngles.z = -rotateAngles.z;
						m_prgMolecules[moleculeToPurturb].rotateMolecule(rotateAngles);
						initAtomDistances();
//						cout << "Move angle failed, reverting... " << endl;
					}
				}
//				cout << "success = " << success << endl;
			}
//			cout << "success = " << success << endl;
		}
//		cout << "success = " << success << endl;
	}
	sortAtomRanks();
	delete[] moleculesTransformed;
	if (!success)
		cout << "Failed to transform moleculeSet..." << endl;
	return success;
}

bool MoleculeSet::performTransformationsNonFrag(Point3D &boxDimensions, FLOAT deltaForCoordinates,
                                                FLOAT deltaForAnglesInRad, FLOAT maxDist, int numTransformations)
{
	int i;
	int moleculeTry, moleculeSetTry;
	const int numMoleculeSetTries = 20;
	const int numMoleculeTries = 60;
	int thingToPurturb;
	int iMoleculeToPerturb, iOtherMolecule;
	Molecule moleculeBackupCopy;
	const FLOAT propabilityOfSwapping = -1;
	bool* moleculesTransformed;
	bool success;
	
	moleculesTransformed = new bool[m_iNumberOfMolecules];
	
	success = false;
	for (moleculeSetTry = 1; (moleculeSetTry <= numMoleculeSetTries) && !success; ++moleculeSetTry) {
		for (i = 0; i < m_iNumberOfMolecules; ++i)
			moleculesTransformed[i] = false;
		
		success = true;
		for (i = 1; (i <= numTransformations) && success; ++i) {
			success = false;
			for (moleculeTry = 1; moleculeTry <= numMoleculeTries; ++moleculeTry) {
				do {
					iMoleculeToPerturb = Molecule::randomInt(0, m_iNumberOfMolecules-1);
				} while (m_prgMolecules[iMoleculeToPerturb].getFrozen() || moleculesTransformed[iMoleculeToPerturb]);
		
				if (Molecule::randomFloat(0,1) <= propabilityOfSwapping) {
					thingToPurturb = 1; // Swap two molecules
				} else {
					if (m_prgMolecules[iMoleculeToPerturb].getNumberOfAtoms() == 1)
						thingToPurturb = Molecule::randomInt(2,4);
					else
						thingToPurturb = Molecule::randomInt(2,7);
				}
					
				if (thingToPurturb <= 1) { // swap two molecules
					Point3D temp;
					// Randomly Pick another molecule within maxDist of iMoleculeToPerturb
					do {
						iOtherMolecule = Molecule::randomInt(0, m_iNumberOfMolecules-1);
					} while ((iOtherMolecule == iMoleculeToPerturb) ||
					        !(m_prgMolecules[iMoleculeToPerturb].checkMaxDistances(m_prgMolecules[iOtherMolecule],
					                                                               m_atomDistances, maxDist)));
					
					// Swap the molecule centers
					temp = m_prgMolecules[iMoleculeToPerturb].getCenter();
					m_prgMolecules[iMoleculeToPerturb].setCenter(m_prgMolecules[iOtherMolecule].getCenter());
					m_prgMolecules[iOtherMolecule].setCenter(temp);
					
					m_prgMolecules[iMoleculeToPerturb].initRotationMatrix();
					m_prgMolecules[iMoleculeToPerturb].localToGlobal();
					m_prgMolecules[iOtherMolecule].initRotationMatrix();
					m_prgMolecules[iOtherMolecule].localToGlobal();
					initAtomDistances();
					
					if (checkMinDistConstraints(iMoleculeToPerturb) && checkMinDistConstraints(iOtherMolecule) &&
					    !isFragmented(maxDist))
						if (tryToEnforceBoxConstraints(boxDimensions)) { // this can't be reverted, so it's last
							moleculesTransformed[iMoleculeToPerturb] = true;
							success = true;
							break; // Success!!
						}
					
					// Swap back the molecule centers
					temp = m_prgMolecules[iMoleculeToPerturb].getCenter();
					m_prgMolecules[iMoleculeToPerturb].setCenter(m_prgMolecules[iOtherMolecule].getCenter());
					m_prgMolecules[iOtherMolecule].setCenter(temp);
					
					m_prgMolecules[iMoleculeToPerturb].initRotationMatrix();
					m_prgMolecules[iMoleculeToPerturb].localToGlobal();
					m_prgMolecules[iOtherMolecule].initRotationMatrix();
					m_prgMolecules[iOtherMolecule].localToGlobal();
					initAtomDistances();
				} else if (thingToPurturb <= 4) { // move the molecule
					Point3D shiftCoordinates;
					FLOAT angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
					FLOAT angleY = Molecule::randomFloat(0,PIE_X_2);
					shiftCoordinates = getVectorInDirection(angleX,angleY, deltaForCoordinates);
					moleculeBackupCopy.copy(m_prgMolecules[iMoleculeToPerturb]);
					m_prgMolecules[iMoleculeToPerturb].moveMolecule(shiftCoordinates);
					m_prgMolecules[iMoleculeToPerturb].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances);
					
					if (checkMinDistConstraints(iMoleculeToPerturb) && !isFragmented(maxDist))
						if (tryToEnforceBoxConstraints(boxDimensions, iMoleculeToPerturb)) { // this can't be reverted, so it's last
							moleculesTransformed[iMoleculeToPerturb] = true;
							success = true;
							break; // Success!!
						}
					
					// Revert the change and try again
					m_prgMolecules[iMoleculeToPerturb].copy(moleculeBackupCopy);
					initAtomIndexes(iMoleculeToPerturb);
					m_prgMolecules[iMoleculeToPerturb].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances);
				}
				else // rotate the molecule
				{
					Point3D rotateAngles;
					rotateAngles.x = 0;
					rotateAngles.y = 0;
					rotateAngles.z = 0;
					switch (thingToPurturb)
					{
						case 5:
							rotateAngles.x = ((FLOAT)Molecule::randomInt(0,1)-0.5) * 2 * deltaForAnglesInRad;
							break;
						case 6:
							rotateAngles.y = ((FLOAT)Molecule::randomInt(0,1)-0.5) * 2 * deltaForAnglesInRad;
							break;
						case 7:
							rotateAngles.z = ((FLOAT)Molecule::randomInt(0,1)-0.5) * 2 * deltaForAnglesInRad;
							break;
					}
					moleculeBackupCopy.copy(m_prgMolecules[iMoleculeToPerturb]);
					m_prgMolecules[iMoleculeToPerturb].rotateMolecule(rotateAngles);
					m_prgMolecules[iMoleculeToPerturb].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances);
					
					if (checkMinDistConstraints(iMoleculeToPerturb) && !isFragmented(maxDist))
						if (tryToEnforceBoxConstraints(boxDimensions, iMoleculeToPerturb)) { // this can't be reverted, so it's last
							moleculesTransformed[iMoleculeToPerturb] = true;
							success = true;
							break; // Success!!
						}
					
					// Revert the change and try again
					m_prgMolecules[iMoleculeToPerturb].copy(moleculeBackupCopy);
					initAtomIndexes(iMoleculeToPerturb);
					m_prgMolecules[iMoleculeToPerturb].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances);
				}
			}
		}
	}
	
	initAtomDistancesToCenterOfMass();
	sortAtomRanks();
	delete[] moleculesTransformed;
	if (!success)
		cout << "Failed to transform non-fragmented moleculeSet" << m_iStructureId << "..." << endl;
	return success;
}

void MoleculeSet::performPSO(MoleculeSet &populationBestSet, MoleculeSet &individualBestSet, FLOAT coordInertia,
	                         FLOAT coordIndividualMinimumAttraction, FLOAT coordPopulationMinimumAttraction,
	                         FLOAT coordMaxVelocity, FLOAT angleInertia, FLOAT angleIndividualMinimumAttraction,
	                         FLOAT anglePopulationMinimumAttraction, FLOAT angleMaxVelocity, Point3D &boxDimensions,
	                         bool enforceMinDistances, FLOAT fAttractionRepulsion)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if (!m_prgMolecules[i].getFrozen())
			m_prgMolecules[i].performPSO(populationBestSet.m_prgMolecules[i], individualBestSet.m_prgMolecules[i], coordInertia,
			        	             coordIndividualMinimumAttraction, coordPopulationMinimumAttraction, coordMaxVelocity,
			                             angleInertia, angleIndividualMinimumAttraction, anglePopulationMinimumAttraction,
			                             angleMaxVelocity, boxDimensions, fAttractionRepulsion);
		
	initAtomDistances();
	
	if (enforceMinDistances)
		enforceMinDistConstraints(boxDimensions);
	else
		sortAtomRanks();
}

void MoleculeSet::getPSOVelocityStats(FLOAT &maxCoordinateVelocity, FLOAT &totCoordinateVelocity, FLOAT &maxAngularVelocity,
                                      FLOAT &totAngularVelocity)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].getVelocityStats(maxCoordinateVelocity, totCoordinateVelocity,
		                                   maxAngularVelocity, totAngularVelocity);
}

void MoleculeSet::enforceMinDistConstraints(Point3D &boxDimensions)
{
	vector<int*> overlappingMoleculePairs;
	int* overlappingMoleculePair;
	int i, j;
	Molecule moleculeBackupCopy;
	FLOAT distance;
	Point3D centerI, centerJ, unitVector;
	int maxTries = m_iNumberOfMolecules * 100;
	
	int tries = 0;
	do {
		++tries;
		for (i = 0; i < m_iNumberOfMolecules-1; ++i)
			for (j = i+1; j < m_iNumberOfMolecules; ++j)
				if (!m_prgMolecules[i].checkMinDistances(m_prgMolecules[j], m_atomDistances)) {
					overlappingMoleculePair = new int[2];
					overlappingMoleculePair[0] = i;
					overlappingMoleculePair[1] = j;
					overlappingMoleculePairs.push_back(overlappingMoleculePair);
				}
		if (overlappingMoleculePairs.size() > 0) {
			i = Molecule::randomInt(0,overlappingMoleculePairs.size()-1);
			overlappingMoleculePair = overlappingMoleculePairs[i];
			if (Molecule::randomInt(0,1)) {
				i = overlappingMoleculePair[0];
				j = overlappingMoleculePair[1];
			} else {
				j = overlappingMoleculePair[0];
				i = overlappingMoleculePair[1];
			}
			
			// Move molecule i away from molecule j
			centerI = m_prgMolecules[i].getCenter();
			centerJ = m_prgMolecules[j].getCenter();
			unitVector.x = centerI.x - centerJ.x;
			unitVector.y = centerI.y - centerJ.y;
			unitVector.z = centerI.z - centerJ.z;
			distance = sqrt(unitVector.x * unitVector.x + unitVector.y * unitVector.y + unitVector.z * unitVector.z);
			if (distance == 0) {
				FLOAT angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
				FLOAT angleY = Molecule::randomFloat(0,PIE_X_2);
				unitVector = getVectorInDirection(angleX, angleY, 1);
			} else {
				unitVector.x /= distance;
				unitVector.y /= distance;
				unitVector.z /= distance;
			}
			moleculeBackupCopy.copy(m_prgMolecules[i]);
			placeMoleculeObservingMaxDist(m_prgMolecules[i], m_prgMolecules[j], unitVector, 0);
			
			m_prgMolecules[i].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances);
			if (!m_prgMolecules[i].checkMinDistances(m_prgMolecules[j], m_atomDistances) && (tries >= maxTries)) {
				cout << "placeMoleculeObservingMaxDist failed for molecules " << i << " and " << j << " at try " << tries << endl;
				
				cout << "Molecule " << i << ":" << endl;
				m_prgMolecules[i].printToScreen();
				
				cout << "Molecule " << j << ":" << endl;
				m_prgMolecules[j].printToScreen();
			}
			
			if ((!tryToEnforceBoxConstraints(boxDimensions,i)) && (tries < maxTries)) {
				m_prgMolecules[i].copy(moleculeBackupCopy);
				initAtomIndexes(i);
				m_prgMolecules[i].initDistanceMatrix(m_iNumberOfAtoms, m_atoms, m_atomDistances);
			}
			
			// Clean up
			for (i = 0; i < (signed int)overlappingMoleculePairs.size(); ++i)
				delete[] overlappingMoleculePairs[i];
			overlappingMoleculePairs.clear();
		} else
			break;
	} while (tries < maxTries);
	initAtomDistancesToCenterOfMass();
	sortAtomRanks();
//	if (tries >= maxTries)
//		cout << "Unable to enforce min dist constrants" << endl;
}


void MoleculeSet::print(ofstream &fout)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].printToFile(fout);
}

void MoleculeSet::printToResumeFile(ofstream &fout, bool printVelocityInfo)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
	{
		m_prgMolecules[i].printToResumeFile(fout, printVelocityInfo);
		fout << endl;
	}
	fout << "Energy: " << m_fEnergy << endl << endl;
}

bool MoleculeSet::readFromResumeFile(ifstream &infile, char* fileLine, int MAX_LINE_LENGTH, bool readVelocityInfo)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if (!m_prgMolecules[i].readFromResumeFile(infile, fileLine, MAX_LINE_LENGTH, readVelocityInfo))
			return false;
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The energy for a structure was missing in the input file." << endl;
		return false;
	}
	if (sscanf(fileLine, "Energy: %lf", &m_fEnergy) != 1)
	{
		cout << "The energy for a structure could not be read." << endl;
		return false;
	}
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "A blank line in the input file after a molecule was missing." << endl;
		return false;
	}
	if (strlen(fileLine) != 0)
	{
		cout << "A line in the input file after a molecule should have been blank and it wasn't." << endl;
		return false;
	}
	initAtomIndexes();
	initAtomDistances();
	sortAtomRanks();
	return true;
}

void MoleculeSet::printToScreen()
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].printToScreen();
}

void MoleculeSet::writeCoordinatesToInputFileWithAtomicNumbers(ofstream &fout)
{
	for (int i=0; i < m_iNumberOfMolecules; i++)
		m_prgMolecules[i].writeCoordinatesToInputFileWithAtomicNumbers(fout);
}

void MoleculeSet::writeCoordinatesToInputFileWithAtomicSymbols(ofstream &fout)
{
	for (int i=0; i < m_iNumberOfMolecules; i++)
		m_prgMolecules[i].writeCoordinatesToInputFileWithAtomicSymbols(fout);
}

void MoleculeSet::writeToGausianLogFile(FILE* fout)
{
	for (int i=0; i < m_iNumberOfMolecules; i++)
		m_prgMolecules[i].writeToGausianLogFile(fout);
}

FLOAT MoleculeSet::getDistanceFromPoint(Point3D &point)
{
	FLOAT distance;
	distance = 0;
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		distance += m_prgMolecules[i].getDistanceFromPoint(point);
	return distance;
}

void MoleculeSet::initAtomIndexes()
{
	int iMolecule, iAtom;
	
	// Count the number of total atoms
	m_iNumberOfAtoms = 0;
	for (iMolecule = 0; iMolecule < m_iNumberOfMolecules; ++iMolecule)
		m_iNumberOfAtoms += m_prgMolecules[iMolecule].getNumberOfAtoms();
	
	// Initialize Memory
	m_atoms = new Atom*[m_iNumberOfAtoms];
	m_atomDistances = new FLOAT*[m_iNumberOfAtoms];
	m_prgAtomDistancesToCenterOfMass = new FLOAT[m_iNumberOfAtoms];
	m_prgAtomToCenterRank = new int[m_iNumberOfAtoms];
	
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		m_atomDistances[iAtom] = new FLOAT[m_iNumberOfAtoms];
	
	// Initialize Indexes
	iAtom = 0;
	for (iMolecule = 0; iMolecule < m_iNumberOfMolecules; ++iMolecule)
		m_prgMolecules[iMolecule].initAtomIndexes(m_atoms, iAtom);
	if (iAtom != m_iNumberOfAtoms) {
		cout << "We really have a problem" << endl;
	}
}

void MoleculeSet::initAtomIndexes(int iChangedMolecule)
{
	int iMolecule, iAtom;
	iAtom = 0;
	for (iMolecule = 0; iMolecule < m_iNumberOfMolecules; ++iMolecule)
		if (iChangedMolecule == iMolecule) {
			m_prgMolecules[iMolecule].initAtomIndexes(m_atoms, iAtom);
			break;
		} else
			iAtom += m_prgMolecules[iMolecule].getNumberOfAtoms();
}

void MoleculeSet::initAtomDistances()
{
	int iAtom, jAtom;
	FLOAT diffX, diffY, diffZ;
	FLOAT distance;
	
	for (iAtom = 0; iAtom < m_iNumberOfAtoms-1; ++iAtom)
		for (jAtom = iAtom+1; jAtom < m_iNumberOfAtoms; ++jAtom)
		{
			diffX = m_atoms[iAtom]->m_globalPoint.x - m_atoms[jAtom]->m_globalPoint.x;
			diffY = m_atoms[iAtom]->m_globalPoint.y - m_atoms[jAtom]->m_globalPoint.y;
			diffZ = m_atoms[iAtom]->m_globalPoint.z - m_atoms[jAtom]->m_globalPoint.z;
			distance = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
			m_atomDistances[iAtom][jAtom] = distance;
			m_atomDistances[jAtom][iAtom] = distance;
		}
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		m_atomDistances[iAtom][iAtom] = 0;
	
	initAtomDistancesToCenterOfMass();
}

void MoleculeSet::initAtomDistancesToCenterOfMass()
{
	int iAtom;
	FLOAT diffX, diffY, diffZ;
	FLOAT distance;
	
	// Calculate the center of mass
	m_centerOfMass.x = 0;
	m_centerOfMass.y = 0;
	m_centerOfMass.z = 0;
	m_fTotalMass = 0;
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
	{
		m_centerOfMass.x += m_atoms[iAtom]->getMass() * m_atoms[iAtom]->m_globalPoint.x;
		m_centerOfMass.y += m_atoms[iAtom]->getMass() * m_atoms[iAtom]->m_globalPoint.y;
		m_centerOfMass.z += m_atoms[iAtom]->getMass() * m_atoms[iAtom]->m_globalPoint.z;
		m_fTotalMass += m_atoms[iAtom]->getMass();
	}
	m_centerOfMass.x /= m_fTotalMass;
	m_centerOfMass.y /= m_fTotalMass;
	m_centerOfMass.z /= m_fTotalMass;
	
	// Calculate distances to the center of mass
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
	{
		diffX = m_atoms[iAtom]->m_globalPoint.x - m_centerOfMass.x;
		diffY = m_atoms[iAtom]->m_globalPoint.y - m_centerOfMass.y;
		diffZ = m_atoms[iAtom]->m_globalPoint.z - m_centerOfMass.z;
		distance = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
		m_prgAtomDistancesToCenterOfMass[iAtom] = distance;
	}
}

void MoleculeSet::sortAtomRanks()
{
	int iAtom;
	
	// Rank or sort atoms relative to the center of mass.
	// The rank will be based first on atomic number and second on distance to the center of mass.
	
	// Initialize m_prgAtomToCenterRank
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		m_prgAtomToCenterRank[iAtom] = iAtom;
	Atom::sortAtoms(m_prgAtomToCenterRank,m_atoms,m_prgAtomDistancesToCenterOfMass,0,m_iNumberOfAtoms-1);
}

bool MoleculeSet::withinDistance(MoleculeSet &otherSet, FLOAT minCompareDist)
{
	return (getDistance(otherSet) <= minCompareDist); 
}

/*
FLOAT MoleculeSet::getDistanceOld(MoleculeSet &otherSet)
{
	int iRank, jRank;
	FLOAT diff;
	FLOAT distance = 0;
	int atom1Index, atom2Index;
	int otherSetAtom1Index, otherSetAtom2Index;
	int totalComparisons = (signed int)((m_iNumberOfAtoms-1) * m_iNumberOfAtoms / 2);
	
	for (iRank = 0; iRank < m_iNumberOfAtoms; ++iRank)
		for (jRank = 0; jRank < m_iNumberOfAtoms; ++jRank)
			if (iRank != jRank)
			{
				atom1Index = m_prgAtomToCenterRank[iRank];
				atom2Index = m_atomToAtomRank[atom1Index][jRank];
				otherSetAtom1Index = otherSet.m_prgAtomToCenterRank[iRank];
				otherSetAtom2Index = otherSet.m_atomToAtomRank[otherSetAtom1Index][jRank];
				
				diff = otherSet.m_atomDistances[atom1Index][atom2Index] -
				                m_atomDistances[otherSetAtom1Index][otherSetAtom2Index];
				
				distance += diff*diff;
			}
	distance /= totalComparisons;
	distance = sqrt(distance);
	return distance;
}*/

FLOAT MoleculeSet::getDistance(MoleculeSet &otherSet)
{
	int iRank, jRank;
	FLOAT diff;
	FLOAT distance = 0;
	int atom1Index, atom2Index;
	int otherSetAtom1Index, otherSetAtom2Index;
	int totalComparisons = (signed int)((m_iNumberOfAtoms-1) * m_iNumberOfAtoms / 2);
	
	for (iRank = 0; iRank < m_iNumberOfAtoms-1; ++iRank)
		for (jRank = iRank+1; jRank < m_iNumberOfAtoms; ++jRank)
		{
			atom1Index = m_prgAtomToCenterRank[iRank];
			atom2Index = m_prgAtomToCenterRank[jRank];
			otherSetAtom1Index = otherSet.m_prgAtomToCenterRank[iRank];
			otherSetAtom2Index = otherSet.m_prgAtomToCenterRank[jRank];
			
			diff =          m_atomDistances[atom1Index][atom2Index] -
			       otherSet.m_atomDistances[otherSetAtom1Index][otherSetAtom2Index];
			
			distance += diff*diff;
		}
	distance /= totalComparisons;
	distance = sqrt(distance);
	return distance;
}

/*bool MoleculeSet::withinDistance(MoleculeSet &otherSet, FLOAT minCompareDist)
{
	int iRank, jRank;
	FLOAT diff;
	FLOAT distance = 0;
	int atom1Index, atom2Index;
	int otherSetAtom1Index, otherSetAtom2Index;

	for (iRank = 0; iRank < m_iNumberOfAtoms; ++iRank)
		for (jRank = 0; jRank < m_iNumberOfAtoms; ++jRank)
			if (iRank != jRank) {
				atom1Index = m_prgAtomToCenterRank[iRank];
				atom2Index = m_atomToAtomRank[atom1Index][jRank];
				otherSetAtom1Index = otherSet.m_prgAtomToCenterRank[iRank];
				otherSetAtom2Index = otherSet.m_atomToAtomRank[otherSetAtom1Index][jRank];

				diff = otherSet.m_atomDistances[atom1Index][atom2Index] -
				                m_atomDistances[otherSetAtom1Index][otherSetAtom2Index];
				if (diff < 0)
					diff = -diff;
				distance += diff;
				if (distance > minCompareDist)
					return false;
			}
	return true;
}


FLOAT MoleculeSet::getDistance(MoleculeSet &otherSet)
{
	int iRank, jRank;
	FLOAT diff;
	FLOAT distance = 0;
	int atom1Index, atom2Index;
	int otherSetAtom1Index, otherSetAtom2Index;
//	int totalComparisons = (signed int)((m_iNumberOfAtoms-1) * m_iNumberOfAtoms / 2);
	
	for (iRank = 0; iRank < m_iNumberOfAtoms; ++iRank)
		for (jRank = 0; jRank < m_iNumberOfAtoms; ++jRank)
			if (iRank != jRank)
			{
				atom1Index = m_prgAtomToCenterRank[iRank];
				atom2Index = m_atomToAtomRank[atom1Index][jRank];
				otherSetAtom1Index = otherSet.m_prgAtomToCenterRank[iRank];
				otherSetAtom2Index = otherSet.m_atomToAtomRank[otherSetAtom1Index][jRank];
				
				diff = otherSet.m_atomDistances[atom1Index][atom2Index] -
				                m_atomDistances[otherSetAtom1Index][otherSetAtom2Index];
				
				if (diff < 0)
					diff = -diff;
				distance += diff;
			}
//	distance /= totalComparisons;
	return distance;
}*/

void MoleculeSet::moveMoleculeSet(Point3D shiftValue)
{
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].moveMolecule(shiftValue);
}

void MoleculeSet::makeChild(MoleculeSet &parent1, MoleculeSet &parent2, int iNumStructureTypes,
                            int iNumStructuresOfEachType[], Point3D &boxDimensions)
{
	Point3D matingVector1, matingVector2;
	FLOAT angleX, angleY;
	Point3D center;
//	Point3D shift1, shift2; // centers of mass of the parents with each coordinate multiplied by -1
	Point3D diff;
	FLOAT *moleculeDistancesSquared1;
	FLOAT *moleculeDistancesSquared2;
	int *sortedMoleculeIndexes1;
	int *sortedMoleculeIndexes2;
	int i, j;
	FLOAT percentFromParent1; // the approximate percentage of molecules to take from the first parent
	int numFromParent1; // the number of molecules of a given type to take from the first parent
	int numFromParent2; // the number of molecules of a given type to take from the second parent
	int parentIndex1, parentIndex2, childIndex; // molecule indeces
	
	if (&parent1 == &parent2) {
		copy(parent1);
		return;
	}

	sortedMoleculeIndexes1 = new int[parent1.m_iNumberOfMolecules];
	sortedMoleculeIndexes2 = new int[parent2.m_iNumberOfMolecules];
	moleculeDistancesSquared1 = new FLOAT[parent1.m_iNumberOfMolecules];
	moleculeDistancesSquared2 = new FLOAT[parent2.m_iNumberOfMolecules];
	
//	int count = 0;
//	do {
//		++count;
//		if (count > 5000) {
//			cout << "count > 5000" << endl;
//			exit(0);
//		}
		angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
		angleY = Molecule::randomFloat(0,PIE_X_2);
		matingVector1 = getVectorInDirection(angleX, angleY, 50000);
		matingVector2.x = -matingVector1.x;
		matingVector2.y = -matingVector1.y;
		matingVector2.z = -matingVector1.z;
/*		matingVector1.x += parent1.m_centerOfMass.x;
		matingVector1.y += parent1.m_centerOfMass.y;
		matingVector1.z += parent1.m_centerOfMass.z;
		matingVector2.x += parent2.m_centerOfMass.x;
		matingVector2.y += parent2.m_centerOfMass.y;
		matingVector2.z += parent2.m_centerOfMass.z;*/
		matingVector1.x += boxDimensions.x/2;
		matingVector1.y += boxDimensions.y/2;
		matingVector1.z += boxDimensions.z/2;
		matingVector2.x += boxDimensions.x/2;
		matingVector2.y += boxDimensions.y/2;
		matingVector2.z += boxDimensions.z/2;
		
		for (i = 0; i < parent1.m_iNumberOfMolecules; ++i) {
			sortedMoleculeIndexes1[i] = i;
			sortedMoleculeIndexes2[i] = i;
			
			center = parent1.m_prgMolecules[i].getCenter();
			diff.x = center.x - matingVector1.x;
			diff.y = center.y - matingVector1.y;
			diff.z = center.z - matingVector1.z;
			moleculeDistancesSquared1[i] = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
			
			center = parent2.m_prgMolecules[i].getCenter();
			diff.x = center.x - matingVector2.x;
			diff.y = center.y - matingVector2.y;
			diff.z = center.z - matingVector2.z;
			moleculeDistancesSquared2[i] = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		}
		
		percentFromParent1 = Molecule::randomFloat(0.2,0.8);
//		percentFromParent1 = 1;
		setNumberOfMolecules(parent1.m_iNumberOfMolecules);
		parentIndex1 = 0;
		parentIndex2 = 0;
		childIndex = 0;
/*		shift1.x = -parent1.m_centerOfMass.x;
		shift1.y = -parent1.m_centerOfMass.y;
		shift1.z = -parent1.m_centerOfMass.z;
		shift2.x = -parent2.m_centerOfMass.x;
		shift2.y = -parent2.m_centerOfMass.y;
		shift2.z = -parent2.m_centerOfMass.z;
		shift1.x = -boxDimensions.x/2;
		shift1.y = -boxDimensions.y/2;
		shift1.z = -boxDimensions.z/2;
		shift2.x = -boxDimensions.x/2;
		shift2.y = -boxDimensions.y/2;
		shift2.z = -boxDimensions.z/2;*/
		for (i = 0; i < iNumStructureTypes; ++i) {
			sortMoleculeIndexes(sortedMoleculeIndexes1, moleculeDistancesSquared1,
			                    parentIndex1, parentIndex1 + iNumStructuresOfEachType[i] - 1);
			sortMoleculeIndexes(sortedMoleculeIndexes2, moleculeDistancesSquared2,
			                    parentIndex2, parentIndex2 + iNumStructuresOfEachType[i] - 1);
			
			numFromParent1 = (int)(percentFromParent1 * iNumStructuresOfEachType[i]);
			numFromParent2 = iNumStructuresOfEachType[i] - numFromParent1;
			if ((numFromParent1 == 0) || (numFromParent2 == 0)) {
				cout << "We have zero! ";
//				exit(0);
			} else {
			}
			for (j = 0; j < numFromParent1; ++j) {
				m_prgMolecules[childIndex].copy(parent1.m_prgMolecules[parentIndex1 + j]);
//				m_prgMolecules[childIndex].moveMolecule(shift1);
				++childIndex;
			}
			for (j = 0; j < numFromParent2; ++j) {
				m_prgMolecules[childIndex].copy(parent2.m_prgMolecules[parentIndex2 + j]);
//				m_prgMolecules[childIndex].moveMolecule(shift2);
				++childIndex;
			}
			parentIndex1 += iNumStructuresOfEachType[i];
			parentIndex2 += iNumStructuresOfEachType[i];
		}
		initAtomIndexes();
		initAtomDistances();
		sortAtomRanks();
//		cout << endl << "Child: " << endl;
//		printToScreen();
//	} while (!tryToEnforceBoxConstraints(boxDimensions));
	enforceMinDistConstraints(boxDimensions);
	
	delete[] sortedMoleculeIndexes1;
	delete[] sortedMoleculeIndexes2;
	delete[] moleculeDistancesSquared1;
	delete[] moleculeDistancesSquared2;
}

void MoleculeSet::makeChildAverage(MoleculeSet &parent1, MoleculeSet &parent2, int iNumStructureTypes,
                            int iNumStructuresOfEachType[], Point3D &boxDimensions)
{
	Point3D matingVector1, matingVector2;
	FLOAT angleX, angleY;
	Point3D center;
//	Point3D shift1, shift2; // centers of mass of the parents with each coordinate multiplied by -1
	Point3D diff;
	FLOAT *moleculeDistancesSquared1;
	FLOAT *moleculeDistancesSquared2;
	int *sortedMoleculeIndexes1;
	int *sortedMoleculeIndexes2;
	int i, j;
	FLOAT percentFromParent1; // the approximate percentage of molecules to take from the first parent
	int numFromParent1; // the number of molecules of a given type to take from the first parent
	int numFromParent2; // the number of molecules of a given type to take from the second parent
	int parentIndex1, parentIndex2, childIndex; // molecule indeces
	
	if (&parent1 == &parent2) {
		copy(parent1);
		return;
	}

	sortedMoleculeIndexes1 = new int[parent1.m_iNumberOfMolecules];
	sortedMoleculeIndexes2 = new int[parent2.m_iNumberOfMolecules];
	moleculeDistancesSquared1 = new FLOAT[parent1.m_iNumberOfMolecules];
	moleculeDistancesSquared2 = new FLOAT[parent2.m_iNumberOfMolecules];
	
//	int count = 0;
//	do {
//		++count;
//		if (count > 5000) {
//			cout << "count > 5000" << endl;
//			exit(0);
//		}
		angleX = Molecule::randomFloat(-PIE_OVER_2,PIE_OVER_2);
		angleY = Molecule::randomFloat(0,PIE_X_2);
		matingVector1 = getVectorInDirection(angleX, angleY, 50000);
		matingVector2.x = -matingVector1.x;
		matingVector2.y = -matingVector1.y;
		matingVector2.z = -matingVector1.z;
/*		matingVector1.x += parent1.m_centerOfMass.x;
		matingVector1.y += parent1.m_centerOfMass.y;
		matingVector1.z += parent1.m_centerOfMass.z;
		matingVector2.x += parent2.m_centerOfMass.x;
		matingVector2.y += parent2.m_centerOfMass.y;
		matingVector2.z += parent2.m_centerOfMass.z;*/
		matingVector1.x += boxDimensions.x/2;
		matingVector1.y += boxDimensions.y/2;
		matingVector1.z += boxDimensions.z/2;
		matingVector2.x += boxDimensions.x/2;
		matingVector2.y += boxDimensions.y/2;
		matingVector2.z += boxDimensions.z/2;
		
		for (i = 0; i < parent1.m_iNumberOfMolecules; ++i) {
			sortedMoleculeIndexes1[i] = i;
			sortedMoleculeIndexes2[i] = i;
			
			center = parent1.m_prgMolecules[i].getCenter();
			diff.x = center.x - matingVector1.x;
			diff.y = center.y - matingVector1.y;
			diff.z = center.z - matingVector1.z;
			moleculeDistancesSquared1[i] = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
			
			center = parent2.m_prgMolecules[i].getCenter();
			diff.x = center.x - matingVector2.x;
			diff.y = center.y - matingVector2.y;
			diff.z = center.z - matingVector2.z;
			moleculeDistancesSquared2[i] = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		}
		
		percentFromParent1 = Molecule::randomFloat(0.2,0.8);
//		percentFromParent1 = 1;
		setNumberOfMolecules(parent1.m_iNumberOfMolecules);
		parentIndex1 = 0;
		parentIndex2 = 0;
		childIndex = 0;
/*		shift1.x = -parent1.m_centerOfMass.x;
		shift1.y = -parent1.m_centerOfMass.y;
		shift1.z = -parent1.m_centerOfMass.z;
		shift2.x = -parent2.m_centerOfMass.x;
		shift2.y = -parent2.m_centerOfMass.y;
		shift2.z = -parent2.m_centerOfMass.z;
		shift1.x = -boxDimensions.x/2;
		shift1.y = -boxDimensions.y/2;
		shift1.z = -boxDimensions.z/2;
		shift2.x = -boxDimensions.x/2;
		shift2.y = -boxDimensions.y/2;
		shift2.z = -boxDimensions.z/2;*/
		for (i = 0; i < iNumStructureTypes; ++i) {
			sortMoleculeIndexes(sortedMoleculeIndexes1, moleculeDistancesSquared1,
			                    parentIndex1, parentIndex1 + iNumStructuresOfEachType[i] - 1);
			sortMoleculeIndexes(sortedMoleculeIndexes2, moleculeDistancesSquared2,
			                    parentIndex2, parentIndex2 + iNumStructuresOfEachType[i] - 1);
			
			numFromParent1 = (int)(percentFromParent1 * iNumStructuresOfEachType[i]);
			numFromParent2 = iNumStructuresOfEachType[i] - numFromParent1;
			if ((numFromParent1 == 0) || (numFromParent2 == 0)) {
				cout << "We have zero! ";
//				exit(0);
			} else {
			}
			for (j = 0; j < numFromParent1; ++j) {
				m_prgMolecules[childIndex].copy(parent1.m_prgMolecules[parentIndex1 + j]);
//				m_prgMolecules[childIndex].moveMolecule(shift1);
				++childIndex;
			}
			for (j = 0; j < numFromParent2; ++j) {
				m_prgMolecules[childIndex].copy(parent2.m_prgMolecules[parentIndex2 + j]);
//				m_prgMolecules[childIndex].moveMolecule(shift2);
				++childIndex;
			}
			parentIndex1 += iNumStructuresOfEachType[i];
			parentIndex2 += iNumStructuresOfEachType[i];
		}
		initAtomIndexes();
		initAtomDistances();
		sortAtomRanks();
//		cout << endl << "Child: " << endl;
//		printToScreen();
//	} while (!tryToEnforceBoxConstraints(boxDimensions));
	enforceMinDistConstraints(boxDimensions);
	
	delete[] sortedMoleculeIndexes1;
	delete[] sortedMoleculeIndexes2;
	delete[] moleculeDistancesSquared1;
	delete[] moleculeDistancesSquared2;
}

void MoleculeSet::sortMoleculeIndexes(int moleculeIndexes[], FLOAT distanceArray[], int lo, int hi)
{
	int left, right;
	FLOAT fMedian;
	int temp;
//	cout << "Entering lo: " << lo << ", hi: " << hi << endl;

	if( hi > lo ) // if at least 2 elements, then
	{
		left=lo; right=hi;
		
		// We don't know what the median is, so we'll just pick something
		fMedian = distanceArray[moleculeIndexes[lo]];
		
		while(right >= left)
		{
			while (distanceArray[moleculeIndexes[left]] < fMedian) {
				left++;
//				cout << "left++" << endl;
			}
			while (distanceArray[moleculeIndexes[right]] > fMedian) {
				right--;
//				cout << "right--" << endl;
			}
			
			if(left > right)
				break;
			
			// Swap
			temp=moleculeIndexes[left];
			moleculeIndexes[left]=moleculeIndexes[right];
			moleculeIndexes[right]=temp;
			
			left++;
			right--;
		}
//		if ((right < lo) || (right > hi) ||
//		    (left < lo) || (left > hi)) {
//			cout << "Problem= right: " << right << ", left: " << left << ", lo: " << lo << ", hi: " << hi << endl;
//			exit(0);
//		}
		
		sortMoleculeIndexes(moleculeIndexes, distanceArray, lo, right);
		sortMoleculeIndexes(moleculeIndexes, distanceArray, left, hi);
	}
}

	
bool MoleculeSet::haveSameAtomsAndMolecules(MoleculeSet &otherMoleculeSet)
{
	if (m_iNumberOfMolecules != otherMoleculeSet.m_iNumberOfMolecules)
		return false;
	for (int i = 0; i < m_iNumberOfMolecules; ++i)
		if (!m_prgMolecules[i].haveSameAtoms(otherMoleculeSet.m_prgMolecules[i]))
			return false;
	return true;
}

/////////// Functions Used for Testing ///////////////////////////////////////////////////////////////////////////////////

bool MoleculeSet::testConstraints(Point3D &boxDimensions)
{
	Point3D minCoordinates, maxCoordinates;
	const FLOAT SOME_BIG_NUMBER = 1000000;
	int i, j;
	
	// Check to see if atoms are too close to one another
	for (i = 0; i < m_iNumberOfAtoms-1; ++i)
		for (j = i+1; j < m_iNumberOfAtoms; ++j)
			if (m_atomDistances[i][j] < m_atoms[i]->getMinDistance(*m_atoms[j]))
				return false;
	
	// Test the constraint that all atoms must be within the boxDimensions
	minCoordinates.x = SOME_BIG_NUMBER;
	minCoordinates.y = SOME_BIG_NUMBER;
	minCoordinates.z = SOME_BIG_NUMBER;
	maxCoordinates.x = -SOME_BIG_NUMBER;
	maxCoordinates.y = -SOME_BIG_NUMBER;
	maxCoordinates.z = -SOME_BIG_NUMBER;
	for (i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].getMinimumAndMaximumCoordinates(minCoordinates, maxCoordinates);

	if ((minCoordinates.x < 0) || (maxCoordinates.x > boxDimensions.x) ||
	    (minCoordinates.y < 0) || (maxCoordinates.y > boxDimensions.y) ||
	    (minCoordinates.z < 0) || (maxCoordinates.z > boxDimensions.z))
	{
//		cout << "Atom set not within the box, test failed!" << endl;
		return false;
	}

	return true;
}

void MoleculeSet::printRankInfo()
{
	int i, j;

	cout << setiosflags(ios::fixed) << setprecision(6);
	cout << "Structure:" << endl;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		cout << "Index: " << i
		     << " Atomic #: " << m_atoms[i]->m_iAtomicNumber << " ("
		     << m_atoms[i]->m_globalPoint.x << ", "
		     << m_atoms[i]->m_globalPoint.y << ", "
		     << m_atoms[i]->m_globalPoint.z << ")" << endl;
	
	cout << "Center of Mass: " << m_centerOfMass.x << ", " << m_centerOfMass.y << ", " << m_centerOfMass.z << endl << endl;
	
	cout << "Distances to the center of mass: " << endl;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		cout << "  " << m_prgAtomDistancesToCenterOfMass[i];
	cout << endl;
	
	cout << "Atom to Center Rank: " << endl;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		cout << "  " << m_prgAtomToCenterRank[i];
	cout << endl << endl;
	
	cout << "Distance Matrix: " << endl;
	
	for (i = 0; i < m_iNumberOfAtoms; ++i)
	{
		for (j = 0; j < m_iNumberOfAtoms; ++j)
			cout << "  " << m_atomDistances[i][j];
		cout << endl;
	}
	cout << endl;
}

FLOAT MoleculeSet::myPower(FLOAT number, int power)
{
	FLOAT answer = 1;
	for (int i = 1; i <= power; ++i)
		answer *= number;
	return answer;
}

FLOAT MoleculeSet::computeLennardJonesEnergy(FLOAT epsilon, FLOAT sigma)
{
	FLOAT energy;
	FLOAT sigmaOverDist;
	int i,j;
	
	energy = 0;
	for (i = 0; i < m_iNumberOfAtoms-1; ++i)
		for (j = i+1; j < m_iNumberOfAtoms; ++j) {
			sigmaOverDist = sigma / m_atomDistances[i][j];
			energy += (myPower(sigmaOverDist,12) - myPower(sigmaOverDist,6));
		}
	energy *= 4 * epsilon;
	m_fEnergy = energy;
	return energy;
}

FLOAT MoleculeSet::computeLennardJonesEnergy(FLOAT epsilon, FLOAT sigma, FLOAT alpha, Point3D s[])
{
	FLOAT energy;
	FLOAT sigmaToTheSixth, sigmaToTheTwelveth;
	Point3D diff;
	FLOAT distSquared;
	int i,j;
	FLOAT distToTheSixth;
	
	sigmaToTheSixth = myPower(sigma, 6);
	sigmaToTheTwelveth = sigmaToTheSixth * sigmaToTheSixth;
	
	energy = 0;
	for (i = 0; i < m_iNumberOfAtoms-1; ++i)
		for (j = i+1; j < m_iNumberOfAtoms; ++j) {
			diff.x = (m_atoms[i]->m_globalPoint.x + alpha * s[i].x) -
			         (m_atoms[j]->m_globalPoint.x + alpha * s[j].x);
			diff.y = (m_atoms[i]->m_globalPoint.y + alpha * s[i].y) -
			         (m_atoms[j]->m_globalPoint.y + alpha * s[j].y);
			diff.z = (m_atoms[i]->m_globalPoint.z + alpha * s[i].z) -
			         (m_atoms[j]->m_globalPoint.z + alpha * s[j].z);
			distSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
			distToTheSixth = myPower(distSquared,3);
			energy += (sigmaToTheTwelveth / (distToTheSixth * distToTheSixth)) - (sigmaToTheSixth / distToTheSixth);
//			sigmaOverDist = sigma / distance;
//			energy += (myPower(sigmaOverDist,12) - myPower(sigmaOverDist,6));
		}
	energy *= 4 * epsilon;
	return energy;
}

void MoleculeSet::computeLennardJonesGradient(FLOAT epsilon, FLOAT sigma, Point3D gradient[])
{
	int i, j;
	FLOAT sigmaToTheSixth;
	sigmaToTheSixth = myPower(sigma,6);
	
	for (i = 0; i < m_iNumberOfAtoms; ++i) {
		gradient[i].x = 0;
		gradient[i].y = 0;
		gradient[i].z = 0;
		for (j = 0; j < m_iNumberOfAtoms; ++j)
			if (i != j) {
				gradient[i].x += ((m_atoms[i]->m_globalPoint.x - m_atoms[j]->m_globalPoint.x) /
				                  myPower(m_atomDistances[i][j],8)) -
				                 (2 * sigmaToTheSixth *
				                  (m_atoms[i]->m_globalPoint.x - m_atoms[j]->m_globalPoint.x) /
				                  myPower(m_atomDistances[i][j],14));
				gradient[i].y += ((m_atoms[i]->m_globalPoint.y - m_atoms[j]->m_globalPoint.y) /
				                  myPower(m_atomDistances[i][j],8)) -
				                 (2 * sigmaToTheSixth *
				                  (m_atoms[i]->m_globalPoint.y - m_atoms[j]->m_globalPoint.y) /
				                  myPower(m_atomDistances[i][j],14));
				gradient[i].z += ((m_atoms[i]->m_globalPoint.z - m_atoms[j]->m_globalPoint.z) /
				                  myPower(m_atomDistances[i][j],8)) -
				                 (2 * sigmaToTheSixth *
				                  (m_atoms[i]->m_globalPoint.z - m_atoms[j]->m_globalPoint.z) /
				                  myPower(m_atomDistances[i][j],14));
			}
		gradient[i].x *= 24 * epsilon * sigmaToTheSixth;
		gradient[i].y *= 24 * epsilon * sigmaToTheSixth;
		gradient[i].z *= 24 * epsilon * sigmaToTheSixth;
	}
//	cout << "A gradient = " << gradient[0].x << ", " << gradient[0].y << ", " << gradient[0].z << ", " << endl;
}

void MoleculeSet::performLennardJonesOptimization(FLOAT epsilon, FLOAT sigma)
{
	Point3D *g = new Point3D[m_iNumberOfAtoms];
	Point3D *s = new Point3D[m_iNumberOfAtoms];
	FLOAT alpha, beta;
	FLOAT gTransposeTimesG, gTransposeTimesGOld;
	int iAtom;
	FLOAT bestEnergy = 1000;
	
	computeLennardJonesGradient(epsilon, sigma, g);
	
	gTransposeTimesG = 0;
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom) {
		s[iAtom].x = -g[iAtom].x;
		s[iAtom].y = -g[iAtom].y;
		s[iAtom].z = -g[iAtom].z;
		gTransposeTimesG += g[iAtom].x * g[iAtom].x +
		                    g[iAtom].y * g[iAtom].y +
		                    g[iAtom].z * g[iAtom].z;
	}
	// Old values: 200, 0.i00001
	for (int i = 1; i <= 20; ++i) {
		alpha = minimizeLennardJonesAlpha(epsilon, sigma, s, 0, 0.0001);
//		cout << setiosflags(ios::fixed) << setprecision(8) << "alpha = " << alpha << endl;
		for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom) {
			m_atoms[iAtom]->m_globalPoint.x += alpha * s[iAtom].x;
			m_atoms[iAtom]->m_globalPoint.y += alpha * s[iAtom].y;
			m_atoms[iAtom]->m_globalPoint.z += alpha * s[iAtom].z;
		}
		initAtomDistances();
		computeLennardJonesGradient(epsilon, sigma, g);
		gTransposeTimesGOld = gTransposeTimesG;
		gTransposeTimesG = 0;
		for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
			gTransposeTimesG += g[iAtom].x * g[iAtom].x +
			                    g[iAtom].y * g[iAtom].y +
			                    g[iAtom].z * g[iAtom].z;
		beta = gTransposeTimesG / gTransposeTimesGOld;
//		cout << setiosflags(ios::fixed) << setprecision(8) << "beta = " << beta << endl;
		if (i % m_iNumberOfAtoms == 0) {
			for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom) {
				s[iAtom].x = -g[iAtom].x;
				s[iAtom].y = -g[iAtom].y;
				s[iAtom].z = -g[iAtom].z;
			}
		} else {
			for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom) {
				s[iAtom].x = -g[iAtom].x + beta * s[iAtom].x;
				s[iAtom].y = -g[iAtom].y + beta * s[iAtom].y;
				s[iAtom].z = -g[iAtom].z + beta * s[iAtom].z;
			}
		}
		m_fEnergy = computeLennardJonesEnergy(epsilon, sigma);
		if (bestEnergy > m_fEnergy)
			bestEnergy = m_fEnergy;
	}
	
	for (int iMolecule = 0; iMolecule < m_iNumberOfMolecules; ++iMolecule)
		m_prgMolecules[iMolecule].globalToLocal();
	
	sortAtomRanks();
	
//	cout << setiosflags(ios::fixed) << setprecision(8) << "bestEnergy = " << bestEnergy << endl;
	
	delete[] g;
	delete[] s;
}

FLOAT MoleculeSet::minimizeLennardJonesAlpha(FLOAT epsilon, FLOAT sigma, Point3D s[], FLOAT a, FLOAT b)
{
	const FLOAT tolerance = 0.0000001;
	FLOAT r;
	FLOAT x1, x2;
	FLOAT f1, f2;
	
	r = (sqrt(5.0) - 1.0) / 2.0;
	x1 = a + (1 - r) * (b - a);
	f1 = computeLennardJonesEnergy(epsilon, sigma, x1, s);
	x2 = a + r * (b - a);
	f2 = computeLennardJonesEnergy(epsilon, sigma, x2, s);
	while ((b - a) > tolerance) {
		if (f1 > f2) {
			a = x1;
			x1 = x2;
			f1 = f2;
			x2 = a + r * (b - a);
			f2 = computeLennardJonesEnergy(epsilon, sigma, x2, s);
		} else {
			b = x2;
			x2 = x1;
			f2 = f1;
			x1 = a + (1 - r) * (b - a);
			f1 = computeLennardJonesEnergy(epsilon, sigma, x1, s);
		}
	}
	return (b + a) / 2;
}

void MoleculeSet::assignReadCoordinates(const Point3D cartesianPoints[], const int atomicNumbers[])
{
	int i;
	for (i = 0; i < m_iNumberOfAtoms; ++i) {
		if (m_atoms[i]->m_iAtomicNumber != atomicNumbers[i]) {
			cout << "Unexpected atomic number in .log file.  Are these atoms in the right order?" << endl;
			return;
		}
		m_atoms[i]->m_globalPoint = cartesianPoints[i];
	}
	
	for (i = 0; i < m_iNumberOfMolecules; ++i)
		m_prgMolecules[i].globalToLocal();
	
	initAtomDistances();
	sortAtomRanks();
}

void MoleculeSet::measureSearchSpace(int &withConstraints, int &withoutConstraints)
{
	int atomicNumber = m_atoms[0]->m_iAtomicNumber;
	int i;
	withConstraints = 0;
	withoutConstraints = 0;
	for (i = 0; i < m_iNumberOfMolecules; ++i)
		if (m_prgMolecules[i].getNumberOfAtoms() > 1) {
			cout << "The function measureSearchSpace only works with one atom per unit/molecule." << endl;
			return;
		}
	for (i = 0; i < m_iNumberOfAtoms; ++i) {
		if (m_atoms[i]->m_iAtomicNumber != atomicNumber) {
			cout << "The function measureSearchSpace only works when all atoms are identical" << endl;
			return;
		}
	}
}

bool MoleculeSet::performBondRotations(FLOAT angleInRad, vector<MoleculeSet*> &moleculeSets)
{
	MoleculeSet *mySet;
	int combos = 0;
	int combosSatisfyingConstraints = 0;
	
	if (m_iNumberOfMolecules != 1) {
		cout << "There must be only one molecule/unit in the input file." << endl;
		return false;
	}
	
	if (!m_prgMolecules[0].findBonds())
		return false;
	if (!m_prgMolecules[0].checkConnectivity())
		return false;
	if (m_prgMolecules[0].hasRings()) {
		cout << "This program can not perform bond rotations on molecules with rings." << endl;
		return false;
	}
	m_prgMolecules[0].setBondRotationalAngle(angleInRad);
	if (m_prgMolecules[0].getNumberOfRotatableBonds() < 1) {
		cout << "This molecule does not appear to contain rotatable bonds." << endl;
		m_prgMolecules[0].printBondInfo();
		cout << "Check the bond length criteria in the file: bondLengths.txt" << endl;
		return false;
	}
	m_prgMolecules[0].initRotationMatrix();
	initAtomIndexes();
	do {
		++combos;
		m_prgMolecules[0].localToGlobal();
		initAtomDistances();
		sortAtomRanks();
		if (checkMinDistConstraints()) {
			++combosSatisfyingConstraints;
			mySet = new MoleculeSet();
			mySet->copy(*this);
			moleculeSets.push_back(mySet);
		}
	} while (m_prgMolecules[0].incrumentBondPositions());
	cout << "Possible combinations: " << combos << endl;
	cout << "Combinations satisfying minimum distance constraints: " << combosSatisfyingConstraints << endl; 
	for (int i = 0; i < (signed int)moleculeSets.size(); ++i)
		moleculeSets[i]->m_prgMolecules[0].globalToLocal();
	return true;
}

bool MoleculeSet::moveOrCopyInputEnergyFile(const char* newDirectory, bool moveOrCopy)
{
	char fileName[500];
	char commandLine[500];
	bool success = true;

	if (m_inputEnergyFile.m_sFullPathName.length() == 0)
		return false;
	m_inputEnergyFile.m_sDirectory = newDirectory;

	if (m_inputEnergyFile.m_sDirectory.length() > 0)
		snprintf(fileName, sizeof(fileName), "%s/%s%d.%s", newDirectory, m_inputEnergyFile.m_sPrefix.c_str(), m_inputEnergyFile.m_iNumber, m_inputEnergyFile.m_sExtension.c_str());
	else
		snprintf(fileName, sizeof(fileName), "%s%d.%s", m_inputEnergyFile.m_sPrefix.c_str(), m_inputEnergyFile.m_iNumber, m_inputEnergyFile.m_sExtension.c_str());
	if (moveOrCopy)
		snprintf(commandLine, sizeof(commandLine), "mv %s %s", m_inputEnergyFile.m_sFullPathName.c_str(), fileName);
	else
		snprintf(commandLine, sizeof(commandLine), "cp %s %s", m_inputEnergyFile.m_sFullPathName.c_str(), fileName);
	if (system(commandLine) == -1) {
		success = false;
		m_inputEnergyFile.m_sFullPathName = "";
	} else {
		m_inputEnergyFile.m_sFullPathName = fileName;
	}
	return success;
}

bool MoleculeSet::moveOrCopyOutputEnergyFile(unsigned int index, const char* newFileName, bool moveOrCopy)
{
	char commandLine[500];
	if (moveOrCopy)
		snprintf(commandLine, sizeof(commandLine), "mv %s %s", m_outputEnergyFiles[index]->m_sFullPathName.c_str(), newFileName);
	else
		snprintf(commandLine, sizeof(commandLine), "cp %s %s", m_outputEnergyFiles[index]->m_sFullPathName.c_str(), newFileName);
	if (system(commandLine) == -1) {
		delete m_outputEnergyFiles[index];
		m_outputEnergyFiles[index] = NULL;
		return false;
	} else {
		m_outputEnergyFiles[index]->m_sFullPathName = newFileName;
		return true;
	}
}

bool MoleculeSet::moveOrCopyOutputEnergyFiles(const char* newDirectory, const char* newPrefix, int newNumber, bool moveOrCopy)
{
	char fileName[500];
	bool success = true;

	for (unsigned int i = 0; i < m_outputEnergyFiles.size(); ++i) {
		if (m_outputEnergyFiles[i] == NULL)
			continue;
		m_outputEnergyFiles[i]->m_sDirectory = newDirectory;
		m_outputEnergyFiles[i]->m_sPrefix = newPrefix;
		m_outputEnergyFiles[i]->m_iNumber = newNumber;
		
		if (m_outputEnergyFiles[i]->m_sDirectory.length() > 0)
			snprintf(fileName, sizeof(fileName), "%s/%s%d.%s", newDirectory, newPrefix, newNumber, m_outputEnergyFiles[i]->m_sExtension.c_str());
		else
			snprintf(fileName, sizeof(fileName), "%s%d.%s", newPrefix, newNumber, m_outputEnergyFiles[i]->m_sExtension.c_str());
		moveOrCopyOutputEnergyFile(i, fileName, moveOrCopy);
	}
	return success;
}

bool MoleculeSet::moveOrCopyOutputEnergyFiles(const char* newDirectory, bool moveOrCopy)
{
	char fileName[500];
	bool success = true;

	for (unsigned int i = 0; i < m_outputEnergyFiles.size(); ++i) {
		if (m_outputEnergyFiles[i] == NULL)
			continue;
		m_outputEnergyFiles[i]->m_sDirectory = newDirectory;
		
		if (m_outputEnergyFiles[i]->m_sDirectory.length() > 0)
			snprintf(fileName, sizeof(fileName), "%s/%s%d.%s", newDirectory, m_outputEnergyFiles[i]->m_sPrefix.c_str(),
			         m_outputEnergyFiles[i]->m_iNumber, m_outputEnergyFiles[i]->m_sExtension.c_str());
		else
			snprintf(fileName, sizeof(fileName), "%s%d.%s", m_outputEnergyFiles[i]->m_sPrefix.c_str(),
			         m_outputEnergyFiles[i]->m_iNumber, m_outputEnergyFiles[i]->m_sExtension.c_str());
		moveOrCopyOutputEnergyFile(i, fileName, moveOrCopy);
	}
	return success;
}

bool MoleculeSet::moveOrCopyOutputEnergyFiles(int newNumber, bool moveOrCopy)
{
	char fileName[500];
	bool success = true;

	for (unsigned int i = 0; i < m_outputEnergyFiles.size(); ++i) {
		if (m_outputEnergyFiles[i] == NULL)
			continue;
		m_outputEnergyFiles[i]->m_iNumber = newNumber;
		
		if (m_outputEnergyFiles[i]->m_sDirectory.length() > 0)
			snprintf(fileName, sizeof(fileName), "%s/%s%d.%s", m_outputEnergyFiles[i]->m_sDirectory.c_str(),
			         m_outputEnergyFiles[i]->m_sPrefix.c_str(), newNumber, m_outputEnergyFiles[i]->m_sExtension.c_str());
		else
			snprintf(fileName, sizeof(fileName), "%s%d.%s",
			         m_outputEnergyFiles[i]->m_sPrefix.c_str(), newNumber, m_outputEnergyFiles[i]->m_sExtension.c_str());
		moveOrCopyOutputEnergyFile(i, fileName, moveOrCopy);
	}
	return success;
}

bool MoleculeSet::deleteOutputEnergyFiles(bool forgetAboutFiles)
{
	char commandLine[500];
	bool success = true;

	for (unsigned int i = 0; i < m_outputEnergyFiles.size(); ++i) {
		if (m_outputEnergyFiles[i] == NULL)
			continue;
		if (fileExists(m_outputEnergyFiles[i]->m_sFullPathName.c_str())) {
			snprintf(commandLine, sizeof(commandLine), "rm %s", m_outputEnergyFiles[i]->m_sFullPathName.c_str());
			if (system(commandLine) == -1)
				success = false;
		}
		if (forgetAboutFiles)
			m_outputEnergyFiles[i] = NULL;
	}
	if (forgetAboutFiles)
		m_outputEnergyFiles.clear();
	
	return success;
}

bool MoleculeSet::fileExists(const char* fileName)
{
	struct stat fileStatistics;
	return stat(fileName, &fileStatistics) == 0; // If no errors occurred in getting stats, the file exists
}
