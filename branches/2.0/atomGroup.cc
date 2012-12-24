
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
}

void AtomGroup::load(AtomGroupTemplate &agTemplate)
{
	cleanUp();
	m_iNumberOfAtoms = agTemplate.m_iNumber;
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	m_localPoints = new COORDINATE4[m_iNumberOfAtoms];
	m_globalPoints = new COORDINATE4[m_iNumberOfAtoms];

	size_t nBytes = 3 * sizeof(FLOAT);
	for (unsigned int i = 0; i < m_iNumberOfAtoms; ++i)
	{
		m_atomicNumbers[i] = agTemplate.m_atomicNumbers[i];
		memcpy(m_localPoints[i], agTemplate.m_coordinates[i], nBytes);
		m_localPoints[i][3] = 1;
	}
	setLocalCenterOfMassToZero();
	std::fill_n(m_centerOfMass, 3, 0);
	std::fill_n(m_angles, 3, 0);
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
