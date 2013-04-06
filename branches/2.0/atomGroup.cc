
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
	cleanUp();
	m_iNumberOfAtoms = agTemplate.m_atomicNumbers.size();
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
	m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];

	for (unsigned int i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atomicNumbers[i] = agTemplate.m_atomicNumbers[i];
		memcpy(m_localPoints[i], agTemplate.m_coordinates[i], sizeof(COORDINATE3));
		m_localPoints[i][3] = 1;
	}
	setLocalCenterOfMassToZero();
	initRotationMatrix();
	localToGlobal();
}

void AtomGroup::setAtoms(unsigned int numAtoms, const COORDINATE4* cartesianPoints,
		const unsigned int* atomicNumbers) {
	if (numAtoms != m_iNumberOfAtoms) {
		cleanUp();
		m_iNumberOfAtoms = numAtoms;
		m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
		m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
		m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];
	}

	memcpy(m_atomicNumbers, atomicNumbers, sizeof(unsigned int) * m_iNumberOfAtoms);
	memcpy(m_localPoints, cartesianPoints, sizeof(COORDINATE4) * m_iNumberOfAtoms);
	setLocalCenterOfMassToZero();
	initRotationMatrix();
	localToGlobal();
}

void AtomGroup::setLocalCenterOfMassToZero()
{
	COORDINATE3 centerOfMass = {0};
	FLOAT totalMass = 0;
	unsigned int i, j;

	for (i = 0; i < m_iNumberOfAtoms; ++i)
	{
		FLOAT mass = Handbook::getMass(m_atomicNumbers[i]);
		for (j = 0; j < 3; ++j)
			centerOfMass[j] += mass * m_localPoints[i][j];
		totalMass += mass;
	}
	for (j = 0; j < 3; ++j)
		centerOfMass[j] /= totalMass;

	for (i = 0; i < m_iNumberOfAtoms; ++i)
		for (j = 0; j < 3; ++j)
			m_localPoints[i][j] -= centerOfMass[j];
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
	memcpy(m_localPoints, atomGroup.m_localPoints, sizeof(COORDINATE4) * m_iNumberOfAtoms);
	m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];
	memcpy(m_globalPoints, atomGroup.m_globalPoints, sizeof(COORDINATE4) * m_iNumberOfAtoms);

	memcpy(m_centerOfMass, atomGroup.m_centerOfMass, sizeof(m_centerOfMass));
	memcpy(m_angles, atomGroup.m_angles, sizeof(m_angles));
	memcpy(m_sinAngles, atomGroup.m_sinAngles, sizeof(m_sinAngles));
	memcpy(m_cosAngles, atomGroup.m_cosAngles, sizeof(m_cosAngles));

	memcpy(m_rotationMatrixX, atomGroup.m_rotationMatrixX, sizeof(m_rotationMatrixX));
	memcpy(m_rotationMatrixY, atomGroup.m_rotationMatrixY, sizeof(m_rotationMatrixY));
	memcpy(m_rotationMatrixZ, atomGroup.m_rotationMatrixZ, sizeof(m_rotationMatrixZ));
	memcpy(m_matrixLocalToGlobal, atomGroup.m_matrixLocalToGlobal, sizeof(m_matrixLocalToGlobal));

	m_bFrozen = atomGroup.m_bFrozen;
}

void AtomGroup::initRotationMatrix()
{
	FLOAT temp[MATRIX_WIDTH][MATRIX_WIDTH];

	for (unsigned int i = 0; i < 3; ++i) {
		m_sinAngles[i] = sin(m_angles[i]);
		m_cosAngles[i] = cos(m_angles[i]);
	}

	memcpy(m_rotationMatrixX, IDENTITY_MATRIX, sizeof(IDENTITY_MATRIX));
	memcpy(m_rotationMatrixY, IDENTITY_MATRIX, sizeof(IDENTITY_MATRIX));
	memcpy(m_rotationMatrixZ, IDENTITY_MATRIX, sizeof(IDENTITY_MATRIX));

	m_rotationMatrixX[1][1] = m_cosAngles[0];
	m_rotationMatrixX[2][1] = -m_sinAngles[0];
	m_rotationMatrixX[1][2] = m_sinAngles[0];
	m_rotationMatrixX[2][2] = m_cosAngles[0];

	m_rotationMatrixY[0][0] = m_cosAngles[1];
	m_rotationMatrixY[2][0] = -m_sinAngles[1];
	m_rotationMatrixY[0][2] = m_sinAngles[1];
	m_rotationMatrixY[2][2] = m_cosAngles[1];

	m_rotationMatrixZ[0][0] = m_cosAngles[2];
	m_rotationMatrixZ[1][0] = -m_sinAngles[2];
	m_rotationMatrixZ[0][1] = m_sinAngles[2];
	m_rotationMatrixZ[1][1] = m_cosAngles[2];

	Matrix::matrixMultiplyMatrix(m_rotationMatrixX,m_rotationMatrixY,temp);
	Matrix::matrixMultiplyMatrix(temp,m_rotationMatrixZ,m_matrixLocalToGlobal);

	// Note: when translation is the last operation, the next three commands have the same effect as
	// multiplying by a translation matrix.
	m_matrixLocalToGlobal[MATRIX_WIDTH-1][0] = m_centerOfMass[0];
	m_matrixLocalToGlobal[MATRIX_WIDTH-1][1] = m_centerOfMass[1];
	m_matrixLocalToGlobal[MATRIX_WIDTH-1][2] = m_centerOfMass[2];
}

bool AtomGroup::localToGlobal()
{
	unsigned int i;
	memcpy(m_globalPoints,m_localPoints,m_iNumberOfAtoms*MATRIX_WIDTH*sizeof(FLOAT));
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		Matrix::matrixMultiplyPoint(m_globalPoints[i],m_matrixLocalToGlobal);
//	for (i = 0; i < m_rotatableBonds.size(); ++i)
//		if (!performBondRotation(i))
//			return false;
	return true;
}
