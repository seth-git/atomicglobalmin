////////////////////////////////////////////////////////////////////////////////
// Purpose: This file contains code managing a molecule or a set of atoms that
//    belong in a group.  Atoms in this group are moved and rotated as a unit.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#ifndef __MOLECULE_H__
#define __MOLECULE_H__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "atom.h"

using namespace std;

// Using aviation terms, three angles are necessary for representing the orientation of molecules: roll, pitch, and yaw.  Rotation around the longitudinal or z axis, the line from the nose of the plane to the tail, is called roll (m_angles.z).  Rotation around the lateral or x axis, the line from wingtip to wingtip, is called pitch (m_angles.x).  Rotation around the vertical axis, the line from beneath to above the plane, is called yaw (m_angles.y).  Roll and yaw, (m_angles.z and m_angles.x) can have values in the range [0,2*PIE], while pitch (m_angles.y) can have values in the range [0,PIE].

class Molecule
{
private:
	Point3D m_centerOfMass;

	Point3D m_angles;
	Point3D m_sinAngles;
	Point3D m_cosAngles;
	FLOAT m_matrixX[MATRIX_SIZE][MATRIX_SIZE];
	FLOAT m_matrixY[MATRIX_SIZE][MATRIX_SIZE];
	FLOAT m_matrixZ[MATRIX_SIZE][MATRIX_SIZE];
	FLOAT m_matrixLocalToGlobal[MATRIX_SIZE][MATRIX_SIZE];
	
	Point3D m_PSOCenterOfMassVelocity;
	Point3D m_PSOAngleVelocity;
	
	int m_iNumberOfAtoms;
	Atom *m_atoms;
	
public:
	Molecule();
	~Molecule();
	void copy(Molecule &molecule);

	void setCenterAndAngles(FLOAT x, FLOAT y, FLOAT z, FLOAT angleX, FLOAT angleY, FLOAT angleZ);
	
	Point3D getCenter() { return m_centerOfMass; }
	void setCenter(Point3D centerOfMass) { m_centerOfMass = centerOfMass; }
	
	FLOAT getAngleX() { return m_angles.x; }
	void setAngleX(FLOAT angleInDegrees) { m_angles.x = angleInDegrees; }
	
	FLOAT getAngleY() { return m_angles.y; }
	void setAngleY(FLOAT angleInDegrees) { m_angles.y = angleInDegrees; }
	
	FLOAT getAngleZ() { return m_angles.z; }
	void setAngleZ(FLOAT angleInDegrees) { m_angles.z = angleInDegrees; }

	int getNumberOfAtoms() { return m_iNumberOfAtoms; }
	
	void initAtomIndexes(Atom **pAtoms, int &iAtom);

	void initVelocities(FLOAT maxCoordVelocity, FLOAT maxAngleVelocityInRad);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function performs the following matrix multiplication:
	//          matrix1 * matrix2 = productMatrix
	// Parameters: matrix1 - the first matrix
	//             matrix2 - the second matrix
	//             productMatrix - the location where the result is stored
	// Returns: nothing
	static void matrixMultiplyMatrix(const double matrix1[MATRIX_SIZE][MATRIX_SIZE],
	                                 const double matrix2[MATRIX_SIZE][MATRIX_SIZE],
	                                 double productMatrix[MATRIX_SIZE][MATRIX_SIZE]);
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function multiplies the 3D "point" by the matrix and
	//          stores the result in point.
	// Parameters: point - the 3D point
	//             matrix - matrix
	// Returns: nothing
	static void matrixMultiplyPoint(Point3D &point, const double matrix[MATRIX_SIZE][MATRIX_SIZE]);
	
	static void printPoint(const Point3D &point);
	
	static void printMatrix(const FLOAT matrix[MATRIX_SIZE][MATRIX_SIZE]);
	
	void writeToGausianFile(ofstream &fout);
	
	void printToFile(ofstream &fout);
	
	void printToResumeFile(ofstream &fout, bool printVelocityInfo);
	
	bool readFromResumeFile(ifstream &infile, char* fileLine, int MAX_LINE_LENGTH, bool readVelocityInfo);
	
	void printToScreen();
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function creates a molecule given a set of cartesian
	//    coordinates and atomic numbers.
	// Parameters: cartesianPoints - the coordinates of each atom
	//             atomicNumbers - the atomic numbers of each atom
	// Returns: nothing
	void makeFromCartesian(vector<Point3D> &cartesianPoints, vector<int> &atomicNumbers);

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function creates a water molecule
	// Parameters: none
	// Returns: nothing
	void makeWater();
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function creates the rotation matrix for a molecule.
	//    This rotation matrix can translate any point from coordinates local
	//    to the molecule to global coordinates.  This is accomplished simply by
	//    mutliplying a point local molecule by the rotation matrix.
	// Parameters: none
	// Returns: nothing
	void initRotationMatrix();
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function recalulates the positions of individual
	//    atoms after the molecule has been rotated or moved.
	// Parameters: none
	// Returns: nothing
	void localToGlobal();
	
	/////////////////////////////////////////////////////////////////////
	// Purpose: This function calculates local coordinates from global coordinates.
	//    This is necessary after optimization.  This function sets angles to zero.
	// Parameters: none
	// Returns: nothing
	void globalToLocal();

	void getMinimumAndMaximumCoordinates(Point3D &min, Point3D &max);

	void moveMolecule(Point3D shiftValue);

	void rotateMolecule(Point3D anglesInRadians);
	
	// This function gets the total distance of all atoms to the given point
	FLOAT getDistanceFromPoint(Point3D &point);

	// This function initializes the distance matrix of the moleculeSet within which it is contained.
	// The parameter atomsInitialized indicates which other atoms in the distance matrix have been initialized.
	// According to atomsInitialized, the atoms in this molecule shouldn't have been initialized.
	void initDistanceMatrix(int iNumAtomsInMoleculeSet, Atom **moleculeSetAtoms, FLOAT **moleculeSetDistanceMatrix);
	
	// This function initializes the distance matrix of the moleculeSet within which it is contained.
	// The parameter atomsInitialized indicates which other atoms in the distance matrix have been initialized.
	// According to atomsInitialized, the atoms in this molecule shouldn't have been initialized.
	void initDistanceMatrix(int iNumAtomsInMoleculeSet, Atom **moleculeSetAtoms, FLOAT **moleculeSetDistanceMatrix, bool atomsInitialized[]);
	
	// This function returns false if atoms in the molecules are too close to one another.
	bool checkMinDistances(Molecule &otherMolecule, FLOAT **distanceMatrix);
	
	// This function returns true if the molecules contain at least one pair of atoms within maxDistance
	bool checkMaxDistances(Molecule &otherMolecule, FLOAT **distanceMatrix, FLOAT maxDistance);
	
	void markAtomsAsInitialized(bool atomsInitialized[]);

	FLOAT getMinDistance(Molecule &otherMolecule);
	
	Point3D getPositionRelativeToMoleculeAlongVector(Point3D &vector);
	
	void performPSO(Molecule &populationBest, Molecule &individualBest, FLOAT coordInertia,
	                FLOAT coordIndividualMinimumAttraction, FLOAT coordPopulationMinimumAttraction, FLOAT coordMaxVelocity,
	                FLOAT angleInertia, FLOAT angleIndividualMinimumAttraction, FLOAT anglePopulationMinimumAttraction,
	                FLOAT angleMaxVelocity, Point3D &boxDimensions, FLOAT fAttractionRepulsion);
	
	void getVelocityStats(FLOAT &maxCoordinateVelocity, FLOAT &totCoordinateVelocity, FLOAT &maxAngularVelocity,
	                      FLOAT &totAngularVelocity);
	
	void getClosestAtoms(Molecule &otherMolecule, Point3D &atomInThisMolecule, Point3D &atomInOtherMolecule,
	                     FLOAT &minDistanceBetweenAtoms);
	
	// Note: if your not familiar with static, it means these functions belong to the class and not any particular object
	//       of the class.  It also means they can be called from anywhere without needing an object of the class.  Cool ha!
	//       I made these functions static, so I don't have to have a copy of these functions inside main, moleculeSet, and molecule.
	static int randomInt(int lo, int hi);
	static FLOAT randomFloat(FLOAT lo, FLOAT hi);
	static void initRandoms();

	/////////////////////////////////////////////////////////////////////
	// Purpose: This function determines if this and another molecule has
	//    the same atoms.
	// Parameters: otherMolecule - the other molecule
	// Returns: true if they have the same atoms
	bool haveSameAtoms(Molecule &otherMolecule);
	
private:
	
	void copyMatrix(FLOAT dest[MATRIX_SIZE][MATRIX_SIZE], FLOAT source[MATRIX_SIZE][MATRIX_SIZE]);
	void cleanUp();
	void adjustToCenterOfMass();

};

#endif

