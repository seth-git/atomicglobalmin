
#include "atomGroup.h"

AtomGroup::AtomGroup()
{
	m_iNumberOfAtoms = 0;
	m_atomicNumbers = NULL;
	m_localPoints = NULL;
	m_globalPoints = NULL;
}

AtomGroup::~AtomGroup()
{
	cleanUp();
}

void AtomGroup::cleanUp()
{
	m_iNumberOfAtoms = 0;
	if (m_atomicNumbers != NULL) {
		delete[] m_atomicNumbers;
		m_atomicNumbers = NULL;
	}
	if (m_localPoints != NULL) {
		delete[] m_localPoints;
		m_localPoints = NULL;
	}
	if (m_globalPoints != NULL) {
		delete[] m_globalPoints;
		m_globalPoints = NULL;
	}
	std::fill_n(m_centerOfMass, 3, 0);
	std::fill_n(m_angles, 3, 0);
}

void AtomGroup::setAtoms(const AtomGroupTemplate &agTemplate)
{
	unsigned int i, j;

	cleanUp();
	m_iNumberOfAtoms = agTemplate.m_atomicNumbers.size();
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
	m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];

	for (i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atomicNumbers[i] = agTemplate.m_atomicNumbers[i];
		memcpy(m_localPoints[i], agTemplate.m_coordinates[i], SIZEOF_COORDINATE3);
		m_localPoints[i][3] = 1;
	}

	COORDINATE3 centerOfMass;
	getCenterOfMass(m_iNumberOfAtoms, m_atomicNumbers, m_localPoints, centerOfMass);
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		for (j = 0; j < 3; ++j)
			m_localPoints[i][j] -= centerOfMass[j];

	initRotationMatrix();
	localToGlobal();
}

void AtomGroup::getCenterOfMass(unsigned int iNumberOfAtoms,
		const unsigned int* atomicNumbers, const COORDINATE4* points,
		COORDINATE3 &centerOfMass) {
	centerOfMass[0] = 0;
	centerOfMass[1] = 0;
	centerOfMass[2] = 0;
	FLOAT totalMass = 0;
	unsigned int i, j;

	for (i = 0; i < iNumberOfAtoms; ++i)
	{
		FLOAT mass = Handbook::getMass(atomicNumbers[i]);
		for (j = 0; j < 3; ++j)
			centerOfMass[j] += mass * points[i][j];
		totalMass += mass;
	}
	for (j = 0; j < 3; ++j)
		centerOfMass[j] /= totalMass;
}

void AtomGroup::setAtoms(unsigned int numAtoms, const COORDINATE4* cartesianPoints,
		const unsigned int* atomicNumbers) {
	unsigned int i, j;

	if (numAtoms != m_iNumberOfAtoms) {
		cleanUp();
		m_iNumberOfAtoms = numAtoms;
		m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
		m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
		m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];
	}

	memcpy(m_atomicNumbers, atomicNumbers, sizeof(unsigned int) * m_iNumberOfAtoms);
	memcpy(m_globalPoints, cartesianPoints, SIZEOF_COORDINATE4 * m_iNumberOfAtoms);
	memcpy(m_localPoints, cartesianPoints, SIZEOF_COORDINATE4 * m_iNumberOfAtoms);

	getCenterOfMass(m_iNumberOfAtoms, m_atomicNumbers, m_localPoints, m_centerOfMass);
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		for (j = 0; j < 3; ++j)
			m_localPoints[i][j] -= m_centerOfMass[j];

	m_angles[0] = 0;
	m_angles[1] = 0;
	m_angles[2] = 0;

	initRotationMatrix();
}

bool AtomGroup::load(TiXmlElement *pAtomGroupElem, const Strings* messages)
{
	cleanUp();
	return true;
}

bool AtomGroup::save(TiXmlElement *pParentElem, const Strings* messages)
{
	return true;
}

void AtomGroup::copy(AtomGroup &atomGroup) {
	cleanUp();

	m_iNumberOfAtoms = atomGroup.m_iNumberOfAtoms;
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	memcpy(m_atomicNumbers, atomGroup.m_atomicNumbers, sizeof(unsigned int) * m_iNumberOfAtoms);
	m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
	memcpy(m_localPoints, atomGroup.m_localPoints, SIZEOF_COORDINATE4 * m_iNumberOfAtoms);
	m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];
	memcpy(m_globalPoints, atomGroup.m_globalPoints, SIZEOF_COORDINATE4 * m_iNumberOfAtoms);

	memcpy(m_centerOfMass, atomGroup.m_centerOfMass, SIZEOF_COORDINATE3);
	memcpy(m_angles, atomGroup.m_angles, SIZEOF_COORDINATE3);
	memcpy(m_sinAngles, atomGroup.m_sinAngles, SIZEOF_COORDINATE3);
	memcpy(m_cosAngles, atomGroup.m_cosAngles, SIZEOF_COORDINATE3);

	memcpy(m_rotationMatrixX, atomGroup.m_rotationMatrixX, SIZEOF_MATRIX);
	memcpy(m_rotationMatrixY, atomGroup.m_rotationMatrixY, SIZEOF_MATRIX);
	memcpy(m_rotationMatrixZ, atomGroup.m_rotationMatrixZ, SIZEOF_MATRIX);
	memcpy(m_matrixLocalToGlobal, atomGroup.m_matrixLocalToGlobal, SIZEOF_MATRIX);

	m_bFrozen = atomGroup.m_bFrozen;
}

void AtomGroup::initRotationMatrix()
{
	FLOAT temp[MATRIX_WIDTH][MATRIX_WIDTH];

	for (unsigned int i = 0; i < 3; ++i) {
		m_sinAngles[i] = sin(m_angles[i]);
		m_cosAngles[i] = cos(m_angles[i]);
	}

	memcpy(m_rotationMatrixX, IDENTITY_MATRIX, SIZEOF_MATRIX);
	memcpy(m_rotationMatrixY, IDENTITY_MATRIX, SIZEOF_MATRIX);
	memcpy(m_rotationMatrixZ, IDENTITY_MATRIX, SIZEOF_MATRIX);

	// Note that the column index comes before the row index (matrix[col][row])
	m_rotationMatrixX[1][1] = m_cosAngles[0];
	m_rotationMatrixX[1][2] = -m_sinAngles[0];
	m_rotationMatrixX[2][1] = m_sinAngles[0];
	m_rotationMatrixX[2][2] = m_cosAngles[0];

	m_rotationMatrixY[0][0] = m_cosAngles[1];
	m_rotationMatrixY[0][2] = m_sinAngles[1];
	m_rotationMatrixY[2][0] = -m_sinAngles[1];
	m_rotationMatrixY[2][2] = m_cosAngles[1];

	m_rotationMatrixZ[0][0] = m_cosAngles[2];
	m_rotationMatrixZ[0][1] = -m_sinAngles[2];
	m_rotationMatrixZ[1][0] = m_sinAngles[2];
	m_rotationMatrixZ[1][1] = m_cosAngles[2];

	Matrix::matrixMultiplyMatrix(m_rotationMatrixX,m_rotationMatrixY,temp);
	Matrix::matrixMultiplyMatrix(temp,m_rotationMatrixZ,m_matrixLocalToGlobal);

	// Note: when translation is the last operation, the next three commands
	// have the same effect as multiplying by a translation matrix.
	m_matrixLocalToGlobal[0][3] = m_centerOfMass[0];
	m_matrixLocalToGlobal[1][3] = m_centerOfMass[1];
	m_matrixLocalToGlobal[2][3] = m_centerOfMass[2];
}

bool AtomGroup::localToGlobal()
{
	unsigned int i;
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		Matrix::matrixMultiplyPoint(m_matrixLocalToGlobal, m_localPoints[i], m_globalPoints[i]);
//	for (i = 0; i < m_rotatableBonds.size(); ++i)
//		if (!performBondRotation(i))
//			return false;
	return true;
}
