
#include "structure.h"

Structure::Structure() {
	m_iNumberOfAtomGroups = 0;
	m_atomGroups = NULL;
	m_atomGroupIndices = NULL;
	m_iNumberOfAtoms = 0;
	m_atomCoordinates = NULL;
	m_atomicNumbers = NULL;
	m_atomDistanceMatrix = NULL;
	m_atomGroupDistanceMatrix = NULL;
}

Structure::~Structure() {
	clear();
}

void Structure::clear() {
	unsigned int i;

	if (m_atomDistanceMatrix != NULL) {
		for (i = 0; i < m_iNumberOfAtoms; ++i)
			delete[] m_atomDistanceMatrix[i];
		delete[] m_atomDistanceMatrix;
		m_atomDistanceMatrix = NULL;
	}

	m_iNumberOfAtoms = 0;
	if (m_atomCoordinates != NULL) {
		delete[] m_atomCoordinates;
		m_atomCoordinates = NULL;
	}
	if (m_atomicNumbers != NULL) {
		delete[] m_atomicNumbers;
		m_atomicNumbers = NULL;
	}

	if (m_atomGroupDistanceMatrix != NULL) {
		for (i = 0; i < m_iNumberOfAtomGroups; ++i)
			delete[] m_atomGroupDistanceMatrix[i];
		delete[] m_atomGroupDistanceMatrix;
		m_atomGroupDistanceMatrix = NULL;
	}

	m_iNumberOfAtomGroups = 0;
	if (m_atomGroups != NULL) {
		delete[] m_atomGroups;
		m_atomGroups = NULL;
	}

	if (m_atomGroupIndices != NULL) {
		delete[] m_atomGroupIndices;
		m_atomGroupIndices = NULL;
	}
}

void Structure::setAtoms(unsigned int numAtoms,
		const COORDINATE4 *cartesianPoints, const unsigned int* atomicNumbers) {
	if (numAtoms != m_iNumberOfAtoms) {
		clear();
		m_iNumberOfAtoms = numAtoms;
		m_iNumberOfAtomGroups = 1;
		m_atomGroups = new AtomGroup[m_iNumberOfAtomGroups];
		m_atomGroups[0].setAtoms(numAtoms, cartesianPoints, atomicNumbers);
		initCoordinateRefs();
	} else {
		unsigned int i, j;
		j = 0;
		for (i = 0; i < m_iNumberOfAtomGroups; ++i) {
			m_atomGroups[i].setAtoms(m_atomGroups[i].getNumberOfAtoms(), &cartesianPoints[j], &atomicNumbers[j]);
			j += m_atomGroups[i].getNumberOfAtoms();
		}
	}
}

void Structure::setAtomGroups(unsigned int numAtomGroupTemplates, const AtomGroupTemplate* atomGroupTemplates) {
	unsigned int i, j, k, l;
	const AtomGroupTemplate* agtemp;
	const AtomGroup* ag;

	unsigned int iNumberOfAtomGroups = 0;
	unsigned int iNumberOfAtoms = 0;
	for (i = 0; i < numAtomGroupTemplates; ++i) {
		agtemp = &atomGroupTemplates[i];
		iNumberOfAtomGroups += agtemp->m_iNumber;
		iNumberOfAtoms += agtemp->m_iNumber * agtemp->m_atomicNumbers.size();
	}
	AtomGroup* atomGroups = new AtomGroup[iNumberOfAtomGroups];

	if (atomsMatch(numAtomGroupTemplates, atomGroupTemplates)) {
		// Copy the coordinates
		COORDINATE4 localAtomCoordinates[m_iNumberOfAtoms];
		for (i = 0, j = 0; i < m_iNumberOfAtomGroups; ++i) {
			ag = &m_atomGroups[i];
			memcpy(&localAtomCoordinates[j], ag->getLocalAtomCoordinates(), SIZEOF_COORDINATE4 * ag->getNumberOfAtoms());
			j += ag->getNumberOfAtoms();
		}

		for (i = 0, k = 0, l = 0; i < numAtomGroupTemplates; ++i) {
			agtemp = &atomGroupTemplates[i];
			for (j = 1; j <= agtemp->m_iNumber; ++j) {
				atomGroups[k++].setAtoms(agtemp->m_atomicNumbers.size(), &localAtomCoordinates[l], &m_atomicNumbers[l]);
				l += agtemp->m_atomicNumbers.size();
			}
		}
	} else {
		for (i = 0, k = 0; i < numAtomGroupTemplates; ++i) {
			agtemp = &atomGroupTemplates[i];
			for (j = 1; j <= agtemp->m_iNumber; ++j)
				atomGroups[k++].setAtoms(*agtemp);
		}
	}

	clear();
	m_iNumberOfAtomGroups = iNumberOfAtomGroups;
	m_iNumberOfAtoms = iNumberOfAtoms;
	m_atomGroups = atomGroups;
	initCoordinateRefs();
}

bool Structure::atomsMatch(unsigned int numAtomGroupTemplates,
		const AtomGroupTemplate* atomGroupTemplates) {

	unsigned int groupi, groupj, groupn;
	const AtomGroupTemplate* templatei;
	unsigned int i;

	unsigned int totalAtoms = 0;
	unsigned int atomi = 0;
	for (groupi = 0; groupi < numAtomGroupTemplates; ++groupi) {
		templatei = &atomGroupTemplates[groupi];
		for (groupj = 0, groupn = templatei->m_iNumber; groupj < groupn; ++groupj) {
			totalAtoms += groupn;
			if (totalAtoms > m_iNumberOfAtoms)
				return false;
			for (i = 0; i < templatei->m_atomicNumbers.size(); ++i)
				if (m_atomicNumbers[atomi++] != templatei->m_atomicNumbers[i])
					return false;
		}
	}
	if (totalAtoms != m_iNumberOfAtoms)
		return false;
	return true;
}

void Structure::insertAtomGroup(AtomGroupTemplate &atomGroupTemplate, unsigned int index) {
	unsigned int iNumberOfAtomGroups = m_iNumberOfAtomGroups + 1;
	AtomGroup* atomGroups = new AtomGroup[iNumberOfAtomGroups];
	unsigned int iNumberOfAtoms = m_iNumberOfAtoms+atomGroupTemplate.m_atomicNumbers.size();

	unsigned int i;
	for (i = 0; i < index; ++i)
		atomGroups[i].copy(m_atomGroups[i]);
	atomGroups[index].setAtoms(atomGroupTemplate);
	for (i = index+1; i < iNumberOfAtomGroups; ++i)
		atomGroups[i].copy(m_atomGroups[i-1]);

	clear();
	m_iNumberOfAtomGroups = iNumberOfAtomGroups;
	m_iNumberOfAtoms = iNumberOfAtoms;
	m_atomGroups = atomGroups;
	initCoordinateRefs();
}

void Structure::deleteAtomGroup(unsigned int index) {
	unsigned int iNumberOfAtomGroups = m_iNumberOfAtomGroups - 1;
	AtomGroup* atomGroups = new AtomGroup[iNumberOfAtomGroups];
	unsigned int iNumberOfAtoms = m_iNumberOfAtoms - m_atomGroups[index].getNumberOfAtoms();

	unsigned int i;
	for (i = 0; i < index; ++i)
		atomGroups[i].copy(m_atomGroups[i]);
	for (i = index; i < iNumberOfAtomGroups; ++i)
		atomGroups[i].copy(m_atomGroups[i+1]);

	clear();
	m_iNumberOfAtomGroups = iNumberOfAtomGroups;
	m_iNumberOfAtoms = iNumberOfAtoms;
	m_atomGroups = atomGroups;
	initCoordinateRefs();
}

void Structure::initCoordinateRefs() {
	// Note: this method assumes the calling method has done the following before calling this method:
	// 1. Called the clear method
	// 2. Initialized m_iNumberOfAtoms, m_iNumberOfAtomGroups, and m_atomGroups
	// If this isn't the case, memory leaks may result.

	unsigned int i, j, k;
	m_atomCoordinates = new const COORDINATE4*[m_iNumberOfAtoms];
	m_atomicNumbers = new unsigned int[m_iNumberOfAtoms];
	const COORDINATE4* coordinates;
	k = 0;
	for (i = 0; i < m_iNumberOfAtomGroups; ++i) {
		memcpy(&m_atomicNumbers[k], m_atomGroups[i].getAtomicNumbers(), sizeof(unsigned int)*m_atomGroups[i].getNumberOfAtoms());
		coordinates = m_atomGroups[i].getGlobalAtomCoordinates();
		for (j = 0; j < m_atomGroups[i].getNumberOfAtoms(); ++j)
			m_atomCoordinates[k++] = &coordinates[j];
	}

	m_atomDistanceMatrix = new FLOAT*[m_iNumberOfAtoms];
	for (i = 0; i < m_iNumberOfAtoms; ++i)
		m_atomDistanceMatrix[i] = new FLOAT[m_iNumberOfAtoms];

	m_atomGroupIndices = new unsigned int[m_iNumberOfAtomGroups];
	j = 0;
	for (i = 0; i < m_iNumberOfAtomGroups; ++i) {
		m_atomGroupIndices[i] = j;
		j += m_atomGroups[i].getNumberOfAtoms();
	}

	m_atomGroupDistanceMatrix = new FLOAT*[m_iNumberOfAtomGroups];
	for (i = 0; i < m_iNumberOfAtomGroups; ++i)
		m_atomGroupDistanceMatrix[i] = new FLOAT[m_iNumberOfAtomGroups];
}

void Structure::updateAtomDistanceMatrix() {
	unsigned int nminus1 = m_iNumberOfAtoms-1;
	unsigned int i, iAtom, jAtom;

	FLOAT distance, diff;
	for (iAtom = 0; iAtom < nminus1; ++iAtom)
		for (jAtom = iAtom+1; jAtom < m_iNumberOfAtoms; ++jAtom)
		{
			distance = 0;
			for (i = 0; i < 3; ++i) {
				diff = m_atomCoordinates[iAtom][i] - m_atomCoordinates[jAtom][i];
				distance += diff*diff;
			}
			distance = sqrt(distance);
			m_atomDistanceMatrix[iAtom][jAtom] = distance;
			m_atomDistanceMatrix[jAtom][iAtom] = distance;
		}
	for (iAtom = 0; iAtom < m_iNumberOfAtoms; ++iAtom)
		m_atomDistanceMatrix[iAtom][iAtom] = 0;
}

FLOAT Structure::findClosestDistance(unsigned int iAtomGroup1, unsigned int iAtomGroup2) {
	unsigned int iAtom1Start = m_atomGroupIndices[iAtomGroup1];
	unsigned int iAtom1End = iAtom1Start + m_atomGroups[iAtomGroup1].getNumberOfAtoms();
	unsigned int iAtom2Start = m_atomGroupIndices[iAtomGroup2];
	unsigned int iAtom2End = iAtom2Start + m_atomGroups[iAtomGroup2].getNumberOfAtoms();
	unsigned int iAtom1, iAtom2;

	FLOAT answer = 1e100; // some big number;
	FLOAT temp;
	for (iAtom1 = iAtom1Start; iAtom1 < iAtom1End; ++iAtom1)
		for (iAtom2 = iAtom2Start; iAtom2 < iAtom2End; ++iAtom2) {
			temp = m_atomDistanceMatrix[iAtom1][iAtom2];
			if (answer > temp)
				answer = temp;
		}

	return answer;
}

void Structure::updateAtomGroupDistanceMatrix() {
	unsigned int nminus1 = m_iNumberOfAtomGroups-1;
	unsigned int i, j;

	FLOAT distance;
	for (i = 0; i < nminus1; ++i)
		for (j = i+1; j < m_iNumberOfAtomGroups; ++j)
		{
			distance = findClosestDistance(i, j);
			m_atomGroupDistanceMatrix[i][j] = distance;
			m_atomGroupDistanceMatrix[j][i] = distance;
		}
	for (i = 0; i < m_iNumberOfAtomGroups; ++i)
		m_atomGroupDistanceMatrix[i][i] = 0;
}

bool Structure::load(TiXmlElement *pStructureElem, const Strings* messages) {
	return true;
}

bool Structure::save(TiXmlElement *pParentElem, const Strings* messages) const {
	return true;
}

void Structure::copy(Structure &structure) {
	unsigned int i;
	clear();

	m_iNumberOfAtomGroups = structure.m_iNumberOfAtomGroups;
	m_atomGroups = new AtomGroup[m_iNumberOfAtomGroups];
	for (unsigned int i = 0; i < m_iNumberOfAtomGroups; ++i)
		m_atomGroups[i].copy(structure.m_atomGroups[i]);

	m_iNumberOfAtoms = structure.m_iNumberOfAtoms;
	initCoordinateRefs();

	if (NULL != structure.m_atomGroupIndices) {
		memcpy(m_atomGroupIndices, structure.m_atomGroupIndices, sizeof(unsigned int) * m_iNumberOfAtomGroups);
	}
	if (NULL != structure.m_atomDistanceMatrix) {
		size_t nbytes = sizeof(FLOAT) * m_iNumberOfAtoms;
		for (i = 0; i < m_iNumberOfAtoms; ++i)
			memcpy(m_atomDistanceMatrix[i], structure.m_atomDistanceMatrix[i], nbytes);
	}
	if (NULL != structure.m_atomGroupDistanceMatrix) {
		size_t nbytes = sizeof(FLOAT) * m_iNumberOfAtomGroups;
		for (i = 0; i < m_iNumberOfAtomGroups; ++i)
			memcpy(m_atomGroupDistanceMatrix[i], structure.m_atomGroupDistanceMatrix[i], nbytes);
	}

	m_energy = structure.m_energy;
	m_bIsTransitionState = structure.m_bIsTransitionState;
}
