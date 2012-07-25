////////////////////////////////////////////////////////////////////////////////
// Purpose: This file manages a molecule.
// Author: Seth Call
// Note: This is free software and may be modified and/or redistributed under
//    the terms of the GNU General Public License (Version 1.2 or any later
//    version).  Copyright 2007 Seth Call.
////////////////////////////////////////////////////////////////////////////////

#include "molecule.h"

Molecule::Molecule ()
{
	setCenterAndAngles(0,0,0,0,0,0);
	m_iNumberOfAtoms = 0;
	m_atoms = NULL;
	m_PSOCenterOfMassVelocity.x = 0;
	m_PSOCenterOfMassVelocity.y = 0;
	m_PSOCenterOfMassVelocity.z = 0;
	m_PSOAngleVelocity.x = 0;
	m_PSOAngleVelocity.y = 0;
	m_PSOAngleVelocity.z = 0;
}

Molecule::~Molecule (void)
{
	cleanUp();
}

void Molecule::cleanUp (void)
{
	m_iNumberOfAtoms = 0;
	delete[] m_atoms;
	m_atoms = NULL;
}

void Molecule::copy(Molecule &molecule)
{
	if (m_iNumberOfAtoms != molecule.m_iNumberOfAtoms) {
		cleanUp();
		m_iNumberOfAtoms = molecule.m_iNumberOfAtoms;
		m_atoms = new Atom[m_iNumberOfAtoms];
	}
	for (int i = 0; i < m_iNumberOfAtoms; ++i)
		m_atoms[i].copy(molecule.m_atoms[i]);
	
	m_centerOfMass = molecule.m_centerOfMass;
	m_angles = molecule.m_angles;
	m_sinAngles = molecule.m_sinAngles;
	m_cosAngles = molecule.m_cosAngles;
	copyMatrix(m_matrixX, molecule.m_matrixX);
	copyMatrix(m_matrixY, molecule.m_matrixY);
	copyMatrix(m_matrixZ, molecule.m_matrixZ);
	copyMatrix(m_matrixLocalToGlobal, molecule.m_matrixLocalToGlobal);
	
	m_PSOCenterOfMassVelocity = molecule.m_PSOCenterOfMassVelocity;
	m_PSOAngleVelocity = molecule.m_PSOAngleVelocity;
}

void Molecule::copyMatrix(FLOAT dest[MATRIX_SIZE][MATRIX_SIZE], FLOAT source[MATRIX_SIZE][MATRIX_SIZE])
{
	int i, j;
	for (i = 0; i < MATRIX_SIZE; ++i)
		for (j = 0; j < MATRIX_SIZE; ++j)
			dest[i][j] = source[i][j];
}

void Molecule::makeFromCartesian(vector<Point3D> &cartesianPoints, vector<int> &atomicNumbers)
{
	cleanUp();
	m_iNumberOfAtoms = cartesianPoints.size();
	delete[] m_atoms;
	m_atoms = new Atom[m_iNumberOfAtoms];
	for (int i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atoms[i].m_iAtomicNumber = atomicNumbers[i];
		m_atoms[i].m_localPoint = cartesianPoints[i];
		m_atoms[i].m_localPoint.w = 1;
	}
	adjustToCenterOfMass();
	m_angles.x = 0;
	m_angles.y = 0;
	m_angles.z = 0;
}

void Molecule::setCenterAndAngles (FLOAT x, FLOAT y, FLOAT z, FLOAT angleX, FLOAT angleY, FLOAT angleZ)
{
	m_centerOfMass.x = x;
	m_centerOfMass.y = y;
	m_centerOfMass.z = z;
	m_angles.x = angleX;
	m_angles.y = angleY;
	m_angles.z = angleZ;
}

void Molecule::initAtomIndexes(Atom **pAtoms, int &iAtom)
{
	for (int i = 0; i < m_iNumberOfAtoms; ++i)
	{
		pAtoms[iAtom] = &m_atoms[i];
		m_atoms[i].m_iMoleculeSetIndex = iAtom;
		++iAtom;
	}
}

void Molecule::initVelocities(FLOAT maxCoordVelocity, FLOAT maxAngleVelocityInRad)
{
	m_PSOCenterOfMassVelocity.x = randomFloat(-maxCoordVelocity,maxCoordVelocity);
	m_PSOCenterOfMassVelocity.y = randomFloat(-maxCoordVelocity,maxCoordVelocity);
	m_PSOCenterOfMassVelocity.z = randomFloat(-maxCoordVelocity,maxCoordVelocity);
	if (m_iNumberOfAtoms > 1) {
		m_PSOAngleVelocity.x = randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
		m_PSOAngleVelocity.y = randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
		m_PSOAngleVelocity.z = randomFloat(-maxAngleVelocityInRad,maxAngleVelocityInRad);
	}
}

void Molecule::matrixMultiplyMatrix(const FLOAT matrix1[MATRIX_SIZE][MATRIX_SIZE],
                                    const FLOAT matrix2[MATRIX_SIZE][MATRIX_SIZE],
                                    FLOAT productMatrix[MATRIX_SIZE][MATRIX_SIZE])
{
	int width, height;
	int index;
	FLOAT cell;

	for (width = 0; width < MATRIX_SIZE; width++)
	{
		for (height = 0; height < MATRIX_SIZE; height++)
		{
			cell = 0;
			for (index = 0; index < MATRIX_SIZE; index++)
			{
				cell += matrix1[index][height] * matrix2[width][index];
			}
			productMatrix[width][height] = cell;
		}
	}
}

void Molecule::matrixMultiplyPoint(Point3D &point, const FLOAT matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	int height;
	int index;
	FLOAT pointMatrix[MATRIX_SIZE]; // vector representation of point
	FLOAT *newPointMatrix[MATRIX_SIZE]; // vector representation of the product
	FLOAT cell;

	// Make a verctor copy of point
	pointMatrix[0] = point.x;
	pointMatrix[1] = point.y;
	pointMatrix[2] = point.z;
	pointMatrix[3] = point.w;

	// Make a verctor of pointers in which to store the result (the result is stored in point)
	newPointMatrix[0] = &point.x;
	newPointMatrix[1] = &point.y;
	newPointMatrix[2] = &point.z;
	newPointMatrix[3] = &point.w;

	for (height = 0; height < MATRIX_SIZE; height++)
	{
		cell = 0;
		for (index = 0; index < MATRIX_SIZE; index++)
		{
			cell += matrix[index][height] * pointMatrix[index];
		}
		*newPointMatrix[height] = cell;
	}
}

void Molecule::initRotationMatrix()
{
	FLOAT temp[MATRIX_SIZE][MATRIX_SIZE];

	m_sinAngles.x = FLOAT(sin(m_angles.x));
	m_sinAngles.y = FLOAT(sin(m_angles.y));
	m_sinAngles.z = FLOAT(sin(m_angles.z));
	m_cosAngles.x = FLOAT(cos(m_angles.x));
	m_cosAngles.y = FLOAT(cos(m_angles.y));
	m_cosAngles.z = FLOAT(cos(m_angles.z));

	m_matrixZ[0][0] = m_cosAngles.z;
	m_matrixZ[1][0] = m_sinAngles.z;
	m_matrixZ[2][0] = 0;
	m_matrixZ[3][0] = 0;
	m_matrixZ[0][1] = -m_sinAngles.z;
	m_matrixZ[1][1] = m_cosAngles.z;
	m_matrixZ[2][1] = 0;
	m_matrixZ[3][1] = 0;
	m_matrixZ[0][2] = 0;
	m_matrixZ[1][2] = 0;
	m_matrixZ[2][2] = 1;
	m_matrixZ[3][2] = 0;
	m_matrixZ[0][3] = 0;
	m_matrixZ[1][3] = 0;
	m_matrixZ[2][3] = 0;
	m_matrixZ[3][3] = 1;

	m_matrixX[0][0] = 1;
	m_matrixX[1][0] = 0;
	m_matrixX[2][0] = 0;
	m_matrixX[3][0] = 0;
	m_matrixX[0][1] = 0;
	m_matrixX[1][1] = m_cosAngles.x;
	m_matrixX[2][1] = m_sinAngles.x;
	m_matrixX[3][1] = 0;
	m_matrixX[0][2] = 0;
	m_matrixX[1][2] = -m_sinAngles.x;
	m_matrixX[2][2] = m_cosAngles.x;
	m_matrixX[3][2] = 0;
	m_matrixX[0][3] = 0;
	m_matrixX[1][3] = 0;
	m_matrixX[2][3] = 0;
	m_matrixX[3][3] = 1;

	m_matrixY[0][0] = m_cosAngles.y;
	m_matrixY[1][0] = 0;
	m_matrixY[2][0] = m_sinAngles.y;
	m_matrixY[3][0] = 0;
	m_matrixY[0][1] = 0;
	m_matrixY[1][1] = 1;
	m_matrixY[2][1] = 0;
	m_matrixY[3][1] = 0;
	m_matrixY[0][2] = -m_sinAngles.y;
	m_matrixY[1][2] = 0;
	m_matrixY[2][2] = m_cosAngles.y;
	m_matrixY[3][2] = 0;
	m_matrixY[0][3] = 0;
	m_matrixY[1][3] = 0;
	m_matrixY[2][3] = 0;
	m_matrixY[3][3] = 1;

	matrixMultiplyMatrix(m_matrixY,m_matrixX,temp);
	matrixMultiplyMatrix(temp,m_matrixZ,m_matrixLocalToGlobal);
	m_matrixLocalToGlobal[MATRIX_SIZE-1][0] = m_centerOfMass.x;
	m_matrixLocalToGlobal[MATRIX_SIZE-1][1] = m_centerOfMass.y;
	m_matrixLocalToGlobal[MATRIX_SIZE-1][2] = m_centerOfMass.z;
}

void Molecule::localToGlobal()
{
	for (int i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atoms[i].m_globalPoint = m_atoms[i].m_localPoint;
		matrixMultiplyPoint(m_atoms[i].m_globalPoint,m_matrixLocalToGlobal);
	}
}

void Molecule::globalToLocal()
{
	FLOAT totalMass;
	int i;
	
	m_centerOfMass.x = 0;
	m_centerOfMass.y = 0;
	m_centerOfMass.z = 0;
	totalMass = 0;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_centerOfMass.x += m_atoms[i].getMass() * m_atoms[i].m_globalPoint.x;
		m_centerOfMass.y += m_atoms[i].getMass() * m_atoms[i].m_globalPoint.y;
		m_centerOfMass.z += m_atoms[i].getMass() * m_atoms[i].m_globalPoint.z;
		totalMass += m_atoms[i].getMass();
	}
	if (m_centerOfMass.x != 0)
		m_centerOfMass.x /= totalMass;
	if (m_centerOfMass.y != 0)
		m_centerOfMass.y /= totalMass;
	if (m_centerOfMass.z != 0)
		m_centerOfMass.z /= totalMass;
	
	for (i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atoms[i].m_localPoint.x = m_atoms[i].m_globalPoint.x - m_centerOfMass.x;
		m_atoms[i].m_localPoint.y = m_atoms[i].m_globalPoint.y - m_centerOfMass.y;
		m_atoms[i].m_localPoint.z = m_atoms[i].m_globalPoint.z - m_centerOfMass.z;
	}
	
	m_angles.x = 0;
	m_angles.y = 0;
	m_angles.z = 0;
	initRotationMatrix();
}

void Molecule::printMatrix(const FLOAT matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	int height;
	int index;

	for (height = 0; height < MATRIX_SIZE; height++)
	{
		for (index = 0; index < MATRIX_SIZE; index++)
			cout << setiosflags(ios::fixed) << setprecision(6) << matrix[index][height] << " ";
		cout << endl;
	}
}

void Molecule::printPoint(const Point3D &point)
{
	cout << setiosflags(ios::fixed) << setprecision(6)
	     << point.x << " "
	     << point.y << " "
	     << point.z << " "
	     << point.w << " "
	     << endl;
}

void Molecule::writeToGausianFile(ofstream &fout)
{
	for (int atomIndex = 0; atomIndex < m_iNumberOfAtoms; ++atomIndex)
		fout << m_atoms[atomIndex].m_iAtomicNumber << setiosflags(ios::fixed) << setprecision(6)
	                                           << " " << m_atoms[atomIndex].m_globalPoint.x
	                                           << " " << m_atoms[atomIndex].m_globalPoint.y
	                                           << " " << m_atoms[atomIndex].m_globalPoint.z << endl;
}

void Molecule::printToFile(ofstream &fout)
{
	fout << setiosflags(ios::fixed) << setprecision(8);
	if (m_iNumberOfAtoms > 1)
	{
		fout << "Center: "      << m_centerOfMass.x
                         << " " << m_centerOfMass.y
                         << " " << m_centerOfMass.z << endl;
		fout << "Angles: "      << (m_angles.x * RAD_TO_DEG)
                         << " " << (m_angles.y * RAD_TO_DEG)
                         << " " << (m_angles.z * RAD_TO_DEG) << endl;
	}
	for (int atomIndex = 0; atomIndex < m_iNumberOfAtoms; ++atomIndex)
		fout << m_atoms[atomIndex].m_iAtomicNumber
           << " " << m_atoms[atomIndex].m_globalPoint.x
           << " " << m_atoms[atomIndex].m_globalPoint.y
           << " " << m_atoms[atomIndex].m_globalPoint.z << endl;
}

void Molecule::printToResumeFile(ofstream &fout, bool printVelocityInfo)
{
	fout << setiosflags(ios::fixed) << setprecision(15);
	fout << "Center: "        << m_centerOfMass.x
	                   << " " << m_centerOfMass.y
	                   << " " << m_centerOfMass.z << endl;
	if (printVelocityInfo) {
		fout << "Center Velocity: " << m_PSOCenterOfMassVelocity.x
		                     << " " << m_PSOCenterOfMassVelocity.y
		                     << " " << m_PSOCenterOfMassVelocity.z << endl;
	}
	if (m_iNumberOfAtoms > 1) {
		fout << "Angles: "        << m_angles.x
		                   << " " << m_angles.y
		                   << " " << m_angles.z << endl;
		if (printVelocityInfo) {
			fout << "Angle Velocity: " << m_PSOAngleVelocity.x
			                    << " " << m_PSOAngleVelocity.y
			                    << " " << m_PSOAngleVelocity.z << endl;
		}
	}
	for (int atomIndex = 0; atomIndex < m_iNumberOfAtoms; ++atomIndex)
		fout << m_atoms[atomIndex].m_iAtomicNumber
           << " " << m_atoms[atomIndex].m_localPoint.x
           << " " << m_atoms[atomIndex].m_localPoint.y
           << " " << m_atoms[atomIndex].m_localPoint.z << endl;
}

bool Molecule::readFromResumeFile(ifstream &infile, char* fileLine, int MAX_LINE_LENGTH, bool readVelocityInfo)
{
	int temp;
	
	if (!infile.getline(fileLine, MAX_LINE_LENGTH))
	{
		cout << "The line containing the center of a molecule is missing in the input file." << endl;
		return false;
	}
	if (sscanf(fileLine, "Center: %lf %lf %lf",
	    &m_centerOfMass.x, &m_centerOfMass.y, &m_centerOfMass.z) != 3)
	{
		cout << "Error reading the center of mass of a molecule." << endl;
		return false;
	}
	if (readVelocityInfo) {
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The center velocity for a molecule was missing in the input file." << endl;
			return false;
		}
		if (sscanf(fileLine, "Center Velocity: %lf %lf %lf", 
		                                       &m_PSOCenterOfMassVelocity.x,
		                                       &m_PSOCenterOfMassVelocity.y,
		                                       &m_PSOCenterOfMassVelocity.z) != 3)
		{
			cout << "Molecule center velocity could not be read for a molecule in the input file." << endl;
			return false;
		}
	}
	if (m_iNumberOfAtoms > 1) {
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line containing the angles of a molecule is missing in the input file." << endl;
			return false;
		}
		if (sscanf(fileLine, "Angles: %lf %lf %lf", &m_angles.x, &m_angles.y, &m_angles.z) != 3)
		{
			cout << "Error reading the angles of a molecule." << endl;
			return false;
		}
		if (readVelocityInfo) {
			if (!infile.getline(fileLine, MAX_LINE_LENGTH))
			{
				cout << "The angular velocity for a molecule was missing in the input file." << endl;
				return false;
			}
			if (sscanf(fileLine, "Angle Velocity: %lf %lf %lf", 
			                                       &m_PSOAngleVelocity.x,
			                                       &m_PSOAngleVelocity.y,
			                                       &m_PSOAngleVelocity.z) != 3)
			{
				cout << "Molecule angle velocity could not be read for a molecule in the input file." << endl;
				return false;
			}
		}
	}
	for (int atomIndex = 0; atomIndex < m_iNumberOfAtoms; ++atomIndex) {
		if (!infile.getline(fileLine, MAX_LINE_LENGTH))
		{
			cout << "The line containing an atom's local coordinates is missing in the input file." << endl;
			return false;
		}
		temp = -1;
		if (sscanf(fileLine, "%d %lf %lf %lf", &temp,
		                                       &m_atoms[atomIndex].m_localPoint.x,
		                                       &m_atoms[atomIndex].m_localPoint.y,
		                                       &m_atoms[atomIndex].m_localPoint.z) != 4)
		{
			cout << "Error reading atom coordinates." << endl;
			return false;
		}
		if (temp != m_atoms[atomIndex].m_iAtomicNumber) {
			cout << "Atomic number mismatch in the input file.  Expected: " << m_atoms[atomIndex].m_iAtomicNumber
			                                              << ", Found: "    << temp << endl;
			return false;
		}
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
	initRotationMatrix();
	localToGlobal();
	
	return true;
}

void Molecule::printToScreen()
{
	if (m_iNumberOfAtoms > 1)
	{
		cout << "Center: " << setiosflags(ios::fixed) << setprecision(6)
                         << " " << m_centerOfMass.x
                         << " " << m_centerOfMass.y
                         << " " << m_centerOfMass.z << endl;
		cout << "Angles: " << setiosflags(ios::fixed) << setprecision(6)
                         << " " << (m_angles.x * RAD_TO_DEG)
                         << " " << (m_angles.y * RAD_TO_DEG)
                         << " " << (m_angles.z * RAD_TO_DEG) << endl;
	}
	for (int atomIndex = 0; atomIndex < m_iNumberOfAtoms; ++atomIndex)
		cout << m_atoms[atomIndex].m_iAtomicNumber << setiosflags(ios::fixed) << setprecision(6)
           << " " << m_atoms[atomIndex].m_globalPoint.x
           << " " << m_atoms[atomIndex].m_globalPoint.y
           << " " << m_atoms[atomIndex].m_globalPoint.z << endl;
}


void Molecule::getMinimumAndMaximumCoordinates(Point3D &min, Point3D &max)
{
	// Note, this function assumes that min and max have been initialized
	for (int i = 0; i < m_iNumberOfAtoms; ++i)
	{
		if (m_atoms[i].m_globalPoint.x < min.x)
			min.x = m_atoms[i].m_globalPoint.x;
		if (m_atoms[i].m_globalPoint.y < min.y)
			min.y = m_atoms[i].m_globalPoint.y;
		if (m_atoms[i].m_globalPoint.z < min.z)
			min.z = m_atoms[i].m_globalPoint.z;
		if (m_atoms[i].m_globalPoint.x > max.x)
			max.x = m_atoms[i].m_globalPoint.x;
		if (m_atoms[i].m_globalPoint.y > max.y)
			max.y = m_atoms[i].m_globalPoint.y;
		if (m_atoms[i].m_globalPoint.z > max.z)
			max.z = m_atoms[i].m_globalPoint.z;
	}
}

void Molecule::moveMolecule(Point3D shiftValue)
{
	for (int i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atoms[i].m_globalPoint.x += shiftValue.x;
		m_atoms[i].m_globalPoint.y += shiftValue.y;
		m_atoms[i].m_globalPoint.z += shiftValue.z;
	}
	m_centerOfMass.x += shiftValue.x;
	m_centerOfMass.y += shiftValue.y;
	m_centerOfMass.z += shiftValue.z;
	m_matrixLocalToGlobal[MATRIX_SIZE-1][0] = m_centerOfMass.x;
	m_matrixLocalToGlobal[MATRIX_SIZE-1][1] = m_centerOfMass.y;
	m_matrixLocalToGlobal[MATRIX_SIZE-1][2] = m_centerOfMass.z;
}

void Molecule::rotateMolecule(Point3D anglesInRadians)
{
	m_angles.x += anglesInRadians.x;
	while (m_angles.x > PIE_X_2)
		m_angles.x -= PIE_X_2;
	while (m_angles.x < 0)
		m_angles.x += PIE_X_2;
	m_angles.y += anglesInRadians.y;
	while (m_angles.y > PIE_X_2)
		m_angles.y -= PIE_X_2;
	while (m_angles.y < 0)
		m_angles.y += PIE_X_2;
	m_angles.z += anglesInRadians.z;
	while (m_angles.z > PIE_X_2)
		m_angles.z -= PIE_X_2;
	while (m_angles.z < 0)
		m_angles.z += PIE_X_2;
	initRotationMatrix();
	localToGlobal();
}

FLOAT Molecule::getDistanceFromPoint(Point3D &point)
{
	Point3D diff;
	FLOAT distance;
	distance = 0;
	for (int i = 0; i < m_iNumberOfAtoms; ++i)
	{
		diff.x = m_atoms[i].m_globalPoint.x - point.x;
		diff.y = m_atoms[i].m_globalPoint.y - point.y;
		diff.z = m_atoms[i].m_globalPoint.z - point.z;
		distance += sqrt((diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z));
	}
	return distance;
}

void Molecule::adjustToCenterOfMass()
{
	Point3D centerOfMass;
	FLOAT totalMass;
	int i;
	
	centerOfMass.x = 0;
	centerOfMass.y = 0;
	centerOfMass.z = 0;
	totalMass = 0;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
	{
		centerOfMass.x += m_atoms[i].getMass() * m_atoms[i].m_localPoint.x;
		centerOfMass.y += m_atoms[i].getMass() * m_atoms[i].m_localPoint.y;
		centerOfMass.z += m_atoms[i].getMass() * m_atoms[i].m_localPoint.z;
		totalMass += m_atoms[i].getMass();
	}
	
	if (centerOfMass.x != 0)
		centerOfMass.x /= totalMass;
	if (centerOfMass.y != 0)
		centerOfMass.y /= totalMass;
	if (centerOfMass.z != 0)
		centerOfMass.z /= totalMass;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atoms[i].m_localPoint.x -= centerOfMass.x;
		m_atoms[i].m_localPoint.y -= centerOfMass.y;
		m_atoms[i].m_localPoint.z -= centerOfMass.z;
	}
}

void Molecule::initDistanceMatrix(int iNumAtomsInMoleculeSet, Atom **moleculeSetAtoms, FLOAT **moleculeSetDistanceMatrix)
{
	int iAtom, jAtom;
	FLOAT diffX, diffY, diffZ;
	FLOAT distance;
	
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		for (jAtom = 0; jAtom < iNumAtomsInMoleculeSet; ++jAtom)
			if (m_atoms[iAtom].m_iMoleculeSetIndex != jAtom) {
				diffX = m_atoms[iAtom].m_globalPoint.x - moleculeSetAtoms[jAtom]->m_globalPoint.x;
				diffY = m_atoms[iAtom].m_globalPoint.y - moleculeSetAtoms[jAtom]->m_globalPoint.y;
				diffZ = m_atoms[iAtom].m_globalPoint.z - moleculeSetAtoms[jAtom]->m_globalPoint.z;
				distance = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
				
				moleculeSetDistanceMatrix[m_atoms[iAtom].m_iMoleculeSetIndex][jAtom] = distance;
				moleculeSetDistanceMatrix[jAtom][m_atoms[iAtom].m_iMoleculeSetIndex] = distance;
			}
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		moleculeSetDistanceMatrix[m_atoms[iAtom].m_iMoleculeSetIndex][m_atoms[iAtom].m_iMoleculeSetIndex] = 0;
}

void Molecule::initDistanceMatrix(int iNumAtomsInMoleculeSet, Atom **moleculeSetAtoms, FLOAT **moleculeSetDistanceMatrix, bool atomsInitialized[])
{
	int iAtom, jAtom;
	FLOAT diffX, diffY, diffZ;
	FLOAT distance;
	
	// Initialize distances between atoms within this molecule
	for (iAtom = 0; iAtom < m_iNumberOfAtoms-1; ++iAtom)
		for (jAtom = iAtom+1; jAtom < m_iNumberOfAtoms; ++jAtom) {
			diffX = m_atoms[iAtom].m_globalPoint.x - m_atoms[jAtom].m_globalPoint.x;
			diffY = m_atoms[iAtom].m_globalPoint.y - m_atoms[jAtom].m_globalPoint.y;
			diffZ = m_atoms[iAtom].m_globalPoint.z - m_atoms[jAtom].m_globalPoint.z;
			distance = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
			moleculeSetDistanceMatrix[m_atoms[iAtom].m_iMoleculeSetIndex][m_atoms[jAtom].m_iMoleculeSetIndex] = distance;
			moleculeSetDistanceMatrix[m_atoms[jAtom].m_iMoleculeSetIndex][m_atoms[iAtom].m_iMoleculeSetIndex] = distance;
		}
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		moleculeSetDistanceMatrix[m_atoms[iAtom].m_iMoleculeSetIndex][m_atoms[iAtom].m_iMoleculeSetIndex] = 0;
	
	// Initialize distances between atoms inside this molecule and those outside this molecule 
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		for (jAtom = 0; jAtom < iNumAtomsInMoleculeSet; ++jAtom)
			if (atomsInitialized[jAtom]) {
				diffX = m_atoms[iAtom].m_globalPoint.x - moleculeSetAtoms[jAtom]->m_globalPoint.x;
				diffY = m_atoms[iAtom].m_globalPoint.y - moleculeSetAtoms[jAtom]->m_globalPoint.y;
				diffZ = m_atoms[iAtom].m_globalPoint.z - moleculeSetAtoms[jAtom]->m_globalPoint.z;
				distance = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
				
				moleculeSetDistanceMatrix[m_atoms[iAtom].m_iMoleculeSetIndex][jAtom] = distance;
				moleculeSetDistanceMatrix[jAtom][m_atoms[iAtom].m_iMoleculeSetIndex] = distance;
			}
}

bool Molecule::checkMinDistances(Molecule &otherMolecule, FLOAT **distanceMatrix)
{
	int i, j;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		for (j = 0; j < otherMolecule.m_iNumberOfAtoms; ++j)
			if (distanceMatrix[m_atoms[i].m_iMoleculeSetIndex][otherMolecule.m_atoms[j].m_iMoleculeSetIndex]
				< m_atoms[i].getMinDistance(otherMolecule.m_atoms[j]))
				return false;
	return true;
}

bool Molecule::checkMaxDistances(Molecule &otherMolecule, FLOAT **distanceMatrix, FLOAT maxDistance)
{
	int i, j;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		for (j = 0; j < otherMolecule.m_iNumberOfAtoms; ++j)
			if (distanceMatrix[m_atoms[i].m_iMoleculeSetIndex][otherMolecule.m_atoms[j].m_iMoleculeSetIndex] <= maxDistance)
				return true;
	return false;
}

void Molecule::markAtomsAsInitialized(bool atomsInitialized[])
{
	for (int iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		atomsInitialized[m_atoms[iAtom].m_iMoleculeSetIndex] = true;
}

FLOAT Molecule::getMinDistance(Molecule &otherMolecule)
{
	if ((m_iNumberOfAtoms != 1) || (otherMolecule.m_iNumberOfAtoms != 1)) {
		cout << "Error: the function Molecule::getMinDistance should only be used with molecules containing one atom" << endl;
		return 0;
	} else {
		return m_atoms[0].getMinDistance(otherMolecule.m_atoms[0]);
	}
}

Point3D Molecule::getPositionRelativeToMoleculeAlongVector(Point3D &vector)
{
	Point3D position;
	position.x = m_centerOfMass.x + vector.x;
	position.y = m_centerOfMass.y + vector.y;
	position.z = m_centerOfMass.z + vector.z;
	position.w = 1;
	return position;
}

void Molecule::performPSO(Molecule &populationBest, Molecule &individualBest, FLOAT coordInertia,
	                      FLOAT coordIndividualMinimumAttraction, FLOAT coordPopulationMinimumAttraction,
                          FLOAT coordMaxVelocity, FLOAT angleInertia, FLOAT angleIndividualMinimumAttraction,
                          FLOAT anglePopulationMinimumAttraction, FLOAT angleMaxVelocity, Point3D &boxDimensions,
                          FLOAT fAttractionRepulsion)
{
	FLOAT coordinateVelocity;
	FLOAT angularVelocity;
	angularVelocity = sqrt(m_PSOAngleVelocity.x * m_PSOAngleVelocity.x + 
	                       m_PSOAngleVelocity.y * m_PSOAngleVelocity.y + 
	                       m_PSOAngleVelocity.z * m_PSOAngleVelocity.z);
	
	m_PSOCenterOfMassVelocity.x = m_PSOCenterOfMassVelocity.x * coordInertia
		+ fAttractionRepulsion * coordPopulationMinimumAttraction * randomFloat(0,1) *
		  (populationBest.m_centerOfMass.x - m_centerOfMass.x)
		+ fAttractionRepulsion * coordIndividualMinimumAttraction * randomFloat(0,1) *
		  (individualBest.m_centerOfMass.x - m_centerOfMass.x);
	m_PSOCenterOfMassVelocity.y = m_PSOCenterOfMassVelocity.y * coordInertia
		+ fAttractionRepulsion * coordPopulationMinimumAttraction * randomFloat(0,1) *
		  (populationBest.m_centerOfMass.y - m_centerOfMass.y)
		+ fAttractionRepulsion * coordIndividualMinimumAttraction * randomFloat(0,1) *
		  (individualBest.m_centerOfMass.y - m_centerOfMass.y);
	m_PSOCenterOfMassVelocity.z = m_PSOCenterOfMassVelocity.z * coordInertia
		+ fAttractionRepulsion * coordPopulationMinimumAttraction * randomFloat(0,1) *
		  (populationBest.m_centerOfMass.z - m_centerOfMass.z)
		+ fAttractionRepulsion * coordIndividualMinimumAttraction * randomFloat(0,1) *
		  (individualBest.m_centerOfMass.z - m_centerOfMass.z);
	
	// Make sure we haven't exceeded the maximum velocity
	if (coordMaxVelocity != 0) {
		coordinateVelocity = sqrt(m_PSOCenterOfMassVelocity.x * m_PSOCenterOfMassVelocity.x + 
		                          m_PSOCenterOfMassVelocity.y * m_PSOCenterOfMassVelocity.y + 
		                          m_PSOCenterOfMassVelocity.z * m_PSOCenterOfMassVelocity.z);
		if (coordinateVelocity > coordMaxVelocity) {
			coordinateVelocity = coordMaxVelocity / coordinateVelocity;
			m_PSOCenterOfMassVelocity.x *= coordinateVelocity;
			m_PSOCenterOfMassVelocity.y *= coordinateVelocity;
			m_PSOCenterOfMassVelocity.z *= coordinateVelocity;
		}
	}
	
	m_centerOfMass.x += m_PSOCenterOfMassVelocity.x;
	m_centerOfMass.y += m_PSOCenterOfMassVelocity.y;
	m_centerOfMass.z += m_PSOCenterOfMassVelocity.z;
	
	if (m_iNumberOfAtoms == 1) {
		// Enforce the box constraints
		if (m_centerOfMass.x < 0)
			m_centerOfMass.x = 0;
		if (m_centerOfMass.x > boxDimensions.x)
			m_centerOfMass.x = boxDimensions.x;
		if (m_centerOfMass.y < 0)
			m_centerOfMass.y = 0;
		if (m_centerOfMass.y > boxDimensions.y)
			m_centerOfMass.y = boxDimensions.y;
		if (m_centerOfMass.z < 0)
			m_centerOfMass.z = 0;
		if (m_centerOfMass.z > boxDimensions.z)
			m_centerOfMass.z = boxDimensions.z;
		initRotationMatrix();
		localToGlobal();
	} else {
		Point3D amountOutsideBox;
		Point3D populationBestDifference, individualBestDifference;
		
		// Make the angles move
		populationBestDifference.x = populationBest.m_angles.x - m_angles.x;
		if (populationBestDifference.x > PIE)
			populationBestDifference.x -= PIE_X_2;
		if (populationBestDifference.x < -PIE)
			populationBestDifference.x += PIE_X_2;
		populationBestDifference.y = populationBest.m_angles.y - m_angles.y;
		populationBestDifference.z = populationBest.m_angles.z - m_angles.z;
		if (populationBestDifference.z > PIE)
			populationBestDifference.z -= PIE_X_2;
		if (populationBestDifference.z < -PIE)
			populationBestDifference.z += PIE_X_2;
		
		individualBestDifference.x = individualBest.m_angles.x - m_angles.x;
		if (individualBestDifference.x > PIE)
			individualBestDifference.x -= PIE_X_2;
		if (individualBestDifference.x < -PIE)
			individualBestDifference.x += PIE_X_2;
		individualBestDifference.y = individualBest.m_angles.y - m_angles.y;
		individualBestDifference.z = individualBest.m_angles.z - m_angles.z;
		if (individualBestDifference.z > PIE)
			individualBestDifference.z -= PIE_X_2;
		if (individualBestDifference.z < -PIE)
			individualBestDifference.z += PIE_X_2;
		
		m_PSOAngleVelocity.x = m_PSOAngleVelocity.x * angleInertia
			+ fAttractionRepulsion * anglePopulationMinimumAttraction * randomFloat(0,1) * populationBestDifference.x
			+ fAttractionRepulsion * angleIndividualMinimumAttraction * randomFloat(0,1) * individualBestDifference.x;
		m_PSOAngleVelocity.y = m_PSOAngleVelocity.y * angleInertia
			+ fAttractionRepulsion * anglePopulationMinimumAttraction * randomFloat(0,1) * populationBestDifference.y
			+ fAttractionRepulsion * angleIndividualMinimumAttraction * randomFloat(0,1) * individualBestDifference.y;
		m_PSOAngleVelocity.z = m_PSOAngleVelocity.z * angleInertia
			+ fAttractionRepulsion * anglePopulationMinimumAttraction * randomFloat(0,1) * populationBestDifference.z
			+ fAttractionRepulsion * angleIndividualMinimumAttraction * randomFloat(0,1) * individualBestDifference.z;
		
		// Make sure we haven't exceeded the maximum velocity
		if (angleMaxVelocity != 0) {
			if (m_PSOAngleVelocity.x > angleMaxVelocity)
				m_PSOAngleVelocity.x = angleMaxVelocity;
			if (m_PSOAngleVelocity.y > angleMaxVelocity)
				m_PSOAngleVelocity.y = angleMaxVelocity;
			if (m_PSOAngleVelocity.z > angleMaxVelocity)
				m_PSOAngleVelocity.z = angleMaxVelocity;
			if (m_PSOAngleVelocity.x < -angleMaxVelocity)
				m_PSOAngleVelocity.x = -angleMaxVelocity;
			if (m_PSOAngleVelocity.y < -angleMaxVelocity)
				m_PSOAngleVelocity.y = -angleMaxVelocity;
			if (m_PSOAngleVelocity.z < -angleMaxVelocity)
				m_PSOAngleVelocity.z = -angleMaxVelocity;
		}
		
		m_angles.x += m_PSOAngleVelocity.x;
		m_angles.y += m_PSOAngleVelocity.y;
		m_angles.z += m_PSOAngleVelocity.z;
		
		while (m_angles.x < 0)
			m_angles.x += PIE_X_2;
		while (m_angles.x >= PIE_X_2)
			m_angles.x -= PIE_X_2;
		if (m_angles.y < -PIE_OVER_2)
			m_angles.y = -PIE_OVER_2;
		if (m_angles.y > PIE_OVER_2)
			m_angles.y = PIE_OVER_2;
		while (m_angles.z < 0)
			m_angles.z += PIE_X_2;
		while (m_angles.z >= PIE_X_2)
			m_angles.z -= PIE_X_2;
		
		// Enforce the box constraints
		initRotationMatrix();
		localToGlobal();
		amountOutsideBox.x = 0;
		amountOutsideBox.y = 0;
		amountOutsideBox.z = 0;
		for (int i = 0; i < m_iNumberOfAtoms; ++i) {
			// We're assuming here that the molecule isn't bigger than the box
			if ((m_atoms[i].m_globalPoint.x < 0) && (m_atoms[i].m_globalPoint.x < amountOutsideBox.x))
				amountOutsideBox.x = m_atoms[i].m_globalPoint.x;
			else if ((m_atoms[i].m_globalPoint.x > boxDimensions.x) && (m_atoms[i].m_globalPoint.x - boxDimensions.x > amountOutsideBox.x))
				amountOutsideBox.x = m_atoms[i].m_globalPoint.x - boxDimensions.x;
			if ((m_atoms[i].m_globalPoint.y < 0) && (m_atoms[i].m_globalPoint.y < amountOutsideBox.y))
				amountOutsideBox.y = m_atoms[i].m_globalPoint.y;
			else if ((m_atoms[i].m_globalPoint.y > boxDimensions.y) && (m_atoms[i].m_globalPoint.y - boxDimensions.y > amountOutsideBox.y))
				amountOutsideBox.y = m_atoms[i].m_globalPoint.y - boxDimensions.y;
			if ((m_atoms[i].m_globalPoint.z < 0) && (m_atoms[i].m_globalPoint.z < amountOutsideBox.z))
				amountOutsideBox.z = m_atoms[i].m_globalPoint.z;
			else if ((m_atoms[i].m_globalPoint.z > boxDimensions.z) && (m_atoms[i].m_globalPoint.z - boxDimensions.z > amountOutsideBox.z))
				amountOutsideBox.z = m_atoms[i].m_globalPoint.z - boxDimensions.z;
		}
		if ((amountOutsideBox.x != 0) || (amountOutsideBox.y != 0) || (amountOutsideBox.z != 0)) {
			Point3D shiftValue;
			shiftValue.x = -amountOutsideBox.x;
			shiftValue.y = -amountOutsideBox.y;
			shiftValue.z = -amountOutsideBox.z;
			moveMolecule(shiftValue);
		}
	}
}

void Molecule::getVelocityStats(FLOAT &maxCoordinateVelocity, FLOAT &totCoordinateVelocity, FLOAT &maxAngularVelocity,
                                FLOAT &totAngularVelocity)
{
	FLOAT coordinateVelocity;
	coordinateVelocity = sqrt(m_PSOCenterOfMassVelocity.x * m_PSOCenterOfMassVelocity.x + 
	                          m_PSOCenterOfMassVelocity.y * m_PSOCenterOfMassVelocity.y + 
	                          m_PSOCenterOfMassVelocity.z * m_PSOCenterOfMassVelocity.z);
	if (maxCoordinateVelocity < coordinateVelocity)
		maxCoordinateVelocity = coordinateVelocity;
	totCoordinateVelocity += coordinateVelocity;
	
	if (maxAngularVelocity < fabs(m_PSOAngleVelocity.x))
		maxAngularVelocity = fabs(m_PSOAngleVelocity.x);
	if (maxAngularVelocity < fabs(m_PSOAngleVelocity.y))
		maxAngularVelocity = fabs(m_PSOAngleVelocity.y);
	if (maxAngularVelocity < fabs(m_PSOAngleVelocity.z))
		maxAngularVelocity = fabs(m_PSOAngleVelocity.z);
	totAngularVelocity += fabs(m_PSOAngleVelocity.x) + fabs(m_PSOAngleVelocity.y) + fabs(m_PSOAngleVelocity.z);
	
/*	FLOAT angleVelocity = sqrt(m_PSOAngleVelocity.x * m_PSOAngleVelocity.x + 
	                           m_PSOAngleVelocity.y * m_PSOAngleVelocity.y + 
	                           m_PSOAngleVelocity.z * m_PSOAngleVelocity.z);
	if (maxAngularVelocity < angleVelocity)
		maxAngularVelocity = angleVelocity;
	totAngularVelocity += angleVelocity;*/
}

void Molecule::getClosestAtoms(Molecule &otherMolecule, Point3D &atomInThisMolecule, Point3D &atomInOtherMolecule,
	                           FLOAT &minDistanceBetweenAtoms)
{
	Point3D diff;
	FLOAT distSquared, tempDistSquared;
	int i, j;
	int iAtomInThisMolecule, iAtomInOtherMolecule;
	
	distSquared = 1e100; // some big number
	iAtomInThisMolecule = -1;
	iAtomInOtherMolecule = -1;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		for (j = 0; j < otherMolecule.m_iNumberOfAtoms; ++j) {
			diff.x = m_atoms[i].m_globalPoint.x - otherMolecule.m_atoms[j].m_globalPoint.x;
			diff.y = m_atoms[i].m_globalPoint.y - otherMolecule.m_atoms[j].m_globalPoint.y;
			diff.z = m_atoms[i].m_globalPoint.z - otherMolecule.m_atoms[j].m_globalPoint.z;
			tempDistSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
			if (tempDistSquared < distSquared) {
				distSquared = tempDistSquared;
				iAtomInThisMolecule = i;
				iAtomInOtherMolecule = j;
			}
		}
	
	atomInThisMolecule = m_atoms[iAtomInThisMolecule].m_globalPoint;
	atomInOtherMolecule = otherMolecule.m_atoms[iAtomInOtherMolecule].m_globalPoint;
	minDistanceBetweenAtoms = m_atoms[iAtomInThisMolecule].getMinDistance(otherMolecule.m_atoms[iAtomInOtherMolecule]);
}

int Molecule::randomInt(int lo, int hi)
{
	return (random() % (hi-lo+1)) + lo;
}

FLOAT Molecule::randomFloat(FLOAT lo, FLOAT hi)
{
	return (((FLOAT)random() / (FLOAT)RAND_MAX) * (FLOAT)(hi-lo)) + (FLOAT)lo;
}

void Molecule::initRandoms()
{
	srandom(time(NULL));
}

bool Molecule::haveSameAtoms(Molecule &otherMolecule)
{
	if (m_iNumberOfAtoms != otherMolecule.m_iNumberOfAtoms)
		return false;
	for (int i = 0; i < m_iNumberOfAtoms; ++i)
		if (m_atoms[i].m_iAtomicNumber != otherMolecule.m_atoms[i].m_iAtomicNumber)
			return false;
	return true;
}
