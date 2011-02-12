////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing a molecule or a set of adams that
//    belong in a group.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 3).
//    Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MOLECULESET_H__
#define __MOLECULESET_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include "typedef.h"
#include "molecule.h"
#include "energyProgram.h"

using namespace std;

#define IS_RUNNING 1
#define NEEDS_RUN  2
#define IS_FINISHED 3

class EnergyFileInfo 
{
public:
	string m_sDirectory;
	string m_sPrefix;
	int m_iNumber;
	string m_sExtension;
	string m_sFullPathName;
	
	EnergyFileInfo() {}
	void copy(const EnergyFileInfo &otherEnergyFileInfo);
};

// This class manages a set of molecules
class MoleculeSet
{
private:
	int m_iStructureId; // an id number for this object
	EnergyFileInfo m_inputEnergyFile;
	vector<EnergyFileInfo*> m_outputEnergyFiles;
	int m_iNumberOfMolecules;
	Molecule* m_prgMolecules;  // array of molecules
	int m_iNumberOfAtoms;
	Atom** m_atoms; // array of pointers to the atoms
	
	FLOAT** m_atomDistances; // 2D array of distances
	Point3D m_centerOfMass;
	FLOAT m_fTotalMass;
	FLOAT* m_prgAtomDistancesToCenterOfMass; // 1D array of distances
        // Ranks in the next two arrays are evaluated first by atomic # and second by distance
	int* m_prgAtomToCenterRank; // Indexes to this array are ranks
				                // Values from this array are indexes to m_prgAtoms
	FLOAT m_fEnergy;
	int m_iRun;
	bool m_bIsTransitionState;
	
public:
	MoleculeSet();
	MoleculeSet(int id);
	void init(int id);
	~MoleculeSet();
	void setId(int id) { m_iStructureId = id; }
	int getId() { return m_iStructureId; }
	void setInputEnergyFile(const char *directory, const char* prefix, int number, const char* extension);
	const char* getInputEnergyFile();
	void setOutputEnergyFile(const char *directory, const char* prefix, int number, const char* extension, unsigned int index, bool checkExistence);
	const char* getOutputEnergyFile(unsigned int index);
	void copy(const MoleculeSet &moleculeSet);
	int getNumberOfMolecules();
	int getNumberOfMoleculesWithMultipleAtoms();
	void setNumberOfMolecules(int iNumberOfMolecules);
	Molecule* getMolecules();
	int getNumberOfAtoms();

	
	// This function initializes m_iNumberOfAtoms and m_prgAtoms
	void initAtomIndexes();
	void initAtomIndexes(int iChangedMolecule);
	FLOAT getEnergy() { return m_fEnergy; }
	void setEnergy(FLOAT energy)  { m_fEnergy = energy; }
	void initVelocities(FLOAT coordinateVelocity, FLOAT maxAngleVelocityInRad);
	bool initPositionsAndAngles(Point3D &boxDimensions, int numTries);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function initializes the positions and angles of molecules
	//    and ensures that each atom is not further than maxAtomDist.
	// Parameters: boxDimensions - the maximum x, y, and z values of coordinates
	//             maxAtomDist - the maximum distance between any two atoms
	//             numTries - maximum number of tries before the function quits
	// Returns: true if the function succeeded
	bool initPositionsAndAnglesWithMaxDist(Point3D &boxDimensions, FLOAT maxAtomDist, int numTries);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function initializes positions and angles of molecules
	//    and ensures that each atom and molecule is not further than maxAtomDist.
	// Parameters: boxDimensions - the maximum x, y, and z values of coordinates
	//             maxAtomDist - the maximum distance between any two atoms
	//             numTries - maximum number of tries before the function quits
	// Returns: true if the function succeeded
	bool initNonFragmentedSructure(Point3D &boxDimensions, FLOAT maxAtomDist, int numTries);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function initializes the current moleculeSet from another
	//    molecule set (the seed).  If there are more of a particular type of molecule that
	//    need to be created in the curent moleculeSet, this function creates
	//    these molecules and assigns them random positions producing a new
	//    completely nonfragmented moleculeSet.
	//    and ensures that each atom and molecule is not further than maxAtomDist.
	// Parameters: seedMoleculeSet - the seed
	//             iNumStructureTypes - the number of different types of molecules
	//             iNumStructuresOfEachTypeOriginal - the number of each type of molecule in the seed
	//             iNumStructuresOfEachTypeNew - the number of each type of molecule in the new molecuelSet
	//             cartesianPointsOriginal - the coordinates for atoms in each molecule
	//             atomicNumbersOriginal - the atomic numbers for atoms in each molecule
	//             boxDimensions - the maximum x, y, and z values of coordinates
	//             maxAtomDist - the maximum distance between any two atoms
	//             numTries - maximum number of tries before the function quits
	// Returns: true if the function succeeded, false otherwise
	bool initFromSeed(MoleculeSet &seedMoleculeSet, int iNumStructureTypes,
                          int* iNumStructuresOfEachTypeOriginal, int* iNumStructuresOfEachTypeNew, vector<Point3D> *cartesianPoints,
                          vector<int> *atomicNumbers, Point3D &boxDimensions, FLOAT maxAtomDist, int numTries);
	
	void unFreezeAll(FLOAT coordinateVelocity, FLOAT maxAngleVelocityInRad);
	
	int getNumberOfMoleculesFrozen();
	
	bool performTransformations(Point3D &boxDimensions, FLOAT deltaForCoordinates, FLOAT deltaForAnglesInRad, int numTransformations);
	
	bool performTransformationsNonFrag(Point3D &boxDimensions, FLOAT deltaForCoordinates, FLOAT deltaForAnglesInRad, FLOAT maxDist, int iNumTransitions);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function performs particle swarm optimization on
	//    this moleculeSet.
	// Parameters: populationBestSet - the moleculeSet that is the best in the population
	//             individualBestSet - the moleculeSet that is the best this individual has seen
	//             coordInertia - the amount to multiply coordinate velocities by each time
	//             coordIndividualMinimumAttraction - the amount coordinates are attracted to individualBestSet
	//             coordPopulationMinimumAttraction - the amount coordinates are attracted to populationBestSet
	//             angleInertia - the amount to multiply angle velocities by each time
	//             angleIndividualMinimumAttraction - the amount angles are attracted to individualBestSet
	//             anglePopulationMinimumAttraction - the amount angles are attracted to populationBestSet
	//             boxDimensions - the maximum values of the box
	//             maxCoordinateVelocity - a return value that is assigned the maximum coordinate velocity of any atom
	//             totCoordinateVelocity - a return value that is assigned the total coordinate velocity of all atoms
	//             maxAngularVelocity - a return value that is assigned the maximum angle velocity of any atom
	//             totAngularVelocity - a return value that is assigned the total angle velocity of all atoms
	//             enforceMinDistances - if true, this fuction enforces the minimum distance constraints
	//             attractionRepulsion - should be +1 for attraction and -1 for repulsion
	// Returns: nothing
	void performPSO(MoleculeSet &populationBestSet, MoleculeSet &individualBestSet, FLOAT coordInertia,
	                FLOAT coordIndividualMinimumAttraction, FLOAT coordPopulationMinimumAttraction,
	                FLOAT coordMaxVelocity, FLOAT angleInertia, FLOAT angleIndividualMinimumAttraction,
	                FLOAT anglePopulationMinimumAttraction, FLOAT angleMaxVelocity, Point3D &boxDimensions,
	                bool enforceMinDistances, FLOAT fAttractionRepulsion);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function performs particle swarm optimization on
	//    this moleculeSet.
	// Parameters: maxCoordinateVelocity - a return value that is assigned the maximum coordinate velocity of any atom
	//             totCoordinateVelocity - a return value that is assigned the total coordinate velocity of all atoms
	//             maxAngularVelocity - a return value that is assigned the maximum angle velocity of any atom
	//             totAngularVelocity - a return value that is assigned the total angle velocity of all atoms
	// Note: All the parameters should be initialized before they are passed to this function.
	// Returns: nothing
	void getPSOVelocityStats(FLOAT &maxCoordinateVelocity, FLOAT &totCoordinateVelocity, FLOAT &maxAngularVelocity,
                             FLOAT &totAngularVelocity);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function enforces the minimum distance constraints
	//    on the given molecule set.  It also ensures that after these
	//    constraints are enforced that no atoms are outside the box.
	// Parameters: boxDimensions - the maximum values of the box
	// Returns: nothing
	void enforceMinDistConstraints(Point3D &boxDimensions);
	
	FLOAT getDistanceFromPoint(Point3D &point);
	
	void writeToGausianComFile(ofstream &fout);
	void writeToGausianLogFile(FILE* fout);

	void writeToOBMol(OpenBabel::OBMol &obMol);

	void print(ofstream &fout);
	void printToResumeFile(ofstream &fout, bool printVelocityInfo);

	// This function reads atom coordinates, center of mass, and angles from a file
	// Note: this function assumes that the number of molecules is already set to the correct number.
	bool readFromResumeFile(ifstream &infile, char* fileLine, int MAX_LINE_LENGTH, bool readVelocityInfo);
	void printToScreen();
	
	// Note: minCompareDist is the minimum distance two MoleculeSets have to be different in the sum of
	// their atom distances before they are considered different MoleculeSets.
	// This function assumes that distances to other atoms have been updated before this function is called.
	bool withinDistance(MoleculeSet &otherSet, FLOAT minCompareDist);
	
	FLOAT getDistanceOld(MoleculeSet &otherSet);
	FLOAT getDistance(MoleculeSet &otherSet);
	
	bool isRunning() { return m_iRun == IS_RUNNING; }
	bool needsRun() { return m_iRun == NEEDS_RUN; }
	bool isFinished() { return m_iRun == IS_FINISHED; }
	void setRunning() { m_iRun = IS_RUNNING; }
	void setNeedsRun() { m_iRun = NEEDS_RUN; }
	void setFinished() { m_iRun = IS_FINISHED; }
	bool getIsTransitionState() { return m_bIsTransitionState; }
	void setIsTransitionState(bool trans)  { m_bIsTransitionState = trans; }
	
	void printRankInfo();
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checks to make sure molecules are not further
	//    than the distance specified from at least one other molecule.
	// Parameters: maxDist - the maximum distance allowed
	// Returns: true if atoms are not further than maxDist
	bool checkMaxDistConstraints(FLOAT maxDist);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function shifts the positions of all the atoms in the
	//    moleculeSet by the given amounts.
	// Parameters: shiftValue - the values to shift the points.
	// Returns: nothing
	void moveMoleculeSet(Point3D shiftValue);

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function computes the Lennard Jones Potential.
	// Parameters: epsilon and sigma - necessary for the calculation
	// Returns: the energy value
	FLOAT computeLennardJonesEnergy(FLOAT epsilon, FLOAT sigma);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function computes the gradient for the Lennard Jones Potential
	// Parameters: gradient - an array where the gradient for each atom is stored
	//             epsilon and sigma - necessary for the calculation
	// Returns: nothing
	void computeLennardJonesGradient(FLOAT epsilon, FLOAT sigma, Point3D gradient[]);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function optimizes the current structure to the nearest
	//    local minimum using the conjugate gradient method.
	// Parameters: epsilon and sigma - necessary for the calculation
	// Returns: nothing
	void performLennardJonesOptimization(FLOAT epsilon, FLOAT sigma);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function assigns coordinates to a molecule set after
	//    they have been read in from a .log or other file.
	// Parameters: cartesianPoints - the coordinates of each atom
	//             atomicNumbers - the atomic numbers of each atom
	// Returns: nothing
	void assignReadCoordinates(const Point3D cartesianPoints[], const int atomicNumbers[]);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function mates two parents and produces a child.
	//    The child is stored in the current moleculeSet.
	// Parameters: parent1 and parent2 - the parents
	//             iNumStructureTypes - the number of different molecule types
	//             iNumStructuresOfEachType - the number of each molecule type
	//             boxDimensions - the constraint box
	// Returns: nothing
	void makeChild(MoleculeSet &parent1, MoleculeSet &parent2, int iNumStructureTypes,
	               int iNumStructuresOfEachType[], Point3D &boxDimensions);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function mates two parents and produces a child.
	//    It does this using an averaging technique.
	//    The child is stored in the current moleculeSet.
	// Parameters: parent1 and parent2 - the parents
	//             iNumStructureTypes - the number of different molecule types
	//             iNumStructuresOfEachType - the number of each molecule type
	//             boxDimensions - the constraint box
	// Returns: nothing
	void makeChildAverage(MoleculeSet &parent1, MoleculeSet &parent2, int iNumStructureTypes,
	               int iNumStructuresOfEachType[], Point3D &boxDimensions);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This funciton checks to make sure that the the given moleculeSet
	//    and this molecule set have the same atoms and molecules.
	// Parameters: otherMoleculeSet - the other MoleculeSet object
	// Returns: true or false
	bool haveSameAtomsAndMolecules(MoleculeSet &otherMoleculeSet);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checkes to see if the current moleculeSet is fragmented
	// Parameters: maxDist - the maximum distance allowed
	// Returns: true if no groups of atoms are further than maxDist
	bool isFragmented(FLOAT maxDist);

	/////////////////////////////////////////////////////////////////////
	// Purpose: The purpose of this function is to move coordinates and
	//    angles of atoms very slightly so they are not exactly the same
	//    as what they were before.
	// Parameters: boxDimensions - the maximum x, y, and z values any atom can have
	// Returns: true if the function succeeded
	bool offSetCoordinatesAndAnglesSlightly(Point3D &boxDimensions);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function places the moleculeSet in the center of the box.
	// Parameters: boxDimensions - the maximum x, y, and z values any atom can have
	// Returns: nothing
	void centerInBox(Point3D &boxDimensions);
	
	void measureSearchSpace(int &withConstraints, int &withoutConstraints);
	
	
	bool performBondRotations(FLOAT angleInRad, vector<MoleculeSet*> &moleculeSets);
	
	bool moveOrCopyInputEnergyFile(const char* newDirectory, bool moveOrCopy);
	bool moveOrCopyOutputEnergyFiles(const char* newDirectory, const char* newPrefix, int newNumber, bool moveOrCopy);
	bool moveOrCopyOutputEnergyFiles(const char* newDirectory, bool moveOrCopy);
	bool moveOrCopyOutputEnergyFiles(int newNumber, bool moveOrCopy);

	bool deleteOutputEnergyFiles(bool forgetAboutFiles);

	static bool fileExists(const char* fileName);
	
private:
	void cleanUp();

	// This function computes distances between all atoms
	// It also computes the center of mass of all atoms and computes distances to the center of mass
	void initAtomDistances();
	void initAtomDistancesToCenterOfMass();

	// This function initializes m_atomToCenterRank and m_atomToAtomRank rank based
	// on their atomic number and distance to the center of mass.
	void sortAtomRanks();

	/////////////////////////////////////////////////////////////////////
	// Purpose: This molecule assigns a position to moleculeToPlace
	//    relative to otherMolecule along unitVector so that the
	//    minimum and maximum distances are observed.
	// Parameters: moleculeToPlace - the molecule whose position is being assigned.
	//             otherMolecule - the molecule not being placed.
	//             unitVector - moleculeToPlace's position is assigned on a line
	//                          determined by otherMolecule's center of mass and unitVector
	//             maxDist - the maximum distance at which the two molecules
	//                       can be placed (if this is zero, the minimum possible distance will be used)
	// Returns: nothing
	void placeMoleculeObservingMaxDist(Molecule &moleculeToPlace, Molecule &otherMolecule, Point3D unitVector, FLOAT maxDist);

	/////////////////////////////////////////////////////////////////////
	// Purpose: Given a line and a point not on the line, this function
	//    finds a point on the line that is closest to the point.
	// Parameters: pointOnLine - an arbitrary point on the line
	//             vectorAlongLine - a vector along the line
	//             point - a point not on the line
	// Returns: the point on the line that is closest to point
	Point3D closestPointFromALineToAPoint(Point3D pointOnLine, Point3D vectorAlongLine, Point3D point);

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function computes the distance between 2 points.
	// Parameters: point1 and point2 - the two points
	// Returns: the distance
	FLOAT distanceBetweenPoints(Point3D point1, Point3D point2);

	Point3D getVectorInDirection(FLOAT angleX, FLOAT angleY, FLOAT length);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checks to make sure atoms are not closer
	//    than the distances specified.
	// Parameters: none
	// Returns: true if atoms are not closer than the minimum distances specified
	bool checkMinDistConstraints();
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checks to make sure atoms are not closer
	//    than the distances specified.
	// Parameters: iMolecule - the molecule to check
	// Returns: true if atoms are not closer than the minimum distances specified
	bool checkMinDistConstraints(int iMolecule);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checks to make sure atoms are not closer
	//    than the distances specified.  This function allows for not all
	//    of the molecules' positions to be initialized.
	// Parameters: iMolecule - the molecule to check
	//             moleculesInitialized - an array indicating which molecules have been initialized
	// Returns: true if atoms are not closer than the minimum distances specified
	bool checkMinDistConstraints(int iMolecule, bool moleculesInitialized[]);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checks to make sure molecules are not further
	//    than the distance specified from at least one other molecule.
	//    This function allows for not all of the molecules' positions to
	//    be initialized.
	// Parameters: iMolecule - the molecule to check
	//             moleculesInitialized - an array indicating which molecules have been initialized
	//             maxDist - the maximum distance
	// Returns: true if atoms are not further than the maximum distance specified
	bool checkMaxDistConstraints(int iMolecule, bool moleculesInitialized[], FLOAT maxDist);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checks to see if the set of molecules is within
	//    the box defined by boxDimensions.  If not, the function tries to fit
	//    the atoms within the box by moving all of them the same distance and
	//    direction.  Coordinates of each atom must be greater than or equal to
	//    zero and less than or equal to boxDimensions.
	// Parameters: boxDimensions - the maximum x, y, and z values any atom can have
	// Returns: true, if the constraints were successfully enforced, and false otherwise
	bool tryToEnforceBoxConstraints(Point3D &boxDimensions);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checks to see if the set of molecules is within
	//    the box defined by boxDimensions.  If not, the function tries to fit
	//    the atoms within the box by moving all of them the same distance and
	//    direction.  Coordinates of each atom must be greater than or equal to
	//    zero and less than or equal to boxDimensions.
	// Parameters: boxDimensions - the maximum x, y, and z values any atom can have
	//             iChangedMolecule - a molecule that was the only one changed
	// Returns: true, if the constraints were successfully enforced, and false otherwise
	bool tryToEnforceBoxConstraints(Point3D &boxDimensions, int iChangedMolecule);

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function checks one molecule to see if it is within
	//    the box.  This function doesn't try to fix the moleculeSet if
	//    it's not within the box.
	// Parameters: boxDimensions - the maximum x, y, and z values any atom can have
	//             iMolecule = the molecule index to check
	// Returns: true, if the constraints were successfully enforced, and false otherwise
	bool checkBoxConstraints(Point3D &boxDimensions, int iMolecule);
	
	bool testConstraints(Point3D &boxDimensions);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This recursive function traverses molecules that are within maxDist of iMolecule.
	// Parameters: maxDist - the maximum distance allowed
	// Returns: true if no groups of atoms are further than maxDist
	void traverseConnectedMolecules(int iMolecule, bool visitedMolecules[], int &iMoleculesVisited, FLOAT maxDist);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function is used with makeChild and sorts atom indexes based
	//    on the distances of their center of mass to some point.
	// Parameters: moleculeIndexes - the indexes of the molecules
	//             distanceArray - the distances of each molecule's center of mass to some point
	//             lo and hi - indexes to moleculeIndexes where the sort starts and ends respectively
	// Returns: nothing
	void sortMoleculeIndexes(int moleculeIndexes[], FLOAT distanceArray[], int lo, int hi);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function performs a line search using the golden
	//    section search method.
	// Parameters: epsilon and sigma - Lennard Jones constants
	//             s - the negative of the gradient
	//             min and max - low and high values within which to search, respectively
	// Returns: the minimum alpha in a conjugate gradient search
	FLOAT minimizeLennardJonesAlpha(FLOAT epsilon, FLOAT sigma, Point3D s[], FLOAT min, FLOAT max);
	
	FLOAT myPower(FLOAT number, int power);
	
	FLOAT computeLennardJonesEnergy(FLOAT epsilon, FLOAT sigma, FLOAT alpha, Point3D s[]);

	bool moveOrCopyOutputEnergyFile(unsigned int index, const char* newFileName, bool moveOrCopy);
};

#endif

